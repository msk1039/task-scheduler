#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <thread>

namespace scheduler {


class CancellationToken {
    uint64_t id;
    std::shared_ptr<std::atomic<bool>> canceled_flag; // atomic shared flag betweent he caller and the scheduler

  public:
    CancellationToken(uint64_t id, std::shared_ptr<std::atomic<bool>> flag)
        : id(id), canceled_flag(std::move(flag)) {}

    uint64_t value() const { return id; }

    void cancel() { canceled_flag->store(true); }

    bool isCanceled() const { return canceled_flag->load(); }
};



class Task {
    uint64_t id;
    std::function<void()> job;
    std::chrono::steady_clock::time_point scheduled_time;
    std::shared_ptr<std::atomic<bool>> canceled_flag;

  public:
    Task(uint64_t id, std::function<void()> job,
         std::chrono::steady_clock::time_point scheduled_time,
         std::shared_ptr<std::atomic<bool>> flag)
        : id(id), job(std::move(job)), scheduled_time(scheduled_time),
          canceled_flag(std::move(flag)) {}

    // this lets the priority_queue can compare the task with Task1>Task2 while rebalancing the heap upon new task inserts.
    bool operator>(const Task& other) const {
        return scheduled_time > other.scheduled_time;
    }

    std::chrono::steady_clock::time_point getScheduledTime() const {
        return scheduled_time;
    }

    bool isCancelled() const { return canceled_flag->load(); }

    bool isExpired() const {
        return scheduled_time <= std::chrono::steady_clock::now();
    }

    void execute() const {
        try {
            job();
        } catch (const std::exception& e) {
            std::cerr << "[scheduler] task " << id << " threw: " << e.what() << '\n';
        } catch (...) {
            std::cerr << "[scheduler] task " << id << " threw unknown exception\n";
        }
    }
};


class Scheduler {
    std::priority_queue<Task, std::vector<Task>, std::greater<Task>> queue;
    std::atomic<uint64_t> next_id{1}; // start the id's from 1, increment and assign for each new task

  public:
    // Add a task to the queue to run after delay. Returns a token that can cancel it.
    CancellationToken add(std::function<void()> job, std::chrono::milliseconds delay) {

        uint64_t id = next_id.fetch_add(1);
        auto cancellationFlag = std::make_shared<std::atomic<bool>>(false);

        auto scheduled_time = std::chrono::steady_clock::now() + delay; // schedule time = current time + detay


        queue.push(Task(id, std::move(job), scheduled_time, cancellationFlag));
        return CancellationToken(id, cancellationFlag);
    }


    void run() {
        while (!queue.empty()) {

            // sleep until the execution time of the next task 
            std::this_thread::sleep_until(queue.top().getScheduledTime());

            while (!queue.empty() && queue.top().isExpired()) {
                Task task = queue.top();
                queue.pop();
                if (!task.isCancelled()) {
                    task.execute();
                }
            }
        }
    }
};

}
