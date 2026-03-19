# kbpf-sentinel

A kernel-level eBPF enforcement module that acts as a sentinel, enforcing safe and controlled XDP program attachment.

## Overview

kbpf-sentinel introduces a kernel-level enforcement layer for eBPF, ensuring that only permitted programs can be attached to network interfaces.

Unlike traditional eBPF workflows that rely solely on user-space control, this project enforces constraints directly in the kernel, reducing trust on user-space and improving system security.

## Features (v1)

- Restrict eBPF program type to XDP
- Limit attachment to specific interfaces (e.g., eth0)
- Kernel-level validation of attach requests
- Basic audit logging via printk

## Architecture

User-space:
- Loads eBPF programs (libbpf)
- Applies policy

Kernel (kbpf-sentinel):
- Enforces attach constraints
- Acts as final gatekeeper

## Future Work

- Support for TC / tracing hooks
- Whitelist-based program validation
- Netlink-based control interface
- Rate limiting & lifecycle control
