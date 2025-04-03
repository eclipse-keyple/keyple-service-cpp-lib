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

#include "keyple/core/service/AbstractObservableStateAdapter.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/service/ObservableLocalReaderAdapter.hpp"
#include "keyple/core/service/cpp/Job.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::service::cpp::Job;

/**
 * Abstract class for all monitoring jobs.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API AbstractMonitoringJobAdapter {
public:
    /**
     * Creates an instance.
     *
     * /!\ C++: cannot use a shared_ptr for reader as this is called from constructors
     *
     * @param reader The reader.
     * @since 2.0.0
     */
    explicit AbstractMonitoringJobAdapter(ObservableLocalReaderAdapter* reader);

    /**
     *
     */
    virtual ~AbstractMonitoringJobAdapter() = default;

    /**
     * Gets the reader.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    ObservableLocalReaderAdapter* getReader() const;

    /**
     * Gets the task of the monitoring job.
     *
     * @param monitoringState reference to the state the monitoring job in
     * running against.
     * @return A not null reference.
     * @since 2.0.0
     */
    virtual std::shared_ptr<Job> getMonitoringJob(
        const std::shared_ptr<AbstractObservableStateAdapter> monitoringState)
        = 0;

    /**
     * Stops/interrupts the monitoring job
     *
     * @since 2.0.0
     */
    virtual void stop() = 0;

private:
    /**
     *
     */
    ObservableLocalReaderAdapter* mReader;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
