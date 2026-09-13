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

#include "keyple/core/service/BasicCardSelectorAdapter.hpp"

#include <memory>
#include <sstream>
#include <string>

namespace keyple {
namespace core {
namespace service {

const std::string&
BasicCardSelectorAdapter::getLogicalProtocolName() const
{
    return mLogicalProtocolName;
}

const std::string&
BasicCardSelectorAdapter::getPowerOnDataRegex() const
{
    return mPowerOnDataRegex;
}

BasicCardSelector&
BasicCardSelectorAdapter::filterByCardProtocol(
    const std::string& logicalProtocolName)
{
    mLogicalProtocolName = logicalProtocolName;

    return *this;
}

BasicCardSelector&
BasicCardSelectorAdapter::filterByPowerOnData(
    const std::string& powerOnDataRegex)
{
    mPowerOnDataRegex = powerOnDataRegex;

    return *this;
}

std::ostream&
operator<<(std::ostream& os, const BasicCardSelectorAdapter& bcsa)
{
    os << "BASIC_CARD_SELECTOR_ADAPTER: {"
       << "LOGICAL_PROTOCOL_NAME: " << bcsa.mLogicalProtocolName << ", "
       << "POWER_ON_DATA_REGEX: " << bcsa.mPowerOnDataRegex << "}";

    return os;
}

/**
 *
 */
std::ostream&
operator<<(
    std::ostream& os, const std::shared_ptr<BasicCardSelectorAdapter> bcsa)
{
    if (bcsa == nullptr) {
        os << "BASIC_CARD_SELECTOR_ADAPTER: null";
    } else {
        os << *bcsa;
    }

    return os;
}
} /* namespace service */
} /* namespace core */
} /* namespace keyple */
