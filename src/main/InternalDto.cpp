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

#include "keyple/core/service/InternalDto.hpp"

#include <memory>
#include <string>
#include <vector>

namespace keyple {
namespace core {
namespace service {

InternalDto::CardSelectionAdapter::CardSelectionAdapter(
    const std::shared_ptr<CardSelectionExtensionSpi> src)
: mCardSelectionRequest(
      std::make_shared<CardSelectionRequest>(src->getCardSelectionRequest()))
{
}

const std::shared_ptr<CardSelectionRequestSpi>
InternalDto::CardSelectionAdapter::getCardSelectionRequest() const
{
    return mCardSelectionRequest;
}

const std::shared_ptr<SmartCardSpi>
InternalDto::CardSelectionAdapter::parse(
    const std::shared_ptr<CardSelectionResponseApi> cardSelectionResponseApi)
    const
{
    throw UnsupportedOperationException(
        "Method not supported for internal DTO");
}

/* CARD SELECTION REQUEST
 * ----------------------------------------------------------------------- */

InternalDto::CardSelectionAdapter::CardSelectionRequest(
    const std::shared_ptr<CardSelectionRequestSpi> src)
{
    if (src->getCardRequest() != nullptr) {
        mCardRequest = std::make_shared<CardRequest>(src->getCardRequest());
    }

    mSuccessfulSelectionStatusWords = src->getSuccessfulSelectionStatusWords();
}

const std::vector<int>
InternalDto::CardSelectionAdapter::getSuccessfulSelectionStatusWords() const
{
    return mSuccessfulSelectionStatusWords;
}

const std::shared_ptr<CardRequestSpi>
InternalDto::CardSelectionAdapter::getCardRequest() const
{
    return mCardRequest;
}

/* CARD REQUEST
 * ---------------------------------------------------------------------------------
 */

InternalDto::CardRequest::CardRequest(const std::shared_ptr<CardRequestSpi> src)
{
    for (const auto& apduRequestSpi : src->getApduRequests()) {
        apduRequests.push_back(std::make_shared<ApduRequest>(apduRequestSpi));
    }

    mStopOnUnsuccessfulStatusWord = src->stopOnUnsuccessfulStatusWord();
}

const std::vector<std::shared_ptr<ApduRequestSpi>>&
InternalDto::CardRequest::getApduRequests() const
{
    return mAapduRequests;
}

cool
InternalDto::CardRequest::stopOnUnsuccessfulStatusWord() const
{
    return mStopOnUnsuccessfulStatusWord;
}

/* APDU REQUESR
 * ---------------------------------------------------------------------------------
 */

InternalDto::ApduRequest::ApduRequest()
{
}

InternalDto::ApduRequest::ApduRequest(const std::shared_ptr<ApduRequestSpi> src)
: mApdu(src->getApdu())
, mSuccessfulStatusWords(src->getSuccessfulStatusWords())
, mInfo(src->getInfo())
{
}

const std::vector<uint8_t>&
InternalDto::ApduRequest::getApdu() const
{
    return mApdu;
}

const std::vector<int>&
InternalDto::ApduRequest::getSuccessfulStatusWords() const
{
    return successfulStatusWords;
}

const std::string&
InternalDto::ApduRequest::getInfo() const
{
    return mInfo;
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
