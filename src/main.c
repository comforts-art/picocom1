/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#define _GNU_SOURCE
#include "cmmApiInit.h"
#include "cmmMem.h"
#include "cmmTypes.h"
#include "cmm_thread.h"
#include "cmm_zlog.h"
#include "ev.h"
#include "mac.h"
#include "macTrace.h"
#include "macTx.h"
#include "moduleInf.h"
#include <getopt.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef ATLL_PCIE
#include "pcxx_ipc.h"
#include "pcxx_oam.h"
#include "rte_pmd_pc802.h"
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_mbuf.h>
#include <rte_mempool.h>
struct rte_mempool* mpool_pc802_tx;
#endif

#define MAX_SPLIT_STR_NUM 10
#define MAX_SPLIT_STR_LEN 20

/*Change Version Number in here*/
#define U_VERSION_MAJOR 3
#define U_VERSION_MINOR 0
#define U_VERSION_BUILD 0

typedef int (*CMDPARSER)(char*);
typedef struct
{
    uint16_t SFN;
    uint16_t Slot;
} SlotInfo_t;

typedef struct CommandList {
    char* cmdName;
    CMDPARSER cmdParser;
    char* cmdParam;
    char* cmdHelp;
} CMDLIST;

typedef struct app_ctx_s {
    ev_async async_watcher;
    struct ev_loop* rtloop;
    struct ev_loop* mainloop;
    ev_timer timeout_watcher;
    ev_io stdin_watcher;

} app_ctx;

static app_ctx gAppCtx;
static int s_tti = 0;

static TestmacCfg_s sAppCfg = { 0 };

static uint16_t sCaseNum = 0;

int CmdHelp(char* buffer);
extern void oamRecvAppMsg();

#ifdef ATLL_PCIE
static pcxxInfo_s ctrl_cb_info = { macRevL1Ctrl, macSendL1Ctrl };
static pcxxInfo_s data_cb_info = { NULL, macSendL1Data };
// static pcxxInfo_s oam_cb_info = { oamRecvFeuMsg, NULL };

static const struct rte_eth_conf dev_conf = {
    .rxmode = {
        .max_rx_pkt_len = RTE_ETHER_MAX_LEN,
    },
};
#endif

//uint8_t g_cellNum = 1;
#ifdef MAC_MULTI_PC802
int g_devMapPort[MAC_PC802_INDEX_MAX] = { -1, -1, -1, -1 };
uint8_t g_cellNum[MAC_PC802_INDEX_MAX] = { 1, 1, 1, 1 };
#else
int g_devMapPort[MAC_PC802_INDEX_MAX] = { -1 };
uint8_t g_cellNum[MAC_PC802_INDEX_MAX] = { 1 };
#endif

#ifdef ATLL_PCIE
static int port_init(uint16_t pc802_index)
{
    struct rte_mempool* mbuf_pool;
    // const struct rte_eth_conf dev_conf;
    struct rte_eth_dev_info dev_info;
    struct rte_eth_txconf tx_conf;
    char temp_name[32] = { 0 };
    // const struct rte_eth_rxconf rx_conf;
    int socket_id;
    int port = pc802_get_port_id(pc802_index);
    if (port < 0)
        rte_exit(EXIT_FAILURE, "pc802 %d is notexist !\n", pc802_index);

    rte_eth_dev_info_get(port, &dev_info);
    socket_id = dev_info.device->numa_node;

    sprintf(temp_name, "MBUF_POOL_ETH%d_TX", pc802_index);
    mbuf_pool = rte_pktmbuf_pool_create(temp_name, 2048,
        128, 0, RTE_MBUF_DEFAULT_BUF_SIZE, socket_id);
    if (mbuf_pool == NULL)
        rte_exit(EXIT_FAILURE, "Cannot create mbuf pool on Line %d\n", __LINE__);
    mpool_pc802_tx = mbuf_pool;

    sprintf(temp_name, "MBUF_POOL_ETH%d_RX", pc802_index);
    mbuf_pool = rte_pktmbuf_pool_create(temp_name, 2048,
        128, 0, RTE_MBUF_DEFAULT_BUF_SIZE, socket_id);
    if (mbuf_pool == NULL)
        rte_exit(EXIT_FAILURE, "Cannot create mbuf pool on Line %d\n", __LINE__);

    rte_eth_dev_configure(port, 1, 1, &dev_conf);
    tx_conf = dev_info.default_txconf;
    rte_eth_tx_queue_setup(port, 0, 128, socket_id, &tx_conf);
    rte_eth_rx_queue_setup(port, 0, 128, socket_id, NULL, mbuf_pool);
#ifndef MAC_MULTI_PC802
    pcxxDataOpen(&data_cb_info);
    pcxxCtrlOpen(&ctrl_cb_info);
#else
    uint8_t cell;
    for (cell = 0; cell < CELL_NUM_PRE_DEV; cell++) {
        pcxxDataOpen(&data_cb_info, pc802_index, cell);
        pcxxCtrlOpen(&ctrl_cb_info, pc802_index, cell);
    }
#endif
    // pcxxOamOpen(&oam_cb_info);
    // pcxx_oam_init( );

    rte_eth_dev_start(port);
    printf("Finished port_init !\n");

    return 0;
}
#endif

