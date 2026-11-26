#******************************************************************************
#* Copyright (c) 2025 Calypso Networks Association https://calypsonet.org/    *
#*                                                                            *
#* See the NOTICE file(s) distributed with this work for additional           *
#* information regarding copyright ownership.                                 *
#*                                                                            *
#* This program and the accompanying materials are made available under the   *
#* terms of the Eclipse Public License 2.0 which is available at              *
#* http://www.eclipse.org/legal/epl-2.0                                       *
#*                                                                            *
#* SPDX-License-Identifier: EPL-2.0                                           *
#******************************************************************************/


SET(LIBRARY_NAME keypleservicecpplib)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DKEYPLESERVICE_EXPORT")

FIND_PACKAGE(Threads REQUIRED)

# Dependencies are supplied by Conan (requires) via CMakeDeps
FIND_PACKAGE(keyple-common-cpp-api REQUIRED CONFIG)
FIND_PACKAGE(keyple-plugin-cpp-api REQUIRED CONFIG)
FIND_PACKAGE(keyple-util-cpp-lib REQUIRED CONFIG)
FIND_PACKAGE(keypop-card-cpp-api REQUIRED CONFIG)
FIND_PACKAGE(keypop-reader-cpp-api REQUIRED CONFIG)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/BasicCardSelectorAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardInsertionActiveMonitoringJobAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardInsertionPassiveMonitoringJobAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardRemovalActiveMonitoringJobAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardRemovalPassiveMonitoringJobAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardResponseAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardSelectionManagerAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardSelectionResponseAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardSelectionResultAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardSelectionScenarioAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InternalDto.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IsoCardSelectorAdapter.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/ReaderApiFactoryAdapter.cpp
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

    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

TARGET_LINK_LIBRARIES(

    ${LIBRARY_NAME}

    PUBLIC

    Keyple::Common
    Keyple::Plugin
    Keyple::Util
    Keypop::Card
    Keypop::Reader
)

ADD_LIBRARY(

    Keyple::Service

    ALIAS

    ${LIBRARY_NAME}
)

INSTALL(

    TARGETS ${LIBRARY_NAME}

    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

INSTALL(

    DIRECTORY ${PROJECT_SOURCE_DIR}/include/

    DESTINATION include
)
