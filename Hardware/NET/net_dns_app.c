/**
  *********************************************************************************************************
  * @file    net_dns_app.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-05-16
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "net_dns_app.h"
#include "hal_rtc.h"
#include "string.h"

#ifdef DNS_DEBUG_LOG_RF_PRINT
#include "radio_rf_app.h"
#endif

/**********************************************************************************************************
 @Function			void NET_DNS_APP_PollExecution(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_APP_PollExecution			: DNS逻辑处理
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_APP_PollExecution(DNS_ClientsTypeDef* pClient)
{
	switch (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent)
	{
	case STOP_MODE:
		NET_DNS_NBIOT_Event_StopMode(pClient);
		break;
	
	case HARDWARE_REBOOT:
		NET_DNS_NBIOT_Event_HardwareReboot(pClient);
		break;
	
	case MODULE_CHECK:
		NET_DNS_NBIOT_Event_ModuleCheck(pClient);
		break;
	
	case PARAMETER_CONFIG:
		NET_DNS_NBIOT_Event_ParameterConfig(pClient);
		break;
	
	case ICCID_CHECK:
		NET_DNS_NBIOT_Event_SimICCIDCheck(pClient);
		break;
	
	case MISC_EQUIP_CONFIG:
		NET_DNS_NBIOT_Event_MiscEquipConfig(pClient);
		break;
	
	case ATTACH_CHECK:
		NET_DNS_NBIOT_Event_AttachCheck(pClient);
		break;
	
	case ATTACH_EXECUTE:
		NET_DNS_NBIOT_Event_AttachExecute(pClient);
		break;
	
	case ATTACH_INQUIRE:
		NET_DNS_NBIOT_Event_AttachInquire(pClient);
		break;
	
	case PARAMETER_CHECKOUT:
		NET_DNS_NBIOT_Event_PatameterCheckOut(pClient);
		break;
	
	case MINIMUM_FUNCTIONALITY:
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	
	case FULL_FUNCTIONALITY:
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	
	case CDP_SERVER_CHECK:
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	
	case CDP_SERVER_CONFIG:
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	
	case SEND_DATA:
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	
	case RECV_DATA:
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	
	case EXECUT_DOWNLINK_DATA:
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	
	case MQTTSN_PROCESS_STACK:
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	
	case DNS_PROCESS_STACK:
		NET_DNS_APP_ProcessExecution(pClient);
		break;
	
	case LISTEN_RUN_CTL:
		
		break;
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_APP_ProcessExecution(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_APP_ProcessExecution			: DNS协议逻辑处理
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_APP_ProcessExecution(DNS_ClientsTypeDef* pClient)
{
	switch (pClient->ProcessState)
	{
	case DNS_PROCESS_CREAT_UDP_SOCKET:
		NET_DNS_Event_CreatUDPSocket(pClient);
		break;
	
	case DNS_PROCESS_SEND_DNS_STRUCT_DATA:
		NET_DNS_Event_SendDnsStructData(pClient);
		break;
	
	case DNS_PROCESS_RECV_DNS_STRUCT_DATA:
		NET_DNS_Event_RecvDnsStructData(pClient);
		break;
	
	case DNS_PROCESS_CLOSE_UDP_SOCKET:
		NET_DNS_Event_CloseUDPSocket(pClient);
		break;
	
	case DNS_PROCESS_OVER_DNS_ANALYSIS:
		NET_DNS_Event_OverDnsAnalysis(pClient);
		break;
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_NBIOT_Event_StopMode(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_StopMode			: 停止模式
 @Input				pClient							: DNS客户端实例
 @Return				void
 @attention			当30分钟或有数据需要发送时退出停止模式尝试重启NB,开启NB运行
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_StopMode(DNS_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	static unsigned char DNSMqttSNStatusBasicIndex;
	static unsigned char DNSMqttSNStatusExtendIndex;
	static unsigned char DNSMqttSNInfoWorkIndex;
	static unsigned char DNSMqttSNInfoBasicIndex;
	static unsigned char DNSMqttSNInfoDynamicIndex;
	static unsigned char DNSMqttSNInfoRadarIndex;
	static unsigned char DNSMqttSNInfoResponseIndex;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 1800;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
		/* NBIOT Module Poweroff */
		NBIOT_Neul_NBxx_HardwarePoweroff(pClient->SocketStack->NBIotStack);
		/* Init Message Index */
		DNSMqttSNStatusBasicIndex = NET_MqttSN_Message_StatusBasicRear();
		DNSMqttSNStatusExtendIndex = NET_MqttSN_Message_StatusExtendRear();
		DNSMqttSNInfoWorkIndex = NET_MqttSN_Message_InfoWorkRear();
		DNSMqttSNInfoBasicIndex = NET_MqttSN_Message_InfoBasicRear();
		DNSMqttSNInfoDynamicIndex = NET_MqttSN_Message_InfoDynamicRear();
		DNSMqttSNInfoRadarIndex = NET_MqttSN_Message_InfoRadarRear();
		DNSMqttSNInfoResponseIndex = NET_MqttSN_Message_InfoResponseRear();
	}
	
	if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
		/* Dictate TimeOut */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		pClient->ProcessState = DNS_PROCESS_CREAT_UDP_SOCKET;
		pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_DNS;
	}
	else {
		/* Dictate isn't TimeOut */
		if ( (NET_MqttSN_Message_StatusBasicRear() != DNSMqttSNStatusBasicIndex) || 
			(NET_MqttSN_Message_StatusExtendRear() != DNSMqttSNStatusExtendIndex) ||
			(NET_MqttSN_Message_InfoWorkRear() != DNSMqttSNInfoWorkIndex) ||
			(NET_MqttSN_Message_InfoBasicRear() != DNSMqttSNInfoBasicIndex) ||
			(NET_MqttSN_Message_InfoDynamicRear() != DNSMqttSNInfoDynamicIndex) ||
			(NET_MqttSN_Message_InfoRadarRear() != DNSMqttSNInfoRadarIndex) ||
			(NET_MqttSN_Message_InfoResponseRear() != DNSMqttSNInfoResponseIndex) ) {
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->ProcessState = DNS_PROCESS_CREAT_UDP_SOCKET;
			pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_DNS;
		}
		else {
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			pClient->ProcessState = DNS_PROCESS_CREAT_UDP_SOCKET;
			pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_DNS;
		}
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_NBIOT_Event_HardwareReboot(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_HardwareReboot		: 硬件重启
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_HardwareReboot(DNS_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_HardwareReboot(pClient->SocketStack->NBIotStack, 8000) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MODULE_CHECK;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRebootFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB HDRBT Ok, Baud:%d", NBIOTBaudRate.Baud);
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRebootFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRebootFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRebootFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		}
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB HDRBT Fail");
#endif
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_NBIOT_Event_ModuleCheck(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_ModuleCheck		: 模块检测
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_ModuleCheck(DNS_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if ((NBIOT_Neul_NBxx_CheckReadManufacturer(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadManufacturerModel(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadModuleVersion(pClient->SocketStack->NBIotStack) == NBIOT_OK)) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateModuleCheckFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB MduCk Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateModuleCheckFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateModuleCheckFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateModuleCheckFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MODULE_CHECK;
		}
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB MduCk Fail");
#endif
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_NBIOT_Event_ParameterConfig(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_ParameterConfig	: 参数配置
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_ParameterConfig(DNS_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadConfigUE(pClient->SocketStack->NBIotStack) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ICCID_CHECK;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB CfgRd Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
		}
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB CfgRd Fail");
#endif
		return;
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.nconfig.autoConnect != AutoConnectVal) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient->SocketStack->NBIotStack, AutoConnect, AutoConnectVal) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf_Level2("NB %s %d Ok", AutoConnect, AutoConnectVal);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf_Level2("NB %s %d Fail", AutoConnect, AutoConnectVal);
#endif
			return;
		}
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.nconfig.crScrambling != CrScramblingVal) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient->SocketStack->NBIotStack, CrScrambling, CrScramblingVal) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf_Level2("NB %s %d Ok", CrScrambling, CrScramblingVal);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf_Level2("NB %s %d Fail", CrScrambling, CrScramblingVal);
#endif
			return;
		}
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.nconfig.crSiAvoid != CrSiAvoidVal) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient->SocketStack->NBIotStack, CrSiAvoid, CrSiAvoidVal) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf_Level2("NB %s %d Ok", CrSiAvoid, CrSiAvoidVal);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf_Level2("NB %s %d Fail", CrSiAvoid, CrSiAvoidVal);
#endif
			return;
		}
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.nconfig.combineAttach != CombineAttachVal) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient->SocketStack->NBIotStack, CombineAttach, CombineAttachVal) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf_Level2("NB %s %d Ok", CombineAttach, CombineAttachVal);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf_Level2("NB %s %d Fail", CombineAttach, CombineAttachVal);
#endif
			return;
		}
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.nconfig.cellReselection != CellReselectionVal) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient->SocketStack->NBIotStack, CellReselection, CellReselectionVal) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf_Level2("NB %s %d Ok", CellReselection, CellReselectionVal);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf_Level2("NB %s %d Fail", CellReselection, CellReselectionVal);
#endif
			return;
		}
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.nconfig.enableBip != EnableBipVal) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient->SocketStack->NBIotStack, EnableBip, EnableBipVal) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf_Level2("NB %s %d Ok", EnableBip, EnableBipVal);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf_Level2("NB %s %d Fail", EnableBip, EnableBipVal);
#endif
			return;
		}
	}
}

