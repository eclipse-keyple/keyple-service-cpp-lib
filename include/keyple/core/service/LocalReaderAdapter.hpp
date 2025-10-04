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

#include "keyple/core/plugin/spi/reader/ReaderSpi.hpp"
#include "keyple/core/service/AbstractReaderAdapter.hpp"
#include "keyple/core/service/ApduResponseAdapter.hpp"
#include "keyple/core/service/CardResponseAdapter.hpp"
#include "keyple/core/service/InternalIsoCardSelector.hpp"
#include "keyple/core/service/KeypleServiceExport.hpp"
#include "keyple/core/util/cpp/KeypleStd.hpp"
#include "keypop/card/spi/ApduRequestSpi.hpp"
#include "keypop/card/spi/CardSelectionRequestSpi.hpp"
#include "keypop/reader/selection/CardSelector.hpp"
#include "keypop/reader/selection/CommonIsoCardSelector.hpp"
#include "keypop/reader/selection/FileControlInformation.hpp"
#include "keypop/reader/selection/FileOccurrence.hpp"

namespace keyple {
namespace core {
namespace service {

using keyple::core::plugin::spi::reader::ReaderSpi;
using keypop::card::spi::ApduRequestSpi;
using keypop::card::spi::CardSelectionRequestSpi;
using keypop::reader::selection::CardSelector;
using keypop::reader::selection::CommonIsoCardSelector;
using keypop::reader::selection::FileControlInformation;
using keypop::reader::selection::FileOccurrence;

/**
 * Local reader adapter.
 *
 * <ul>
 *   <li>RL-CMD-USED.1
 *   <li>RL-CLA-ACCEPTED.1
 *   <li>RL-PERF-TIME.1
 * </ul>
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API LocalReaderAdapter : public AbstractReaderAdapter {
public:
    /**
     * Constructor.
     *
     * @param readerSpi The reader SPI.
     * @param pluginName The name of the plugin.
     * @since 2.0.0
     */
    LocalReaderAdapter(
        std::shared_ptr<ReaderSpi> readerSpi, const std::string& pluginName);

    /**
     *
     */
    virtual ~LocalReaderAdapter() = default;

    /**
     * Gets ReaderSpi associated to this reader.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    std::shared_ptr<ReaderSpi> getReaderSpi() const;

    /**
     * Gets the logical channel's opening state.
     *
     * @return True if the channel is open, false if not.
     * @since 2.0.0
     */
    bool isLogicalChannelOpen() const;

    /**
     * Close both logical and physical channels
     *
     * <p>This method doesn't raise any exception.
     *
     * @since 2.0.0
     */
    void closeLogicalAndPhysicalChannelsSilently();

    /**
     * {@inheritDoc}
     *
     * <p>Invoke  ReaderSpi#onUnregister() on the associated SPI.
     *
     * @since 2.0.0
     */
    void doUnregister() override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    std::vector<std::shared_ptr<CardSelectionResponseApi>>
    processCardSelectionRequests(
        const std::vector<std::shared_ptr<CardSelectorBase>>& cardSelectors,
        const std::vector<std::shared_ptr<CardSelectionRequestSpi>>&
            cardSelectionRequests,
        const MultiSelectionProcessing multiSelectionProcessing,
        const ChannelControl channelControl) final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    std::shared_ptr<CardResponseApi> processCardRequest(
        const std::shared_ptr<CardRequestSpi> cardRequest,
        const ChannelControl channelControl) final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    bool isContactless() final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    bool isCardPresent() override;

    /**
     * Activates a protocol (for configurable reader only).
     *
     * @param readerProtocol The reader protocol.
     * @param applicationProtocol The corresponding application protocol to
     * associate.
     *
     * @since 2.0.0
     */
    void activateReaderProtocol(
        const std::string& readerProtocol,
        const std::string& applicationProtocol);

    /**
     * Deactivates a protocol (for configurable reader only).
     *
     * @param readerProtocol The reader protocol.
     *
     * @since 2.0.0
     */
    void deactivateReaderProtocol(const std::string& readerProtocol);

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void releaseChannel() final;

    /**
     * (private)<br>
     * Local implementation of ApduRequestSpi.
     */
    class ApduRequest : public ApduRequestSpi {
    public:
        friend class LocalReaderAdapter;

