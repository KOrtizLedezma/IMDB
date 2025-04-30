# IMDB

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)

A lightweight, thread-safe in-memory key-value store written in C++ with:

- Role-based authentication (admin/user)
- Persistence with optional eviction (LRU-based)
- Worker thread pool for concurrent clients
- Pub/Sub notifications for `DEL` and `DELALL`
- Optimized with STL and no external dependencies

---

## Features

- `SET <key> <value>` — store a key-value pair
- `GET <key>` — retrieve a value
- `DEL <key>` — delete a key
- `EXISTS <key>` — check if key exists
- `GETALL` — (admin only) list all stored keys
- `DELALL` — (admin only) delete all keys
- `CREATEUSER <username> <password> <USER|ADMIN>` — (admin only)
- `PROMOTE <username>` — (admin only)
- `SUBSCRIBE DEL|DELALL` — listen for deletions
- `UNSUBSCRIBE` — cancel all subscriptions
- `EXIT` — disconnect from server

---

## Build Instructions

### Dependencies
- C++20
- POSIX-compatible system (Linux/macOS)

### Build with `make`
```bash
  make
  ./inmemorydb
```
---

## Persistence

On launch, the server loads from `database.txt`. Data is saved after each `SET` and `DEL`

---

## LRU Cache

Evicts the last recent user keys automatically when memory usage exceeds the limit (default : 10MB).
To customize:
Edit `Datastore store(10 * 1024 * 1024);` in `main.cpp`. 

---

## Multithreading
All client connections are handled by a worker thread pool. DataStore operations are protected by mutexes.

---

## Pub/Sub

Clients can subscribe to `DEL` or `DELALL` events:

```bash 
  SUBSCRIBE DEL
  DEL secret
  # -> [EVENT:DEL] user deleted secret
```

---

## Disclaimer

This project is intended for educational purposes and **does not guarantee durability or security** in production environments.
