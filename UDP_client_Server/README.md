# Question 1: TCP Echo Server – netstat Analysis

## Environment
- Operating System: Ubuntu Linux (WSL2)
- Compiler: gcc
- Networking Tools: netstat

---

## Experiment Description

A TCP echo server and client were implemented using Berkeley sockets.  
The server uses `fork()` to create a child process for each client connection.

Steps performed:
1. The TCP server was started in the background.
2. The TCP client was executed and connected to the server.
3. Before sending input from the client, the server **child process** handling the client was terminated using the `kill` command.
4. The `netstat -a` command was executed to observe TCP connection states.

---

## netstat Output

```bash
Active Internet connections (servers and established)
Proto Recv-Q Send-Q Local Address           Foreign Address         State
tcp        0      0 127.0.0.54:domain       0.0.0.0:*               LISTEN
tcp        0      0 0.0.0.0:5000            0.0.0.0:*               LISTEN
tcp        0      0 127.0.0.53:domain       0.0.0.0:*               LISTEN
tcp        0      0 10.255.255.254:domain   0.0.0.0:*               LISTEN
tcp        1      0 localhost:44604         localhost:5000          CLOSE_WAIT
tcp        0      0 localhost:5000          localhost:44604         FIN_WAIT2
```

---

## Justification of TCP States

### LISTEN
- Indicates that the parent TCP server process is actively listening for incoming connections.
- Killing the child process does not affect the listening socket.

### CLOSE_WAIT
- Observed on the client side.
- Occurs when the client receives a FIN segment from the server after the server child process is terminated.
- The client has acknowledged the FIN but has not yet closed its socket.

### FIN_WAIT2
- Observed on the server side.
- Indicates that the server has sent a FIN and received an acknowledgment.
- The server is waiting for the client to complete connection termination.

---

## Conclusion

The observed TCP states demonstrate TCP’s reliable and graceful connection termination mechanism.  
Killing the server child process causes the connection to transition through appropriate TCP states while the server parent continues to listen for new connections.


## Question 2: UDP Echo Server with Vector Clocks

### (a) Modified UDP Echo Server and Client
- A UDP echo server and client (from Lecture 9) were modified.
- The client sends **10 different messages** to the server.
- The server:
  - Receives each message
  - Prints the received message
  - Sends an echo reply back to the client
- The echo operation is performed **10 times**.
- Client and server were executed in **different terminals**.

---

### (b) UDP Echo with Vector Clocks
- Both the UDP client and server maintain a **vector clock of size 2**.
  - Process 0: Client
  - Process 1: Server
- The vector clock is updated:
  - Before sending a message
  - After receiving a message
- Upon receiving a message, vector clocks are merged using the maximum value rule.
- Each message is printed along with the current vector clock value.

---

### Output
- The execution output of both the UDP server and client is captured.
- The output snapshot is provided as `image.png` in the submitted ZIP file.
- The output confirms correct message exchange and causal ordering using vector clocks.
---

![output](image.png)
---


## Question 3: Endianness Experiment

```bash
0 14
14 0
```

### Comment on Endianness

The output shows that the least significant byte is stored first in memory, indicating that the processor uses little-endian byte ordering.
The htons() function converts the value from host byte order to network byte order (big-endian).