sources = files(
	'main.c',
        'testmac_cfgDes.c',
        'engine.c',
        'shell.c'
)


ldflags = []
cflags = ['-D__PROJECT_VERSION="' + meson.project_version() + '"']
cflags += '-DALLOW_EXPERIMENTAL_API'

deps = [dpdk, common]

if host_machine.cpu_family().startswith('arm') or host_machine.cpu_family().startswith('aarch')
deps += [cc.find_library('zlog', required: false, dirs: join_paths(meson.source_root(),'third_part/lib_arm'))]
deps += [cc.find_library('ev', required: false, dirs: join_paths(meson.source_root(),'third_part/lib_arm'))]
else
deps += [cc.find_library('zlog', required: false, dirs: join_paths(meson.source_root(),'third_part/lib'))]
deps += [cc.find_library('ev', required: false, dirs: join_paths(meson.source_root(),'third_part/lib'))]
endif
deps += [cc.find_library('rte_net_i40e', required: false)]
deps += [cc.find_library('rte_net_ixgbe', required: false)]
deps += [cc.find_library('rte_net_ice', required: false)]
deps += [cc.find_library('rte_bus_vdev', required: false)]
#deps += [cc.find_library('rte_net_pc802',required: false)]

deps += [dependency('threads')]
deps += [cc.find_library('dl', required: false)]
deps += [cc.find_library('m', required: false)]

public_headers = include_directories('../inc')
common_headers = include_directories('../common/inc')
libev_headers = include_directories('../third_part/libev')
zlog_headers = include_directories('../third_part/zlog/src')
tcg_headers = include_directories('../tcg/inc')
share_headers = include_directories('../share/FAPI_Head')
share2_headers = include_directories('../share')

testmac = executable('testmac',
		sources,
		c_args: cflags,
		link_args: ldflags,
		install: true,
		dependencies: [dpdk, mac, deps, dpdk_pc802, tcg],
                include_directories: [public_headers, common_headers, zlog_headers, libev_headers, tcg_headers, share_headers, share2_headers])
