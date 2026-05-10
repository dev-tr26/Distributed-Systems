
## Que 1: UDP Client--Server with Vector Clock

### Experiment

-   Server is started first
-   Three UDP clients are run in separate terminals
-   Each client sends 5 msg
-   Server processes 15 msg and exits.

------------------------------------------------------------------------

### Output

-   The server output shows messages received from different clients
    along with updated vector clocks.
-   Each client output shows sent messages, echo replies from server , and
    updated vector clock values.

------------------------------------------------------------------------

### Justification

Every time a message is sent or received, the vector clock is updated.

The server combines (merges) the received vector clock with its own and then increments its entry.

This ensures causal ordering (events are ordered correctly based on cause and effect).

Since multiple clients run at the same time, their messages may interleave, showing how vector clocks maintain correct event ordering in distributed systems.





------------------------------------------------------------------------

# Question 2: TCP Server -- Zombie Process Remove


## Modification to Server Code

The server was modified to handle the SIGCHLD signal by ignoring it:
signal(SIGCHLD, SIG_IGN); and instructs the os to automatically remove
child processes when they terminate, thereby preventing zombie
processes after handling multiple client connections.

No change in client code 
after client and server code execution the process table was seen using the command:

ps -el | grep Z
ps -ef | grep defunct


------------------------------------------------------------------------

## Output of ps Commands

rudra@rudra-HP-Laptop-15s-fr4xxx:~/DS_LAB4$ ps -el | grep Z
ps -ef | grep defunct
F S   UID     PID    PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD
rudra       4561    4539  0 02:35 pts/1    00:00:00 grep --color=auto defunct

-   No processes were found in the zombie state.
-   This confirms that child processes were successfully cleaned.

---------------------------------------------------------------------






## Question 3: TCP shutdown() instead of close()

Modification : shutdown(SHUT_WR) instead of exit() or close().

### Explanation

-   close() closes the socket for both reading (receiving) and writing (sending) data completely 
-   shutdown(SHUT_WR) closes only the write side 
-   client can receive data after sending FIN but annot send data
-   it is called half-closed connection
-   internally client sends FIN to server , it enters FIN_WAIT state server sends its FIN when it finishes and client enters TIME_WAIT

## When needed

-   When a client finishes sending data but still expects a response
-   Used in protocols like HTTP where client sends req then wait for server's response , it should not close the connection completely before receiving the reply 

