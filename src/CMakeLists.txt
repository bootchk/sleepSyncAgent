
cmake_minimum_required(VERSION 3.5.1)

project(sleepSyncAgent CXX ASM)


# Use cmake modules nRF5 specific
set(CMAKE_MODULE_PATH "/home/bootch/git/nRF5Cmake/")
message("${CMAKE_MODULE_PATH}")


# environment (location of NRF_SDK and other tools)
# i.e. file CMakeEnv.cmake on the MODULE_PATH
include("CMakeEnv")

# scripts specific to nRF5x building
# Defines cmake macros prefixed with "nRF5"
# Sets many cmake vars for tool flags
include("nRF5")

nRF5CheckSetupPreconditions()
nRF5SetBuildOptions()
nRF5SetIncludePaths()
# nRF5SetSDKSourcesBasic()

# Also BLE
# nRF5SDKSourcesBLE()



# Primary artifacts: libraries cross compiled in various configs

# no startup sources or linker scripts

# No need for separate library for 52810 or 52810e emulated (that is required for executable)
# Yes need separate library for wSD? i.e w/ provisioning
# TODO

# NRF52832
add_library(sleepSyncAgent52832 "")

set_target_properties( sleepSyncAgent52832
    PROPERTIES
       CHIP "nrf52832_xxaa"
       SOFTDEVICE "none"
       FLOAT_ABI "soft"
    )
nRF5CheckTargetProperties(sleepSyncAgent52832)
nRF5ConfigTargetByProperties(sleepSyncAgent52832)

target_compile_definitions( sleepSyncAgent52832 PUBLIC "NRF52" )
target_compile_definitions( sleepSyncAgent52832 PUBLIC "TASKS" )


add_library(sleepSyncAgent52xNoSD "")
set_target_properties( sleepSyncAgent52xNoSD
    PROPERTIES
       CHIP "nrf52832_xxaa"
       SOFTDEVICE "none"
       FLOAT_ABI "soft"
    )
nRF5CheckTargetProperties(sleepSyncAgent52xNoSD)
nRF5ConfigTargetByProperties(sleepSyncAgent52xNoSD)

target_compile_definitions( sleepSyncAgent52xNoSD PUBLIC "NRF52" )
target_compile_definitions( sleepSyncAgent52xNoSD PUBLIC "TASKS" )



# import libs from outside project

add_library(radioSoC52wSD STATIC IMPORTED)
set_target_properties( radioSoC52wSD 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/radioSoC/Debug52wSD/libradioSoC52wSD.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/radioSoC/src"
    )
add_library(radioSoC52810NoSD STATIC IMPORTED)
set_target_properties( radioSoC52810NoSD 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/radioSoC/cmakeBuild/libradioSoC52810NoSD.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/radioSoC/src"
        IMPORTED_LINK_INTERFACE_LIBRARIES nRF5x52810
    )


# No dependence on nRF5x, since it is already linked into radioSoC


