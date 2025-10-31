# Dockerfile
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    git \
    cmake \
    wget \
    gcc-arm-none-eabi \
    libusb-1.0-0-dev \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
    build-essential \
    python3 \
    python3-pip \
    gdb-multiarch \
    minicom \
    ninja-build \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

RUN useradd -m -s /bin/bash vscode && \
    echo "vscode ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

WORKDIR /opt
RUN git clone --depth 1 --branch 1.5.1 https://github.com/raspberrypi/pico-sdk.git && \
    cd pico-sdk && \
    git submodule update --init

ENV PICO_SDK_PATH=/opt/pico-sdk

USER vscode
WORKDIR /workspaces

CMD ["/bin/bash"]