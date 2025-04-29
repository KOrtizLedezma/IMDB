# IMDB

## Stage 1: Core Functionality

- DataStore
  - Build the core key-value store. 
  - Support basic operations: `SET`, `GET`, `DEL`, `EXISTS`.
  - Store data in `std::unordered_map<std::string, Value>.`
  - Plan for thread-safety later (start single-threaded).
- Command Parser
  - Read plain-text commands from clients.
  - Parse into structured commands (`Command` class).
  - Validate correct command formats.
- Single-Client TCP Server
  - Implement a basic TCP server that:
   - Listens on a port.
   - Accepts one client at a time.
   - Reads and writes simple text messages.
  - Pass incoming commands to Command Parser and DataStore.
  
## Stage 2: Robustness and Persistence

- Persistence Manager
  - Implement periodic snapshot saves of the DataStore to a disk file.
  - Load the saved file on server startup.
  - Keep format simple: e.g., one line per key-value pair.
- Logger
  - Create a logger to write events like:
   - New client connections.
   - Commands received and executed.
   - Errors and warnings.
- Thread-Safe DataStore
  - Add `std::mutex` locks to DataStore methods.
  - Prevent race conditions between threads during access/modifications.
  
## Stage 3: Scalability and Multi-Client Support

- Worker Thread Pool
  - Set up a pool of worker threads.
  - Each client is assigned to a worker thread.
  - Each worker reads commands, parses, and accesses the DataStore safely.
- Authentication Manager
  - Require a password upon client connection.
  - Deny commands if authentication fails.
- Timeouts and Connection Management
  - Detect idle clients and close connections after a timeout.
  - Prevent server from hanging on dead sockets.
  
## Stage 4: Advance Features

- Eviction Policy (LRU Cache)
  - Track access time for each key.
  - When memory usage is too high, automatically remove last recently used keys.
- Pub/Sub Monitoring Module
  - Allow clients to subscribe to database events (e.g., all `SET` commands).
  - Push notifications to subscribers.
- Cluster mode
  - Split key ranges across multiple server instances.
  - Clients redirect to the correct server based on key hash.

## Checklist
- [x] Build DataStore
- [x] Build Command Parser
- [x] Build Single-Client TCP Server
- [ ] Add Persistence
- [ ] Add Logger 
- [ ] Make DataStore Thread-Safe
- [ ] Add Worker Threads
- [ ] Add Authentication
- [ ] Add Connection Timeouts
- [ ] Add Eviction
- [ ] Add Pub/Sub Monitoring
- [ ] Add Clustering
  