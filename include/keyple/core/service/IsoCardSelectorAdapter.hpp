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

#include <cstdint>
#include <string>
#include <vector>

#include "keyple/core/service/InternalIsoCardSelector.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keypop/reader/selection/IsoCardSelector.hpp"

namespace keyple {
namespace core {
namespace service {

using keypop::reader::selection::IsoCardSelector;

/**
 * Implementation of public {@link IsoCardSelector} API.
 *
 * @since 3.0.0
 */
class KEYPLESERVICE_API IsoCardSelectorAdapter final
: public IsoCardSelector,
  public InternalIsoCardSelector {
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
    const std::vector<uint8_t> getAid() const override;

    /**
     * {@inheritDoc}
     *
     * @since 3.0.0
     */
    FileOccurrence getFileOccurrence() const override;

    /**
     * {@inheritDoc}
     *
     * @since 3.0.0
     */
    FileControlInformation getFileControlInformation() const override;

    /**
     * {@inheritDoc}
     *
     * @since 3.0.0
     */
    IsoCardSelector& filterByDfName(const std::vector<uint8_t>& aid) override;

    /**
     * {@inheritDoc}
     *
     * @since 3.0.0
     */
    IsoCardSelector& filterByDfName(const std::string& aid) override;

    /**
     * {@inheritDoc}
     *
     * @since 3.0.0
     */
    IsoCardSelector&
    setFileOccurrence(const FileOccurrence fileOccurrence) override;

    /**
     * {@inheritDoc}
     *
     * @since 3.0.0
     */
    IsoCardSelector& setFileControlInformation(
        const FileControlInformation fileControlInformation) override;

    /**
     * {@inheritDoc}
     *
     * @since 3.0.0
     */
    IsoCardSelector&
    filterByCardProtocol(const std::string& logicalProtocolName) override;

    /**
     * {@inheritDoc}
     *
     * @since 3.0.0
     */
    IsoCardSelector&
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

    /**
     *
     */
    std::vector<uint8_t> mAid;

    /**
     *
     */
    FileOccurrence mFileOccurrence = FileOccurrence::FIRST;

    /**
     *
     */
    FileControlInformation mFileControlInformation
        = FileControlInformation::FCI;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
