# Copyright (c) 2023 ASPEED Technology Inc.
#
# SPDX-License-Identifier: Apache-2.0
#

zephyr_sources(soc.c)
zephyr_include_directories(${ZEPHYR_BASE}/soc/arm/common/cortex_m)
zephyr_linker_sources(ROM_START SORT_KEY 0x1sboot sboot.ld)
set(SOC_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld CACHE INTERNAL "")
