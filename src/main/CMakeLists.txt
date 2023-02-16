#/*************************************************************************************************
# * Copyright (c) 2023 Calypso Networks Association https://calypsonet.org/                       *
# *                                                                                               *
# * See the NOTICE file(s) distributed with this work for additional information regarding        *
# * copyright ownership.                                                                          *
# *                                                                                               *
# * This program and the accompanying materials are made available under the terms of the Eclipse *
# * Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0                 *
# *                                                                                               *
# * SPDX-License-Identifier: EPL-2.0                                                              *
# *************************************************************************************************/

SET(LIBRARY_NAME keypleservicecpplib)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DKEYPLESERVICE_EXPORT")

FIND_PACKAGE(Threads REQUIRED)

ADD_LIBRARY(
    ${LIBRARY_NAME}

    ${LIBRARY_TYPE}

    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractMonitoringJobAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractPluginAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractReaderAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractObservableLocalPluginAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractObservableStateAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ApduResponseAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AutonomousObservableLocalPluginAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardInsertionActiveMonitoringJobAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardInsertionPassiveMonitoringJobAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardRemovalActiveMonitoringJobAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardRemovalPassiveMonitoringJobAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardResponseAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardSelectionManagerAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardSelectionResponseAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardSelectionResultAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardSelectionScenarioAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalConfigurableReaderAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalPluginAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalPoolPluginAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalReaderAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MonitoringState.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalConfigurableReaderAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalPluginAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalReaderAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableReaderStateServiceAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PluginEventAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ReaderEventAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ScheduledCardSelectionsResponseAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SmartCardServiceAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SmartCardServiceProvider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WaitForCardInsertionStateAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WaitForCardProcessingStateAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WaitForCardRemovalStateAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WaitForStartDetectStateAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ExecutorService.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/Job.cpp
)

TARGET_INCLUDE_DIRECTORIES(
    ${LIBRARY_NAME}

    PUBLIC

    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/spi
)

TARGET_LINK_LIBRARIES(
    ${LIBRARY_NAME}

    PUBLIC

    CalypsoNet::TerminalCard
    CalypsoNet::TerminalReader
    Keyple::CommonApi
    Keyple::PluginApi
    Keyple::Util
    Threads::Threads
)

ADD_LIBRARY(Keyple::Service ALIAS ${LIBRARY_NAME})
