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
#include <typeinfo>
#include <vector>

#include "keyple/core/common/KeypleReaderExtension.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/service/MultiSelectionProcessing.hpp"
#include "keyple/core/util/cpp/LoggerFactory.hpp"
#include "keypop/card/CardResponseApi.hpp"
#include "keypop/card/CardSelectionResponseApi.hpp"
#include "keypop/card/ChannelControl.hpp"
#include "keypop/card/ProxyReaderApi.hpp"
#include "keypop/card/spi/CardRequestSpi.hpp"
#include "keypop/card/spi/CardSelectionRequestSpi.hpp"
#include "keypop/reader/CardReader.hpp"
#include "keypop/reader/cpp/CardSelectorBase.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::common::KeypleReaderExtension;
using keyple::core::util::cpp::Logger;
using keyple::core::util::cpp::LoggerFactory;
using keypop::card::CardResponseApi;
using keypop::card::CardSelectionResponseApi;
using keypop::card::ChannelControl;
using keypop::card::ProxyReaderApi;
using keypop::card::spi::CardRequestSpi;
using keypop::card::spi::CardSelectionRequestSpi;
using keypop::reader::CardReader;
using keypop::reader::cpp::CardSelectorBase;

/**
 * Abstract class for all readers.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API AbstractReaderAdapter : virtual public CardReader,
                                                public ProxyReaderApi {
public:
    /**
     * Constructor.
     *
     * @param readerName The name of the reader.
     * @param readerExtension The associated reader extension SPI.
     * @param pluginName The name of the plugin.
     * @since 2.0.0
     */
    AbstractReaderAdapter(
        const std::string& readerName,
        std::shared_ptr<KeypleReaderExtension> readerExtension,
        const std::string& pluginName);

    /**
     *
     */
    virtual ~AbstractReaderAdapter() = default;

    /**
     * Gets the plugin name.
     *
     * @return A not empty String.
     * @since 2.0.0
     */
    const std::string& getPluginName() const;

    /**
     * Performs a selection scenario following a card detection.
     *
     * <p>Each scenario selection case consists in checking if the card matches
     * the profile defined in the CardSelectionRequestSpi and possibly sending
     * the optional commands provided.<br> The cases are processed sequentially
     * in the order in which they are found in the list.<br> Processing
     * continues or stops after the first successful selection according to the
     * policy defined by MultiSelectionProcessing}.<br> At the end of the
     * treatment of each case, the channel is left open or is closed according
     * to the channel control policy.
     *
     * C++: method should be final but cannot perform UTs if so...
     *
     * @param cardSelectors A list of CardSelector.
     * @param cardSelectionRequests A list of selection cases composed of one or
     * more {@link CardSelectionRequestSpi}.
     * @param multiSelectionProcessing The multi selection policy.
     * @param channelControl The channel control policy.
     * @return An empty list if no response was received.
     * @throw ReaderBrokenCommunicationException if the communication with the
     * reader has failed.
     * @throw CardBrokenCommunicationException if the communication with the
     * card has failed.
     * @since 2.0.0
     */
    virtual const std::vector<std::shared_ptr<CardSelectionResponseApi>>
    transmitCardSelectionRequests(
        const std::vector<std::shared_ptr<CardSelectorBase>>& cardSelectors,
        const std::vector<std::shared_ptr<CardSelectionRequestSpi>>&
            cardSelectionRequests,
        const MultiSelectionProcessing multiSelectionProcessing,
        const ChannelControl channelControl);

    /**
     * Check if the reader status is "registered".
     *
     * @throw IllegalStateException is thrown when reader is not or no longer
     * registered.
     * @since 2.0.0
     */
    void checkStatus() const;

    /**
     * Changes the reader status to registered.
     *
     * @since 2.0.0
     */
    void doRegister();

    /**
     * Changes the reader status to unregistered if is not already unregistered.
     *
     * <p>This method may be overridden in order to meet specific needs in
     * certain implementations of readers.
     *
     * @since 2.0.0
     */
    virtual void doUnregister();

    /**
     * Abstract method performing the actual card selection process.
     *
     * @param cardSelectors A list of CardSelector.
     * @param cardSelectionRequests A list of selection cases composed of one or
     * more CardSelectionRequestSpi.
     * @param multiSelectionProcessing The multi selection policy.
     * @param channelControl The channel control policy.
     * @return A not empty list containing at most as many responses as there
     * are selection cases.
     * @throw ReaderBrokenCommunicationException if the communication with the
     * reader has failed.
     * @throw CardBrokenCommunicationException if the communication with the
     * card has failed.
     * @throw UnexpectedStatusWordException If status word verification is
     * enabled in the card request and the card returned an unexpected code.
     * @since 2.0.0
     */
    virtual std::vector<std::shared_ptr<CardSelectionResponseApi>>
    processCardSelectionRequests(
        const std::vector<std::shared_ptr<CardSelectorBase>>& cardSelectors,
        const std::vector<std::shared_ptr<CardSelectionRequestSpi>>&
            cardSelectionRequests,
        const MultiSelectionProcessing multiSelectionProcessing,
        const ChannelControl channelControl)
        = 0;

    /**
     * Abstract method performing the actual transmission of the card request.
     *
     * @param cardRequest The card request.
     * @param channelControl The channel control policy to apply.
     * @return A not null reference.
     * @throw ReaderBrokenCommunicationException if the communication with the
     * reader has failed.
     * @throw CardBrokenCommunicationException if the communication with the
     * card has failed.
     * @throw UnexpectedStatusWordException If status word verification is
     * enabled in the card request and the card returned an unexpected code.
     * @since 2.0.0
     */
    virtual std::shared_ptr<CardResponseApi> processCardRequest(
        const std::shared_ptr<CardRequestSpi> cardRequest,
        const ChannelControl channelControl)
        = 0;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::string& getName() const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    std::shared_ptr<KeypleReaderExtension>
    getExtension(const std::type_info& readerExtensionClass) const;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::shared_ptr<CardResponseApi> transmitCardRequest(
        const std::shared_ptr<CardRequestSpi> cardRequest,
        const ChannelControl channelControl) final;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger
        = LoggerFactory::getLogger(typeid(AbstractReaderAdapter));

    /**
     *
     */
    const std::string mReaderName;

    /**
     *
     */
    std::shared_ptr<KeypleReaderExtension> mReaderExtension;

    /**
     *
     */
    const std::string mPluginName;

    /**
     *
     */
    bool mIsRegistered;

    /**
     *
     */
    uint64_t mBefore;
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
