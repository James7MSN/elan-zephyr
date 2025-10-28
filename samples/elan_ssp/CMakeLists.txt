cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(elan_ssp_test)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/ssp_test.c)
target_sources(app PRIVATE src/dma_test.c)
target_sources(app PRIVATE src/spi_debug_test.c)