/**********************************************************************************************************
 @Function			void NET_NET_NBIOT_Event_SimICCIDCheck(DNS_ClientsTypeDef* pClient)
 @Description			NET_NET_NBIOT_Event_SimICCIDCheck		: Sim卡检测
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_SimICCIDCheck(DNS_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadICCID(pClient->SocketStack->NBIotStack) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MISC_EQUIP_CONFIG;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateSimICCIDCheckFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB ICCIDCk Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateSimICCIDCheckFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateSimICCIDCheckFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateSimICCIDCheckFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ICCID_CHECK;
		}
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB ICCIDCk Fail");
#endif
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_NBIOT_Event_MiscEquipConfig(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_MiscEquipConfig	: 其他配置
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_MiscEquipConfig(DNS_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadSupportedBands(pClient->SocketStack->NBIotStack) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_CHECK;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateMiscEquipConfigFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB MiscRd Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateMiscEquipConfigFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateMiscEquipConfigFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateMiscEquipConfigFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MISC_EQUIP_CONFIG;
		}
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB MiscRd Fail");
#endif
		return;
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.band != DNS_NBIOT_BAND) {
		if (NBIOT_Neul_NBxx_SetSupportedBands(pClient->SocketStack->NBIotStack, DNS_NBIOT_BAND) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_CHECK;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateMiscEquipConfigFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf_Level2("NB BandSet %d Ok", DNS_NBIOT_BAND);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateMiscEquipConfigFailureCnt++;
				if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateMiscEquipConfigFailureCnt > 3) {
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateMiscEquipConfigFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MISC_EQUIP_CONFIG;
			}
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf_Level2("NB BandSet %d Fail", DNS_NBIOT_BAND);
#endif
			return;
		}
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_NBIOT_Event_AttachCheck(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_AttachCheck		: 注网检查
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_AttachCheck(DNS_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadAttachOrDetach(pClient->SocketStack->NBIotStack) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_CHECK;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachCheckFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB CGATT %d Ok", pClient->SocketStack->NBIotStack->Parameter.netstate);
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachCheckFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachCheckFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachCheckFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_CHECK;
		}
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB CGATT %d Fail", pClient->SocketStack->NBIotStack->Parameter.netstate);
#endif
		return;
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.netstate != Attach) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_EXECUTE;
	}
	else {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_INQUIRE;
	}
}


/**********************************************************************************************************
 @Function			void NET_DNS_NBIOT_Event_AttachExecute(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_AttachExecute		: 注网进行
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_AttachExecute(DNS_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_SetAttachOrDetach(pClient->SocketStack->NBIotStack, Attach) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_INQUIRE;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachExecuteFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB Set CGATT %d Ok", Attach);
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachExecuteFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachExecuteFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachExecuteFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_EXECUTE;
		}
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB Set CGATT %d Fail", Attach);
#endif
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_NBIOT_Event_AttachInquire(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_AttachInquire		: 注网查询
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_AttachInquire(DNS_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 60;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadAttachOrDetach(pClient->SocketStack->NBIotStack) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_INQUIRE;
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB CGATT %d Ok", pClient->SocketStack->NBIotStack->Parameter.netstate);
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachInquireFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachInquireFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachInquireFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_INQUIRE;
		}
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB CGATT %d Fail", pClient->SocketStack->NBIotStack->Parameter.netstate);
#endif
		return;
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.netstate != Attach) {
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachInquireFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachInquireFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachInquireFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_INQUIRE;
		}
	}
	else {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CHECKOUT;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachInquireFailureCnt = 0;
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_NBIOT_Event_PatameterCheckOut(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_PatameterCheckOut	: 参数检出
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_PatameterCheckOut(DNS_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if ((NBIOT_Neul_NBxx_CheckReadIMEI(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadIMEISV(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadIMSI(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadCGPADDR(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadCGDCONT(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadRSSI(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadStatisticsRADIO(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadStatisticsCELL(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadDateTime(pClient->SocketStack->NBIotStack) == NBIOT_OK)) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = DNS_PROCESS_STACK;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictatePatameterCheckOutFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB Pmte Ck Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictatePatameterCheckOutFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictatePatameterCheckOutFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictatePatameterCheckOutFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CHECKOUT;
		}
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("NB Pmte Ck Fail");
#endif
		return;
	}
	
	/* Set System Time */
	RTC_Set_Date(pClient->SocketStack->NBIotStack->Parameter.dataTime.year, pClient->SocketStack->NBIotStack->Parameter.dataTime.month, pClient->SocketStack->NBIotStack->Parameter.dataTime.day);
	RTC_Set_Time(pClient->SocketStack->NBIotStack->Parameter.dataTime.hour, pClient->SocketStack->NBIotStack->Parameter.dataTime.min,   pClient->SocketStack->NBIotStack->Parameter.dataTime.sec);
}

