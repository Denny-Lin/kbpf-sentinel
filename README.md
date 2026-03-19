# kbpf-sentinel

A kernel-level eBPF enforcement module that acts as a sentinel, enforcing safe and controlled XDP program attachment.

---

## Overview

kbpf-sentinel introduces a kernel-level enforcement layer for eBPF, ensuring that only permitted programs can be attached to network interfaces.

Unlike traditional eBPF workflows that rely solely on user-space control, this project enforces constraints directly in the kernel, reducing trust on user-space and improving system security.

### Motivation

While Linux provides safety guarantees through the BPF verifier and access control via capabilities and LSM, it lacks a centralized mechanism to enforce fine-grained policies on where and how eBPF programs are attached.

kbpf-sentinel fills this gap by introducing a kernel-level enforcement layer for eBPF attachment behavior.

---

## Features (v1)

- Restrict eBPF program type to XDP
- Limit attachment to specific interfaces (e.g., eth0)
- Kernel-level validation of attach requests
- Basic audit logging via printk

---

## Architecture

kbpf-sentinel introduces a kernel-level enforcement layer between user-space and eBPF execution.

<img width="851" height="331" alt="architecture" src="https://github.com/user-attachments/assets/cf40db5f-2161-464d-b2c2-aab5affdc891" />

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
- **Cilium / eBPF tools** control network behavior (what to do)

kbpf-sentinel focuses on:

> **How eBPF programs are allowed to be used**

It enforces attachment-level policies directly in the kernel, which is not centrally handled by existing mechanisms.

---

## Security Design

kbpf-sentinel follows a defense-in-depth model:

- User-space defines policy (flexible, dynamic)
- Kernel enforces non-bypassable constraints (strict, minimal)

### Key Principle

> The kernel does not decide what to do,  
> it enforces what must not be violated.

---

## Future Work: Advanced Security Model

The system is designed to support stronger security guarantees:

### 1. Identity Integrity (Digital Signature)

- eBPF programs can be cryptographically signed
- Kernel verifies signatures using a trusted public key
- Prevents loading tampered or malicious bytecode

### 2. Context Binding (Hardware Identity)

- Policies bind programs to specific interfaces (e.g., MAC / PCI ID)
- Prevents misuse of legitimate tools on sensitive interfaces

### 3. Threat Protection

- Mitigates "living-off-the-land" attacks
- Limits damage even if user-space is compromised

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

---

## License

GPL-2.0
