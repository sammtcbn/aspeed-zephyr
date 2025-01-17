/*
 * Copyright (c) 2021 ASPEED
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT aspeed_pwm

#define LOG_LEVEL CONFIG_PWM_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(pwm_aspeed);

#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/sys_io.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <soc.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/reset.h>
#include <zephyr/drivers/pinctrl.h>
#include "pwm_aspeed.h"

#define NUM_OF_CHANNELS DT_INST_PROP(0, npwms)

#define PWM_ASPEED_FIXED_PERIOD 0xff

/* Structure Declarations */

struct pwm_aspeed_data {
	uint32_t clk_src;
	uint32_t curr_period_cycles;
};

struct pwm_aspeed_cfg {
	struct pwm_register_s *base;
	const struct device *clock_dev;
	const clock_control_subsys_t clk_id;
	const struct reset_dt_spec reset;
	const struct pinctrl_dev_config *pcfg;
};

#define DEV_CFG(dev)  ((const struct pwm_aspeed_cfg *const)(dev)->config)
#define DEV_DATA(dev) ((struct pwm_aspeed_data *)(dev)->data)

/* API Functions */

static int pwm_aspeed_init(const struct device *dev)
{
	struct pwm_aspeed_data *priv = DEV_DATA(dev);
	const struct pwm_aspeed_cfg *config = DEV_CFG(dev);
	int ret;

	ret = pinctrl_apply_state(config->pcfg, PINCTRL_STATE_DEFAULT);
	if (ret) {
		return ret;
	}
	clock_control_get_rate(config->clock_dev, config->clk_id, &priv->clk_src);
	reset_line_deassert_dt(&config->reset);
	LOG_INF("priv->clk_src = %d", priv->clk_src);

	return 0;
}

static void aspeed_set_pwm_channel_enable(const struct device *dev, uint32_t pwm, bool enable)
{
	struct pwm_register_s *pwm_reg = DEV_CFG(dev)->base;
	union pwm_general_register_s general_reg;

	general_reg.value = pwm_reg->pwm_gather[pwm].pwm_general.value;
	general_reg.fields.enable_pwm_pin = enable;
	general_reg.fields.enable_pwm_clock = enable;
	pwm_reg->pwm_gather[pwm].pwm_general.value = general_reg.value;
}

static int aspeed_set_pwm_period(const struct device *dev, uint32_t pwm, uint32_t period_cycles)
{
	struct pwm_register_s *pwm_reg = DEV_CFG(dev)->base;
	struct pwm_aspeed_data *priv = DEV_DATA(dev);
	union pwm_general_register_s general_reg;
	uint32_t div_h, div_l, expected_period_cycles = period_cycles;

	if (period_cycles < (PWM_ASPEED_FIXED_PERIOD + 1)) {
		return -ENOTSUP;
	}
	period_cycles /= (PWM_ASPEED_FIXED_PERIOD + 1);

#ifdef CONFIG_PWM_ASPEED_ACCURATE_FREQ
	int diff, min_diff = INT_MAX;
	uint32_t tmp_div_h, tmp_div_l;
	bool found = 0;

	/* calculate for target frequence */
	for (tmp_div_h = 0; tmp_div_h < 0x10; tmp_div_h++) {
		tmp_div_l = period_cycles / BIT(tmp_div_h) - 1;

		if (tmp_div_l < 0 || tmp_div_l > 255) {
			continue;
		}

		diff = period_cycles - (BIT(tmp_div_h) * (tmp_div_l + 1));
		if (abs(diff) < abs(min_diff)) {
			min_diff = diff;
			div_l = tmp_div_l;
			div_h = tmp_div_h;
			found = 1;
			if (diff == 0) {
				break;
			}
		}
	}
	if (!found) {
		return -ERANGE;
	}
#else
	uint32_t divisor;
	/*
	 * Pick the smallest value for div_h so that div_l can be the biggest
	 * which results in a finer resolution near the target period value.
	 */
	divisor = 256;
	div_h = BIT(find_msb_set(period_cycles / divisor) - 1);
	if (div_h > 0xf) {
		div_h = 0xf;
	}

	divisor = BIT(div_h);
	div_l = period_cycles / divisor;

	if (div_l == 0) {
		return -ERANGE;
	}

	div_l -= 1;

	if (div_l > 255) {
		div_l = 255;
	}
#endif

	/*
	 * The PWM frequency = PCLK(200Mhz) / (clock division L bit *
	 * clock division H bit * (period bit(0xff) + 1))
	 */
	LOG_DBG("div_h %x, div_l : %x\n", div_h, div_l);

	general_reg.value = pwm_reg->pwm_gather[pwm].pwm_general.value;
	general_reg.fields.pwm_clock_division_h = div_h;
	general_reg.fields.pwm_clock_division_l = div_l;
	pwm_reg->pwm_gather[pwm].pwm_general.value = general_reg.value;
	priv->curr_period_cycles = expected_period_cycles;
	return 0;
}