/**********************************************************************************************************
 @Function			void NET_DNS_Event_CreatUDPSocket(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_Event_CreatUDPSocket			: CreatUDPSocket
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_Event_CreatUDPSocket(DNS_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	/* Creat UDP Socket */
	if (pClient->SocketStack->Open(pClient->SocketStack) == DNS_OK) {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = DNS_PROCESS_STACK;
		pClient->ProcessState = DNS_PROCESS_SEND_DNS_STRUCT_DATA;
		pClient->DictateRunCtl.dictateCreatUDPSocketFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("Creat UDP Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->ProcessState = DNS_PROCESS_CREAT_UDP_SOCKET;
			pClient->DictateRunCtl.dictateCreatUDPSocketFailureCnt++;
			if (pClient->DictateRunCtl.dictateCreatUDPSocketFailureCnt > 3) {
				pClient->DictateRunCtl.dictateCreatUDPSocketFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
				pClient->ProcessState = DNS_PROCESS_CREAT_UDP_SOCKET;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = DNS_PROCESS_STACK;
			pClient->ProcessState = DNS_PROCESS_CREAT_UDP_SOCKET;
		}
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("Creat UDP Fail");
#endif
		return;
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_Event_SendDnsStructData(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_Event_SendDnsStructData		: SendDnsStructData
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_Event_SendDnsStructData(DNS_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	unsigned char Sendhostname[DNS_HOSTNAME_SIZE];
	
	if (pClient->AnalysisTick < DNS_ANALYSIS_DATA) {
		/* Have Domain name need to resolution */
		
		/* It is the first time to execute */
		if (pClient->DictateRunCtl.dictateEnable != true) {
			pClient->DictateRunCtl.dictateEnable = true;
			pClient->DictateRunCtl.dictateTimeoutSec = 30;
			Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
			pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
		}
		
		/* Serialize dnsDataStructure Command Buffer */
		memset((void*)Sendhostname, 0, sizeof(Sendhostname));
		memcpy(Sendhostname, pClient->AnalysisData[pClient->AnalysisTick].hostname, sizeof(pClient->AnalysisData[pClient->AnalysisTick].hostname));
		DNSSerialize_dnsDataStructure(pClient, Sendhostname);
		
		/* Send dnsDataStructure Command Buffer to DNS Server */
		if (pClient->SocketStack->Write(pClient->SocketStack, (char *)pClient->Sendbuf, pClient->Sendlen) != DNS_OK) {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->ProcessState = DNS_PROCESS_CREAT_UDP_SOCKET;
				pClient->DictateRunCtl.dictateSendDnsStructDataFailureCnt++;
				if (pClient->DictateRunCtl.dictateSendDnsStructDataFailureCnt > 3) {
					pClient->DictateRunCtl.dictateSendDnsStructDataFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
					pClient->ProcessState = DNS_PROCESS_CREAT_UDP_SOCKET;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = DNS_PROCESS_STACK;
				pClient->ProcessState = DNS_PROCESS_SEND_DNS_STRUCT_DATA;
			}
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf_Level2("DNS Sd %s Fail", pClient->AnalysisData[pClient->AnalysisTick].hostnameAddr);
#endif
			return;
		}
		else {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = DNS_PROCESS_STACK;
			pClient->ProcessState = DNS_PROCESS_RECV_DNS_STRUCT_DATA;
			pClient->DictateRunCtl.dictateSendDnsStructDataFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf_Level2("DNS Sd %s Ok", pClient->AnalysisData[pClient->AnalysisTick].hostnameAddr);
#endif
		}
	}
	else {
		/* None Domain name need to resolution */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = DNS_PROCESS_STACK;
		pClient->ProcessState = DNS_PROCESS_CLOSE_UDP_SOCKET;
		pClient->DictateRunCtl.dictateSendDnsStructDataFailureCnt = 0;
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_Event_RecvDnsStructData(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_Event_RecvDnsStructData		: RecvDnsStructData
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_Event_RecvDnsStructData(DNS_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	int Rlength = 0;													//读取数据长度
	int Rleftlength = 0;												//剩余读取数据长度
	
	if (pClient->AnalysisTick < DNS_ANALYSIS_DATA) {
		/* Have Domain name need to resolution */
		
		/* It is the first time to execute */
		if (pClient->DictateRunCtl.dictateEnable != true) {
			pClient->DictateRunCtl.dictateEnable = true;
			pClient->DictateRunCtl.dictateTimeoutSec = 60;
			Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
			pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
		}
		
		/* Recv dnsDataStructure Command Buffer to DNS Server */
		if (pClient->SocketStack->Read(pClient->SocketStack, (char *)pClient->Recvbuf, pClient->Recvbuf_size, &Rlength, &Rleftlength) != DNS_OK) {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->ProcessState = DNS_PROCESS_CREAT_UDP_SOCKET;
				pClient->DictateRunCtl.dictateRecvDnsStructDataFailureCnt++;
				if (pClient->DictateRunCtl.dictateRecvDnsStructDataFailureCnt > 3) {
					pClient->DictateRunCtl.dictateRecvDnsStructDataFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
					pClient->ProcessState = DNS_PROCESS_CREAT_UDP_SOCKET;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = DNS_PROCESS_STACK;
				pClient->ProcessState = DNS_PROCESS_RECV_DNS_STRUCT_DATA;
			}
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf_Level2("DNS Wait Recv %s", pClient->AnalysisData[pClient->AnalysisTick].hostnameAddr);
#endif
			return;
		}
		else {
			/* Dictate execute is Success */
			if (Rlength == 0) {
				/* No Recv Data */
				if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
					/* Dictate TimeOut */
					pClient->DictateRunCtl.dictateEnable = false;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
					pClient->ProcessState = DNS_PROCESS_CREAT_UDP_SOCKET;
					pClient->DictateRunCtl.dictateRecvDnsStructDataFailureCnt++;
					if (pClient->DictateRunCtl.dictateRecvDnsStructDataFailureCnt > 3) {
						pClient->DictateRunCtl.dictateRecvDnsStructDataFailureCnt = 0;
						pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
						pClient->ProcessState = DNS_PROCESS_CREAT_UDP_SOCKET;
					}
				}
				else {
					/* Dictate isn't TimeOut */
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = DNS_PROCESS_STACK;
					pClient->ProcessState = DNS_PROCESS_RECV_DNS_STRUCT_DATA;
				}
				return;
			}
			else {
				/* Is Recv Data */
				pClient->Recvlen = Rlength;
				
				/* Deserialize dnsDataStructure Command Buffer */
				if (DNSDeserialize_dnsDataStructure(pClient, pClient->AnalysisData[pClient->AnalysisTick].hostname) != DNS_OK) {
					/* Deserialize into IP Error */
					pClient->DictateRunCtl.dictateEnable = false;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
					pClient->ProcessState = DNS_PROCESS_CREAT_UDP_SOCKET;
					pClient->DictateRunCtl.dictateRecvDnsStructDataFailureCnt++;
					if (pClient->DictateRunCtl.dictateRecvDnsStructDataFailureCnt > 3) {
						pClient->DictateRunCtl.dictateRecvDnsStructDataFailureCnt = 0;
						pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
						pClient->ProcessState = DNS_PROCESS_CREAT_UDP_SOCKET;
					}
#ifdef DNS_DEBUG_LOG_RF_PRINT
					Radio_Trf_Debug_Printf_Level2("DNS Analysis %s Fail", pClient->AnalysisData[pClient->AnalysisTick].hostnameAddr);
#endif
					return;
				}
				else {
					/* Deserialize into IP OK */
					pClient->DictateRunCtl.dictateEnable = false;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = DNS_PROCESS_STACK;
					pClient->ProcessState = DNS_PROCESS_SEND_DNS_STRUCT_DATA;
					pClient->DictateRunCtl.dictateRecvDnsStructDataFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
					Radio_Trf_Debug_Printf_Level2("DNS Analysis %s OK", pClient->AnalysisData[pClient->AnalysisTick].hostnameAddr);
					Radio_Trf_Debug_Printf_Level2("%s : %s", pClient->AnalysisData[pClient->AnalysisTick].hostnameAddr, pClient->AnalysisData[pClient->AnalysisTick].hostIP);
#endif
					pClient->AnalysisTick += 1;
				}
			}
		}
	}
	else {
		/* None Domain name need to resolution */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = DNS_PROCESS_STACK;
		pClient->ProcessState = DNS_PROCESS_CLOSE_UDP_SOCKET;
		pClient->DictateRunCtl.dictateRecvDnsStructDataFailureCnt = 0;
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_Event_CloseUDPSocket(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_Event_CloseUDPSocket			: CloseUDPSocket
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_Event_CloseUDPSocket(DNS_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	/* Close UDP Socket */
	if (pClient->SocketStack->Close(pClient->SocketStack) == DNS_OK) {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = DNS_PROCESS_STACK;
		pClient->ProcessState = DNS_PROCESS_OVER_DNS_ANALYSIS;
		pClient->DictateRunCtl.dictateCloseUDPSocketFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("Close UDP Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->ProcessState = DNS_PROCESS_OVER_DNS_ANALYSIS;
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = DNS_PROCESS_STACK;
			pClient->ProcessState = DNS_PROCESS_CLOSE_UDP_SOCKET;
		}
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf_Level2("Close UDP Fail");
#endif
		return;
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_Event_OverDnsAnalysis(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_Event_OverDnsAnalysis			: OverDnsAnalysis
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_Event_OverDnsAnalysis(DNS_ClientsTypeDef* pClient)
{
	MQTTSN_Transport_Init(&MqttSNSocketNetHandler, &NbiotClientHandler, MQTTSN_SERVER_LOCAL_PORT, (char*)DNS_GetHostIP(pClient, (unsigned char*)MQTTSN_SERVER_HOST_NAME), MQTTSN_SERVER_TELE_PORT);
	
	pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
	pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MISC_EQUIP_CONFIG;
	pClient->ProcessState = DNS_PROCESS_CREAT_UDP_SOCKET;
}

/********************************************** END OF FLEE **********************************************/
