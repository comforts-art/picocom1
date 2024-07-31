# PICOCOM PROPRIETARY INFORMATION
#
# This software is supplied under the terms of a license agreement or
# nondisclosure agreement with PICOCOM and may not be copied
# or disclosed except in accordance with the terms of that agreement.
#
# Copyright PICOCOM.
#!/bin/sh
export DPDK_LIB=/root/dpdk_arm_libs
FULL_LIB_DPDK_PC=`find $DPDK_LIB -name libdpdk.pc`
export PKG_CONFIG_PATH=`dirname $FULL_LIB_DPDK_PC`
export testMac_ver="v3.1.0.0"
export testMac_comm_id=`git log|head -n 1|awk '{printf $2}'`

cmake . -Bbuild_PCIe
#cmake    --build build
#cmake -DBUILD_ATLL_TESTS=ON -DINTERNAL_TEST=ON --DBUILD_FAPI_TESTS=ON -DBUILD_TCG_TESTS=ON -DBUILD_UE_SIM_TESTS=ON --build build
cmake -DINTERNAL_TEST=ON -DBUILD_TCG_TESTS=ON -DARM_PLATFORM=ON -DBUILD_ATLL_PCIE=ON --build build_PCIe
cd build_PCIe
make clean
make
#rm -rf *

#popd
