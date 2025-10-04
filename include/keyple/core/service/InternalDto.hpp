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
#include "keypop/card/CardSelectionResponseApi.hpp"
#include "keypop/card/spi/CardSelectionExtensionSpi.hpp"
#include "keypop/card/spi/CardSelectionRequestSpi.hpp"
#include "keypop/card/spi/SmartCardSpi.hpp"
#include "keypop/reader/selection/spi/CardSelectionExtension.hpp"

namespace keyple {
namespace core {
namespace service {

using keypop::card::CardSelectionResponseApi;
using keypop::card::spi::CardSelectionExtensionSpi;
using keypop::card::spi::CardSelectionRequestSpi;
using keypop::card::spi::SmartCardSpi;
using keypop::reader::selection::spi::CardSelectionExtension;

/**
 * Contains internal DTOs used for serialization and deserialization processes.
 *
 * @since 2.1.1
 */
class KEYPLESERVICE_API InternalDto {
public:
    /**
     * Local implementation of CardSelectionExtension and
     * CardSelectionExtensionSpi.
     */
    class CardSelectionAdapter final : public CardSelectionExtension,
                                       public CardSelectionExtensionSpi {
    public:
        /**
         * Default constructor.
         *
         * @since 2.1.1
         */
        CardSelectionAdapter() = default;

        /**
         * (package-private)<br>
         * Builds a new instance using the provided source object.
         *
         * @param src The source.
         * @since 2.1.1
         */
        explicit CardSelectionAdapter(
            const std::shared_ptr<CardSelectionExtensionSpi> src);

        /**
         *
         */
        const std::shared_ptr<CardSelectionRequestSpi>
        getCardSelectionRequest() const override;

        /**
         *
         */
        const std::shared_ptr<SmartCardSpi>
        parse(const std::shared_ptr<CardSelectionResponseApi>
                  cardSelectionResponseApi) const override;

    private:
        /**
         *
         */
        std::shared_ptr<CardSelectionRequest> mCardSelectionRequest;
    };

    /**
     * Local implementation of {@link CardSelectionRequestSpi}.
     *
     * @since 2.1.1
     */
    class CardSelectionRequest final : public CardSelectionRequestSpi {
    public:
    private:
        /**
         *
         */
        std::shared_ptr<CardRequest> mCardRequest;

        /**
         *
         */
        std::vector<int> mCuccessfulSelectionStatusWords;

        /**
         * Default constructor.
         *
         * @since 2.1.1
         */
        CardSelectionRequest() = default;

        /**
         * Builds a new instance using the provided source object.
         *
         * @param src The source.
         * @since 2.1.1
         */
        explicit CardSelectionRequest(
            const std::shared_ptr<CardSelectionRequestSpi> src);

        /**
         *
         */
        const std::vector<int>
        getSuccessfulSelectionStatusWords() const override;

        /**
         *
         */
        const std::shared_ptr<CardRequestSpi> getCardRequest() const override;
    };

    /**
     * Local implementation of CardRequestSpi.
     *
     * @since 2.1.1
     */
    class CardRequest final : public CardRequestSpi {
    public:
        /**
         * Default constructor.
         *
         * @since 2.1.1
         */
        CardRequest() = default;

        /**
         * Builds a new instance using the provided source object.
         *
         * @param src The source.
         * @since 2.1.1
         */
        explicit CardRequest(const std::shared_ptr<CardRequestSpi> src);

        /**
         *
         */
        const std::vector<std::shared_ptr<ApduRequestSpi>>&
        getApduRequests() const override;

        /**
         *
         */
        bool stopOnUnsuccessfulStatusWord() const override;

        pruvate :
            /**
             *
             */
            std::vector<std::shared_ptr<ApduRequest>>
                mApduRequests;

        /**
         *
         */
        bool mStopOnUnsuccessfulStatusWord = false;
    };

    /**
     * Local implementation of {@link ApduRequestSpi}.
     *
     * @since 2.1.1
     */
    static class ApduRequest implements ApduRequestSpi {
    public:
        /**
         * Default constructor.
         *
         * @since 2.1.1
         */
        ApduRequest()
        {
        }

        /**
         * Builds a new instance using the provided source object.
         *
         * @param src The source.
         * @since 2.1.1
         */
        ApduRequest(const std::shared_ptr<ApduRequestSpi> src);

        /**
         *
         */
        const std::vector<uint8_t>& getApdu() const override;

        /**
         *
         */
        const std::vector<int>& getSuccessfulStatusWords() const override;

        /**
         *
         */
        const std::string& getInfo() const override;

    private:
        /**
         *
         */
        const std::vector<uint8_t> mApdu;

        /**
         *
         */
        const std::vector<int> mSuccessfulStatusWords;

        /**
         *
         */
        const std::string mInfo;
    };

private:
    /**
     * Private constructor.
     */
    InternalDto();
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
