/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef __OAM_INTERFACE_H__
#define __OAM_INTERFACE_H__
#include <stdint.h>

#define ETH_MAC_ADDR_LEN 6
#define MAX_CELL_NUM 2
#define MAX_ANT_NUM_PER_CELL 4
#define MAX_ANTEN_NUM (MAX_CELL_NUM*MAX_ANT_NUM_PER_CELL)
#define MAX_TAG_NUM 22

typedef enum
{
    PHY_VERSION_GET_REQ,
	PHY_VERSION_GET_RSP,
	PHY_CELLSTATE_GET_REQ,
	PHY_CELLSTATE_GET_RSP,
	PC802_TEMP_GET_REQ,
	PC802_TEMP_GET_RSP,
    PHY_OAM_MSG_MAX
}OAM_PHY_API_MSGID;

typedef enum
{
    ECPRI_GET_REQ = 1000,
    ECPRI_GET_RESP,
    ECPRI_SET_REQ,
    ECPRI_SET_RESP,
    ECPRI_TEST_NTFY,
    ECPRI_ERR_IND,
}OAM_eCPRI_API_MSGID;

typedef enum
{
    MSG_OK,
    MSG_INVALID_STATE,
}OAM_ERROR_CODE;
typedef enum
{
    TAG_BASIC_CFG = 0x1000,//basic msg
    TAG_PTP_CFG,
    TAG_COMP_CFG,
    TAG_MTU_CFG,
    TAG_RTX_CFG,
    TAG_RU_CFG,
    TAG_DU_INTERNAL_CFG,
    TAG_DU_CFG,
    TAG_DU_KPIS,
    TAG_DU_TSU_KPIS,//debug msg
    TAG_DU_PTP_KPIS,
    TAG_DU_SLOT_RX,
    TAG_DU_DEBUG_CFG,
    TAG_DU_PERF_TX,
    TAG_DU_PERF_CNT_TX,
    TAG_DU_PERF_RX,
    TAG_DU_PERF_CNT_RX,
    TAG_ECPRI_NET_LINKUP_CFG,
    TAG_ECPRI_RX_HWA_ERROR,
    TAG_MAC_READ,
    TAG_PCS_READ,
    TAG_SFP_READ,

}OAM_TLV_TAG_VAL;


typedef struct
{
    uint32_t start_flag;
    uint32_t msg_type;
    uint32_t seq_id;
    uint32_t msg_num;
    uint16_t Msg_id;
	uint16_t Msg_size;
}OamMsgHdr_t;

typedef struct
{
    uint16_t Tag;
    uint16_t Length; /*in bytes*/
}OamTlv_t;

typedef struct
{
    uint16_t TagsNum;
    uint16_t tags[];
}eCPRIGetReq;

typedef struct
{
    uint8_t TlvNum;
    OamTlv_t TLVs[];
}eCPRIGetRes;

typedef struct
{ 
    uint16_t num_tlvs;    
    OamTlv_t tlvs[];
}stOam_set_req;

typedef struct{ 
    uint8_t error_number;
    uint16_t tags[];
}stOam_set_rsp;

typedef struct
{
    uint32_t temp_0; /*Temperature value of ts_id_0, C degree*/
    uint32_t temp_1; /*Temperature value of ts_id_1, C degree*/
    uint32_t temp_2; /*Temperature value of ts_id_2, C degree*/
	uint32_t temp_3; /*Temperature value of ts_id_3, C degree*/
}Pc802TempTlv_t;

/*message struct: msg header + tlvs*/

typedef struct
{
    uint8_t eth_type;// 0: 25Gbps 1: 10Gpbs
    uint8_t max_antenna_cnt;
    uint8_t max_section_cnt;
    uint8_t max_symbol_cnt_per_slot;
    uint8_t cur_antenna_cnt;
    uint8_t cur_section_cnt;
    uint8_t cur_symbol_cnt_per_slot;
    uint8_t rev0;
} eCPRIBasicConfigure_t;

typedef struct
{
    uint8_t ptp_enable;// 1: enable 0: disable
    uint8_t ptp_step_mode;// 0: 1step 1: 2 step // startup required------------------------------------------
    uint8_t ecpri_step_mode;// 0: 1step 1: 2 step// startup required------------------------------------------
    uint8_t ptp_domain;
} eCPRIPTPTlv_t;

