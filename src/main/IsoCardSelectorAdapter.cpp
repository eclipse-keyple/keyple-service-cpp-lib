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

#include "keyple/core/service/IsoCardSelectorAdapter.hpp"

#include <string>
#include <vector>

#include "keyple/core/util/HexUtil.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::util::HexUtil;

const std::string&
IsoCardSelectorAdapter::getLogicalProtocolName() const
{
    return mLogicalProtocolName;
}

const std::string&
IsoCardSelectorAdapter::getPowerOnDataRegex() const
{
    return mPowerOnDataRegex;
}

const std::vector<uint8_t>
IsoCardSelectorAdapter::getAid() const
{
    return mAid;
}

FileOccurrence
IsoCardSelectorAdapter::getFileOccurrence() const
{
    return mFileOccurrence;
}

FileControlInformation
IsoCardSelectorAdapter::getFileControlInformation() const
{
    return mFileControlInformation;
}

IsoCardSelector&
IsoCardSelectorAdapter::filterByDfName(const std::vector<uint8_t>& aid)
{
    mAid = aid;

    return *this;
}

IsoCardSelector&
IsoCardSelectorAdapter::filterByDfName(const std::string& aid)
{
    mAid = HexUtil::toByteArray(aid);

    return *this;
}

IsoCardSelector&
IsoCardSelectorAdapter::setFileOccurrence(const FileOccurrence fileOccurrence)
{
    mFileOccurrence = fileOccurrence;

    return *this;
}

IsoCardSelector&
IsoCardSelectorAdapter::setFileControlInformation(
    const FileControlInformation fileControlInformation)
{
    mFileControlInformation = fileControlInformation;

    return *this;
}

IsoCardSelector&
IsoCardSelectorAdapter::filterByCardProtocol(
    const std::string& logicalProtocolName)
{
    mLogicalProtocolName = logicalProtocolName;

    return *this;
}

IsoCardSelector&
IsoCardSelectorAdapter::filterByPowerOnData(const std::string& powerOnDataRegex)
{
    mPowerOnDataRegex = powerOnDataRegex;

    return *this;
}

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