        /**
         *
         */
        virtual ~ApduRequest() = default;

        /**
         *
         */
        std::vector<uint8_t> getApdu() const override;

        /**
         *
         */
        const std::vector<int>& getSuccessfulStatusWords() const override;

        /**
         *
         */
        const std::string& getInfo() const override;

        /**
         *
         */
        friend std::ostream&
        operator<<(std::ostream& os, const ApduRequest& ar);

        /**
         *
         */
        friend std::ostream&
        operator<<(std::ostream& os, const std::shared_ptr<ApduRequest> ar);

        /**
         *
         */
        friend std::ostream& operator<<(
            std::ostream& os,
            const std::vector<std::shared_ptr<ApduRequest>>& ars);

    private:
        /**
         *
         */
        static const int DEFAULT_SUCCESSFUL_CODE;

        /**
         *
         */
        std::vector<uint8_t> mApdu;

        /**
         *
         */
        const std::vector<int> mSuccessfulStatusWords;

        /**
         *
         */
        std::string mInfo;

        /**
         *
         */
        explicit ApduRequest(const std::vector<uint8_t>& apdu);

        /**
         *
         */
        ApduRequest& setInfo(const std::string& info);
    };

protected:
    /**
     * @return null or the name of the physical protocol used for the last card
     * communication.
     */
    const std::string& getCurrentPhysicalProtocolName() const;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger
        = LoggerFactory::getLogger(typeid(LocalReaderAdapter));

    /**
     * Predefined ISO values
     */
    static const int SW_6100;

    static const int SW_6C00;

    static const int SW1_MASK;
    static const int SW2_MASK;

    /**
     *
     */
    std::shared_ptr<ReaderSpi> mReaderSpi;

    /**
     *
     */
    uint64_t mBefore;

    /**
     *
     */
    bool mIsLogicalChannelOpen;

    /**
     *
     */
    bool mUseDefaultProtocol;

    /**
     *
     */
    std::string mCurrentLogicalProtocolName;

    /**
     *
     */
    std::string mCurrentPhysicalProtocolName;

    /**
     *
     */
    std::map<const std::string, const std::string> mProtocolAssociations;

    /**
     * This POJO contains the card selection status.
     */
    class SelectionStatus {
    public:
        friend LocalReaderAdapter;

        /**
         * Constructor.
         *
         * @param powerOnData A String containing the power-on data (optional).
         * @param selectApplicationResponse The response to the select
         * application command (optional).
         * @param hasMatched A boolean.
         */
        SelectionStatus(
            const std::string& powerOnData,
            const std::shared_ptr<ApduResponseAdapter>
                selectApplicationResponse,
            const bool hasMatched);

    private:
        /**
         *
         */
        const std::string mPowerOnData;

        /**
         *
         */
        const std::shared_ptr<ApduResponseAdapter> mSelectApplicationResponse;

        /**
         *
         */
        const bool mHasMatched;
    };

    /**
     * Determines the current protocol used by the card.
     *
     * <p>The Map mProtocolAssociations containing the protocol names (reader
     * and application) is iterated and the reader protocol (key of the Map) is
     * checked with the reader.<br>
     *
     * <p>If the Map is not empty:
     * <li>The boolean mUseDefaultProtocol is set to false.
     * <li>If the test provided by the reader SPI is positive (the protocol
     * presented is the one used by the current card) then the field
     * currentLogicalProtocolName is set with the name of the protocol known to
     * the application. <li>If none of the protocols present in the Map matches
     * then the currentLogicalProtocolName is set to null.
     * </ul>
     *
     * <p>If the Map is empty, no other check is done, the String field
     * currentLogicalProtocolName is set to null and the boolean field
     * useDefaultProtocol is set to true.
     */
    void computeCurrentProtocol();

    /**
     * Close the logical channel.
     */
    void closeLogicalChannel();

    /**
     * Computes the P2 parameter of the ISO7816-4 Select Application APDU
     * command from the provided FileOccurrence and FileControlInformation.
     *
     * @param fileOccurrence The file's position relative to the current file.
     * @param fileControlInformation The file control information output.
     * @throw IllegalStateException If one of the provided argument is
     * unexpected.
     */
    uint8_t computeSelectApplicationP2(
        const FileOccurrence fileOccurrence,
        const FileControlInformation fileControlInformation);