# TODO No dependence on embeddedMath, since it is already linked into radioSoC
add_library(embeddedMath STATIC IMPORTED)
set_target_properties( embeddedMath
   PROPERTIES 
       IMPORTED_LOCATION /home/bootch/git/embeddedMath/Debug52/libembeddedMath52.a
       INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/embeddedMath/src" )


add_library(NRFDrivers STATIC IMPORTED)
set_target_properties( NRFDrivers PROPERTIES IMPORTED_LOCATION /home/bootch/git/libNRFDrivers/Debug/libNRFDrivers.a )


add_library(BLEProvisionee STATIC IMPORTED)
set_target_properties( BLEProvisionee 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/libBLEProvisionee/Debug/libBLEProvisionee.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/libBLEProvisionee/objects"
    )
# Same library but built for 52810
add_library(BLEProvisionee52810 STATIC IMPORTED)
set_target_properties( BLEProvisionee52810 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/libBLEProvisionee/Debug/libBLEProvisionee52810.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/libBLEProvisionee/objects"
    )
# Same library but built for 52810e emulated
add_library(BLEProvisionee52810e STATIC IMPORTED)
set_target_properties( BLEProvisionee52810e 
    PROPERTIES 
        IMPORTED_LOCATION /home/bootch/git/libBLEProvisionee/Debug/libBLEProvisionee52810e.a
        INTERFACE_INCLUDE_DIRECTORIES "/home/bootch/git/libBLEProvisionee/objects"
    )



IF(EXISTS /home/bootch/git/radioSoC/Debug52/libradioSoC52.a)
else()
    message("FATAL_ERROR libradioSoC52.a not exist")
ENDIF()

IF(EXISTS /home/bootch/git/nRF5x/Debug52/libnRF5x52.a )
else()
    message("FATAL_ERROR libnRF5x52.a not exist")
ENDIF()

IF(EXISTS /home/bootch/git/embeddedMath/Debug52/libembeddedMath52.a)
else()
    message("FATAL_ERROR libembeddedMath52.a not exist")
ENDIF()



# ??? order is important unless we specify interlibrary dependencies
target_link_libraries(sleepSyncAgent52832 
PRIVATE
    radioSoC52wSD
    embeddedMath
    )

#     nRF5x52
#   BLEProvisionee
#    NRFDrivers
  

target_link_libraries(sleepSyncAgent52xNoSD
PUBLIC
    radioSoC52810NoSD
    embeddedMath
    )
   
   
   
# sources 
#     
# SDK startup sources are property of target
# other SDK sources are built into libraries e.g. SEGGER_RTT in radioSoC
# and certain SDK modules in NRFDrivers
list(APPEND Sources
       "${CMAKE_CURRENT_LIST_DIR}/globals.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/globals.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/syncAgent/syncAgent.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/syncAgentImp/syncAgentImp.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/syncAgentImp/syncAgentLoop.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/syncAgentImp/syncAgentRole.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/syncAgentImp/syncAgentToApp.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/syncAgentImp/state/role.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/syncAgentImp/state/syncMode.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/syncAgentImp/state/syncMode.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/clique/clique.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/clique/cliqueReversion.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/clique/cliqueScatter.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/clique/cliqueRoles.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/clique/periodTime.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/clique/schedule.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/cliqueHistory/cliqueHistory.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/cliqueMerger/cliqueMerger.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/fishPolicy/deepFishingPolicy.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/fishPolicy/syncRecoveryfishPolicy.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/fishPolicy/slottedFishSession.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/logging/flashLogger.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/logging/logger.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/logging/remoteLogger.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/message/message.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/message/serializer.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/messageHandler/commonMessageHandler.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/messageHandler/deepFishingMessageHandler.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/messageHandler/fishSlotMsgHandler.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/messageHandler/syncSlotMsgHandler.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/messageHandler/messageDispatcher.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/modules/deltaSync.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/modules/syncBehaviour.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/modules/syncOffset.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/modules/syncPowerManager.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/modules/syncSender.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/network/granularity.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/network/intraCliqueManager.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/network/scatter.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/network/sendRepeater.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/network/topology.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/policy/syncPolicy/adaptiveXmitSyncPolicy.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/policy/dropoutMonitor.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/policy/fishWaitingPolicy.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/receiver/receiver.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/schedule/syncSchedule.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/schedule/radioPrelude.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/schedule/sleepDuration.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/schedule/taskTimer.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/sleepers/oversleepMonitor.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/sleepers/scheduleSleeper.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/sleepers/syncPowerSleeper.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/sleepers/syncSleeper.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/slots/fish/fishSlot.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/slots/fishing/fishingDoneCallbacks.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/slots/fishing/fishingManager.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/slots/fishing/fishSchedule.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/slots/fishing/fishSlot.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/slots/info/infoSlot.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/slots/merge/mergePolicy.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/slots/merge/mergeSchedule.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/slots/merge/mergeSlot.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/slots/sync/syncSlot.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/slots/sync/syncSlotProperty.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/slots/syncing/syncSlotKinds.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/slots/syncing/syncSubSlots.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/slots/syncing/syncWorkSlot.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/slots/syncing/syncSlotSchedule.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/slots/abandonMastershipSlot.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/syncPeriod/combinedSyncPeriod.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/syncPeriod/modalSyncPeriod.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/task/fishTask.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/task/mergerTask.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/task/provisionTask.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/task/task.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/work/workIn.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/work/workOut.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/work/workControlProxy.cpp"
   )
# "${CMAKE_CURRENT_LIST_DIR}/message/messageFactory.cpp"

list(APPEND ProvisioningSources
    "${CMAKE_CURRENT_LIST_DIR}/network/topologyProvisioner.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/provisioning/provisioningPublisher.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/syncPeriod/provisionedSyncPeriod.cpp"
)

# TODO provisioned version
# TODO name 52810SD
target_sources(sleepSyncAgent52832
   PUBLIC
      ${Sources}
   )
target_sources(sleepSyncAgent52xNoSD
   PUBLIC
      ${Sources}
   )


#nRF5GenerateOtherArtifacts(sleepSyncAgent52810)
# not generate FLASH_SOFTDEVICE target, already created by sniffer
# nRF5AddCustomTargets(sleepSyncAgent52810)