static void sys_load_app_in_ram(char* appname)
{
    char fileName[24];
    FILE* fp;
    char string1[512];
    uint32_t* addr_start = NULL;
    uint32_t* addr_end = NULL;
    unsigned char* p;
    volatile unsigned char readData;
    int argsNum;
    uint32_t pageSize = getpagesize();
    uint32_t cnt = 0;
    uint32_t pid = getpid();
    uint32_t len;
    char* binName = strrchr(appname, '/');

    printf(" ");
    printf("AppInRam: Start: pid=%d\n", pid);
    sprintf(fileName, "/proc/%d/maps", pid);

    if ((fp = fopen(fileName, "r")) == NULL) {
        printf("AppInRam: can't open file %s\n", fileName);
        return;
    }

    printf("binName=<%s> appname=<%s>\n", binName, appname);

    while (NULL != fgets(string1, 512, fp)) {
        char* foundName = strrchr(string1, '/'); // find last occurence

        if (!foundName)
            continue;

        len = strlen(foundName);
        printf("MAP info: <%s>\n", string1);
        printf("foundName=<%s> len=%d\n", foundName, len);

        if (len) {
            foundName[len - 1] = 0;
            printf("foundName_fix=<%s>", foundName);
            if (strcmp(foundName, binName) == 0) {
                argsNum = sscanf(string1, "%lx-%lx", (uint32_t*)&addr_start, (uint32_t*)&addr_end);
                printf("*** read: start=%p end=%p\n", addr_start, addr_end);
                if (argsNum != 2 || (!addr_start) || (!addr_end)) {
                    printf("AppInRam: Warning. Looked for 2 args in 'proc//maps' string, found: %d", argsNum);
                    continue;
                }
                p = (unsigned char*)addr_start;
                printf("*** read: start=%p end=%p", addr_start, addr_end);
                while (p < (unsigned char*)addr_end) {
                    readData = *p; // access to all pages to make linux loader load all program data to RAM
                    p += pageSize;
                }
                cnt++;
            }
        }
    }

    fclose(fp);
    printf("AppInRam: End. %d areas checked to be loaded in RAM\n", cnt);

    return;
}

static void appSigQuit(int sig)
{
    printf("Capture sign no:%d ", sig);
    uint8_t dev, cell;

    for (dev = 0; dev < MAC_PC802_INDEX_MAX; dev++) {
        for (cell = 0; cell < g_cellNum[dev]; cell++) {
            if (g_devMapPort[dev] >= 0)
                macStop(dev, cell);
        }
    }
    sleep(1);
    ev_loop_destroy(gAppCtx.mainloop);
    exit(0);
}

void splitStr(char* targetStr, char (*resultStrs)[MAX_SPLIT_STR_LEN])
{
    int i = 0;
    char seps[] = "{}[]: ,._/=\t\n";
    char* token = NULL;

    /* Establish string and get the first token: */
    if (targetStr != NULL) {
        printf("First Split %s\n", targetStr);
    }
    token = strtok(NULL, seps);

    while (token != NULL && i < MAX_SPLIT_STR_NUM) {
        /* While there are tokens in "string" */
        strcpySafe(resultStrs[i], token, MAX_SPLIT_STR_LEN);
        i++;
        /* Get next token: */
        token = strtok(NULL, seps);
    }
    return;
}

