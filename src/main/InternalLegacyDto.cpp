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

#include "keyple/core/service/InternalLegacyDto.hpp"

#include <iomanip>
#include <memory>
#include <sstream>
#include <vector>

#include "keyple/core/service/InternalCardSelector.hpp"

namespace keyple {
namespace core {
namespace service {

InternalLegacyDto::InternalLegacyDto()
{
}

const std::vector<std::shared_ptr<LegacyCardSelectionRequestV0>>
InternalLegacyDto::mapToLegacyCardSelectionRequestsV0(
    const std::vector<std::shared_ptr<CardSelector>>& cardSelectors,
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>>&
        cardSelectionRequests)
{
    std::vector<std::shared_ptr<LegacyCardSelectionRequestV0>> result;

    for (int i = 0; i < cardSelectors.size(); i++) {
        result.push_back(mapToLegacyCardSelectionRequestV0(
            cardSelectors[i], cardSelectionRequests[i]));
    }

    return result;
}

const std::vector<std::shared_ptr<LegacyCardSelectionRequestV1>>
InternalLegacyDto::mapToLegacyCardSelectionRequestsV1(
    const std::vector<std::shared_ptr<CardSelector>>& cardSelectors,
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>>&
        cardSelectionRequests)
{
    std::vector<std::shared_ptr<LegacyCardSelectionRequestV1>> result;

    for (int i = 0; i < cardSelectors.size(); i++) {
        result.push_back(mapToLegacyCardSelectionRequestV1(
            cardSelectors[i], cardSelectionRequests[i]));
    }

    return result;
}

std::shared_ptr<LegacyCardSelectionRequestV0>
InternalLegacyDto::mapToLegacyCardSelectionRequestV0(
    std::shared_ptr<CardSelector> cardSelector,
    std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequestSpi)
{
    auto result = std::make_shared<LegacyCardSelectionRequestV0>();

    result->mCardRequest = cardSelectionRequestSpi->getCardRequest() != nullptr
                               ? mapToLegacyCardRequestV0(
                                     cardSelectionRequestSpi->getCardRequest())
                               : nullptr;

    result->mCardSelector
        = mapToLegacyCardSelector(cardSelector, cardSelectionRequestSpi);

    return result;
}

std::shared_ptr<LegacyCardSelectionRequestV1>
InternalLegacyDto::mapToLegacyCardSelectionRequestV1(
    std::shared_ptr<CardSelector> cardSelector,
    std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequestSpi)
{
    auto result = std::make_shared<LegacyCardSelectionRequestV1>();

    result->mCardRequest = cardSelectionRequestSpi->getCardRequest() != nullptr
                               ? mapToLegacyCardRequestV1(
                                     cardSelectionRequestSpi->getCardRequest())
                               : nullptr;

    result->mCardSelector
        = mapToLegacyCardSelector(cardSelector, cardSelectionRequestSpi);

    return result;
}

std::shared_ptr<LegacyCardSelector>
InternalLegacyDto::mapToLegacyCardSelector(
    std::shared_ptr<CardSelector> cardSelector,
    std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequestSpi)
{
    auto result = std::make_shared<LegacyCardSelector>();
    auto basicCardSelector
        = std::dynamic_pointer_cast<InternalCardSelector>(cardSelector);

    result->mCardProtocol = basicCardSelector->getLogicalProtocolName();
    result->mPowerOnDataRegex = basicCardSelector->getPowerOnDataRegex();

    const auto isoCardSelector
        = std::dynamic_pointer_cast<InternalIsoCardSelector>(cardSelector);
    if (isoCardSelector) {
        result->mAid = isoCardSelector->getAid();
        result->mFileOccurrence = isoCardSelector->getFileOccurrence();
        result->mFileControlInformation
            = isoCardSelector->getFileControlInformation();
    } else {
        result->mAid = std::vector<uint8_t>(0);
        result->mFileOccurrence = CommonIsoCardSelector::FileOccurrence::FIRST;
        result->mFileControlInformation
            = CommonIsoCardSelector::FileControlInformation::FCI;
    }
    result->mSuccessfulSelectionStatusWords
        = cardSelectionRequestSpi->getSuccessfulSelectionStatusWords();

    return result;
}

std::shared_ptr<LegacyCardRequest>
InternalLegacyDto::mapToLegacyCardRequestV0(
    const std::shared_ptr<CardRequestSpi> cardRequest)
{
    auto result = std::make_shared<LegacyCardRequestV0>();

    result->mApduRequests
        = mapToLegacyApduRequests(cardRequest->getApduRequests());
    result->mIsStatusCodesVerificationEnabled
        = cardRequest->stopOnUnsuccessfulStatusWord();

    return result;
}

std::shared_ptr<LegacyCardRequest>
InternalLegacyDto::mapToLegacyCardRequestV1(
    const std::shared_ptr<CardRequestSpi> cardRequest)
{
    auto result = std::make_shared<LegacyCardRequestV1>();

    result->mApduRequests
        = mapToLegacyApduRequests(cardRequest->getApduRequests());
    result->mStopOnUnsuccessfulStatusWord
        = cardRequest->stopOnUnsuccessfulStatusWord();

    return result;
}

const std::vector<std::shared_ptr<LegacyApduRequest>>
InternalLegacyDto::mapToLegacyApduRequests(
    const std::vector<std::shared_ptr<ApduRequestSpi>>& apduRequests)
{
    std::vector<std::shared_ptr<LegacyApduRequest>> result;

    for (const auto& apduRequestSpi : apduRequests) {
        result.push_back(mapToLegacyApduRequest(apduRequestSpi));
    }

    return result;
}

std::shared_ptr<LegacyApduRequest>
InternalLegacyDto::mapToLegacyApduRequest(
    const std::shared_ptr<ApduRequestSpi> apduRequestSpi)
{
    auto result = std::make_shared<LegacyApduRequest>();

    result->mApdu = apduRequestSpi->getApdu();
    result->mInfo = apduRequestSpi->getInfo();
    result->mSuccessfulStatusWords = apduRequestSpi->getSuccessfulStatusWords();

    return result;
}

std::ostream&
operator<<(std::ostream& os, const InternalLegacyDto::LegacyCardSelector& lcs)
{
    std::stringstream ssAid;
    for (const auto val : lcs.mAid) {
        ssAid << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
              << static_cast<int>(val);
    }

    std::stringstream ssSw;
    for (auto it = std::begin(lcs.mSuccessfulSelectionStatusWords);
         it != std::end(lcs.mSuccessfulSelectionStatusWords);
         ++it) {
        ssSw << std::uppercase << std::hex << std::setfill('0') << std::setw(4)
             << static_cast<int>(*it);
        if (it != lcs.mSuccessfulSelectionStatusWords.end() - 1) {
            ssSw << ", ";
        }
    }

    os << "LEGACY_CARD_SELECTOR: {"
       << "CARD_PROTOCOL: " << lcs.mCardProtocol << ", "
       << "POWER_ON_DATA_REGEX: " << lcs.mPowerOnDataRegex << ", "
       << "AID: " << ssAid.str() << ", "
       << "FILE_OCCURRENCE: " << lcs.mFileOccurrence << ", "
       << "FILE_CONTROL_INFORMATION: " << lcs.mFileControlInformation << ", "
       << "SUCCESSFUL_SELECTION_STATUS_WORDS: " << ssSw.str() << "}";

    return os;
}

std::ostream&
operator<<(
    std::ostream& os,
    const std::shared_ptr<InternalLegacyDto::LegacyCardSelector> lcs)
{
    if (lcs == nullptr) {
        os << "LEGACY_CARD_SELECTOR: null";
    } else {
        os << *lcs;
    }

    return os;
}

std::ostream&
operator<<(std::ostream& os, const InternalLegacyDto::LegacyApduRequest& lar)
{
    std::stringstream ssApdu;
    for (const auto val : lar.mApdu) {
        ssApdu << std::uppercase << std::hex << std::setfill('0')
               << std::setw(2) << static_cast<int>(val);
    }

    std::stringstream ssSw;
    for (auto it = std::begin(lar.mSuccessfulStatusWords);
         it != std::end(lar.mSuccessfulStatusWords);
         ++it) {
        ssSw << std::uppercase << std::hex << std::setfill('0') << std::setw(4)
             << static_cast<int>(*it);
        if (it != lar.mSuccessfulStatusWords.end() - 1) {
            ssSw << ", ";
        }
    }

    os << "LEGACY_APDU_REQUEST: {"
       << "APDU: " << ssApdu.str() << ", "
       << "SUCCESSFUL_STATUS_WORD: " << ssSw.str() << ", "
       << "INFO: " << lar.mInfo << "}";

    return os;
}

std::ostream&
operator<<(
    std::ostream& os,
    const std::shared_ptr<InternalLegacyDto::LegacyApduRequest> lar)
{
    if (lar == nullptr) {
        os << "LEGACY_APDU_REQUEST: null";
    } else {
        os << *lar;
    }

    return os;
}

std::ostream&
operator<<(std::ostream& os, const InternalLegacyDto::LegacyCardRequestV0& lcr)
{
    os << "LEGACY_CARD_REQUEST_V0: {"
       << "APDU_REQUESTS: {";

    for (auto it = std::begin(lcr.mApduRequests);
         it != std::end(lcr.mApduRequests);
         ++it) {
        os << **it;
        if (it != lcr.mApduRequests.end() - 1) {
            os << ", ";
        }
    }

    os << "}, "
       << "IS_STATUS_CODES_VERIFICATION_ENABLED: "
       << lcr.mIsStatusCodesVerificationEnabled << "}";

    return os;
}

std::ostream&
operator<<(
    std::ostream& os,
    const std::shared_ptr<InternalLegacyDto::LegacyCardRequestV0> lcr)
{
    if (lcr == nullptr) {
        os << "LEGACY_CARD_REQUEST_V0: null";
    } else {
        os << *lcr;
    }

    return os;
}

std::ostream&
operator<<(std::ostream& os, const InternalLegacyDto::LegacyCardRequestV1& lcr)
{
    os << "LEGACY_CARD_REQUEST_V1: {"
       << "APDU_REQUESTS: {";

    for (auto it = std::begin(lcr.mApduRequests);
         it != std::end(lcr.mApduRequests);
         ++it) {
        os << **it;
        if (it != lcr.mApduRequests.end() - 1) {
            os << ", ";
        }
    }

    os << "}, "
       << "STOP_ON_UNSUCCESSFUL_STATUS_WORD: "
       << lcr.mStopOnUnsuccessfulStatusWord << "}";

    return os;
}

std::ostream&
operator<<(
    std::ostream& os,
    const std::shared_ptr<InternalLegacyDto::LegacyCardRequestV1> lcr)
{
    if (lcr == nullptr) {
        os << "LEGACY_CARD_REQUEST_V1: null";
    } else {
        os << *lcr;
    }

    return os;
}

std::ostream&
operator<<(
    std::ostream& os,
    const InternalLegacyDto::LegacyCardSelectionRequestV0& lcsr)
{
    os << "LEGACY_CARD_SELECTION_REQUEST_V0: {"
       << "CARD_SELECTOR: ";

    if (lcsr.mCardSelector == nullptr) {
        os << "null";
    } else {
        os << *lcsr.mCardSelector;
    }

    os << ", "
       << "CARD_REQUEST: ";

    if (lcsr.mCardRequest == nullptr) {
        os << "null";
    } else {
        os << *lcsr.mCardRequest;
    }

    os << "}";

    return os;
}

std::ostream&
operator<<(
    std::ostream& os,
    const std::shared_ptr<InternalLegacyDto::LegacyCardSelectionRequestV0> lcsr)
{
    if (lcsr == nullptr) {
        os << "LEGACY_CARD_SELECTION_REQUEST_V0: null";
    } else {
        os << *lcsr;
    }

    return os;
}

std::ostream&
operator<<(
    std::ostream& os,
    const InternalLegacyDto::LegacyCardSelectionRequestV1& lcsr)
{
    os << "LEGACY_CARD_SELECTION_REQUEST_V1: {"
       << "CARD_SELECTOR: ";

    if (lcsr.mCardSelector == nullptr) {
        os << "null";
    } else {
        os << *lcsr.mCardSelector;
    }

    os << ", "
       << "CARD_REQUEST: ";

    if (lcsr.mCardRequest == nullptr) {
        os << "null";
    } else {
        os << *lcsr.mCardRequest;
    }

    os << "}";

    return os;
}

std::ostream&
operator<<(
    std::ostream& os,
    const std::shared_ptr<InternalLegacyDto::LegacyCardSelectionRequestV1> lcsr)
{
    if (lcsr == nullptr) {
        os << "LEGACY_CARD_SELECTION_REQUEST_V1: null";
    } else {
        os << *lcsr;
    }

    return os;
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
