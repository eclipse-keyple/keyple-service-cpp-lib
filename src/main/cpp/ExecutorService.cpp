/******************************************************************************
 * Copyright (c) 2025 Calypso Networks Association https://calypsonet.org/    *
 *                                                                            *
 * See the NOTICE file(s) distributed with this work for additional           *
 * information regarding copyright ownership.                                 *
 *                                                                            *
 * This program and the accompanying materials are made available under the   *
 * terms of the Eclipse Public License 2.0 which is available at              *
 * http://www.eclipse.org/legal/epl-2.0                                       *
 *                                                                            *
 * SPDX-License-Identifier: EPL-2.0                                           *
 ******************************************************************************/

#include "keyple/core/service/cpp/ExecutorService.hpp"

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "keyple/core/service/AbstractObservableStateAdapter.hpp"
#include "keyple/core/util/cpp/LoggerFactory.hpp"
#include "keyple/core/util/cpp/Thread.hpp"

namespace keyple {
namespace core {
namespace service {
namespace cpp {

using keyple::core::service::AbstractObservableStateAdapter;
using keyple::core::util::cpp::Thread;

ExecutorService::ExecutorService()
: mRunning(false)
, mTerminated(false)
{
}

ExecutorService::~ExecutorService()
{
    shutdown();
}

void
ExecutorService::run()
{
    while (true) {
        std::unique_lock<std::mutex> lock(mMutex);

        // Wait until there's a job or the service is shutting down
        mCondition.wait(lock, [this]{
            return !mPool.empty() || !mRunning;
        });

        // Check if we should terminate
        if (!mRunning && mPool.empty()) {
            break;
        }

        // Get the job and remove it from the pool
        std::shared_ptr<Job> job = mPool.front();
        mPool.erase(mPool.begin());

        // Unlock the mutex before running the job
        // This allows other threads to submit new jobs while one is being processed
        lock.unlock();

        if (!job->isCancelled()) {
            /*
             * A failing job must not bring down the worker thread, and even
             * less the process: a Java ThreadPoolExecutor captures the
             * exception of a task in its Future and keeps the pool alive.
             */
            try {
                job->run();

            } catch (const std::exception& e) {
                keyple::core::util::cpp::LoggerFactory::getLogger(
                    typeid(ExecutorService))
                    ->error("Job [%] failed: %\n", job->getName(), e.what());

            } catch (...) {
                keyple::core::util::cpp::LoggerFactory::getLogger(
                    typeid(ExecutorService))
                    ->error(
                        "Job [%] failed with an unknown exception\n",
                        job->getName());
            }
        }
    }

    mTerminated = true;
}

void
ExecutorService::execute(std::shared_ptr<Job> job)
{
    {
        std::lock_guard<std::mutex> lock(mMutex);
        if (!mThread) {
            mRunning = true;
            mThread = std::unique_ptr<std::thread>(new std::thread(&ExecutorService::run, this));
        }
        mPool.push_back(job);
    }
    mCondition.notify_one();
}

std::shared_ptr<Job>
ExecutorService::submit(std::shared_ptr<Job> job)
{
    /*
     * Return the job we were given directly: the worker thread may already
     * have dequeued (and even completed) it by the time we could re-lock
     * mMutex, so reading it back via mPool.back() is a data race that can
     * return an empty-vector access or the wrong job entirely.
     */
    execute(job);
    return job;
}

void
ExecutorService::shutdown()
{
    {
        std::lock_guard<std::mutex> lock(mMutex);
        if (!mThread) {
            return;
        }
        mRunning = false;
    }

    mCondition.notify_one();

    if (mThread->joinable()) {
        mThread->join();
    }

    mThread.reset();
    mTerminated = true;
}

} /* namespace cpp */
} /* namespace service */
} /* namespace core */
} /* namespace keyple */