static void simTtiInd()
{
    if (sAppCfg.phyStub == false) {
        return;
    }

    CtrlMsg_s repMsg = { 0 };
    repMsg.msgId = APP_SIM_TTI;

    macAppMgrSend(0, 0, (char*)&repMsg, sizeof(repMsg));
}

static void appTestStart(uint16_t caseId, uint8_t dev_index, uint8_t cellId)
{
    if (sAppCfg.operMode == MANUAL)
        return;
    CtrlMsg_s repMsg = { 0 };
    repMsg.msgId = APP_START_TEST;
    repMsg.paras[0] = caseId;
    repMsg.paras[2] = dev_index;
    repMsg.paras[3] = cellId;
    macAppMgrSend(dev_index, cellId, (char*)&repMsg, sizeof(repMsg));
}
static void appTestStop(uint16_t caseId, uint8_t dev_index, uint8_t cellId)
{
    if (sAppCfg.operMode == AUTO)
        return;
    CtrlMsg_s repMsg = { 0 };

    repMsg.msgId = APP_STOP_TEST;
    repMsg.paras[0] = caseId;
    repMsg.paras[2] = dev_index;
    repMsg.paras[3] = cellId;
    macAppMgrSend(dev_index, cellId, (char*)&repMsg, sizeof(repMsg));
}

static void appTcConfigCase(uint16_t caseId, uint8_t dev_index, uint8_t cellId)
{
    if (sAppCfg.operMode == AUTO)
        return;
    CtrlMsg_s repMsg = { 0 };
    repMsg.msgId = APP_TC_CONFIG_CASE;
    repMsg.paras[0] = caseId;
    repMsg.paras[2] = dev_index;
    repMsg.paras[3] = cellId;
    macAppMgrSend(dev_index, cellId, (char*)&repMsg, sizeof(repMsg));
}

static void appTcDlCase(uint16_t sfn, uint16_t slot, uint8_t dev_index, uint8_t cellId)
{
    if (sAppCfg.operMode == AUTO)
        return;
    CtrlMsg_s repMsg = { 0 };
    repMsg.msgId = APP_TC_DL_CASE;
    repMsg.paras[0] = sfn;
    repMsg.paras[1] = slot;
    repMsg.paras[2] = dev_index;
    repMsg.paras[3] = cellId;
    macAppMgrSend(dev_index, cellId, (char*)&repMsg, sizeof(repMsg));
}

static void appTcUlCase(uint16_t sfn, uint16_t slot, uint8_t dev_index, uint8_t cellId)
{
    if (sAppCfg.operMode == AUTO)
        return;
    CtrlMsg_s repMsg = { 0 };
    repMsg.msgId = APP_TC_UL_CASE;
    repMsg.paras[0] = sfn;
    repMsg.paras[1] = slot;
    repMsg.paras[2] = dev_index;
    repMsg.paras[3] = cellId;
    macAppMgrSend(dev_index, cellId, (char*)&repMsg, sizeof(repMsg));
}

static void appTcTxdataCase(uint16_t sfn, uint16_t slot, uint8_t dev_index, uint8_t cellId)
{
    if (sAppCfg.operMode == AUTO)
        return;
    CtrlMsg_s repMsg = { 0 };
    repMsg.msgId = APP_TC_TX_CASE;
    repMsg.paras[0] = sfn;
    repMsg.paras[1] = slot;
    repMsg.paras[2] = dev_index;
    repMsg.paras[3] = cellId;
    macAppMgrSend(dev_index, cellId, (char*)&repMsg, sizeof(repMsg));
}

