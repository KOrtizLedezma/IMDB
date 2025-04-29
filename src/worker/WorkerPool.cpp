#include "WorkerPool.hpp"

WorkerPool::WorkerPool(size_t numThreads) : stop(false) {
  for (size_t i = 0; i < numThreads; i++) {
    workers.emplace_back(std::thread(&WorkerPool::workerLoop, this));
  }
}

WorkerPool::~WorkerPool() {
  stop = true;
  condition.notify_all();
  for (auto& thread : workers) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

void WorkerPool::enqueue(std::function<void()> task) {
  {
    std::unique_lock<std::mutex> lock(queueMutex);
    tasks.push(std::move(task));
  }
  condition.notify_one();
}

void WorkerPool::workerLoop() {
  while (!stop) {
    std::function<void()> task;

    {
      std::unique_lock<std::mutex> lock(queueMutex);
      condition.wait(lock, [this] {return stop || !tasks.empty();});

      if (stop && tasks.empty()) {
        return;
      }

      task = std::move(tasks.front());
      tasks.pop();
    }

    task();
  }
}