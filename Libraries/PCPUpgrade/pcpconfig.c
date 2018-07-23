/**
  *********************************************************************************************************
  * @file    pcpconfig.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-07-16
  * @brief   PCP	: 设备和中国电信物联网开放平台之间应用层升级协议
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "pcpconfig.h"
#include "pcpfunc.h"
#include "pcptransport.h"
#include "pcpcrccheck.h"
#include "pcpsock.h"
#include "platform_config.h"
#include "platform_map.h"
#include "delay.h"
#include "usart.h"
#include "radio_rf_app.h"

unsigned char PCP_SendBuf[PCP_BUFFER_SIZE];
unsigned char PCP_RecvBuf[PCP_BUFFER_SIZE];
unsigned char PCP_DataStack[PCP_DATASTACK_SIZE];

/**********************************************************************************************************
 @Function			void PCP_Client_Init(PCP_ClientsTypeDef* pClient, PCP_CoAPNetTransportTypeDef* NetSock, NET_NBIOT_ClientsTypeDef* NetNbiotStack)
 @Description			PCP_Client_Init						: 初始化PCP客户端
 @Input				pClient								: PCP客户端实例
					NetSock								: PCP CoAP 协议栈
					NetNbiotStack							: NET NBIOT 协议栈
 @Return				void
**********************************************************************************************************/
void PCP_Client_Init(PCP_ClientsTypeDef* pClient, PCP_CoAPNetTransportTypeDef* NetSock, NET_NBIOT_ClientsTypeDef* NetNbiotStack)
{
	pClient->Sendbuf									= PCP_SendBuf;
	pClient->Recvbuf									= PCP_RecvBuf;
	pClient->Sendbuf_size								= sizeof(PCP_SendBuf);
	pClient->Recvbuf_size								= sizeof(PCP_RecvBuf);
	pClient->Sendlen									= 0;
	pClient->Recvlen									= 0;
	pClient->DataProcessStack							= PCP_DataStack;
	pClient->DataProcessStack_size						= sizeof(PCP_DataStack);
	
	pClient->Command_Timeout_Sec							= PCP_COMMAND_TIMEOUT_SEC;
	pClient->Command_Failure_Cnt							= PCP_COMMAND_FAILURE_CNT;
	
	pClient->DictateRunCtl.dictateEnable					= false;
	pClient->DictateRunCtl.dictateTimeoutSec				= 0;
	pClient->DictateRunCtl.dictateInitializedFailureCnt		= 0;
	pClient->DictateRunCtl.dictateReadyFailureCnt			= 0;
	pClient->DictateRunCtl.dictateRecvFailureCnt				= 0;
	pClient->DictateRunCtl.dictateSendFailureCnt				= 0;
	pClient->DictateRunCtl.dictateExecuteFailureCnt			= 0;
	pClient->DictateRunCtl.dictateActiveUploadFailureCnt		= 0;
	pClient->DictateRunCtl.dictateEvent					= PCP_EVENT_INITIALIZED;
	
	pClient->UpgradeExecution.upgradeStatus					= PCP_UPGRADE_COMPLETE;
	sprintf((char*)pClient->UpgradeExecution.DeviceSoftVersion, "V%d.%d", TCFG_Utility_Get_Major_Softnumber(), TCFG_Utility_Get_Sub_Softnumber());
	sprintf((char*)pClient->UpgradeExecution.PlatformSoftVersion, "V%d.%d", TCFG_Utility_Get_Major_Softnumber(), TCFG_Utility_Get_Sub_Softnumber());
	pClient->UpgradeExecution.PackSliceIndex				= 0;
	pClient->UpgradeExecution.PackSliceSize					= 0;
	pClient->UpgradeExecution.PackSliceNum					= 0;
	pClient->UpgradeExecution.PackCheckCode					= 0;
	
	pClient->CoAPStack									= NetSock;
	pClient->NetNbiotStack								= NetNbiotStack;
}

/**********************************************************************************************************
 @Function			void PCP_UpgradeDataDownload_Callback(PCP_ClientsTypeDef* pClient, u16 SliceIndex, u8* UpgradeData, u16 UpgradeDataLength)
 @Description			PCP_UpgradeDataDownload_Callback		: PCP升级包处理回调函数
 @Input				pClient							: PCP客户端实例
					SliceIndex						: PCP分片序号
					UpgradeData						: 升级包数据
					UpgradeDataLength					: 升级包长度
 @Return				void
**********************************************************************************************************/
void PCP_UpgradeDataDownload_Callback(PCP_ClientsTypeDef* pClient, u16 SliceIndex, u8* UpgradeData, u16 UpgradeDataLength)
{
	u8 TempBuf[21];
	memset(TempBuf, 0, 21);
	memcpy(TempBuf, UpgradeData, 20);
	Radio_Trf_Debug_Printf_Level2("Down : %s", TempBuf);
}

/********************************************** END OF FLEE **********************************************/