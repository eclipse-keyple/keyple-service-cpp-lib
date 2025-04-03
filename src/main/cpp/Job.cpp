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

#include "keyple/core/service/cpp/Job.hpp"

#include <iostream>
#include <string>

#include "keyple/core/util/cpp/exception/IllegalArgumentException.hpp"

namespace keyple {
namespace core {
namespace service {
namespace cpp {

using keyple::core::util::cpp::exception::IllegalArgumentException;

Job::Job(const std::string& name)
: Thread(name)
, mCancelled(false)
{
}

bool
Job::cancel(const bool mayInterruptIfRunning)
{
    if (mayInterruptIfRunning) {
        throw IllegalArgumentException(
            "Unsupported value for mayInterruptIfRunning (true)");
    }

    if (!isAlive()) {
        return false;
    }

    mCancelled = true;

    return true;
}

bool
Job::isDone()
{
    return mDone;
}

bool
Job::isCancelled() const
{
    return mCancelled;
}

} /* namespace cpp */
} /* namespace service */
} /* namespace core */
} /* namespace keyple */