static void appTcTestCase(uint16_t id, uint16_t cellNum, uint8_t dev_index, uint8_t cellId)
{
    if (sAppCfg.operMode == AUTO)
        return;
    CtrlMsg_s repMsg = { 0 };
    repMsg.msgId = APP_TX_CASE;
    repMsg.paras[0] = id;
    repMsg.paras[1] = cellNum;
    repMsg.paras[2] = dev_index;
    repMsg.paras[3] = cellId;
    printf("appMsg send, dev_index:%d cellId:%d\n", dev_index, cellId);
    macAppMgrSend(dev_index, cellId, (char*)&repMsg, sizeof(repMsg));
}
static void appTcStop(uint16_t caseId, uint8_t dev_index, uint8_t cellId)
{
    if (sAppCfg.operMode == AUTO)
        return;
    CtrlMsg_s repMsg = { 0 };
    repMsg.msgId = APP_TC_STOP;
    repMsg.paras[0] = caseId;
    repMsg.paras[2] = dev_index;
    repMsg.paras[3] = cellId;
    macAppMgrSend(dev_index, cellId, (char*)&repMsg, sizeof(repMsg));
}

static uint16_t getTestCaseNum()
{
    if (sAppCfg.operMode == MANUAL) {
        return 0;
    }
    return 12;
}

int CmdTcConfigCase(char* buffer)
{
    printf("Tc Config case!!\n");
    int caseId = 0;
    int dev = 0;
    int cell = 0;

    char resultStr[MAX_SPLIT_STR_NUM][MAX_SPLIT_STR_LEN] = {};
    splitStr(buffer, resultStr);
    if (resultStr[0]) {
        caseId = strtoul(resultStr[0], NULL, 0);
    }
    if (resultStr[1]) {
        dev = strtoul(resultStr[1], NULL, 0);
    }
    if (resultStr[2]) {
        cell = strtoul(resultStr[2], NULL, 0);
    }

    appTcConfigCase(caseId, dev, cell);
    return 0;
}

int CmdTcDlCase(char* buffer)
{
    printf("Tc Dltti case!!\n");
    int para1 = 0;
    int para2 = 0;
    int para3 = 0;
    int para4 = 0;

    char resultStr[MAX_SPLIT_STR_NUM][MAX_SPLIT_STR_LEN] = {};
    splitStr(buffer, resultStr);
    if (resultStr[0]) {
        para1 = strtoul(resultStr[0], NULL, 0);
    }
    if (resultStr[1]) {
        para2 = strtoul(resultStr[1], NULL, 0);
    }
    if (resultStr[2]) {
        para3 = strtoul(resultStr[2], NULL, 0);
    }
    if (resultStr[3]) {
        para4 = strtoul(resultStr[3], NULL, 0);
    }

    appTcDlCase(para1, para2, para3, para4);
    return 0;
}

int CmdTcUlCase(char* buffer)
{
    printf("Tc Ultti case!!\n");
    int para1 = 0;
    int para2 = 0;
    int para3 = 0;
    int para4 = 0;

    char resultStr[MAX_SPLIT_STR_NUM][MAX_SPLIT_STR_LEN] = {};
    splitStr(buffer, resultStr);
    if (resultStr[0]) {
        para1 = strtoul(resultStr[0], NULL, 0);
    }
    if (resultStr[1]) {
        para2 = strtoul(resultStr[1], NULL, 0);
    }
    if (resultStr[2]) {
        para3 = strtoul(resultStr[2], NULL, 0);
    }
    if (resultStr[3]) {
        para4 = strtoul(resultStr[3], NULL, 0);
    }

    appTcUlCase(para1, para2, para3, para4);
    return 0;
}

int CmdTcTxCase(char* buffer)
{
    printf("Tc Txdata case!!\n");
    int para1 = 0;
    int para2 = 0;
    int para3 = 0;
    int para4 = 0;

    char resultStr[MAX_SPLIT_STR_NUM][MAX_SPLIT_STR_LEN] = {};
    splitStr(buffer, resultStr);
    if (resultStr[0]) {
        para1 = strtoul(resultStr[0], NULL, 0);
    }
    if (resultStr[1]) {
        para2 = strtoul(resultStr[1], NULL, 0);
    }
    if (resultStr[2]) {
        para3 = strtoul(resultStr[2], NULL, 0);
    }
    if (resultStr[3]) {
        para4 = strtoul(resultStr[3], NULL, 0);
    }

    appTcTxdataCase(para1, para2, para3, para4);
    return 0;
}

