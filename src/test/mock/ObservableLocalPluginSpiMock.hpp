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
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "keyple/core/common/KeyplePluginExtension.hpp"
#include "keyple/core/plugin/PluginIOException.hpp"
#include "keyple/core/plugin/spi/ObservablePluginSpi.hpp"
#include "keyple/core/plugin/spi/reader/ReaderSpi.hpp"

/* Mock */
#include "mock/ReaderSpiMock.hpp"

using keyple::core::common::KeyplePluginExtension;
using keyple::core::plugin::PluginIOException;
using keyple::core::plugin::spi::ObservablePluginSpi;
using keyple::core::plugin::spi::reader::ReaderSpi;

using testing::Return;

class ObservableLocalPluginSpiMock final : public KeyplePluginExtension,
                                           public ObservablePluginSpi {
public:
    ObservableLocalPluginSpiMock(
        const std::string& name,
        const std::shared_ptr<PluginIOException> pluginError)
    : mName(name)
    , mPluginError(pluginError)
    {
    }

    int
    getMonitoringCycleDuration() const override
    {
        return mMonitoringCycleDuration;
    }

    const std::string&
    getName() const override
    {
        return mName;
    }

    void
    onUnregister() override
    {
        /* Nothing to do */
    }

    const std::vector<std::string>
    searchAvailableReaderNames() override
    {
        if (mPluginError != nullptr) {
            throw *mPluginError.get();
        }

        std::lock_guard<std::mutex> lock(mMutex);

        std::vector<std::string> readerNames;
        std::transform(
            mStubReaders.begin(),
            mStubReaders.end(),
            back_inserter(readerNames),
            [](const auto& reader) { return reader.first; });

        return readerNames;
    }

    std::shared_ptr<ReaderSpi>
    searchReader(const std::string& readerName) override
    {
        if (mPluginError != nullptr) {
            throw *mPluginError.get();
        }

        std::lock_guard<std::mutex> lock(mMutex);

        const auto it = mStubReaders.find(readerName);
        if (it != mStubReaders.end()) {
            return it->second;
        }

        return nullptr;
    }

    const std::vector<std::shared_ptr<ReaderSpi>>
    searchAvailableReaders() override
    {
        std::lock_guard<std::mutex> lock(mMutex);

        std::vector<std::shared_ptr<ReaderSpi>> readers;
        std::transform(
            mStubReaders.begin(),
            mStubReaders.end(),
            back_inserter(readers),
            [](const auto& reader) { return reader.second; });

        return readers;
    }

    void
    addReaderName(const std::vector<std::string>& names)
    {
        std::lock_guard<std::mutex> lock(mMutex);

        for (const auto& readerName : names) {
            auto r = std::make_shared<ReaderSpiMock>(readerName);
            EXPECT_CALL(*r.get(), onUnregister).WillRepeatedly(Return());
            EXPECT_CALL(*r.get(), closePhysicalChannel)
                .WillRepeatedly(Return());
            mStubReaders.insert({readerName, r});
        }
    }

    void
    removeReaderName(const std::vector<std::string>& names)
    {
        std::lock_guard<std::mutex> lock(mMutex);

        for (const auto& readerName : names) {
            mStubReaders.erase(readerName);
        }
    }

private:
    const std::string mName;
    const int mMonitoringCycleDuration = 0;
    std::map<std::string, std::shared_ptr<ReaderSpi>> mStubReaders;
    const std::shared_ptr<PluginIOException> mPluginError;
    std::mutex mMutex;
};
