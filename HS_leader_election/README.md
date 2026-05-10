# Hirschberg–Sinclair Leader Election (Sockets Implementation)

## Overview

This project implements the **Hirschberg–Sinclair (HS) leader election algorithm** using the **POSIX sockets API** in C.

Each node in the system acts as both:

* A **server** (to receive messages)
* A **client** (to send messages)

The algorithm is executed on a **ring topology** of size `n = 4`. All nodes communicate with their left and right neighbors to elect a leader.

---

## Compilation

```bash
gcc -o hs HS_leader_election.c -lpthread -lm
```

---

## Execution

```bash
./hs <total_nodes> <my_id> <left_id> <right_id>
```

Example for 4 nodes in a ring (IDs 1, 2, 3, 4):

```bash
./hs 4 1 4 2 
```

---

## Q1 a) Source File

The source code is attached as:
**`HS_leader_election.c`**

---

## Q1 b) Output

The output for execution on 4 nodes is provided in:
**`Output.pdf`**

Additionally, for practical verification, the outputs for all possible ring executions with node 4 as the highest ID are attached as:

* **Ring 1 (1-2-3-4) output:** `output_ring1.png`
* **Ring 2 (1-2-4-3) output:** `output_ring2.png`
* **Ring 3 (1-3-2-4) output:** `output_ring3.png`

---

## Q1 c) Justification for Correctness

### 1. Leader Election

* Nodes 1, 2, and 3 receive probes from node 4 and become **lost** (since 4 > 1, 2, 3).
* Node 4 never receives a higher ID probe, so it remains a **candidate** and correctly becomes the **leader**.

### 2. Phase-wise Expansion

* Probe distance doubles in each phase (`2^phase`).
* Node 4 successfully completes all phases, confirming dominance.

### 3. Probe & Echo

* Every **SENDPASS** from node 4 results in a corresponding **SENDECHO**, verifying correct message traversal.

### 4. Leader Messages

* All messages originating from node 4 are traced in the output to ensure proper leader influence.

### 5. Leader Propagation & Termination

* The **LEADER 4** message circulates the ring; all nodes acknowledge the leader.
* Node 4 reaches **won**, and others **lost**, confirming proper termination.

---

## Q2 Message Calculation for 4 Nodes

### Ring 1 (1-2-3-4)

* **SENDPASS:** 12
* **SENDECHO:** 8
* **LEADER:** 8 

**Total:** 28 messages

### Ring 2 (1-2-4-3)

* **SENDPASS:** 12
* **SENDECHO:** 8
* **LEADER:** 7

**Total:** 27 messages

### Ring 3 (1-3-2-4)

* **SENDPASS:** 16
* **SENDECHO:** 8
* **LEADER:** 8

**Total:** 32 messages

### Average Messages

[
\text{Average} = \frac{28 + 27 + 32}{3} = 28
]

> For `n = 4`, approximately **29 messages** are required to elect the leader.

---

## Leader Election Result

In all ring executions:

**Leader = Node 4** (highest ID)

---

## Attachments

* `HS_leader_election.c` – Source code
* `Output.pdf` – Sample output for 4-node ring
* `output_ring1.png` – Execution output for Ring 1 (1-2-3-4)
* `output_ring2.png` – Execution output for Ring 2 (1-2-4-3)
* `output_ring3.png` – Execution output for Ring 3 (1-3-2-4)