    /**
     * Sends the select application command to the card and returns the
     * requested data according to AidSelector attributes (ISO7816-4 selection
     * data) into an {@link ApduResponseAdapter}.
     *
     * @param cardSelector The card selector.
     * @return A not null {@link ApduResponseAdapter}.
     * @throw ReaderIOException if the communication with the reader has failed.
     * @throw CardIOException if the communication with the card has failed.
     */
    std::shared_ptr<ApduResponseAdapter> processExplicitAidSelection(
        std::shared_ptr<InternalIsoCardSelector> cardSelector);

    /**
     * Selects the card with the provided AID and gets the FCI response in
     * return.
     *
     * @param cardSelector The card selector.
     * @return A not null ApduResponseAdapter containing the FCI.
     * @see processSelection(CardSelector, CardSelectionRequestSpi)
     */
    std::shared_ptr<ApduResponseAdapter>
    selectByAid(std::shared_ptr<InternalIsoCardSelector> cardSelector);

    /**
     * Checks the provided power-on data with the PowerOnDataFilter.
     *
     * <p>Returns true if the power-on data is accepted by the filter.
     *
     * @param powerOnData A String containing the power-on data.
     * @param cardSelector The card selector.
     * @return True or false.
     * @throw IllegalStateException if no power-on data is available and the
     * PowerOnDataFilter is set.
     * @see processSelection(CardSelector, CardSelectionRequestSpi)
     */
    bool checkPowerOnData(
        const std::string& powerOnData,
        std::shared_ptr<InternalCardSelector> cardSelector);

    /**
     * Select the card according to the CardSelector.
     *
     * <p>The selection status is returned.<br>
     * 3 levels of filtering/selection are applied successively if they are enabled: protocol, power
     * on data and AID.<br>
     * As soon as one of these operations fails, the method returns with a
     * failed selection status.
     *
     * <p>Conversely, the selection is considered successful if none of the
     * filters have rejected the card, even if none of the filters are active.
     *
     * @param cardSelector A not null CardSelector.
     * @param cardSelectionRequest A not null CardSelectionRequestSpi.
     * @return A not null {@link SelectionStatus}.
     * @throw ReaderBrokenCommunicationException If the communication with the
     * reader has failed.
     * @throw CardBrokenCommunicationException If the communication with the
     * card has failed.
     */
    std::shared_ptr<SelectionStatus> processSelection(
        std::shared_ptr<CardSelectorBase> cardSelector,
        std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequest);

    /**
     * Attempts to select the card and executes the optional requests if any.
     *
     * @param cardSelectionRequest The CardSelectionRequestSpi to be processed.
     * @param channelControl The channelControl.
     * @return A not null reference.
     * @throw ReaderBrokenCommunicationException If the communication with the
     * reader has failed.
     * @throw CardBrokenCommunicationException If the communication with the
     * card has failed.
     * @throw UnexpectedStatusWordException If status word verification is
     * enabled in the card request and the card returned an unexpected code.
     */
    std::shared_ptr<CardSelectionResponseApi> processCardSelectionRequest(
        std::shared_ptr<CardSelectorBase> cardSelector,
        std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequest,
        const ChannelControl channelControl);

    /**
     * Transmits an ApduRequestSpi and receives the ApduResponseAdapter.
     *
     * <p>The time measurement is carried out and logged with the detailed
     * information of the exchanges (TRACE level).
     *
     * @param apduRequest The APDU request to transmit.
     * @return A not null reference.
     * @throw ReaderIOException if the communication with the reader has failed.
     * @throw CardIOException if the communication with the card has failed.
     */
    std::shared_ptr<ApduResponseAdapter>
    processApduRequest(const std::shared_ptr<ApduRequestSpi> apduRequest);

    /**
     *
     */
    friend std::ostream& operator<<(std::ostream& os, ApduRequest& ar);

    /**
     *
     */
    friend std::ostream&
    operator<<(std::ostream& os, const std::shared_ptr<ApduRequest> ar);
};

} /* namespace service */
} /* namespace core */
} /* namespace keyple */
