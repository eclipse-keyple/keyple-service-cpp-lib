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

#include <iomanip>
#include <memory>
#include <sstream>
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

std::ostream&
operator<<(std::ostream& os, const InternalDto::CardSelectionAdapter& csa)
{
    os << "CARD_SELECTION_ADAPTER: {"
       << "CARD_SELECTION_REQUEST: ";

    if (csa.mCardSelectionRequest == nullptr) {
        os << "null";
    } else {
        os << *csa.mCardSelectionRequest;
    }

    os << "}";

    return os;
}

std::ostream&
operator<<(
    std::ostream& os,
    const std::shared_ptr<InternalDto::CardSelectionAdapter> csa)
{
    if (csa == nullptr) {
        os << "CARD_SELECTION_ADAPTER: null";
    } else {
        os << *csa;
    }

    return os;
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

std::ostream&
operator<<(std::ostream& os, const InternalDto::CardSelectionRequest& csr)
{
    std::stringstream ssSw;
    for (auto it = std::begin(csr.mSuccessfulSelectionStatusWords);
         it != std::end(csr.mSuccessfulSelectionStatusWords);
         ++it) {
        ssSw << std::uppercase << std::hex << std::setfill('0') << std::setw(4)
             << static_cast<int>(*it);
        if (it != csr.mSuccessfulSelectionStatusWords.end() - 1) {
            ssSw << ", ";
        }
    }

    os << "CARD_SELECTION_REQUEST: {"
       << "CARD_REQUEST: ";

    if (csr.mCardRequest == nullptr) {
        os << "null";
    } else {
        os << *csr.mCardRequest;
    }

    os << ", "
       << "SUCCESSFUL_SELECTION_STATUS_WORDS: " << ssSw.str() << "}";

    return os;
}

std::ostream&
operator<<(
    std::ostream& os,
    const std::shared_ptr<InternalDto::CardSelectionRequest> csr)
{
    if (csr == nullptr) {
        os << "CARD_SELECTION_REQUEST: null";
    } else {
        os << *csr;
    }

    return os;
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

std::ostream&
operator<<(std::ostream& os, const InternalDto::CardRequest& cr)
{
    os << "CARD_REQUEST: {"
       << "APDU_REQUESTS: " << cr.mApduRequests << ", "
       << "STOP_ON_UNSUCCESSFUL_STATUS_WORD: "
       << cr.mStopOnUnsuccessfulStatusWord << "}";

    return os;
}

std::ostream&
operator<<(std::ostream& os, const std::shared_ptr<InternalDto::CardRequest> cr)
{
    if (cr == nullptr) {
        os << "CARD_REQUEST: null";
    } else {
        os << *cr;
    }

    return os;
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

std::ostream&
operator<<(std::ostream& os, const InternalDto::ApduRequest& ar)
{
    std::stringstream ssApdu;
    for (const auto val : ar.mApdu) {
        ssApdu << std::uppercase << std::hex << std::setfill('0')
               << std::setw(2) << static_cast<int>(val);
    }

    std::stringstream ssSw;
    for (auto it = std::begin(ar.mSuccessfulStatusWords);
         it != std::end(ar.mSuccessfulStatusWords);
         ++it) {
        ssSw << std::uppercase << std::hex << std::setfill('0') << std::setw(4)
             << static_cast<int>(*it);
        if (it != ar.mSuccessfulStatusWords.end() - 1) {
            ssSw << ", ";
        }
    }

    os << "APDU_REQUEST: {"
       << "APDU: " << ssApdu.str() << ", "
       << "SUCCESSFUL_STATUS_WORD: " << ssSw.str() << ", "
       << "INFO: " << ar.mInfo << "}";

    return os;
}

std::ostream&
operator<<(std::ostream& os, const std::shared_ptr<InternalDto::ApduRequest> ar)
{
    if (ar == nullptr) {
        os << "APDU_REQUEST: null";
    } else {
        os << *ar;
    }

    return os;
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
