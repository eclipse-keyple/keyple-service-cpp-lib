/**************************************************************************************************
 * Copyright (c) 2024 Calypso Networks Association https://calypsonet.org/                        *
 *                                                                                                *
 * This program and the accomping materials are made available under the *
 * terms of the MIT License which is available at https://opensource.org/licenses/MIT.            *
 *                                                                                                *
 * SPDX-License-Identifier: MIT *
 **************************************************************************************************/

#pragma once

#include <memory>

#include "keyple/core/service/BasicCardSelectorAdapter.hpp"
#include "keyple/core/service/CardSelectionManagerAdapter.hpp"
#include "keyple/core/service/IsoCardSelectorAdapter.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keypop/reader/ReaderApiFactory.hpp"
#include "keypop/reader/selection/CardSelectionManager.hpp"

namespace keyple {
namespace core {
namespace service {

using keypop::reader::ReaderApiFactory;
using keypop::reader::selection::BasicCardSelector;
using keypop::reader::selection::CardSelectionManager;
using keypop::reader::selection::IsoCardSelector;

/**
 * Implementation of {@link ReaderApiFactory}.
 *
 * @since 3.0.0
 */
class KEYPLESERVICE_API ReaderApiFactoryAdapter : public ReaderApiFactory {
public:
    /**
     * {@inheritDoc}
     *
     * @since 3.0.0
     */
    std::shared_ptr<CardSelectionManager> createCardSelectionManager() override;

    /**
     * {@inheritDoc}
     *
     * @since 3.0.0
     */
    std::shared_ptr<BasicCardSelector> createBasicCardSelector() override;

    /**
     * {@inheritDoc}
     *
     * @since 3.0.0
     */
    std::shared_ptr<IsoCardSelector> createIsoCardSelector() override;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