typedef struct
{
    uint8_t isStaticComp;
    uint8_t method;
    uint8_t comIqWidth;//same as decomIqWidth
    uint8_t decomIqWidth;//same as comIqWidth
}eCPRICompressionTlv_t;

typedef struct
{
    uint32_t mtu;
}eCPRIEthernetMTUTlv_t;

typedef struct
{
    uint32_t maxTxLateThreshold;
}eCPRITxRxTlv_t;

typedef struct
{
    uint8_t ru_id;
    uint8_t rev0;
    uint16_t s_interval;
    uint32_t Ta4min;
    uint32_t Ta4max;
    uint32_t T1amin_cp_ul;
    uint32_t T1amax_cp_ul;
    uint32_t T1amin_cp_dl;
    uint32_t T1amax_cp_dl;
    uint32_t T1amin_up;
    uint32_t T1amax_up;
    uint8_t ru_mac[ETH_MAC_ADDR_LEN];
    uint16_t rev1;
}eCPRIORUTlv_t;

typedef struct
{
    uint8_t pcie_enable;
    uint8_t pipeline_mode;//0:non-pipeline,1:pipeline
    uint8_t outgoing_core_cnt;// startup required------------------------------------------
    uint8_t ingoing_core_cnt; // startup required------------------------------------------
}eCPRIDULocalTlv_t;

typedef struct
{
    uint8_t cp_enable;
    uint8_t ru_cnt; // startup required------------------------------------------
    uint8_t du_mac[ETH_MAC_ADDR_LEN];
    uint8_t du_port_id;
    uint8_t rev0;
    uint16_t vlan_id;
}eCPRIDUTlv_t;

typedef struct
{
    uint32_t m_rx_total_l;
    uint32_t m_rx_total_h;
    uint32_t m_rx_on_time_l;
    uint32_t m_rx_on_time_h;
    uint32_t m_rx_early_l;
    uint32_t m_rx_early_h;
    uint32_t m_rx_late_l;
    uint32_t m_rx_late_h;
    uint32_t m_rx_seqid_err_l;//optional
    uint32_t m_rx_seqid_err_h;//optional
    uint32_t m_rx_on_time_c_l;
    uint32_t m_rx_on_time_c_h;
    uint32_t m_rx_early_c_l;
    uint32_t m_rx_early_c_h;
    uint32_t m_rx_late_c_l;
    uint32_t m_rx_late_c_h;
    uint32_t m_rx_seqid_err_c_l;//optional
    uint32_t m_rx_seqid_err_c_h;//optional
    uint32_t m_rx_corrupt_l;//optional
    uint32_t m_rx_corrupt_h;//optional
    uint32_t m_rx_err_drop_l;//optional
    uint32_t m_rx_err_drop_h;//optional
    uint32_t m_rx_pkt_dupl_l;//optional
    uint32_t m_rx_pkt_dupl_h;//optional
    uint32_t m_tx_total_l;
    uint32_t m_tx_total_h;
    uint32_t m_tx_total_c_l;
    uint32_t m_tx_total_c_h;
}eCPRIODUKpisTlv_t;

typedef struct stEcpri_tsu_perf_kpis {
    uint32_t m_tx_tsu_ptp;
    uint32_t m_tx_tsu_ecpri;
    uint32_t m_rx_tsu_ptp;
    uint32_t m_rx_tsu_ecpri;
}stEcpri_tsu_perf_kpis;

typedef struct stEcpri_ptp_perf_kpis {
    uint32_t m_tx_announce;
    uint32_t m_tx_sync;
    uint32_t m_tx_follow_up;
    uint32_t m_rx_request;
    uint32_t m_tx_response;
}stEcpri_ptp_perf_kpis;

typedef struct stEcpri_slot_rx{
    uint32_t slot7_cnt[MAX_ANTEN_NUM];
    uint32_t slot8_cnt[MAX_ANTEN_NUM];
    uint32_t slot9_cnt[MAX_ANTEN_NUM];
    uint32_t slot17_cnt[MAX_ANTEN_NUM];
    uint32_t slot18_cnt[MAX_ANTEN_NUM];
    uint32_t slot19_cnt[MAX_ANTEN_NUM];
}stEcpri_slot_rx;

typedef struct stEcpri_perf_data_tx {
    uint32_t m_cnt;
    uint32_t m_ptx_push_u_desc_fifo;
    uint32_t m_ptx_build_u_desc;
    uint32_t m_ptx_wait_u;
    uint32_t m_comp_push_desc_fifo;
    uint32_t m_comp_build_desc;
    uint32_t m_comp_wait;
    uint32_t m_pkt_pack;
    uint32_t m_ptx_comp_total;
    uint32_t m_tsc;
    uint32_t m_idx;
} stEcpri_perf_data_tx;