static void aspeed_set_pwm_duty(const struct device *dev, uint32_t pwm, uint32_t pulse_cycles)
{
	struct pwm_register_s *pwm_reg = DEV_CFG(dev)->base;
	struct pwm_aspeed_data *priv = DEV_DATA(dev);
	union pwm_duty_cycle_register_s duty_reg;
	uint32_t period_cycles = priv->curr_period_cycles;
	uint32_t duty_pt = (pulse_cycles * 256) / period_cycles;

	LOG_DBG("cur_period = %d, duty_cycle = %d, duty_pt = %d\n", period_cycles, pulse_cycles,
		duty_pt);
	if (duty_pt == 0) {
		aspeed_set_pwm_channel_enable(dev, pwm, false);
	} else {
		duty_reg.value = pwm_reg->pwm_gather[pwm].pwm_duty_cycle.value;
		duty_reg.fields.pwm_falling_point = duty_pt;
		pwm_reg->pwm_gather[pwm].pwm_duty_cycle.value = duty_reg.value;
		aspeed_set_pwm_channel_enable(dev, pwm, true);
	}
}

static void aspeed_set_pwm_polarity(const struct device *dev, uint32_t pwm, uint8_t polarity)
{
	struct pwm_register_s *pwm_reg = DEV_CFG(dev)->base;
	union pwm_general_register_s general_reg;

	general_reg.value = pwm_reg->pwm_gather[pwm].pwm_general.value;
	general_reg.fields.inverse_pwm_pin = polarity;
	pwm_reg->pwm_gather[pwm].pwm_general.value = general_reg.value;
}

static int pwm_aspeed_set_cycles(const struct device *dev, uint32_t pwm, uint32_t period_cycles,
				 uint32_t pulse_cycles, pwm_flags_t flags)
{
	int ret;
	struct pwm_register_s *pwm_reg = DEV_CFG(dev)->base;
	union pwm_duty_cycle_register_s duty_reg;

	if (pwm > NUM_OF_CHANNELS) {
		return -EINVAL;
	}

	duty_reg.value = pwm_reg->pwm_gather[pwm].pwm_duty_cycle.value;
	duty_reg.fields.pwm_period = PWM_ASPEED_FIXED_PERIOD;
	pwm_reg->pwm_gather[pwm].pwm_duty_cycle.value = duty_reg.value;

	ret = aspeed_set_pwm_period(dev, pwm, period_cycles);
	if (ret) {
		return ret;
	}
	aspeed_set_pwm_duty(dev, pwm, pulse_cycles);
	aspeed_set_pwm_polarity(dev, pwm, flags);
	return 0;
}

static int pwm_aspeed_get_cycles_per_sec(const struct device *dev, uint32_t pwm, uint64_t *cycles)
{
	struct pwm_aspeed_data *priv = DEV_DATA(dev);

	if (pwm > NUM_OF_CHANNELS) {
		return -EINVAL;
	}

	*cycles = priv->clk_src;
	return 0;
}

/* Device Instantiation */

static const struct pwm_driver_api pwm_aspeed_api = {
	.set_cycles = pwm_aspeed_set_cycles,
	.get_cycles_per_sec = pwm_aspeed_get_cycles_per_sec,
};

#define PWM_ASPEED_INIT(n)                                                                         \
	PINCTRL_DT_INST_DEFINE(n);                                                                 \
	static struct pwm_aspeed_data pwm_aspeed_data_##n;                                         \
	static const struct pwm_aspeed_cfg pwm_aspeed_cfg_##n = {                                  \
		.base = (struct pwm_register_s *)DT_REG_ADDR(DT_PARENT(DT_DRV_INST(n))),           \
		.clock_dev = DEVICE_DT_GET(DT_INST_CLOCKS_CTLR(n)),                                \
		.clk_id = (clock_control_subsys_t)DT_INST_CLOCKS_CELL(n, clk_id),                  \
		.pcfg = PINCTRL_DT_INST_DEV_CONFIG_GET(n),                                         \
		.reset = RESET_DT_SPEC_INST_GET(n),                                                \
	};                                                                                         \
	DEVICE_DT_INST_DEFINE(n, pwm_aspeed_init, NULL, &pwm_aspeed_data_##n, &pwm_aspeed_cfg_##n, \
			      POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &pwm_aspeed_api);

DT_INST_FOREACH_STATUS_OKAY(PWM_ASPEED_INIT)
