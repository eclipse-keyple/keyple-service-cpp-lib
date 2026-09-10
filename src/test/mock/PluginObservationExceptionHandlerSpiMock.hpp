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
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "keyple/core/service/spi/PluginObservationExceptionHandlerSpi.hpp"
#include "keyple/core/util/cpp/exception/Exception.hpp"
#include "keyple/core/util/cpp/exception/RuntimeException.hpp"

using keyple::core::service::spi::PluginObservationExceptionHandlerSpi;
using keyple::core::util::cpp::exception::Exception;
using keyple::core::util::cpp::exception::RuntimeException;

class PluginObservationExceptionHandlerSpiMock final
: public PluginObservationExceptionHandlerSpi {
public:
    explicit PluginObservationExceptionHandlerSpiMock(
        std::unique_ptr<RuntimeException> throwEx)
    : mInvoked(false)
    , mThrowEx(std::move(throwEx))
    {
    }

    void
    onPluginObservationError(
        const std::string& pluginName, std::unique_ptr<Exception> e) final
    {
        mInvoked = true;
        if (mThrowEx != nullptr) {
            throw *mThrowEx.get();
        }

        mPluginName = pluginName;
        mE = std::move(e);
    }

    bool
    isInvoked() const
    {
        return mInvoked;
    }

    const std::string&
    getPluginName() const
    {
        return mPluginName;
    }

    const Exception*
    getE() const
    {
        return mE ? mE.get() : nullptr;
    }

private:
    bool mInvoked = false;
    std::string mPluginName;
    std::unique_ptr<Exception> mE;
    std::unique_ptr<RuntimeException> mThrowEx;
};
