# kbpf-sentinel

A kernel-level eBPF enforcement module that acts as a sentinel, enforcing safe and controlled XDP program attachment.

---

## Overview

kbpf-sentinel introduces a kernel-level enforcement layer for eBPF, ensuring that only permitted programs can be attached to network interfaces.

Unlike traditional eBPF workflows that rely solely on user-space control, this project enforces constraints directly in the kernel, reducing trust on user-space and improving system security.

---

## Motivation

Linux provides safety guarantees through the BPF verifier and access control via capabilities and LSM.

However, there is no dedicated and commonly-used mechanism to enforce fine-grained policies specifically on how eBPF programs are attached (e.g., interface-level constraints).

kbpf-sentinel introduces a focused kernel-level enforcement layer for controlling eBPF attachment behavior.

---

## Features (v1)

- Restrict eBPF program type to XDP
- Limit attachment to specific interfaces (e.g., eth0)
- Kernel-level validation of attach requests
- Basic audit logging via printk

---

## Architecture

kbpf-sentinel introduces a kernel-level enforcement layer between user-space and eBPF execution.

<img width="851" height="331" alt="architecture" src="https://github.com/user-attachments/assets/a5f0aef0-500a-40e2-b39f-b4c1bfbe6a61" />

### Flow

1. User-space loads eBPF programs (libbpf / iproute2)
2. An attach request is issued to the kernel
3. kbpf-sentinel intercepts the attach request and enforces constraints (program type, interface, policy)
4. If allowed, the eBPF program is attached (e.g., XDP)
5. Otherwise, the request is rejected and logged

> kbpf-sentinel acts as a kernel-level gatekeeper, enforcing attach policies before execution.

---

## Control vs Meta-Control

```
User-space (Control Plane)
  ↓
kbpf-sentinel (Meta-Control / Enforcement)
  ↓
eBPF Program (Data Plane)
```

- Traditional systems control behavior  
- kbpf-sentinel controls how that behavior is allowed to be installed  

---

## Why Not LSM / Cilium?

Linux already provides several mechanisms for security and control, but they operate at different layers.

- **LSM (SELinux/AppArmor)** focuses on process-level permissions (who can act)  
  While LSM can restrict access to the `bpf()` syscall, it does not provide fine-grained control over where eBPF programs are attached.

- **Cilium / eBPF tools** control network behavior (what to do)

kbpf-sentinel focuses on:

> **How eBPF programs are allowed to be used**

It enforces attachment-level policies directly in the kernel, which are not commonly enforced in a centralized or dedicated manner by existing mechanisms.

---

## Security Design

kbpf-sentinel follows a defense-in-depth model:

- User-space defines policy (flexible, dynamic)  
- Kernel enforces non-bypassable constraints (strict, minimal)  

### Key Principle

> The kernel does not decide what to do,  
> it enforces what must not be violated.

---

## High-Security Design (Planned)

For high-security environments, kbpf-sentinel can be extended with a multi-layer protection model:

### 1. Policy Integrity (Signature / Hash Validation)

- Policy files can be signed or hashed
- Kernel verifies integrity before accepting policy updates
- Prevents unauthorized or tampered policy injection

### 2. Context-Aware Enforcement (Core Feature)

- Programs are bound to specific interfaces (e.g., MAC / PCI ID)
- Prevents misuse of legitimate eBPF tools on unintended interfaces

### 3. Runtime Enforcement (Primary Layer)

- Every attach request is validated in kernel space
- Ensures constraints are enforced even if user-space is compromised

> Signature protects **what is loaded**,  
> kbpf-sentinel enforces **how it is used**.

---

## Example

```bash
# Allowed
ip link set dev eth0 xdp obj prog.o

# Rejected by kbpf-sentinel
ip link set dev lo xdp obj prog.o
```

Kernel log:

```
[KBPF] Reject: interface lo not allowed
```

---

## Design Philosophy

- Minimal kernel footprint  
- Explicit trust boundary  
- Policy in user-space, enforcement in kernel  
- Security over convenience  
- Incremental security (enforcement first, integrity later)

---

## License

GPL-2.0
