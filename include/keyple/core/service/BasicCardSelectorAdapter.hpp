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

#include <string>

#include "keyple/core/service/InternalCardSelector.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keypop/reader/selection/BasicCardSelector.hpp"

namespace keyple {
namespace core {
namespace service {

using keypop::reader::selection::BasicCardSelector;

/**
 * Implementation of public BasicCardSelector API.
 *
 * @since 3.0.0
 */
class KEYPLESERVICE_API BasicCardSelectorAdapter final
: public BasicCardSelector,
  public InternalCardSelector {
public:
    /**
     * {@inheritDoc}
     *
     * @since 3.0.0
     */
    const std::string& getLogicalProtocolName() const override;

    /**
     * {@inheritDoc}
     *
     * @since 3.0.0
     */
    const std::string& getPowerOnDataRegex() const override;

    /**
     * {@inheritDoc}
     *
     * @since 3.0.0
     */
    BasicCardSelector&
    filterByCardProtocol(const std::string& logicalProtocolName) override;

    /**
     * {@inheritDoc}
     *
     * @since 3.0.0
     */
    BasicCardSelector&
    filterByPowerOnData(const std::string& powerOnDataRegex) override;

private:
    /**
     *
     */
    std::string mLogicalProtocolName;

    /**
     *
     */
    std::string mPowerOnDataRegex;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
