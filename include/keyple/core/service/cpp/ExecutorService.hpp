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

#pragma once

#include <memory>
#include <mutex>
#include <thread>
#include <typeinfo>
#include <vector>

#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/service/cpp/Job.hpp"

namespace keyple {
namespace core {
namespace service {
namespace cpp {

class KEYPLESERVICE_API ExecutorService final {
public:
    /**
     *
     */
    ExecutorService();

    /**
     *
     */
    ~ExecutorService();

    /**
     *
     */
    void execute(std::shared_ptr<Job> job);

    /**
     *
     */
    std::shared_ptr<Job> submit(std::shared_ptr<Job> job);

    /**
     *
     */
    void shutdown();

    /**
     * /!\ MSVC requires operator= to be deleted because of std::future
     * not being copyable.
     */
    ExecutorService& operator=(ExecutorService o) = delete;

    /**
     * /!\ MSVC requires copy constructor to be deleted because of std::future
     * not being copyable.
     */
    ExecutorService(const ExecutorService& o) = delete;

private:
    /**
     *
     */
    std::vector<std::shared_ptr<Job>> mPool;

    /**
     *
     */
    std::mutex mMutex;

    /**
     *
     */
    void run();

    /**
     *
     */
    std::atomic<bool> mRunning;

    /**
     *
     */
    std::atomic<bool> mTerminated;

    /**
     *
     */
    std::thread* mThread;
};

} /* namespace cpp */
} /* namespace service */
} /* namespace core */
} /* namespace keyple */
