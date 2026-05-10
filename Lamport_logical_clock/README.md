# Lamport's Logical Clocks Simulator

## Overview
This program implements Lamport's logical clocks algorithm for synchronizing distributed systems. It simulates multiple concurrent processes that communicate via message passing while maintaining causal consistency.

## Compilation and Execution

### Compile the Program
```bash
g++ Q1.cpp -o 1
```

**Requirements:**
- C++11 or later
- Standard C++ compiler (g++, clang++, MSVC, etc.)

### Run the Program
```bash
./1
```

**Note:** The program reads input from a file named `input.txt` in the same directory.

## Input Format

Create an `input.txt` file with the following structure:

```
begin process <process_name>
<operation>
<operation>
...
end process

begin process <process_name>
<operation>
<operation>
...
end process
```

### Operations
- **send pN msg** - Send message `msg` to process `pN`
- **recv pN msg** - Receive message `msg` from process `pN` (blocks if not available)
- **print msg** - Print message `msg` to the terminal

### Example Input (Sample 1 - Normal Execution)
```
begin process p1
send p2 m1
print abc
print def
recv p2 m2
end process

begin process p2
print x1
recv p1 m1
print x2
send p1 m2
print x3
end process
```

### Example Input (Sample 2 - Deadlock Scenario)
```
begin process p1
recv p2 m1
send p2 m2
print p1_done
end process

begin process p2
recv p1 m2
send p1 m1
print p2_done
end process
```

## Output Format

The output shows all events that occurred during simulation, ordered by Lamport clock value:

- **sent pN msg pM (T)** - Process pN sent msg to pM at Lamport time T
- **received pN msg pM (T)** - Process pN received msg from pM at Lamport time T
- **printed pN msg (T)** - Process pN printed msg at Lamport time T
- **system deadlocked** - Detected when all processes are waiting for messages

## Sample Input and Output

### Input (input.txt)
```
begin process p1
send p2 m1
print abc
print def
recv p2 m2
end process

begin process p2
print x1
recv p1 m1
print x2
send p1 m2
print x3
end process
```

### Output
```
sent p1 m1 p2 (1)
printed p2 x1 (1)
printed p1 abc (2)
received p2 m1 p1 (2)
printed p1 def (3)
printed p2 x2 (3)
received p1 m2 p2 (4)
sent p2 m2 p1 (4)
printed p2 x3 (5)
```


## Files Included
- `Q1.cpp` - Main program source code
- `input.txt` - Sample input file
- `README.md` - This documentation file
