//
// Created by silverly on 26/12/2021.
//

#ifndef IVY_SAFE_QUEUE_H
#define IVY_SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

// A thread safe queue.
template <class T>
class SafeQueue
{
public:
    SafeQueue()
            : q()
            , m()
            , c()
            , timeout(false){}

    ~SafeQueue(){}

    /**
     * Add an element to the queue.
     */
    void enqueue(T t){
        std::lock_guard<std::mutex> lock(m);
        q.push(t);
        c.notify_one();
    }

    /**
     * Get the "front"-element.
     * If the queue is empty, wait till an element is available OR someone destroy the queue by calling unlock_all
     */
    T dequeue(void){
        std::unique_lock<std::mutex> lock(m);
        while(q.empty())
        {
            if(timeout){ return 0; }

            // release lock as long as the wait and reacquire it afterwards.
            c.wait(lock);
        }
        T val = q.front();
        q.pop();
        return val;
    }

    inline bool empty(){
        std::unique_lock<std::mutex> lock(m);
        return q.empty();
    }

    void unlock_all(){
        std::unique_lock<std::mutex> lock(m);
        timeout=true;
        c.notify_all();
    }

    int size(){
        std::unique_lock<std::mutex> lock(m);
        return (int)q.size();
    }

private:
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable c;
    bool timeout;
};

#endif //IVY_SAFE_QUEUE_H
