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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "keyple/core/service/PluginEvent.hpp"
#include "keyple/core/service/spi/PluginObserverSpi.hpp"
#include "keyple/core/util/cpp/exception/RuntimeException.hpp"

using keyple::core::service::PluginEvent;
using keyple::core::service::spi::PluginObserverSpi;
using keyple::core::util::cpp::exception::RuntimeException;

class PluginObserverSpiMock final : public PluginObserverSpi {
public:
    explicit PluginObserverSpiMock(const std::shared_ptr<RuntimeException> e)
    : mThrowEx(e)
    {
    }

    void
    onPluginEvent(const std::shared_ptr<PluginEvent> pluginEvent) final
    {
        mEventTypeReceived.insert({pluginEvent->getType(), pluginEvent});
        if (mThrowEx) {
            throw *mThrowEx.get();
        }
    }

    bool
    hasReceived(const PluginEvent::Type eventType) const
    {
        return mEventTypeReceived.find(eventType) != mEventTypeReceived.end();
    }

    const std::shared_ptr<PluginEvent>
    getLastEventOfType(const PluginEvent::Type eventType) const
    {
        return mEventTypeReceived.at(eventType);
    }

private:
    std::map<PluginEvent::Type, std::shared_ptr<PluginEvent>>
        mEventTypeReceived;
    const std::shared_ptr<RuntimeException> mThrowEx;
};
