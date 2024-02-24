#!/bin/env bash

ssh -t pi@raspberrypi "openocd -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg -c \"bindto 0.0.0.0; init; reset halt\""
