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

#include "keyple/core/service/CardSelectionScenarioAdapter.hpp"

#include <memory>
#include <vector>

#include "keyple/core/util/KeypleAssert.hpp"
#include "keypop/card/spi/CardSelectionRequestSpi.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::util::Assert;
using keypop::card::spi::CardSelectionRequestSpi;

CardSelectionScenarioAdapter::CardSelectionScenarioAdapter(
    const std::vector<std::shared_ptr<CardSelectorBase>>& cardSelectors,
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>>&
        cardSelectionRequests,
    const MultiSelectionProcessing multiSelectionProcessing,
    const ChannelControl channelControl)
: mCardSelectors(cardSelectors)
, mCardSelectionRequests(cardSelectionRequests)
, mMultiSelectionProcessing(multiSelectionProcessing)
, mChannelControl(channelControl)
{
    Assert::getInstance().notEmpty(
        cardSelectionRequests, "cardSelectionRequests");
}

const std::vector<std::shared_ptr<CardSelectorBase>>&
CardSelectionScenarioAdapter::getCardSelectors() const
{
    return mCardSelectors;
}

const std::vector<std::shared_ptr<CardSelectionRequestSpi>>&
CardSelectionScenarioAdapter::getCardSelectionRequests() const
{
    return mCardSelectionRequests;
}

MultiSelectionProcessing
CardSelectionScenarioAdapter::getMultiSelectionProcessing() const
{
    return mMultiSelectionProcessing;
}

ChannelControl
CardSelectionScenarioAdapter::getChannelControl() const
{
    return mChannelControl;
}

std::ostream&
operator<<(
    std::ostream& os, const std::shared_ptr<CardSelectionScenarioAdapter> sa)
{
    (void)sa;

    os << "CARD_SELECTION_SCENARIO : {"
       << "<TODO> CardSelectionRequest, "
       << "<TODO> MultiSelectionProcessing, "
       << "<TODO> ChannelControl"
       << "}";

    return os;
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
