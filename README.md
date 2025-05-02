# Tunnel Bus Simulation

This project simulates buses traveling through a one-lane tunnel, managing direction and access with mutexes and semaphores using POSIX threads in C.

## 🚀 Features

- Buses from City X and City Y take turns using a shared tunnel.
- Ensures mutual exclusion and fairness.
- Each bus performs multiple round trips.

## 🛠️ Technologies

- C language
- POSIX Threads (`pthread`)
- Semaphores (`semaphore.h`)

## 👥 Contributors

- **Zemouri Ali** — Project leader  
- **Chemlal Adem** —
- **Ouabas mehdi** —
- **Madani Abderrahmane Ilyes** — 

## 📂 How to Compile

```bash
gcc -pthread tunnel.c -o tunnel
./tunnel
