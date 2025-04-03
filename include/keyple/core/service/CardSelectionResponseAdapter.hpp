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
#include <string>

#include "keyple/core/service/ApduResponseAdapter.hpp"
#include "keyple/core/service/CardResponseAdapter.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keypop/card/CardSelectionResponseApi.hpp"

namespace keyple {
namespace core {
namespace service {

using keypop::card::CardSelectionResponseApi;

/**
 * This POJO contains the data from a card obtained in response to a card
 * selection request.
 *
 * <p>These data are the selection status and the responses, if any, to the
 * additional APDUs sent to the card CardResponseAdapter).
 *
 * @see keypop::card::spi::CardSelectionRequestSpi
 * @since 2.0.0
 */
class KEYPLESERVICE_API CardSelectionResponseAdapter final
: public CardSelectionResponseApi {
public:
    /**
     * Builds a card selection response including the selection status and a
     * CardResponseAdapter (list of keypop::card::ApduResponseAdapter).
     *
     * @param powerOnData The card power-on data, null if the power-on data is
     * not available.
     * @param selectApplicationResponse The response to the Select Application
     * command, null if no Select Application command was performed...
     * @param hasMatched True if the card inserted matches the selection
     * filters.
     * @param cardResponse null if no card response is available.
     * @since 2.0.0
     */
    CardSelectionResponseAdapter(
        const std::string& powerOnData,
        const std::shared_ptr<ApduResponseAdapter> selectApplicationResponse,
        const bool hasMatched,
        const std::shared_ptr<CardResponseAdapter> cardResponse);

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::string& getPowerOnData() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::shared_ptr<ApduResponseApi>
    getSelectApplicationResponse() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    bool hasMatched() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::shared_ptr<CardResponseApi> getCardResponse() const override;

    /**
     *
     */
    friend std::ostream&
    operator<<(std::ostream& os, const CardSelectionResponseAdapter& csra);

    /**
     *
     */
    friend std::ostream& operator<<(
        std::ostream& os,
        const std::shared_ptr<CardSelectionResponseAdapter> csra);

private:
    /**
     *
     */
    const std::string mPowerOnData;

    /**
     *
     */
    const std::shared_ptr<ApduResponseAdapter> mSelectApplicationResponse;

    /**
     *
     */
    const bool mHasMatched;

    /**
     *
     */
    const std::shared_ptr<CardResponseAdapter> mCardResponse;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