int CmdTestCase(char* buffer)
{
    printf("Tc Tx case!!\n");
    int para1 = 0;
    int cellNum = 0;
    int dev = 0;
    int cell = 0;

    char resultStr[MAX_SPLIT_STR_NUM][MAX_SPLIT_STR_LEN] = {};
    splitStr(buffer, resultStr);
    if (resultStr[0]) {
        para1 = strtoul(resultStr[0], NULL, 0);
    }
    if (resultStr[1]) {
        cellNum = strtoul(resultStr[1], NULL, 0);
    }
    if (resultStr[2]) {
        dev = strtoul(resultStr[2], NULL, 0);
    }
    if (resultStr[3]) {
        cell = strtoul(resultStr[3], NULL, 0);
    }

    appTcTestCase(para1, cellNum, dev, cell);
    return 0;
}

int CmdTcStop(char* buffer)
{
    printf("Tc Stop!!\n");
    int tcId = 0;
    int dev = 0;
    int cell = 0;

    char resultStr[MAX_SPLIT_STR_NUM][MAX_SPLIT_STR_LEN] = {};
    splitStr(buffer, resultStr);
    if (resultStr[0]) {
        tcId = strtoul(resultStr[0], NULL, 0);
    }
    if (resultStr[1]) {
        dev = strtoul(resultStr[1], NULL, 0);
    }
    if (resultStr[2]) {
        cell = strtoul(resultStr[2], NULL, 0);
    }

    appTcStop(tcId, dev, cell);
    return 0;
}

static CMDLIST CmdList[] = {
    { "help", CmdHelp, "", "Print this help" },
    { "tc_config_case", CmdTcConfigCase, "caseId,dev_index,cellId", "Tc Star Config Case" },
    { "tc_dl_case", CmdTcDlCase, "para1,para2,dev_index,cellId", "Tc Star Dltti Case" },
    { "tc_ul_case", CmdTcUlCase, "para1,para2,dev_index,cellId", "Tc Star Ultti Case" },
    { "tc_tx_case", CmdTcTxCase, "para1,para2,dev_index,cellId", "Tc Star Txdata Case" },
    { "tc_case", CmdTestCase, "para1,cellNum,dev_index,cellId", "Tc Star Test Case" },
    { "tc_stop", CmdTcStop, "testId,dev_index,cellId", "Tc Stop" },
    { NULL, NULL, NULL, NULL }
};

int ConPrompt()
{
    printf("\npicocom_test_mac>>");
    fflush(stdout);
    return 0;
}
int CmdHelp(char* buffer)
{
    int i = 0;
    if (NULL != buffer)
        printf("\nHelp not support para\n\n");
    printf("\nHelp for testmac console commands\n\n");
    while (CmdList[i].cmdParser != NULL) {
        printf("%s %s\n    %s\n", CmdList[i].cmdName, CmdList[i].cmdParam, CmdList[i].cmdHelp);
        i++;
    }

    return 0;
}

static void cmd_callback(struct ev_loop* loop, ev_io* w, int revents)
{
    if ((NULL == w) || (NULL == loop)) {
        printf("Unknown command Event %d\n", revents);
    }
    ev_io_stop(loop, w);
    char buff[512];
    memset(buff, 0, 512);
    int len = read(w->fd, buff, 512);
    buff[len - 1] = '\0';
    char* cmd;
    cmd = strtok(buff, " ");
    int readIdx = 0;
    int cmdproc = 0;
    int ret = 0;
    if (cmd) {

        while (CmdList[readIdx].cmdParser != NULL) {
            if (strcasecmp(cmd, CmdList[readIdx].cmdName) == 0) {
                ret = CmdList[readIdx].cmdParser(buff);
                if (ret != 0) {
                    cmdproc = 1;
                }
                cmdproc = 1;
                break;
            }
            readIdx++;
        }
        if (cmdproc == 0) { // unknown command proc
            printf("Unknown command\n");
            ConPrompt();
        }
    }
    ConPrompt();
    ev_io_start(loop, &gAppCtx.stdin_watcher);
}

