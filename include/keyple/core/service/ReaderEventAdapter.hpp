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

#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keypop/reader/CardReaderEvent.hpp"
#include "keypop/reader/selection/ScheduledCardSelectionsResponse.hpp"

namespace keyple {
namespace core {
namespace service {

using keypop::reader::CardReaderEvent;
using keypop::reader::selection::ScheduledCardSelectionsResponse;

using Type = CardReaderEvent::Type;

/**
 * Implementation of CardReaderEvent.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API ReaderEventAdapter final : public CardReaderEvent {
public:
    /**
     * CardReaderEvent constructor for simple insertion notification mode
     *
     * @param pluginName The name of the current plugin (should be not null).
     * @param readerName The name of the current reader (should be not null).
     * @param type The type of event (should be not null).
     * @param scheduledCardSelectionsResponse The responses received during the
     * execution of the card selection scenario (can be null).
     * @since 2.0.0
     */
    ReaderEventAdapter(
        const std::string& pluginName,
        const std::string& readerName,
        const Type type,
        std::shared_ptr<ScheduledCardSelectionsResponse>
            scheduledCardSelectionsResponse);

    /**
     * Returns the plugin name.
     *
     * @return A not empty String.
     * @since 2.0.0
     */
    const std::string& getPluginName() const;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::string& getReaderName() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    Type getType() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::shared_ptr<ScheduledCardSelectionsResponse>
    getScheduledCardSelectionsResponse() const override;

private:
    /**
     *
     */
    const std::string mPluginName;

    /**
     *
     */
    const std::string mReaderName;

    /**
     *
     */
    const std::shared_ptr<ScheduledCardSelectionsResponse>
        mScheduledCardSelectionsResponse;

    /**
     *
     */
    const Type mType;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
