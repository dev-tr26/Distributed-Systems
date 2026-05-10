# Vector Clock
## proggraming language - C++

This program simulates message passing between multiple processes using Vector clock :

- `send`, `recv`, and `print` operations  
- Deadlock detection  
- Reporting undelivered messages  

---

## Compilation & Execution Instructions

### Requirements

- C++ compiler supporting (e.g., `g++`)
- Command-prompt for execution  

### Compilation

g++ vector_clock.cpp

Make a `input.txt` and paste a test case in that.
### Exexution command

a.exe < input.txt


# Sample test cases to test the program.

### case 1 ---> given in pdf

```bash
begin process p1 
send p2 m1 
print abc 
print def 
end process 

begin process p2 
print x1 
recv p1 m1 
print x2 
send p1 m2 
print x3 
end process 
```

### case 2 ---> normal
```bash
begin process p1
send p2 message1
print P1
recv p2 message2
print P1_done
end process

begin process p2
recv p1 message1
print P2
send p1 message2
print P2_done
end process
```

### case 3 ---> deadlock found
```bash
begin process p1
recv p2 message1
print P1
end process

begin process p2
recv p1 message2
print P2
end process
```


# These outputs are from the test cases that are Given above

### test case 1 output 
```bash
sent p1 m1 p2 (1)
printed p2 x1 (1)
printed p1 abc (2)
received p2 m1 p1 (3)
printed p1 def (3)
printed p2 x2 (4)
sent p2 m2 p1 (5)
printed p2 x3 (6)
message m2 (p2 -> p1) not delivered
```



###  test case 2 output
```bash
sent p1 message1 p2 (1)
received p2 message1 p1 (2)
printed p1 P1 (2)
printed p2 P2 (3)
sent p2 message2 p1 (4)
received p1 message2 p2 (5)
printed p2 P2_done (5)
printed p1 P1_done (6)
```


### test case 3 output
```bash
system deadlocked
```




## Socket Program Output and Justification

This program demonstrates how file descriptors are assigned in a Unix/Linux system.

### Explanation

In Ubuntu os , every input/output resource is represented using a **file descriptor**.

By default:

* `0` is reserved for standard input (stdin)
* `1` is reserved for standard output (stdout)
* `2` is reserved for standard error (stderr)

The kernel always assigns the **lowest available file descriptor** to a newly created file or socket.

---

### Step-by-step Justification

1. **Socket creation**

   ```c
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   ```

   * This creates a TCP socket.
   * Since file descriptors 0, 1, and 2 are reserved for standard input, output, and error, the kernel assigns the lowest available file descriptor to new resources. In a typical execution environment, the socket receives file descriptor 3, and the file opened afterward receives file descriptor 4


2. **File opening**

   ```c
   fd = open("data", O_RDONLY);
   ```

   * This opens the file named `data` in read-only mode.
   * The next available file descriptor after `3` is `4`.
   * Hence, the second `printf` prints:

     ```
     4
     ```

3. **Infinite loop**

   ```c
   for (;;);
   ```

   * This creates an infinite loop.
   * The program does not terminate and keeps running after printing the file descriptor values.

---

### Final Output

```
3
4
```