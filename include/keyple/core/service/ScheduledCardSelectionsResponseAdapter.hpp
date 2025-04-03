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
#include <vector>

#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keypop/card/CardSelectionResponseApi.hpp"
#include "keypop/reader/selection/ScheduledCardSelectionsResponse.hpp"

namespace keyple {
namespace core {
namespace service {

using keypop::card::CardSelectionResponseApi;
using keypop::reader::selection::ScheduledCardSelectionsResponse;

/**
 * POJO containing the card selection responses received during the card
 * selection process.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API ScheduledCardSelectionsResponseAdapter
: public ScheduledCardSelectionsResponse {
public:
    /**
     * Constructor
     *
     * @param cardSelectionResponses The card selection responses.
     * @since 2.0.0
     */
    explicit ScheduledCardSelectionsResponseAdapter(
        const std::vector<std::shared_ptr<CardSelectionResponseApi>>&
            cardSelectionResponses);

    /**
     * Gets the card responses.
     *
     * @return A list of CardSelectionResponseApi.
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<CardSelectionResponseApi>>&
    getCardSelectionResponses() const;

private:
    /**
     *
     */
    const std::vector<std::shared_ptr<CardSelectionResponseApi>>
        mCardSelectionResponses;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
