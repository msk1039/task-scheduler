#include "scheduler/scheduler.hpp"

#include <iostream>

int main() {
    using namespace std::chrono_literals;

    scheduler::Scheduler s;

    auto t1 = s.add([]() { std::cout << "Hello from 3 sec!\n"; }, 3000ms);
    std::cout << "task added. token - " << t1.value() << '\n';

    auto t2 = s.add([]() { std::cout << "Hello from 6 sec!\n"; }, 6000ms);
    std::cout << "task added. token - " << t2.value() << '\n';

    auto t3 = s.add([]() { std::cout << "Hello from 4 sec!\n"; }, 4000ms);
    std::cout << "task added. token - " << t3.value() << '\n';

    // cancel t2 before it fires:
    t2.cancel();

    s.run();
    return 0;
}
