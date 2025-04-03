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

#include "keyple/core/service/AbstractReaderAdapter.hpp"

#include <memory>
#include <string>
#include <vector>

#include "keyple/core/util/KeypleAssert.hpp"
#include "keyple/core/util/cpp/System.hpp"
#include "keyple/core/util/cpp/exception/Exception.hpp"
#include "keyple/core/util/cpp/exception/IllegalStateException.hpp"
#include "keypop/card/CardBrokenCommunicationException.hpp"
#include "keypop/card/UnexpectedStatusWordException.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::util::Assert;
using keyple::core::util::cpp::System;
using keyple::core::util::cpp::exception::Exception;
using keyple::core::util::cpp::exception::IllegalStateException;
using keypop::card::CardBrokenCommunicationException;
using keypop::card::UnexpectedStatusWordException;

AbstractReaderAdapter::AbstractReaderAdapter(
    const std::string& readerName,
    std::shared_ptr<KeypleReaderExtension> readerExtension,
    const std::string& pluginName)
: mReaderName(readerName)
, mReaderExtension(readerExtension)
, mPluginName(pluginName)
, mIsRegistered(false)
, mBefore(0)
{
}

const std::string&
AbstractReaderAdapter::getPluginName() const
{
    return mPluginName;
}

const std::vector<std::shared_ptr<CardSelectionResponseApi>>
AbstractReaderAdapter::transmitCardSelectionRequests(
    const std::vector<std::shared_ptr<CardSelectorBase>>& cardSelectors,
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>>&
        cardSelectionRequests,
    const MultiSelectionProcessing multiSelectionProcessing,
    const ChannelControl channelControl)
{
    checkStatus();

    std::vector<std::shared_ptr<CardSelectionResponseApi>>
        cardSelectionResponses;

    uint64_t timeStamp = System::nanoTime();
    uint64_t elapsed10ms = (timeStamp - mBefore) / 100000;
    mBefore = timeStamp;

    mLogger->trace(
        "Reader [%] --> cardSelectionRequests: %, elapsed % ms\n",
        getName(),
        cardSelectionRequests,
        elapsed10ms / 10.0);

    try {
        cardSelectionResponses = processCardSelectionRequests(
            cardSelectors,
            cardSelectionRequests,
            multiSelectionProcessing,
            channelControl);
    } catch (const UnexpectedStatusWordException& e) {
        throw CardBrokenCommunicationException(
            e.getCardResponse(),
            false,
            "An unexpected status word was received",
            std::make_shared<UnexpectedStatusWordException>(e));
    }

    timeStamp = System::nanoTime();
    elapsed10ms = (timeStamp - mBefore) / 100000;
    mBefore = timeStamp;

    mLogger->trace(
        "Reader [%] <-- cardSelectionResponses: %, elapsed % ms\n",
        getName(),
        cardSelectionResponses,
        elapsed10ms / 10.0);

    return cardSelectionResponses;
}

void
AbstractReaderAdapter::checkStatus() const
{
    if (!mIsRegistered) {
        throw IllegalStateException(
            "This reader, " + getName() + " is not registered");
    }
}

void
AbstractReaderAdapter::doRegister()
{
    mIsRegistered = true;
}

void
AbstractReaderAdapter::doUnregister()
{
    mIsRegistered = false;
}

const std::string&
AbstractReaderAdapter::getName() const
{
    return mReaderName;
}

std::shared_ptr<KeypleReaderExtension>
AbstractReaderAdapter::getExtension(
    const std::type_info& readerExtensionClass) const
{
    (void)readerExtensionClass;

    checkStatus();

    return mReaderExtension;
}

const std::shared_ptr<CardResponseApi>
AbstractReaderAdapter::transmitCardRequest(
    const std::shared_ptr<CardRequestSpi> cardRequest,
    const ChannelControl channelControl)
{
    checkStatus();

    Assert::getInstance().notNull(cardRequest, "cardRequest");

    std::shared_ptr<CardResponseApi> cardResponse = nullptr;

    uint64_t timeStamp = System::nanoTime();
    uint64_t elapsed10ms = (timeStamp - mBefore) / 100000;
    mBefore = timeStamp;

    mLogger->trace(
        "Reader [%] --> cardRequest: %, elapsed % ms\n",
        getName(),
        cardRequest,
        elapsed10ms / 10.0);

    try {
        cardResponse = processCardRequest(cardRequest, channelControl);
    } catch (const Exception& e) {
        /* Finally block in Java */
        (void)e;
        timeStamp = System::nanoTime();
        elapsed10ms = (timeStamp - mBefore) / 100000;
        mBefore = timeStamp;
        mLogger->trace(
            "Reader [%] <-- cardResponse: %, elapsed % ms\n",
            getName(),
            cardResponse,
            elapsed10ms / 10.0);
        throw;
    }

    /* Finally block in Java */
    timeStamp = System::nanoTime();
    elapsed10ms = (timeStamp - mBefore) / 100000;
    mBefore = timeStamp;
    mLogger->debug(
        "[%] receive => %, elapsed % ms\n",
        getName(),
        cardResponse,
        elapsed10ms / 10.0);

    return cardResponse;
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
