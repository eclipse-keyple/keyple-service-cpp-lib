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

#include "keyple/core/service/AbstractObservableStateAdapter.hpp"
#include "keyple/core/util/cpp/Thread.hpp"

namespace keyple {
namespace core {
namespace service {
namespace cpp {

using keyple::core::service::AbstractObservableStateAdapter;
using keyple::core::util::cpp::Thread;

ExecutorService::ExecutorService()
: mRunning(true)
, mTerminated(false)
{
    mThread = new std::thread(&ExecutorService::run, this);
}

ExecutorService::~ExecutorService()
{
    mRunning = false;

    while (!mTerminated) {
        Thread::sleep(10);
    }
}

void
ExecutorService::run()
{
    /* Emulates a SingleThreadExecutor (e.g. only one thread at a time) */

    while (mRunning) {
        std::shared_ptr<Job> job;

        {
            std::lock_guard<std::mutex> lock(mMutex);
            if (mPool.size() > 0) {
                job = mPool[0];
            }
        }

        if (job != nullptr) {
            /* Start first service and wait until completion */
            job->run();

            /* Remove from vector */
            {
                std::lock_guard<std::mutex> lock(mMutex);
                mPool.erase(mPool.begin());
            }

            job = nullptr;
        }

        Thread::sleep(100);
    }

    mTerminated = true;
}

void
ExecutorService::execute(std::shared_ptr<Job> job)
{
    std::lock_guard<std::mutex> lock(mMutex);
    mPool.push_back(job);
}

std::shared_ptr<Job>
ExecutorService::submit(std::shared_ptr<Job> job)
{
    std::lock_guard<std::mutex> lock(mMutex);

    /*
     * Limit the queue to maximum 2 jobs:
     * - 1 job currently executing (mPool[0])
     * - 1 job waiting to be executed (mPool[1])
     *
     * When submitting a new job:
     * - If pool is empty or has only 1 job → add the new job
     * - If pool has 2+ jobs → replace all waiting jobs with the new one
     *
     * This prevents accumulation of obsolete jobs when state transitions
     * happen faster than job execution.
     */
    if (mPool.size() >= 2) {
        /* Remove all waiting jobs (keep only the running one at index 0) */
        mPool.erase(mPool.begin() + 1, mPool.end());
    }

    mPool.push_back(job);

    return mPool.back();
}

void
ExecutorService::shutdown()
{
    mRunning = false;

    while (!mTerminated) {
        Thread::sleep(10);
    }
}

} /* namespace cpp */
} /* namespace service */
} /* namespace core */
} /* namespace keyple */
