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

#include <map>
#include <memory>

#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keypop/reader/selection/CardSelectionResult.hpp"
#include "keypop/reader/selection/spi/SmartCard.hpp"

namespace keyple {
namespace core {
namespace service {

using keypop::reader::selection::CardSelectionResult;
using keypop::reader::selection::spi::SmartCard;

/**
 * Implementation of CardSelectionResult.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API CardSelectionResultAdapter final
: public CardSelectionResult {
public:
    /**
     * Constructor
     *
     * @since 2.0.0
     */
    CardSelectionResultAdapter();

    /**
     * Append a SmartCard to the internal list
     *
     * @param selectionIndex The index of the selection that resulted in the
     * smart card.
     * @param smartCard The smart card.
     * @since 2.0.0
     */
    void addSmartCard(
        const int selectionIndex, std::shared_ptr<SmartCard> smartCard);

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::map<int, std::shared_ptr<SmartCard>>&
    getSmartCards() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    std::shared_ptr<SmartCard> getActiveSmartCard() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    int getActiveSelectionIndex() const override;

private:
    /**
     *
     */
    int mActiveSelectionIndex;

    /**
     *
     */
    std::map<int, std::shared_ptr<SmartCard>> mSmartCardMap;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
