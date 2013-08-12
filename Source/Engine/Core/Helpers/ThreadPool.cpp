// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "ThreadPool.h"
#include <utility>

namespace selene
{

        ThreadPool::ThreadPool(size_t numWorkers):
                jobs_(), jobMutex_(), activator_(), workers_()
        {
                for(size_t i = 0; i < numWorkers; ++i)
                {
                        std::unique_ptr<Worker> worker(new Worker(*this));
                        workers_.emplace_front(std::move(worker));
                }
        }
        ThreadPool::~ThreadPool() {}

        //-------------------------------------------------------------------------------
        void ThreadPool::addJob(Job&& job)
        {
                std::lock_guard<std::mutex> lock(jobMutex_);
                jobs_.emplace_back(std::move(job));
                activator_.notify_all();
        }

        ThreadPool::Worker::Worker(ThreadPool& threadPool):
                threadPool_(threadPool), thread_(), mutex_(), isActive_(true)
        {
                thread_ = std::thread(std::bind(&ThreadPool::Worker::executeJobs, this));
        }
        ThreadPool::Worker::~Worker()
        {
                mutex_.lock();
                isActive_ = false;
                mutex_.unlock();

                threadPool_.jobMutex_.lock();
                threadPool_.activator_.notify_all();
                threadPool_.jobMutex_.unlock();

                thread_.join();
        }

        //-------------------------------------------------------------------------------
        void ThreadPool::Worker::executeJobs()
        {
                Job job;
                bool isActive = true;

                while(isActive)
                {
                        threadPool_.jobMutex_.lock();
                        if(!threadPool_.jobs_.empty())
                        {
                                job = std::move(threadPool_.jobs_.front());
                                threadPool_.jobs_.pop_front();
                                threadPool_.jobMutex_.unlock();

                                if(job)
                                        job();
                        }
                        else
                        {
                                threadPool_.jobMutex_.unlock();

                                std::unique_lock<std::mutex> lock(threadPool_.jobMutex_);
                                threadPool_.activator_.wait(lock);
                        }

                        mutex_.lock();
                        isActive = isActive_;
                        mutex_.unlock();
                }
        }

}
