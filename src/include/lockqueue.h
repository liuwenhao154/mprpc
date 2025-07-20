#pragma once
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>

// async lock queue header file
template<typename T>
class LockQueue {
public:
    void Push(const T& item)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(item);
        m_cond.notify_one(); // Notify one waiting thread
    }
    T Pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_queue.empty() && !m_exit) {
            m_cond.wait(lock, [this] { return !m_queue.empty() || m_exit; });
        }
        if(m_exit) return T(); // Return default value if exit is requested
        T item = m_queue.front();
        m_queue.pop();
        return item;
    }

    void Exit() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_exit = true;
        m_cond.notify_all(); // Notify all waiting threads to exit
    }

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_exit = false; // Flag to indicate exit condition
};
