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

#include "keyple/core/plugin/spi/reader/observable/ObservableReaderSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/insertion/CardInsertionWaiterBlockingSpi.hpp"
#include "keyple/core/plugin/spi/reader/observable/state/insertion/WaitForCardInsertionBlockingSpi.hpp"
#include "keyple/core/service/AbstractMonitoringJobAdapter.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/service/cpp/Job.hpp"
#include "keyple/core/util/cpp/LoggerFactory.hpp"

namespace keyple {
namespace core {
namespace service {

/**
 * Detect the card insertion thanks to the method
 * CardInsertionWaiterBlockingSpi::waitForCardInsertion() or
 * WaitForCardInsertionBlockingSpi::waitForCardInsertion().
 *
 * <p>This method is invoked in another thread.
 *
 * <p>The job waits indefinitely for the waitForCardInsertion method to return
 * unless the stop() method is invoked. In this case, the job is aborted.
 *
 * <p>When a card is present, an internal CARD_INSERTED event is fired.
 *
 * <p>All runtime exceptions that may occur during the monitoring process are
 * caught and notified at the application level through the appropriate
 * exception handler.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API CardInsertionPassiveMonitoringJobAdapter final
: public AbstractMonitoringJobAdapter {
public:
    /**
     * Constructor.
     *
     * /!\ C++: cannot use a shared_ptr for reader as this is called from constructors
     *
     * @param reader The reader.
     * @since 2.0.0
     */
    explicit CardInsertionPassiveMonitoringJobAdapter(
        ObservableLocalReaderAdapter* reader);

    /**
     * Gets the monitoring process.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    std::shared_ptr<Job> getMonitoringJob(
        std::shared_ptr<AbstractObservableStateAdapter> monitoringState)
        override;

    /**
     * Terminates the monitoring process.
     *
     * @since 2.0.0
     */
    void stop() override;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger = LoggerFactory::getLogger(
        typeid(CardInsertionPassiveMonitoringJobAdapter));

    /**
     *
     */
    std::shared_ptr<ObservableReaderSpi> mReaderSpi;

    /**
     *
     */
    class CardInsertionPassiveMonitoringJob final : public Job {
    public:
        /**
         *
         */
        CardInsertionPassiveMonitoringJob(
            const std::shared_ptr<AbstractObservableStateAdapter>
                monitoringState,
            CardInsertionPassiveMonitoringJobAdapter* parent);

        /**
         * Monitoring loop
         *
         * C++: this replaces run() override
         *
         * <p>Waits for the presence of a card until no card is present. <br>
         * Triggers a CARD_INSERTED event and exits as soon as a communication
         * with a card is established.
         *
         * <p>Any exceptions are notified to the application using the exception
         * handler.
         */
        void execute() final;

    private:
        /**
         *
         */
        const std::shared_ptr<AbstractObservableStateAdapter> mMonitoringState;

        /**
         *
         */
        CardInsertionPassiveMonitoringJobAdapter* mParent;
    };
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
