#ifndef THREADING_UTILS_HPP
#define THREADING_UTILS_HPP

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>
#include <chrono> 
#include <optional>

class Event {
// Python threading.Event() equivalent class
public:
    Event() : flag(false) {}

    // Set the event
    void set() {
        std::lock_guard<std::mutex> lock(mtx);
        flag = true;
        cv.notify_all();
    }

    // Clear the event
    void clear() {
        std::lock_guard<std::mutex> lock(mtx);
        flag = false;
    }

    // Wait until the event is set
    void wait() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return flag; });
    }

    // Wait with timeout
    bool wait_for(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mtx);
        return cv.wait_for(lock, timeout, [this] { return flag; });
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    bool flag;
};


// Thread-safe queue 
template <typename T> 
class TSQueue { 
private: 
    // Underlying queue 
    std::queue<T> m_queue; 
  
    // mutex for thread synchronization 
    std::mutex m_mutex; 
  
    // Condition variable for signaling 
    std::condition_variable m_cond; 
  
public: 
    // Pushes an element to the queue 
    void push(T item) 
    { 
  
        // Acquire lock 
        std::unique_lock<std::mutex> lock(m_mutex); 
  
        // Add item 
        m_queue.push(item); 
  
        // Notify one thread that 
        // is waiting 
        m_cond.notify_one(); 
    } 
  
    // // Pops an element off the queue. Blocking wait until queue contains atleast one element
    // T pop() 
    // { 
  
    //     // acquire lock 
    //     std::unique_lock<std::mutex> lock(m_mutex); 
  
    //     // wait until queue is not empty 
    //     m_cond.wait(lock, 
    //                 [this]() { return !m_queue.empty(); }); 
  
    //     // retrieve item 
    //     T item = m_queue.front(); 
    //     m_queue.pop(); 
  
    //     // return item 
    //     return item; 
    // } 

    // Pops an element off the queue. Blocking wait until queue contains at least one element
    // Optional timeout duration can be passed. Returns std::nullopt if timeout occurs.
    std::optional<T> pop(std::chrono::milliseconds timeout = std::chrono::milliseconds::zero()) 
    {
        std::unique_lock<std::mutex> lock(m_mutex);  // Acquire lock
        
        // If timeout is set to zero, wait indefinitely
        if (timeout == std::chrono::milliseconds::zero()) {
            m_cond.wait(lock, [this]() { return !m_queue.empty(); });
        } else {
            // Wait for the queue to have elements or until timeout
            if (!m_cond.wait_for(lock, timeout, [this]() { return !m_queue.empty(); })) {
                return std::nullopt;  // Timeout occurred, return std::nullopt
            }
        }

        // Retrieve item from the queue
        T item = m_queue.front();
        m_queue.pop();

        return item;  // Return the popped item
    }

    // Clear all items from the queue
    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        while (!m_queue.empty()) {
            m_queue.pop();
        }
    }

    // Get the number of items in the queue
    size_t size() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }
}; 

class thread_pool
{
    // Task function
    using task_type = std::function<void()>;

public:
    explicit thread_pool(std::size_t thread_count = std::thread::hardware_concurrency())
    {
        for (std::size_t i{ 0 }; i < thread_count; ++i) {
            m_workers.emplace_back(std::bind(&thread_pool::thread_loop, this));
        }
    }

    ~thread_pool()
    {
        m_stop.store(true);
        m_notifier.notify_all();

        for (auto &thread : m_workers) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        m_workers.clear();
    }

    thread_pool(thread_pool const &) = delete;
    thread_pool(thread_pool &&) = default;

    thread_pool &operator=(thread_pool const &) = delete;
    thread_pool &operator=(thread_pool &&) = default;

    // Push a new task into the queue
    template <class Func, class... Args>
    auto push(Func &&fn, Args &&...args)
    {
        using return_type = typename std::result_of<Func(std::atomic<bool>&, Args...)>::type;

        // auto task = std::make_shared<std::packaged_task<return_type()>>(
        //     [this, fn = std::forward<Func>(fn), args...]() mutable {
        //         return std::invoke(std::forward<Func>(fn), m_stop, std::forward<Args>(args)...);
        //     }
        // );
         auto task = std::make_shared<std::packaged_task<return_type()>>(
        [this, fn = std::forward<Func>(fn), ...args = std::forward<Args>(args)]() mutable {
            // Directly call the function with m_stop and args
            return fn(m_stop, std::forward<Args>(args)...);
        }
    );

        auto future = task->get_future();
        std::unique_lock<std::mutex> lock{ m_mutex };

        m_tasks.emplace([task]() mutable {
            (*task)();
        });

        lock.unlock();
        m_notifier.notify_one();
        return future;
    }

    // Remove all pending tasks from the queue
    void clear()
    {
        std::unique_lock<std::mutex> lock{ m_mutex };

        while (!m_tasks.empty()) {
            m_tasks.pop();
        }
    }

    // Wait all workers to finish
    void join()
    {
        m_stop.store(true);
        m_notifier.notify_all();

        for (auto &thread : m_workers) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        m_workers.clear();
    }

    // Get the number of active and waiting workers
    std::size_t thread_count() const
    {
        return m_workers.size();
    }

    // Get the number of active workers
    std::size_t active_count() const
    {
        return m_active;
    }

private:
    // Thread main loop
    void thread_loop()
    {
        while (true) {
            // Wait for a new task
            auto task = next_task();

            if (task) {
                ++m_active;
                task();
                --m_active;
            }
            else if (m_stop) {
                // No more task + stop required
                break;
            }
        }
    }

    // Get the next pending task
    task_type next_task()
    {
        std::unique_lock<std::mutex> lock{ m_mutex };

        m_notifier.wait(lock, [this]() {
            return !m_tasks.empty() || m_stop;
        });

        if (m_tasks.empty()) {
            // No pending task
            return {};
        }

        auto task = m_tasks.front();
        m_tasks.pop();
        return task;
    }

    std::atomic<bool> m_stop{ false };
    std::atomic<std::size_t> m_active{ 0 };

    std::condition_variable m_notifier;
    std::mutex m_mutex;

    std::vector<std::thread> m_workers;
    std::queue<task_type> m_tasks;
};


#endif