#!/bin/env bash

cmake --build build -j30 && \
    openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "init; program build/n64-sniffer.elf verify; reset run; shutdown"
