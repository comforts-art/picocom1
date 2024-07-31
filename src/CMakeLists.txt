set(CMAKE_VERBOSE_MAKEFILE TRUE)

include_directories(../common/inc)
include_directories(../share)
include_directories(${LIBEV_INC_DIR})
include_directories(${LIBZLOG_INC_DIR})
include_directories(../inc)
include_directories(../mac/inc)
if(BUILD_ATLL_PCIE)
    include_directories(${PCIE_DRIVER_INCLUDE_DIR})
endif(BUILD_ATLL_PCIE)
add_definitions(-Wno-format-security -Wno-strict-aliasing)
add_definitions(-Wno-unused-but-set-variable)
add_definitions(-DTESTMAC_GIT_COMMIT_SHA1="$ENV{testMac_comm_id}")
add_definitions(-DNR_TESTMAC_VER="$ENV{testMac_ver}") 
if(BUILD_ATLL_SHM)
    add_definitions(-DATLL_SHM)
else()
    add_definitions(-DATLL_PCIE)
endif(BUILD_ATLL_SHM)
file (GLOB SRC_LIST "*.c")
add_executable(test_mac ${SRC_LIST}
)

file(GLOB ConfigFiles ${CMAKE_CURRENT_SOURCE_DIR}/test_CPHY ${CMAKE_CURRENT_SOURCE_DIR}/*.conf)

MESSAGE (STATUS "This is the current dir:" ${ConfigFiles})

foreach(ConfigFile ${ConfigFiles})
  add_custom_command(TARGET test_mac PRE_BUILD 
       COMMAND cp -f -r ${ConfigFile} $<TARGET_FILE_DIR:test_mac>)
endforeach()
if(BUILD_ATLL_SHM)
    target_link_libraries(test_mac -Wl,--start-group cmm_static  mac_static atl_v1_static -Wl,--end-group ${LIB_EV} ${LIB_ZLOG} pthread m rt)
else()
    target_link_libraries(test_mac -Wl,--start-group cmm_static  mac_static -Wl,--end-group ${LIB_EV} ${LIB_ZLOG} ${PCIE_DRIVER_LIBS})
endif(BUILD_ATLL_SHM)

