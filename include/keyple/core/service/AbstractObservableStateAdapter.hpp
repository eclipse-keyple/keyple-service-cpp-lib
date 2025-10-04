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

#include <future>
#include <memory>
#include <typeinfo>

#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/service/MonitoringState.hpp"
#include "keyple/core/service/ObservableLocalReaderAdapter.hpp"
#include "keyple/core/service/cpp/ExecutorService.hpp"
#include "keyple/core/service/cpp/Job.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::service::cpp::ExecutorService;

class AbstractMonitoringJobAdapter;

/**
 * (package-private)<br>
 * Abstract class for all states of a {@link ObservableLocalReaderAdapter}.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API AbstractObservableStateAdapter
: public std::enable_shared_from_this<AbstractObservableStateAdapter> {
public:
    /**
     * Create a new state with a state identifier and a monitor job
     *
     * /!\ C++: cannot use a shared_ptr for reader as this is called from constructors
     *
     * @param monitoringState the state identifier
     * @param reader the current reader
     * @param monitoringJob the job to be executed in background (may be null if
     * no background job is required)
     * @param executorService the executor service
     * @since 2.0.0
     */
    AbstractObservableStateAdapter(
        const MonitoringState monitoringState,
        ObservableLocalReaderAdapter* reader,
        std::shared_ptr<AbstractMonitoringJobAdapter> monitoringJob,
        std::shared_ptr<ExecutorService> executorService);

    /**
     *
     */
    virtual ~AbstractObservableStateAdapter() = default;

    /**
     * Get the current state identifier of the state machine
     *
     * @return the current state identifier
     * @since 2.0.0
     */
    MonitoringState getMonitoringState() const;

    /**
     * Gets the reader.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    ObservableLocalReaderAdapter* getReader() const;

    /**
     * Switch state in the parent reader
     *
     * @param stateId the new state
     * @since 2.0.0
     */
    void switchState(const MonitoringState stateId);

    /**
     * Invoked when activated, a custom behaviour can be added here.
     *
     * @since 2.0.0
     * @throw IllegalStateException if a job is defined with a null executor
     * service.
     */
    void onActivate();

    /**
     * Invoked when deactivated. Cancel the monitoringJob if necessary.
     *
     * @since 2.0.0
     */
    void onDeactivate();

    /**
     * Handle Internal Event.
     *
     * @param event internal event received by reader
     * @since 2.0.0
     */
    virtual void
    onEvent(typename ObservableLocalReaderAdapter::InternalEvent event)
        = 0;

private:
    /**
     * Identifier of the currentState
     */
    MonitoringState mMonitoringState;

    /**
     * Reference to Reader
     */
    ObservableLocalReaderAdapter* mReader;

    /**
     * Background job definition if any
     */
    std::shared_ptr<AbstractMonitoringJobAdapter> mMonitoringJob;

    /**
     * Result of the background job if any
     */
    std::shared_ptr<Job> mMonitoringEvent;

    /**
     * Executor service used to execute AbstractMonitoringJobAdapter
     */
    std::shared_ptr<ExecutorService> mExecutorService;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
