#!/bin/env sh

ELF_NAME=n64-sniffer.elf

cmake --build build -j30 && \
    scp build/${ELF_NAME} pi@raspberrypi:/home/pi && \
    ssh pi@raspberrypi "openocd -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg -c \"program ${ELF_NAME} verify reset exit\""
