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
#include <typeinfo>

#include "keyple/core/service/AbstractMonitoringJobAdapter.hpp"
#include "keyple/core/service/AbstractObservableStateAdapter.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/service/ObservableLocalReaderAdapter.hpp"

namespace keyple {
namespace core {
namespace service {

using InternalEvent = ObservableLocalReaderAdapter::InternalEvent;

/**
 * Wait for start the card detection state implementation.
 *
 * <p>The state during which the reader does not wait for a card to be inserted
 * but for a signal from the application to do so (switch to the
 * WAIT_FOR_CARD_INSERTION state).
 *
 * <ul>
 *   <li>Upon START_DETECT event, the machine changes state for
 * WAIT_FOR_CARD_INSERTION.
 * </ul>
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API WaitForStartDetectStateAdapter final
: public AbstractObservableStateAdapter {
public:
    /**
     * Creates an instance.
     *
     * /!\ C++: cannot use a shared_ptr for reader as this is called from constructors
     *
     * @param reader The observable local reader adapter.
     * @param monitoringJob The monitoring job.
     * @param executorService The executor service to use.
     * @since 2.0.0
     */
    WaitForStartDetectStateAdapter(
        ObservableLocalReaderAdapter* reader,
        std::shared_ptr<AbstractMonitoringJobAdapter> monitoringJob,
        std::shared_ptr<ExecutorService> executorService);

    /**
     * Creates an instance.
     *
     * /!\ C++: cannot use a shared_ptr for reader as this is called from constructors
     *
     * @param reader The observable local reader adapter.
     * @since 2.0.0
     */
    explicit WaitForStartDetectStateAdapter(
        ObservableLocalReaderAdapter* reader);

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void onEvent(const InternalEvent event) override;

private:
    /**
     * Logger
     */
    const std::unique_ptr<Logger> mLogger
        = LoggerFactory::getLogger(typeid(WaitForStartDetectStateAdapter));
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
