FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-20.04

# Install dependencies
RUN apt update \
    && apt install -y gcc-arm-none-eabi cmake libtool libhidapi-dev libusb-1.0-0-dev udev

# Upgrade cmake version
RUN apt remove -y cmake \
    && wget https://github.com/Kitware/CMake/releases/download/v3.24.1/cmake-3.24.1-Linux-x86_64.sh -q -O /tmp/cmake-install.sh \
    && chmod u+x /tmp/cmake-install.sh \
    && mkdir /opt/cmake-3.24.1 \
    && /tmp/cmake-install.sh --skip-license --prefix=/opt/cmake-3.24.1 \
    && rm /tmp/cmake-install.sh \
    && ln -s /opt/cmake-3.24.1/bin/* /usr/local/bin

# Install openocd
RUN git clone https://github.com/raspberrypi/openocd.git \
    && cd openocd \
    && ./bootstrap \
    && ./configure --enable-cmsis-dap \
    && make -j32 \
    && sudo make install


