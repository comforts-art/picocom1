# PICOCOM PROPRIETARY INFORMATION
#
# This software is supplied under the terms of a license agreement or
# nondisclosure agreement with PICOCOM and may not be copied
# or disclosed except in accordance with the terms of that agreement.
#
# Copyright PICOCOM.
# additional target to perform copy third part libs


# get all project files

set(LIBEV_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_part/libev")
set(LIBZLOG_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_part/zlog/src")

if(USE_32BITS)
 set(LIB_ZLOG ${CMAKE_CURRENT_SOURCE_DIR}/third_part/lib32/libzlog.a)
 set(LIB_EV ${CMAKE_CURRENT_SOURCE_DIR}/third_part/lib32/libev.a)
 
else()
if(ARM_PLATFORM)
 set(LIB_ZLOG ${CMAKE_CURRENT_SOURCE_DIR}/third_part/lib_arm/libzlog.a)
 set(LIB_EV ${CMAKE_CURRENT_SOURCE_DIR}/third_part/lib_arm/libev.a)
else()

 set(LIB_ZLOG ${CMAKE_CURRENT_SOURCE_DIR}/third_part/lib/libzlog.a)
 set(LIB_EV ${CMAKE_CURRENT_SOURCE_DIR}/third_part/lib/libev.a)
endif(ARM_PLATFORM)
endif(USE_32BITS)
#MESSAGE (STATUS "current dir" ${CMAKE_CURRENT_SOURCE_DIR})


#file(GLOB LibFiles ${CMAKE_CURRENT_SOURCE_DIR}/third_part/lib/*.a)



#MESSAGE (STATUS "get libs" ${LibFiles})
#add_custom_target(
#        third_part_libs
#	COMMAND mkdir libs
#        COMMAND copy_if_different ${LibFiles} libs/
#)
