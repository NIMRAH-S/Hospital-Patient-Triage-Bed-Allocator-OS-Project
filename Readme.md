# Hospital Patient Triage & Bed Allocator

A Linux-based Operating Systems semester project developed in C using POSIX APIs and system programming concepts. The project simulates a hospital emergency management system where patients are prioritized, scheduled, allocated beds, treated concurrently, and discharged using synchronization and IPC mechanisms.

---

# Features

- Multi-threaded hospital simulation
- Patient triage system
- ICU / Isolation / General bed allocation
- Priority-based scheduling
- FCFS scheduling simulation
- POSIX Threads
- Mutexes & Condition Variables
- Semaphores
- Shared Memory
- Named FIFO Communication
- Process creation using `fork()` and `execv()`
- Fragmentation analysis
- Best-Fit memory allocation
- Stress testing support
- Logging system

---

# Operating System Concepts Used

## Process Management
- `fork()`
- `execv()`
- `waitpid()`
- `SIGCHLD`

## Inter Process Communication (IPC)
- Anonymous Pipes
- Named FIFOs
- Shared Memory

## Synchronization
- POSIX Threads
- Mutex Locks
- Condition Variables
- Semaphores

## Scheduling
- FCFS Scheduling
- Priority Scheduling

## Memory Management
- Best-Fit Allocation
- Fragmentation Analysis
- Paging Simulation

---

# Project Structure

```text
OSProject/
│
├── src/
│   ├── admissions.c
│   ├── patient_simulator.c
│   ├── scheduler.c
│   └── common.h
│
├── scripts/
│   ├── triage.sh
│   ├── start_hospital.sh
│   ├── stop_hospital.sh
│   └── stress_test.sh
│
├── logs/
│   ├── memory_log.txt
│   └── schedule_log.txt
│
├── Makefile
├── README.md
└── report.pdf
```

---

# Requirements

- Ubuntu / Linux / WSL
- GCC Compiler
- POSIX Thread Library
- Make Utility
- Valgrind (optional)

---

# Installation

## Clone Repository

```bash
git clone <repository-link>
cd OSProject
```

---

# Build Project

```bash
make all
```

---

# Run Project

```bash
make run
```

---

# Add Patients

Open another terminal:

```bash
cd ~/OSProject
```

Example:

```bash
./scripts/triage.sh Ali 21 9
./scripts/triage.sh Sara 22 5
./scripts/triage.sh Ahmed 45 8
```

---

# Stress Testing

```bash
make test
```

---

# Stop System

```bash
bash scripts/stop_hospital.sh
```

---

# Logs

## Memory Log

```bash
cat logs/memory_log.txt
```

## Scheduling Log

```bash
cat logs/schedule_log.txt
```

---

# Valgrind Analysis

```bash
valgrind --leak-check=full ./admissions
```

---

# Sample Output

```text
Receptionist received Patient 1234
Patient 1234 allocated Bed 0 (ICU)
Patient 1234 treatment started for 5 seconds
```

---

# Team Members

- Nimrah Shahid
- Saweba Gill
- Saman Mahwash

---

# Learning Outcomes

This project improved understanding of:

- Linux system programming
- Concurrent programming
- Process synchronization
- Memory management techniques
- Scheduling algorithms
- IPC mechanisms
- Multi-threaded application development

---

# License

This project is developed for educational purposes as part of an Operating Systems Lab semester project.
