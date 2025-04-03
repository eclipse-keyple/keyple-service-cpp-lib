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

#include <memory>
#include <vector>

#include "keyple/core/service/InternalCardSelector.hpp"

namespace keyple {
namespace core {
namespace service {

InternalLegacyDto::InternalLegacyDto()
{
}

const std::vector<std::shared_ptr<LegacyCardSelectionRequest>>
InternalLegacyDto::mapToLegacyCardSelectionRequests(
    const std::vector<std::shared_ptr<CardSelector>>& cardSelectors,
    const std::vector<std::shared_ptr<CardSelectionRequestSpi>>&
        cardSelectionRequests)
{
    std::vector<std::shared_ptr<LegacyCardSelectionRequest>> result;

    for (int i = 0; i < cardSelectors.size(); i++) {
        result.push_back(mapToLegacyCardSelectionRequest(
            cardSelectors[i], cardSelectionRequests[i]));
    }

    return result;
}

std::shared_ptr<LegacyCardSelectionRequest>
InternalLegacyDto::mapToLegacyCardSelectionRequest(
    std::shared_ptr<CardSelector> cardSelector,
    std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequestSpi)
{
    auto result = std::make_shared<LegacyCardSelectionRequest>();

    result->mCardRequest = cardSelectionRequestSpi->getCardRequest() != nullptr
                               ? mapToLegacyCardRequest(
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
InternalLegacyDto::mapToLegacyCardRequest(
    const std::shared_ptr<CardRequestSpi> cardRequest)
{
    auto result = std::make_shared<LegacyCardRequest>();

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

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