static void tmr_callback(EV_P_ ev_timer* w, int revents)
{
    static int s_checkTti = 0;
    static uint8_t dev_index = 0;
    static uint8_t cellId = 0;
    static uint16_t caseId = 0;
    static uint16_t totalRuntime = 0;
    static MacStatus_e s_phyState = PHY_IDLE;
    if ((NULL == w) || (NULL == loop)) {
        printf("Unknown command Event %d\n", revents);
    }
    s_tti++;
    simTtiInd();
    oamRecvAppMsg();
    if (sAppCfg.runtimeTotal > 0) {
        if (totalRuntime > sAppCfg.runtimeTotal)
            exit(0);
    }

    if (sAppCfg.runtimePerTc != 0) {
        if (s_tti % sAppCfg.runtimePerTc == 0) {
            if (sAppCfg.phyStub == false) // TC control in TTI-IND HANDLE
                return;
            if (sCaseNum == 0)
                return;

            CMM_INFO(LOG_APP, "-------- Time is up %u case %d---------", s_tti, caseId);
#if 0
            if (((s_tti / sAppCfg.runtimePerTc) & 1) == 0)
                appTestStart(caseId);
            else
                appTestStop(caseId);
#endif
            {
                appTestStop(caseId, dev_index, cellId);

                if (++caseId == sCaseNum) {
                    caseId = 0;
                }
                appTestStart(caseId, dev_index, cellId);
            }
            totalRuntime += sAppCfg.runtimePerTc;
        }
    }

    MacStatus_e phyState = getPhyState(dev_index, cellId);
    if ((PHY_CFG == phyState) || (PHY_INIT == phyState)) {
        if (s_checkTti > 1000000) {
            CMM_ERROR(LOG_APP, "-------- Wait PHY timeout[%d]---------", phyState);
            exit(0);
        }
        if (s_phyState != phyState) {
            s_checkTti = 0;
            s_phyState = phyState;
        } else
            s_checkTti++;
    }
}

static void loadCfg()
{
    FILE* pfile;
    int8_t buf[1] = "";
    sAppCfg.runMode = RUNMODE_FAPI_GENERATOR;
    sAppCfg.operMode = MANUAL;
    sAppCfg.cliMode = CLIMODE_LOCAL_MODE;
    sAppCfg.pdschMode = PDSCHMODE_AUTO_MODE;
    sAppCfg.phyStub = false;
    sAppCfg.runtimeTotal = 0;
    sAppCfg.runtimePerTc = 0;
    sAppCfg.atllloglevel = -1;
    sAppCfg.sfnMode = false;
	sAppCfg.cellMask = 0;
    pfile = fopen("atll_log_level.txt", "r");
    if (pfile != NULL) {
        fgets(buf, 1, pfile);
        sAppCfg.atllloglevel = buf[0];
    }
}

#define TIMEOUT 0.

static void
testmac_usage(const char* prgname)
{
    printf("%s [EAL options] --\n",
        prgname);
}

static int
testmac_slot_num(TestmacCfg_s* options,
    const char* q_arg)
{
    char* end = NULL;
    long pm;

    pm = strtoul(q_arg, &end, 0);
    if ((pm == '\0') || (end == NULL) || (*end != '\0'))
        pm = 0;

    options->runtimePerTc = pm;

    return pm;
}

static int
testmac_parse_portmask(TestmacCfg_s* options,
    const char* q_arg)
{
    strcpy(options->remote_trace_ip, q_arg);

    if (options->remote_trace_ip == NULL)
        return -1;
    return sizeof(q_arg);
}

static int
testmac_parse_cellmask(TestmacCfg_s* options,
    const char* q_arg)
{
    char* end = NULL;
    long pm;
	uint8_t i, cellNum;
	uint32_t mask;

    pm = strtoul(q_arg, &end, 16);
    if ((pm == '\0') || (end == NULL) || (*end != '\0'))
        pm = 0;

    options->cellMask = pm;

	if (options->cellMask != 0) {
		mask = options->cellMask;
		CMM_NOTICE(LOG_MAC, "cellMask: 0x%x", mask);
		for (i = 0; i < MAC_PC802_INDEX_MAX; i++) {
			cellNum = mask & 0x03;
		    CMM_NOTICE(LOG_MAC, "dev: %d cellNum: %d", i, cellNum);
			g_cellNum[i] = cellNum;
			mask = mask >> 2;
		}
	}

    return pm;
}

