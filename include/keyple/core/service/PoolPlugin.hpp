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
#include <vector>

#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/service/Plugin.hpp"
#include "keypop/reader/CardReader.hpp"
#include "keypop/reader/selection/spi/SmartCard.hpp"

namespace keyple {
namespace core {
namespace service {

using keypop::reader::CardReader;
using keypop::reader::selection::spi::SmartCard;

/**
 * Plugin able to handle the access to an undefined number of CardReader.
 *
 * <p>It is typically used to define a plugin built on top of an HSM interface
 * that can allocate a large number of virtual reader slots.
 *
 * <p>A PoolPlugin can't be observable.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API PoolPlugin : virtual public Plugin {
public:
    /**
     * Gets a list of group references that identify a group of readers.
     *
     * <p>A group reference can represent a family of readers that all have the
     * same characteristics (for example, containing a SAM with identical key
     * sets).
     *
     * @return a list of String
     * @since 2.0.0
     */
    virtual const std::vector<std::string> getReaderGroupReferences() const = 0;

    /**
     * Gets a CardReader and makes it exclusive to the caller until the
     * releaseReader(CardReader) method is invoked.
     *
     * <p>The allocated reader belongs to the group targeted with provided
     * reference.
     *
     * @param readerGroupReference The reference of the group to which the
     * reader belongs (may be null depending on the implementation made).
     * @return A not null reference.
     * @throws KeyplePluginException If the allocation failed due to lack of
     * available reader.
     * @since 2.0.0
     */
    virtual std::shared_ptr<CardReader>
    allocateReader(const std::string& readerGroupReference) = 0;

    /**
     * Returns the selected SmartCard from a CardReader.
     *
     * <p>This method is used to retrieve the selected card from a reader that
     * has been allocated with the allocateReader(String) method.
     *
     * @param reader The reader from which to get the selected card.
     * @return null if no smart card is selected by default, the selected smart
     * card otherwise.
     * @throws IllegalArgumentException If the provided reader is null.
     * @since 2.2.0
     */
    virtual std::shared_ptr<SmartCard>
    getSelectedSmartCard(const std::shared_ptr<CardReader> reader) = 0;

    /**
     * Releases a Reader previously allocated with allocateReader.
     *
     * <p>This method must be invoked as soon as the reader is no longer needed
     * by the caller of allocateReader in order to free the resource.
     *
     * @param reader The Reader to be released.
     * @since 2.0.0
     */
    virtual void releaseReader(const std::shared_ptr<CardReader> reader) = 0;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