typedef struct stEcpri_perf_data_cnt_tx {
    uint32_t m_ptx_build_s_desc_cnt;
    uint32_t m_ptx_push_s_desc_cnt;
    uint32_t m_ptx_wait_s_desc_cnt;
    uint32_t m_ptx_build_c_desc_cnt;
    uint32_t m_ptx_push_c_desc_cnt;
    uint32_t m_ptx_wait_c_desc_cnt;
    uint32_t m_ptx_build_u_desc_cnt;
    uint32_t m_ptx_push_u_desc_cnt;
    uint32_t m_ptx_wait_u_desc_cnt;
    uint32_t m_ptx_build_desc_total_cnt;
    uint32_t m_ptx_push_desc_total_cnt;
    uint32_t m_ptx_wait_desc_total_cnt;
    uint32_t m_comp_build_push_desc_cnt;
    uint32_t m_comp_wait_desc_cnt;
} stEcpri_perf_data_cnt_tx;

typedef struct stEcpri_perf_data_rx {
    uint32_t m_cnt;
    uint32_t m_prx_push_desc_fifo;
    uint32_t m_prx_build_desc;
    uint32_t m_prx_wait;
    uint32_t m_decomp_push_desc_fifo;
    uint32_t m_decomp_build_desc;
    uint32_t m_decomp_wait;
    uint32_t m_pkt_unpack;
    uint32_t m_prx_decomp_total;
} stEcpri_perf_data_rx;

typedef struct stEcpri_perf_data_cnt_rx {
    uint32_t m_prx_build_push_desc_cnt;
    uint32_t m_prx_wait_desc_cnt;
    uint32_t m_prx_rcv_msg_cnt;
    uint32_t m_decomp_build_push_desc_cnt;
    uint32_t m_decomp_wait_desc_cnt;
} stEcpri_perf_data_cnt_rx;

typedef struct stEcpri_debug_cfg{
    uint8_t m_plane_debug_enable;
    uint8_t u_plane_debug_enable;
    uint8_t c_plane_debug_enable;
    uint8_t s_plane_debug_enable;
    uint8_t ptp_debug_enable;
    uint8_t tsu_mgr_debug_enable;
    uint8_t phy_mgr_debug_enable;
    uint8_t data_rtx_debug_enable;
    uint8_t du_mgr_debug_enable;
    uint8_t ru_mgr_debug_enable;
    uint8_t hwa_debug_enable;
    uint8_t receiving_windows_debug_enable;
} stEcpri_debug_cfg;

typedef struct stEcpri_net_linkup_status {
    uint32_t net_ecpri_linkup_status;
}stEcpri_net_linkup_status;

typedef struct stEcpri_rx_complete_error_count {
    uint32_t timed_ecpri_complete_error;
    uint32_t non_ecpri_complete_error;
}stEcpri_rx_complete_error_count;

typedef struct stEcpri_mac_register {
    uint32_t rx_64_b_pkts;
    uint32_t rx_65_b_pkts;
    uint32_t rx_128_b_pkts;
    uint32_t rx_256_b_pkts;
    uint32_t rx_512_b_pkts;
    uint32_t rx_1024_b_pkts;
    uint32_t rx_1519_b_pkts;
    uint32_t rx_undersize_pkts;
    uint32_t rx_oversize_pkts;
    uint32_t rx_in_pkts;
    uint32_t rx_in_lsb_bytes;
    uint32_t rx_in_msb_bytes;
    uint32_t rx_bad_crc_pkts;
    uint32_t rx_tag_pkts;
    uint32_t rx_dmac_fltrd_pkts;
    uint32_t rx_uc_pkts;
    uint32_t rx_mc_pkts;
    uint32_t rx_bc_pkts;
    uint32_t rx_discard_pkts;
    uint32_t rx_overflow_pkts;
}stEcpri_mac_register;

typedef struct stEcpri_pcs_status {
    uint32_t pcs_status_1;
    uint32_t pcs_status_2;
    uint32_t pcs_mltbser_status_4;
}stEcpri_pcs_status;

typedef struct stEcpri_sfp_status {
    uint32_t sfp_status;
}stEcpri_sfp_status;

#endif


/*message struct: msg header + tlvs*/
