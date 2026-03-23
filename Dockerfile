# Use ARM64 Ubuntu for native M5 Pro performance
FROM --platform=linux/arm64 ubuntu:22.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install essential packages for Linux Kernel and eBPF development
RUN apt-get update && apt-get install -y \
    build-essential \
    kmod \
    flex \
    bison \
    libelf-dev \
    libssl-dev \
    bc \
    clang \
    llvm \
    git \
    vim \
    wget \
    cpio \
    libncurses-dev \
    pahole

# Set the working directory inside the container
WORKDIR /work

# Default command to keep the container interactive
CMD ["/bin/bash"]
