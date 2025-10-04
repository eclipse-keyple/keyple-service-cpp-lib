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

#include <algorithm>
#include <map>
#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "keyple/core/util/cpp/exception/RuntimeException.hpp"
#include "keypop/reader/CardReaderEvent.hpp"
#include "keypop/reader/spi/CardReaderObserverSpi.hpp"

using keyple::core::util::cpp::exception::RuntimeException;
using keypop::reader::CardReaderEvent;
using keypop::reader::spi::CardReaderObserverSpi;

class ReaderObserverSpiMock final : public CardReaderObserverSpi {
public:
    explicit ReaderObserverSpiMock(const std::shared_ptr<RuntimeException> e)
    : mThrowEx(e)
    {
    }

    void
    onReaderEvent(const std::shared_ptr<CardReaderEvent> readerEvent) override
    {
        mEventTypeReceived.insert({readerEvent->getType(), readerEvent});
        if (mThrowEx != nullptr) {
            throw *mThrowEx.get();
        }
    }

    bool
    hasReceived(const CardReaderEvent::Type eventType)
    {
        return std::any_of(
            mEventTypeReceived.begin(), mEventTypeReceived.end(), [&](auto p) {
                return eventType == p.first;
            });
    }

    const std::shared_ptr<CardReaderEvent>
    getLastEventOfType(const CardReaderEvent::Type eventType)
    {
        const auto it = mEventTypeReceived.find(eventType);
        if (it != mEventTypeReceived.end()) {
            return it->second;
        }

        return nullptr;
    }

private:
    std::map<CardReaderEvent::Type, std::shared_ptr<CardReaderEvent>>
        mEventTypeReceived;
    std::shared_ptr<RuntimeException> mThrowEx;
};
