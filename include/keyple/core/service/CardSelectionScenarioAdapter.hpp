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
#include <vector>

#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/service/MultiSelectionProcessing.hpp"
#include "keypop/card/ChannelControl.hpp"
#include "keypop/card/spi/CardSelectionRequestSpi.hpp"
#include "keypop/reader/cpp/CardSelectorBase.hpp"

namespace keyple {
namespace core {
namespace service {

using keypop::card::ChannelControl;
using keypop::card::spi::CardSelectionRequestSpi;
using keypop::reader::cpp::CardSelectorBase;

/**
 * This POJO contains a selection scenario composed of one or more card
 * selection requests and indicators specifying the expected behavior.
 *
 * <p>It comprises:
 *
 * <ul>
 *   <li>A list of CardSelectionRequestSpi corresponding to the selection
 * targets. <li>A MultiSelectionProcessing indicator specifying whether all
 * scheduled card selections are to be executed or whether to stop at the first
 * one that is successful. <li>A ChannelControl indicator controlling the
 * physical channel the end of the selection process.
 * </ul>
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API CardSelectionScenarioAdapter final {
public:
    /**
     * (package-private)<br>
     * Builds a card selection scenario from a list of selection cases and two
     * enum constants guiding the expected behaviour of the selection process.
     *
     * <p>Note: the CardSelectionRequestSpi list should be carefully ordered in
     * accordance with the cards expected in the application to optimize the
     * processing time of the selection process. The first selection case in the
     * list will be processed first.
     *
     * @param cardSelectors A list of card selectors.
     * @param cardSelectionRequests A list of card selection requests.
     * @param multiSelectionProcessing The multi selection processing policy.
     * @param channelControl The channel control policy.
     * @throw IllegalArgumentException if the card selection request list is
     * null or empty, if one of the indicators is null.
     * @since 2.0.0
     */
    CardSelectionScenarioAdapter(
        const std::vector<std::shared_ptr<CardSelectorBase>>& cardSelectors,
        const std::vector<std::shared_ptr<CardSelectionRequestSpi>>&
            cardSelectionRequests,
        const MultiSelectionProcessing multiSelectionProcessing,
        const ChannelControl channelControl);

    /**
     * Gets the card selectors list.
     *
     * @return A not null reference
     * @since 3.0.0
     */
    const std::vector<std::shared_ptr<CardSelectorBase>>&
    getCardSelectors() const;

    /**
     * Gets the card selection requests list.
     *
     * @return A not null reference
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>>&
    getCardSelectionRequests() const;

    /**
     * Gets the multi selection processing policy.
     *
     * @return A not null reference
     * @since 2.0.0
     */
    MultiSelectionProcessing getMultiSelectionProcessing() const;

    /**
     * Gets the channel control policy.
     *
     * @return A not null reference
     * @since 2.0.0
     */
    ChannelControl getChannelControl() const;

    /**
     * Converts the card selection scenario into a string where the data is
     * encoded in a json format.
     *
     * @return A not empty String
     * @since 2.0.0
     */
    friend KEYPLESERVICE_API std::ostream& operator<<(
        std::ostream& os,
        const std::shared_ptr<CardSelectionScenarioAdapter> sa);

private:
    /**
     *
     */
    std::vector<std::shared_ptr<CardSelectorBase>> mCardSelectors;

    /**
     *
     */
    std::vector<std::shared_ptr<CardSelectionRequestSpi>>
        mCardSelectionRequests;

    /**
     *
     */
    MultiSelectionProcessing mMultiSelectionProcessing;

    /**
     *
     */
    ChannelControl mChannelControl;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