static int testmac_long_options(TestmacCfg_s* options, struct option* lgopts, int option_index)
{
    int retval;
    int val;

    if (strcmp(lgopts[option_index].name, "trace_ip") == 0) {
        retval = testmac_parse_portmask(options, optarg);
        return retval;
    } else
        return -1;
}

static int
testmac_parse_args(TestmacCfg_s* options,
    int argc, char** argv)
{
    int opt, retval, option_index;
    char **argvopt = argv, *prgname = argv[0];

    static struct option lgopts[] = {
        { "trace_ip", required_argument, 0, 0 },
        { NULL, 0, 0, 0 }
    };

    loadCfg();

    while ((opt = getopt_long(argc, argvopt, "p:T:C:", lgopts,
                &option_index))
        != EOF) {
        switch (opt) {
        case 0:
            retval = testmac_long_options(options, lgopts, option_index);
            if (retval < 0) {
                testmac_usage(prgname);
                return -1;
            }
            break;

        case 'T':
            retval = testmac_slot_num(options, optarg);
            if (retval < 0) {
                testmac_usage(prgname);
                return -1;
            }
            break;

        case 'p':
            retval = testmac_parse_portmask(options, optarg);
            if (retval < 0) {
                testmac_usage(prgname);
                return -1;
            }
            break;

        case 'C':
            retval = testmac_parse_cellmask(options, optarg);
            if (retval < 0) {
                testmac_usage(prgname);
                return -1;
            }
            break;
			
        default:
            testmac_usage(prgname);
            return -1;
        }
    }

    if (optind >= 0)
        argv[optind - 1] = prgname;

    retval = optind - 1;
    optind = 1; /* reset getopt lib */
    return retval;
}

#ifdef ATLL_PCIE
int main(int argc, char** argv)
{
    int diag, ret;
    uint32_t lcore_id;
    int slaveCoreNum = 0;
    uint32_t slaveCore[10] = { 0 };
    sys_load_app_in_ram(argv[0]);
    diag = rte_eal_init(argc, argv);
    if (diag < 0)
        rte_panic("Cannot init EAL\n");

    argc -= diag;
    argv += diag;
    // port_init(0);
    // port_init(pc802_get_port_id(0));
    static int cpuId = 0;
    static int txCfg = 0;
    int port_id = 0;
    int pc802_index = 0;

    pcxx_oam_init();
    for (pc802_index = 0; pc802_index < MAC_PC802_INDEX_MAX; pc802_index++) {
        port_id = pc802_get_port_id(pc802_index);
        if (port_id < 0)
            continue;
        g_devMapPort[pc802_index] = port_id;
        printf("port_init, pc802_index: %d port_id: %d\n", pc802_index, port_id);
        port_init(pc802_index);
    }
    loadCfg();
    ret = testmac_parse_args(&sAppCfg, argc, argv);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Invalid test mac arguments\n");
    printf("ip = %sm %d\n", sAppCfg.remote_trace_ip, sAppCfg.runtimePerTc);
    cmmApiInit(sAppCfg.remote_trace_ip);
    loadTxCfg(15);

    if (0 != cmmLogInit("./log.conf")) {
        printf("read zlog configuration file fail\n");
        return -1;
    }
    RTE_LCORE_FOREACH_SLAVE(lcore_id)
    {
        slaveCore[slaveCoreNum++] = lcore_id;
        printf("lcore_id = %d\n", lcore_id);
    }

    macInit(sAppCfg.runMode, sAppCfg.operMode, &slaveCore[0], sAppCfg.phyStub, sAppCfg.pdschMode, sAppCfg.runtimePerTc, sAppCfg.atllloglevel, sAppCfg.sfnMode);
    sleep(1);

    int VerMajor = U_VERSION_MAJOR;
    int VerMinor = U_VERSION_MINOR;
    int VerPatch = U_VERSION_BUILD;

    CMM_INFO(LOG_APP, "      PICOCOM TEST MAC: R %i.%i.%i    \n", VerMajor, VerMinor, VerPatch);
	printf("**************************************************************************************\n");
    printf("*---------- Product:    Picocom PC802-NR-TESTMAC\n");
    printf("*---------- Version:    %s\n", NR_TESTMAC_VER);
    printf("*---------- Build Time: Date-%s, Time-%s\n", __DATE__, __TIME__);
    printf("*---------- Build SHA1: %s\n", TESTMAC_GIT_COMMIT_SHA1);
    printf("**************************************************************************************\n");

    if (sCaseNum != 0) {
        appTestStart(0, 0, 0);
    }
    gAppCtx.mainloop = ev_loop_new(EVBACKEND_EPOLL);
    if (NULL == gAppCtx.mainloop) {

        return -1;
    }
    double after = 0;
    double repeat = 0.1;

    ev_timer_init((ev_timer*)&gAppCtx.timeout_watcher, tmr_callback, after, repeat); // unit:10s
    ev_timer_start(gAppCtx.mainloop, &gAppCtx.timeout_watcher);

    if ((sAppCfg.cliMode == CLIMODE_LOCAL_MODE) || (sAppCfg.cliMode == CLIMODE_REMOTE_LOCAL_MODE)) {
        ConPrompt();
        ev_io_init(&gAppCtx.stdin_watcher, cmd_callback, 0, EV_READ);
        ev_io_start(gAppCtx.mainloop, &gAppCtx.stdin_watcher);
    }

    ev_run(gAppCtx.mainloop, 0);
    ev_loop_destroy(gAppCtx.mainloop);
		
    return 0;
}
#endif

