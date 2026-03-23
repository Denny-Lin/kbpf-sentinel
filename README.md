# kbpf-sentinel

A kernel-level eBPF enforcement module that acts as a sentinel, enforcing safe and controlled XDP program attachment.

---

## Overview

**kbpf-sentinel** introduces a kernel-level enforcement layer for eBPF, ensuring that only permitted programs can be attached to authorized network interfaces. 

Unlike traditional eBPF workflows that rely solely on user-space control, this project enforces security constraints directly within the kernel, reducing reliance on untrusted user-space tools and significantly hardening system security.

---

## Motivation

While Linux provides safety via the BPF verifier and access control via LSM/Capabilities, there is no dedicated, lightweight mechanism to enforce fine-grained policies on **where** and **how** eBPF programs are attached (e.g., interface-specific binding).

**kbpf-sentinel** fills this gap by acting as a specialized gatekeeper for eBPF attachment behavior.

---

## Features (v1)

- **Type Restriction**: Strictly limit eBPF programs to XDP.
- **Interface Binding**: Restrict attachment to authorized interfaces (e.g., `eth0`).
- **Kernel-Level Validation**: Intercept and validate attach requests within kernel-space.
- **Audit Logging**: Real-time logging of unauthorized attempts via `printk`.

---

## Architecture

kbpf-sentinel operates as a security layer between the user-space loader and the kernel's eBPF subsystem.

![architecture](https://github.com/user-attachments/assets/a5f0aef0-500a-40e2-b39f-b4c1bfbe6a61)

---

### Logic Flow

1. **User-space**: Requests eBPF attachment (via `libbpf` or `iproute2`).
2. **Sentinel (LKM)**: Intercepts the request and evaluates against the kernel policy.
3. **Decision**: 
   - **Allowed**: Program is attached to the XDP hook.
   - **Rejected**: Operation is blocked and logged.

---

## Development Workflow (Mac M5 Pro / ARM64)

This project utilizes a specialized cross-platform workflow to handle Linux Kernel development on Apple Silicon (ARM64).

### 1. Build Environment
- **Docker-based Toolchain**: Native ARM64 Ubuntu container for high-performance compilation.
- **I/O Optimization**: To avoid macOS **VirtioFS bottlenecks** during kernel extraction (preventing `Directory renamed` errors), the Linux Kernel source is stored and prepared in the container's **native overlay filesystem** (`/root/kernel-build/`).

### 2. Kernel Preparation
To resolve symbols like `_printk` and `strcmp`, we perform a targeted symbol table generation:
```bash
# Inside Docker
cd /root/kernel-build/kernel-src
make defconfig
make modules_prepare
make vmlinux   # Generate vmlinux.symvers for symbol resolution
cp vmlinux.symvers Module.symvers
```

## Testing and Verification

Validation is performed using a minimalist **BusyBox-based rootfs** inside **QEMU (ARM64)**.

### Execution in QEMU:

1. **Load the Sentinel LKM**:
```bash
insmod /mnt/kmod/sentinel_main.ko
# Log: KBPF-Sentinel: Sentinel LKM engine initialized. Enforcement active.
```
   
2. **Verify Authorized Interface (eth0)**:
```bash
ip link set eth0 up
# Log: KBPF-Sentinel: [PASS] Authorized interface eth0 detected.
```
3. **Verify Unauthorized Interface (lo)**:
```bash
ip link set lo up
# Log: KBPF-Sentinel: [BLOCK] Unauthorized interface lo detected. Enforcing policy.
```

### Live Demo (Kernel Logs)

> [!IMPORTANT]
> The image below demonstrates the **kbpf-sentinel** successfully identifying and blocking an unauthorized `lo` interface activation while permitting `eth0`.

<img width="689" height="170" alt="Screenshot 2026-03-22 at 7 45 27 PM" src="https://github.com/user-attachments/assets/97e952ed-716f-402e-8c0e-40fca8ab6cdc" />

---

## High-Security Design (Planned)

- **Policy Integrity**: Cryptographic signature validation for security policies.
- **Hardware Identity Binding**: Binding programs to immutable identifiers like MAC or PCI IDs.
- **LSM Integration**: Transitioning from event-notifiers to proactive LSM hooks for pre-attach blocking.

## Roadmap: Towards v2 (Enforcement & Dynamic Policy)

The next evolution (**v2**) will transition **kbpf-sentinel** from passive auditing to **proactive, software-defined enforcement**.

### v2 Architecture: Dynamic Policy Orchestration

> [!TIP]
> In **v2**, we introduce a **sysfs-based orchestration layer**. This allows administrators to push security policies into the kernel without recompiling the LKM, creating a **Software-Defined Security Gatekeeper**.

<img width="1536" height="1024" alt="v2 architecture" src="https://github.com/user-attachments/assets/3ae5eef5-f956-44f8-9905-cf0fe1d94cba" />

### Key v2 Enhancements:

1. **Proactive Blocking (LSM Hooks)**:
   - **Target Hook**: `security_bpf_prog_attach`
   - **Mechanism**: Move the interception point from post-event notifiers to proactive LSM hooks.
   - **Result**: Unauthorized `ip link` commands will be **immediately rejected** with `Operation not permitted`.

2. **Dynamic Policy Loading (sysfs Interface)**:
   - **Interface**: `/sys/kernel/sentinel/policy`
   - **Mechanism**: Implements `kobject` attributes with `store()` and `show()` handlers, allowing tools to `echo` new whitelists into the running kernel.

3. **Cryptographic Signature Verification**:
   - **Security**: The sentinel will verify a **digital signature** attached to any new policy before applying it in kernel-space, ensuring policy integrity.

4. **Observability**:
   - Integrated support for real-time **violation counters** and status monitoring via the sysfs interface.

---

## Design Philosophy

- **Minimal Footprint**: Lightweight LKM for minimal performance overhead.
- **Explicit Trust Boundary**: Never trust user-space inputs.
- **Fail-Fast**: Reject unauthorized requests at the earliest possible stage.
