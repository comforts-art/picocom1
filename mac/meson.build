sources = files(
	'src/l1_stub_buf.c',
	'src/l1TraceAgent.c',
	'src/mac.c',
	'src/mac_cell.c',
	'src/mac_rx.c',
	'src/macTrace.c',
	'src/macTx.c',
	'src/mac_ue.c'
)

public_headers = include_directories('inc')
libev_headers = include_directories('../third_part/libev')
zlog_headers = include_directories('../third_part/zlog/src')
share_headers = include_directories('../share/FAPI_Head')
share2_headers = include_directories('../share')
common_headers = include_directories('../common/inc')
tcg_headers = include_directories('../tcg/inc')
libmac = library('mac', sources,
	dependencies: [common, dpdk], c_args: '-DEAL_THREAD_MODE',  include_directories: [public_headers, common_headers, libev_headers, zlog_headers, share_headers, share2_headers, tcg_headers])
mac = declare_dependency(link_with: libmac,
	include_directories: include_directories('./inc'))

