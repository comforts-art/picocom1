cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE TRUE)


include(third-part.cmake)
project(nr-testMAC C)

if(ARM_PLATFORM)
 message(STATUS "target is arm")
 set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc)
endif(ARM_PLATFORM)

if(USE_32BITS)
 message(STATUS "using 32bits")
 set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
 
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
 
else()
endif(USE_32BITS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -MD -O1 -Wall  -Wextra")
#set(LD_FLAGS "${LD_FLAGS} -fprofile-arcs -ftest-coverage")


if(BUILD_ATLL_SHM)
    add_subdirectory(atl)
    if(BUILD_ATLL_TESTS)
        add_subdirectory(atl/test)
    endif(BUILD_ATLL_TESTS)
else()
    execute_process(COMMAND grep -i ubuntu /etc/os-release
    OUTPUT_VARIABLE out_is_ubuntu
    ERROR_VARIABLE  err_is_ubunut
    RESULT_VARIABLE IS_UBUNTU)
    
    execute_process(COMMAND grep -i centos /etc/os-release
    OUTPUT_VARIABLE out_is_centos
    ERROR_VARIABLE  err_is_centos
    RESULT_VARIABLE IS_CENTOS)
    
    if(${IS_CENTOS} EQUAL 0)
        message(WARN "IS CENTOS OS")
        include(pcie_driver_centos.cmake)
    else()
        if(${IS_UBUNTU} EQUAL 0)
            message(WARN "IS UBUNTU OS")
if(ARM_PLATFORM)
            include(pcie_driver_ubuntu_arm.cmake)
else()
            include(pcie_driver_ubuntu.cmake)
endif(ARM_PLATFORM)
        else()
            message(FATAL_ERROR "Not supported OS except CentOS and Ubunut !")
        endif(${IS_UBUNTU} EQUAL 0)
    endif(${IS_CENTOS} EQUAL 0)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PCIE_DRIVER_C_FLAGS}")
endif(BUILD_ATLL_SHM)

if(BUILD_UE_SIM_TESTS)
    enable_testing()
    add_subdirectory(ue_sim/test)
endif(BUILD_UE_SIM_TESTS)

add_subdirectory(common)
add_subdirectory(src)
add_subdirectory(mac)