#ifdef ATLL_SHM
int main(int argc, char** argv)
{
    sys_load_app_in_ram(argv[0]);
    static int cpuId = 0;
    static int txCfg = 0;
    if (argc > 1) {
        int i;
        for (i = 0; i < argc; i++)
            printf("Arg %s", argv[i]);
        printf("\n");
        cpuId = atoi(argv[1]);
    }

    if (argv[2] != NULL) {
        if (strcmp("MANUAL", argv[2]) == 0)
            loadCfg(MANUAL);
        if (strcmp("AUTO", argv[2]) == 0)
            loadCfg(AUTO);
    }
    if (signal(SIGINT, appSigQuit) == SIG_ERR) {
        printf("unable to install SIGQUIT handler.\n");
        return -1;
    }

    sCaseNum = getTestCaseNum();
    if (argv[3] != NULL)
        cmmApiInit(argv[3]);

    if (argv[4] != NULL) {
        txCfg = atoi(argv[4]);
        loadTxCfg(txCfg);
    }
    if (0 != cmmLogInit("./log.conf")) {
        printf("read zlog configuration file fail\n");
        return -1;
    }
    macInit(sAppCfg.runMode, sAppCfg.operMode, &cpuId, sAppCfg.phyStub, sAppCfg.pdschMode, sAppCfg.runtimePerTc, sAppCfg.atllloglevel, sAppCfg.sfnMode);
    sleep(1);

    int VerMajor = U_VERSION_MAJOR;
    int VerMinor = U_VERSION_MINOR;
    int VerPatch = U_VERSION_BUILD;

    CMM_INFO(LOG_APP, "      PICOCOM TEST MAC: R %i.%i.%i    \n", VerMajor, VerMinor, VerPatch);

    if (sCaseNum != 0) {
        appTestStart(0);
    }
    gAppCtx.mainloop = ev_loop_new(EVBACKEND_EPOLL);
    if (NULL == gAppCtx.mainloop) {

        return -1;
    }
    double after = 0;
    double repeat = 0.1;

    ev_timer_init((ev_timer*)&gAppCtx.timeout_watcher, tmr_callback, after, repeat); // unit:10s
    ev_timer_start(gAppCtx.mainloop, &gAppCtx.timeout_watcher);

    if ((sAppCfg.cliMode == CLIMODE_LOCAL_MODE) || (sAppCfg.cliMode == CLIMODE_REMOTE_LOCAL_MODE)) {
        ConPrompt();
        ev_io_init(&gAppCtx.stdin_watcher, cmd_callback, 0, EV_READ);
        ev_io_start(gAppCtx.mainloop, &gAppCtx.stdin_watcher);
    }

    ev_run(gAppCtx.mainloop, 0);
    ev_loop_destroy(gAppCtx.mainloop);
    return 0;
}
#endif
