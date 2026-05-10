# Scheduler

> A execution queue that stores and runs lambda functions, with cancellation and scheduling.

- A `Scheduler` class that accepts `std::function<void()>` tasks and inserts them in a priority_queue.
- Tasks are added with a delay using `std::chrono`
- Each task returns a `CancellationToken` that lets callers cancel before execution
- Tasks executed when their time expires


---

## Usage

```
#include "scheduler/scheduler.hpp"

int main() {
    using namespace std::chrono_literals;

    scheduler::Scheduler s;


    auto task1 = s.add([]() { std::cout << "runs after 2s\n"; }, 2000ms);
    auto task2 = s.add([]() { std::cout << "runs after 5s\n"; }, 5000ms);

    // add returns a CancellationToken object which can be used to cancel a task execution

    task2.cancel(); // cancel before it fires

    s.run();     // blocks until all tasks are done
}
```

---
## Design Notes


- **`std::priority_queue`** with `std::greater<Task>` gives a min-heap ordered by scheduled time the earliest task is always at the top.
operator overloading was used so that priority_queue can compare the task with Task1>Task2 while rebalancing the heap upon new task inserts.
```
bool operator>(const Task& other) const {
        return scheduled_time > other.scheduled_time;
    }
```
- Instead of polling every ms for new task, the thread is blocked until the schedule time of next task in the queue
```std::this_thread::sleep_until(queue.top().getScheduledTime());```
- **Exception safety**: each task's execution is wrapped in `try/catch` so one throwing lambda cannot crash the entire program. 
- **`std::shared_ptr<std::atomic<bool>>`** keeps track of a shared cancellation flag so both the `CancellationToken` (held by the caller) and the `Task` (inside the queue) share ownership of the same flag. Calling `cancel()` on the caller held `CancellationToken` updated the `canceled_flag` of the original `Task` object stored in the priority_queue
---

## Public API

**`scheduler::Scheduler`**

| Method | Description |
|---|---|
| `add(job, delay)` | Schedule a `std::function<void()>` to run after `delay` ms. Returns a `CancellationToken`. |
| `run()` | Block and execute all tasks in scheduled order. |

**`scheduler::CancellationToken`**

| Method | Description |
|---|---|
| `cancel()` | Cancel the task before it executes. |
| `isCanceled()` | Check if the task has been cancelled. |
| `value()` | Returns the unique task ID. |

---

## Build & Run

\`\`\`bash
cmake -S . -B build
cmake --build build --parallel
./build/demo
\`\`\`

Requires: CMake 3.20+, a C++20 compiler 

---



## What I Learned

- Lambdas, std::function, std::chrono, cancellation tokens

---

