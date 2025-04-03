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
#include <ostream>
#include <vector>

#include "keyple/core/service/ApduResponseAdapter.hpp"
#include "keyple/core/service/CardResponseAdapter.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keypop/card/CardResponseApi.hpp"

namespace keyple {
namespace core {
namespace service {

using keypop::card::CardResponseApi;

/**
 * This POJO contains an ordered list of the responses received following a card
 * request and indicators related to the status of the channel and the
 * completion of the card request.
 *
 * @see keypop::card::spi::CardRequestSpi
 * @since 2.0.0
 */
class KEYPLESERVICE_API CardResponseAdapter final : public CardResponseApi {
public:
    /**
     * Builds a card response from all {@link ApduResponseApi} received from the
     * card and booleans indicating if the logical channel is still open.
     *
     * @param apduResponses A not null list.
     * @param isLogicalChannelOpen true if the logical channel is open, false if
     * not.
     * @since 2.0.0
     */
    CardResponseAdapter(
        const std::vector<std::shared_ptr<ApduResponseApi>>& apduResponses,
        const bool isLogicalChannelOpen);

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<ApduResponseApi>>&
    getApduResponses() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    bool isLogicalChannelOpen() const override;

    /**
     *
     */
    friend std::ostream&
    operator<<(std::ostream& os, const CardResponseAdapter& cra);

    /**
     *
     */
    friend std::ostream& operator<<(
        std::ostream& os, const std::shared_ptr<CardResponseAdapter> cra);

private:
    /**
     *
     */
    const std::vector<std::shared_ptr<ApduResponseApi>> mApduResponses;

    /**
     *
     */
    const bool mIsLogicalChannelOpen;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
