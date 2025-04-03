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

#include <atomic>
#include <memory>
#include <typeinfo>

#include "keyple/core/service/AbstractMonitoringJobAdapter.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/service/cpp/Job.hpp"
#include "keyple/core/util/cpp/LoggerFactory.hpp"
#include "keypop/reader/CardReader.hpp"

namespace keyple {
namespace core {
namespace service {

/**
 * This monitoring job polls the CardReader::isCardPresent() method to detect a
 * card insertion or a card removal.
 *
 * <p>All runtime exceptions that may occur during the monitoring process are
 * caught and notified at the application level through the
 * keypop::reader::spi::CardReaderObservationExceptionHandlerSpi mechanism.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API CardInsertionActiveMonitoringJobAdapter final
: public AbstractMonitoringJobAdapter {
public:
    /**
     * Build a monitoring job to detect the card insertion
     *
     * /!\ C++: cannot use a shared_ptr for reader as this is called from constructors
     *
     * @param reader reader that will be polled with the method isCardPresent()
     * @param sleepDurationMillis time interval between two presence polls.
     * @param monitorInsertion if true, polls for CARD_INSERTED, else
     * CARD_REMOVED
     * @since 2.0.0
     */
    CardInsertionActiveMonitoringJobAdapter(
        ObservableLocalReaderAdapter* reader,
        const int64_t sleepDurationMillis,
        const bool monitorInsertion);

    /**
     * Gets the monitoring process.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    std::shared_ptr<Job> getMonitoringJob(
        const std::shared_ptr<AbstractObservableStateAdapter> monitoringState)
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
        typeid(CardInsertionActiveMonitoringJobAdapter));

    /**
     *
     */
    const int64_t mSleepDurationMillis;

    /**
     *
     */
    const bool mMonitorInsertion;

    /**
     *
     */
    CardReader* mReader;

    /**
     *
     */
    std::atomic<bool> mLoop;

    /**
     *
     */
    class CardInsertionActiveMonitoringJob final : public Job {
    public:
        /**
         *
         */
        CardInsertionActiveMonitoringJob(
            std::shared_ptr<AbstractObservableStateAdapter> monitoringState,
            CardInsertionActiveMonitoringJobAdapter* parent);

        /**
         * Monitoring loop
         *
         * C++: this replaces run() override
         *
         * <p>Polls for the presence of a card and loops until no card responds.
         * <br> Triggers a CARD_INSERTED event and exits as soon as a
         * communication with a card is established.
         *
         * <p>Any exceptions are notified to the application using the exception
         * handler.
         */
        void execute() final;

    private:
        /**
         *
         */
        std::shared_ptr<AbstractObservableStateAdapter> mMonitoringState;

        /**
         *
         */
        CardInsertionActiveMonitoringJobAdapter* mParent;
    };
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
