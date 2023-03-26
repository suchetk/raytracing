#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <functional>
#include <thread>
#include <queue>
#include <iostream>

class threadPool {
public:
    threadPool() {}
    void start(int n);
    void queueJob(const std::function<void()>& job);
    void stop();
    bool busy();
    void threadLoop();

private:
    bool should_terminate = false;           // Tells threads to stop looking for jobs
    std::mutex queue_mutex;                  // Prevents data races to the job queue
    std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination 
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> jobs;
};

void foo() {
    int x = 5;
    std::cout << x << std::endl;
}

void threadPool::start(int n) {
    // const uint32_t num_threads = std::thread::hardware_concurrency(); // Max # of threads the system supports
    // std::cout << "Starting threadpool with " << num_threads << " threads." << std::endl;
    threads.resize(n);
    for (uint32_t i = 0; i < threads.size(); i++) {
        threads.at(i) = std::thread([this] { this->threadLoop(); });
    }
}

void threadPool::threadLoop() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            mutex_condition.wait(lock, [this] {
                return !jobs.empty() || should_terminate;
            });
            if (should_terminate) {
                return;
            }
            job = jobs.front();
            jobs.pop();
            // std::cout << "Dequeued Job" << std::endl;
        }
        job();
    }
}

void threadPool::queueJob(const std::function<void()>& job) {
    // std::cout << "queuing job" << std::endl;
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        jobs.push(job);
    }
    mutex_condition.notify_one();
}

bool threadPool::busy() {
    bool poolbusy;
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        poolbusy = jobs.empty();
    }
    return poolbusy;
}

void threadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        should_terminate = true;
    }
    mutex_condition.notify_all();
    for (std::thread& active_thread : threads) {
        active_thread.join();
    }
    threads.clear();
    std::cout << "Shut down threadpool" << std::endl;
}

#endif