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
#include <utility>
#include <vector>

#include "keyple/core/util/cpp/exception/UnsupportedOperationException.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::util::cpp::exception::UnsupportedOperationException;

/* CARD SELECTION ADAPTER
 * ----------------------------------------------------------------------- */

InternalDto::CardSelectionAdapter::CardSelectionAdapter(
    const std::shared_ptr<CardSelectionExtensionSpi> src)
: mCardSelectionRequest(
      std::unique_ptr<CardSelectionRequest>(
          new CardSelectionRequest(src->getCardSelectionRequest())))
{
}

std::unique_ptr<CardSelectionRequestSpi>
InternalDto::CardSelectionAdapter::getCardSelectionRequest()
{
    return std::move(mCardSelectionRequest);
}

std::shared_ptr<SmartCardSpi>
InternalDto::CardSelectionAdapter::parse(
    const std::shared_ptr<
        CardSelectionResponseApi>& /* cardSelectionResponseApi */)
{
    throw UnsupportedOperationException(
        "Method not supported for internal DTO");
}

/* CARD SELECTION REQUEST
 * ----------------------------------------------------------------------- */

InternalDto::CardSelectionRequest::CardSelectionRequest(
    const std::shared_ptr<CardSelectionRequestSpi> src)
{
    if (src->getCardRequest() != nullptr) {
        mCardRequest = std::make_shared<CardRequest>(src->getCardRequest());
    }

    mSuccessfulSelectionStatusWords = src->getSuccessfulSelectionStatusWords();
}

const std::vector<int>&
InternalDto::CardSelectionRequest::getSuccessfulSelectionStatusWords() const
{
    return mSuccessfulSelectionStatusWords;
}

const std::shared_ptr<CardRequestSpi>
InternalDto::CardSelectionRequest::getCardRequest() const
{
    return mCardRequest;
}

/* CARD REQUEST
 * ----------------------------------------------------------------------- */

InternalDto::CardRequest::CardRequest(const std::shared_ptr<CardRequestSpi> src)
{
    for (const auto& apduRequestSpi : src->getApduRequests()) {
        mApduRequests.push_back(std::make_shared<ApduRequest>(apduRequestSpi));
    }

    mStopOnUnsuccessfulStatusWord = src->stopOnUnsuccessfulStatusWord();
}

const std::vector<std::shared_ptr<ApduRequestSpi>>&
InternalDto::CardRequest::getApduRequests() const
{
    return mApduRequests;
}

bool
InternalDto::CardRequest::stopOnUnsuccessfulStatusWord() const
{
    return mStopOnUnsuccessfulStatusWord;
}

/* APDU REQUEST
 * ----------------------------------------------------------------------- */

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

void
InternalDto::ApduRequest::setApdu(const std::vector<uint8_t>& apdu)
{
    mApdu = apdu;
}

const std::vector<int>&
InternalDto::ApduRequest::getSuccessfulStatusWords() const
{
    return mSuccessfulStatusWords;
}

const std::string&
InternalDto::ApduRequest::getInfo() const
{
    return mInfo;
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
