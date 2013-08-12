// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "../Macros/Macros.h"

#include <condition_variable>
#include <forward_list>
#include <functional>
#include <memory>
#include <thread>
#include <deque>
#include <mutex>

namespace selene
{

        /**
         * \addtogroup Core
         * @{
         */

        /**
         * Represents thread pool.
         */
        class ThreadPool
        {
        public:
                /**
                 * Represents job.
                 */
                typedef std::function<void()> Job;

                /**
                 * \brief Constructs thread pool with given number of workers.
                 * \param[in] numWorkers number of workers
                 */
                ThreadPool(size_t numWorkers = 2);
                ThreadPool(const ThreadPool&) = delete;
                ~ThreadPool();
                ThreadPool& operator =(const ThreadPool&) = delete;

                /**
                 * \brief Adds job to the queue.
                 * \param[in] job job, which shall be added to the queue
                 */
                void addJob(Job&& job);

        private:
                /**
                 * Represents worker.
                 */
                class Worker
                {
                public:
                        /**
                         * \brief Constructs worker.
                         * \param[in] threadPool thread pool, which owns current worker
                         */
                        Worker(ThreadPool& threadPool);
                        Worker(const Worker&) = delete;
                        ~Worker();
                        Worker& operator =(const Worker&) = delete;

                private:
                        ThreadPool& threadPool_;

                        std::thread thread_;
                        std::mutex mutex_;

                        volatile bool isActive_;

                        /**
                         * \brief Executes jobs from the queue.
                         */
                        void executeJobs();

                };

                std::deque<Job> jobs_;
                std::mutex jobMutex_;
                std::condition_variable activator_;

                std::forward_list<std::unique_ptr<Worker>> workers_;

        };

        /**
         * @}
         */

}

#endif
