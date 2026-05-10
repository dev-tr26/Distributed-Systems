# LAB 5

---

## **Q1: NTP Client Implementation**

### (a) Overview

A Network Time Protocol (NTP) client program was implemented in **C++** to obtain the current date and time from an NTP server. The client:

* Creates a **UDP socket**.
* Sends a **48-byte request packet** to the NTP server.
* Receives a response containing timestamp information.
* Extracts the **transmit timestamp**.
* Converts from **NTP epoch (1900)** to **Unix epoch (1970)**.
* Prints the following:

  * Time received from the NTP server in **UTC**.
  * Time converted to **Indian Standard Time (IST)**.
  * The **local machine time**.

**File name:** `ntp_client.cpp`

**Compilation Command:**

```bash
g++ ntp_client.cpp -o ntp_client
```

**Execution Commands:**

```bash
./ntp_client time.nplindia.org
./ntp_client samay1.nic.in
./ntp_client samay2.nic.in
```

---

### (b) NTP Servers in India

The NTP client was tested with the following Indian NTP servers:

1. `time.nplindia.org`
2. `samay1.nic.in`
3. `samay2.nic.in`

These servers provide accurate time using the **Network Time Protocol (NTP)**.

---

### (c) Output of the NTP Client

The terminal output shows:

* UTC time from the NTP server.
* IST time calculated from UTC.
* Local machine time.

**Attached File:** `q1c.png` (screenshot of program execution).

---

### (d) Structure of the NTP Request Packet

* NTP uses a **48-byte packet format**.
* The first byte contains **Leap Indicator (LI), Version Number (VN), and Mode**.
* Other fields include:

  * Stratum
  * Poll Interval
  * Precision
  * Root Delay
  * Root Dispersion
  * Reference ID
  * Timestamps (Reference, Originate, Receive, Transmit)

**Attached File:** `ntp_req_packet.png` (diagram of 48-byte packet structure).

---

## **Q2: MPI Send/Receive Output**

### Initial Observation

Output:

```text
Process 1 received 100
Process 0 sent 100
```

**Reason:**

* `MPI_Send` is blocking, but it only blocks until the **message buffer is safe**, not until the other process prints.
* `printf` output from different processes is **not synchronized**; it can appear in any order.
* Sending and receiving happen at the MPI runtime level, but **stdout is buffered per process**.

---

# MPI Send/Receive Output with Sleep

This section explains the effect of adding `sleep()` before `MPI_Recv` in Process 1.

---

### Version 1: `sleep(10)` before `MPI_Recv`

```text
Process 0 sent 100
Process 1 received 100
```

**Explanation:**

* Process 1 waits 10 seconds before calling `MPI_Recv`.
* Process 0 sends immediately and prints first.
* The longer sleep ensures Process 0’s send message appears on the console **before** Process 1 prints.
* MPI still guarantees the message is safely delivered.

---

### Version 2: `sleep(5)` before `MPI_Recv`

```text
Process 0 sent 100
Process 1 received 100
```

**Explanation:**

* Process 1 waits only 5 seconds before receiving.
* Process 0 sends immediately and prints first.
* Output order remains the same, but the receive happens sooner than in the 10-second version.

---