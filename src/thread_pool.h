#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thread>
#include <atomic>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <iostream>

class ThreadPool{
public:
    using Task = std::function<void()>;

    ThreadPool(const ThreadPool&)=delete;
    ThreadPool& operator=(const ThreadPool&)=delete;

    ThreadPool(unsigned thread_nums,unsigned max_queue_size);
    ~ThreadPool();

    template<typename FuncType>
    void Submit(FuncType func);

private:
    std::atomic_bool done_;
    unsigned thread_nums_;
    unsigned max_queue_size_;

    std::mutex m_;
    std::condition_variable queue_empty_;
    std::condition_variable queue_full_;

    std::queue<Task> queue_;
    std::vector<std::thread> workers_;

    void Worker();
    bool GetTask(Task& task);
};

inline ThreadPool::ThreadPool(unsigned thread_nums,unsigned max_queue_size):
    done_(false),thread_nums_(thread_nums),max_queue_size_(max_queue_size){
    try{
        for(unsigned i=0;i<thread_nums;++i){
            workers_.push_back(std::thread(&ThreadPool::Worker,this));
        }
    }catch(...){
        done_=true;
        throw;
    }
}

inline ThreadPool::~ThreadPool(){
    done_=true;
    for(unsigned i=0;i<thread_nums_;++i){
        if(workers_[i].joinable()){
            workers_[i].join();
        }
    }
}

inline void ThreadPool::Worker(){
    while(!done_){
        Task task;
        GetTask(task);
        task();
    }
}

inline bool ThreadPool::GetTask(Task& task){
    std::unique_lock<std::mutex> lk(m_);
    queue_empty_.wait(lk,[this](){return !queue_.empty();});
    task=std::move(queue_.front());
    queue_.pop();
    queue_full_.notify_one();
    //std::cout<<"gettask"<<std::endl;
}

template<typename FuncType>
void ThreadPool::Submit(FuncType func){
    std::unique_lock<std::mutex> lk(m_);
    queue_full_.wait(lk,[this](){return queue_.size()<max_queue_size_;});
    queue_.push(std::move(Task(func)));
    queue_empty_.notify_one();
    //std::cout<<"submit"<<std::endl;
}

#endif