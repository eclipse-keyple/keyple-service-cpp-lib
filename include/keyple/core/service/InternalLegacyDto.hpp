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
#include <string>
#include <vector>

#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keypop/card/spi/ApduRequestSpi.hpp"
#include "keypop/card/spi/CardRequestSpi.hpp"
#include "keypop/card/spi/CardSelectionRequestSpi.hpp"
#include "keypop/reader/selection/CardSelector.hpp"
#include "keypop/reader/selection/CommonIsoCardSelector.hpp"

namespace keyple {
namespace core {
namespace service {

using keypop::card::spi::ApduRequestSpi;
using keypop::card::spi::CardRequestSpi;
using keypop::card::spi::CardSelectionRequestSpi;
using keypop::reader::selection::CardSelector;
using keypop::reader::selection::CommonIsoCardSelector;

/**
 * Contains internal legacy DTOs used for serialization and deserialization
 * processes.
 *
 * <p>They are compliant with Core JSON API level 1 and 0.
 *
 * @since 3.0.0
 */
class KEYPLESERVICE_API InternalLegacyDto final {
public:
    /**
     * @since 2.1.1
     */
    class LegacyCardSelector final {
    public:
        /**
         *
         */
        std::string mCardProtocol;

        /**
         *
         */
        std::string mPowerOnDataRegex;

        /**
         *
         */
        std::vector<uint8_t> mAid;

        /**
         *
         */
        CommonIsoCardSelector::FileOccurrence mFileOccurrence;

        /**
         *
         */
        CommonIsoCardSelector::FileControlInformation mFileControlInformation;

        /**
         *
         */
        std::vector<int> mSuccessfulSelectionStatusWords;
    };

    /**
     * @since 3.3.1
     */
    class LegacyCardRequestV0 final {
    public:
        /**
         *
         */
        std::vector<std::shared_ptr<LegacyApduRequest>> mApduRequests;

        /**
         *
         */
        bool mIsStatusCodesVerificationEnabled;
    };

    /**
     * @since 2.1.1
     */
    class LegacyApduRequest {
    public:
        /**
         *
         */
        std::vector<uint8_t> mApdu;

        /**
         *
         */
        std::vector<int> mSuccessfulStatusWords;

        /**
         *
         */
        std::string mInfo;
    };

    /**
     * @since 2.1.1
     */
    class LegacyCardRequestV1 final {
    public:
        /**
         *
         */
        std::vector<std::shared_ptr<LegacyApduRequest>> mApduRequests;

        /**
         *
         */
        bool mStopOnUnsuccessfulStatusWord;
    };

    /**
     * @since 3.3.1
     */
    class LegacyCardSelectionRequestV0 final {
    public:
        /**
         *
         */
        std::shared_ptr<LegacyCardSelector> mCardSelector;

        /**
         *
         */
        std::shared_ptr<LegacyCardRequestV0> mCardRequest;
    };

    /**
     * @since 2.1.1
     */
    class LegacyCardSelectionRequestV1 final {
    public:
        /**
         *
         */
        std::shared_ptr<LegacyCardSelector> mCardSelector;

        /**
         *
         */
        std::shared_ptr<LegacyCardRequestV1> mCardRequest;
    };

    /**
     *
     */
    static const std::vector<std::shared_ptr<LegacyCardSelectionRequestV0>>
    mapToLegacyCardSelectionRequestsV0(
        const std::vector<std::shared_ptr<CardSelector>>& cardSelectors,
        const std::vector<std::shared_ptr<CardSelectionRequestSpi>>&
            cardSelectionRequests);

    /**
     *
     */
    static const std::vector<std::shared_ptr<LegacyCardSelectionRequestV1>>
    mapToLegacyCardSelectionRequestsV1(
        const std::vector<std::shared_ptr<CardSelector>>& cardSelectors,
        const std::vector<std::shared_ptr<CardSelectionRequestSpi>>&
            cardSelectionRequests);

    /**
     *
     */
    static std::shared_ptr<LegacyCardSelectionRequestV0>
    mapToLegacyCardSelectionRequestV0(
        std::shared_ptr<CardSelector> cardSelector,
        std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequestSpi);

    /**
     *
     */
    static std::shared_ptr<LegacyCardSelectionRequestV1>
    mapToLegacyCardSelectionRequestV1(
        std::shared_ptr<CardSelector> cardSelector,
        std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequestSpi);

    /**
     *
     */
    static std::shared_ptr<LegacyCardSelector> mapToLegacyCardSelector(
        std::shared_ptr<CardSelector> cardSelector,
        std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequestSpi);

    /**
     *
     */
    static std::shared_ptr<LegacyCardRequest>
    mapToLegacyCardRequestV0(const std::shared_ptr<CardRequestSpi> cardRequest);

    /**
     *
     */
    static std::shared_ptr<LegacyCardRequest>
    mapToLegacyCardRequestV1(const std::shared_ptr<CardRequestSpi> cardRequest);

    /**
     *
     */
    static const std::vector<std::shared_ptr<LegacyApduRequest>>
    mapToLegacyApduRequests(
        const std::vector<std::shared_ptr<ApduRequestSpi>>& apduRequests);

    /**
     *
     */
    static std::shared_ptr<LegacyApduRequest> mapToLegacyApduRequest(
        const std::shared_ptr<ApduRequestSpi> apduRequestSpi);

private:
    /**
     *
     */
    InternalLegacyDto();
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
