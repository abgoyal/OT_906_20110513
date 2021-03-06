/****************************************************************************
*
*     Copyright (c) 2007-2008 Broadcom Corporation
*
*   Unless you and Broadcom execute a separate written software license 
*   agreement governing use of this software, this software is licensed to you 
*   under the terms of the GNU General Public License version 2, available 
*    at http://www.gnu.org/licenses/old-licenses/gpl-2.0.html (the "GPL"). 
*
*   Notwithstanding the above, under no circumstances may you combine this 
*   software in any way with any other Broadcom software provided under a license 
*   other than the GPL, without Broadcom's express prior written consent.
*
****************************************************************************/
/**
*
*   @file   rpc_ipc_config.h
*
*   @brief  This file defines the initial config types
*
****************************************************************************/

#ifndef _RPC_IPC_CONFIG_H_
#define _RPC_IPC_CONFIG_H_

#define CFG_RPC_CMD_MAX_PACKETS	16
#define CFG_RPC_CMD_MAX_CP2AP_PACKETS	32
#define CFG_RPC_CMD_PKT_SIZE		2048

#define CFG_RPC_CMD_MAX_PACKETS2	2
#define CFG_RPC_CMD_PKT_SIZE2		25600

#define CFG_RPC_CMD_MAX_PACKETS3	2
#define CFG_RPC_CMD_PKT_SIZE3		65536

#define CFG_RPC_CMD_START_THRESHOLD		1
#define CFG_RPC_CMD_END_THRESHOLD			0

/*TE --> NW i.e. UL IPC Buffer Count*/
#define CFG_RPC_PKTDATA_MAX_TE2NW_PACKETS	64
/*NW --> TE i.e. DL IPC Buffer Count*/
#define CFG_RPC_PKTDATA_MAX_NW2TE_PACKETS	64

#define PDCP_MAX_HEADER_SIZE 168

/* Increased from 1600 to allow 168 for header decompresion */
#define CFG_RPC_PKTDATA_PKT_SIZE		(1600 + PDCP_MAX_HEADER_SIZE)

#define CFG_RPC_PKT_START_THRESHOLD		1
#define CFG_RPC_PKT_END_THRESHOLD		    0

/*TE --> NW i.e. UL IPC CSD Buffer Count*/
#define CFG_RPC_CSDDATA_MAX_TE2NW_PACKETS		4
/*NW --> TE i.e. DL IPC CSD Buffer Count*/
#define CFG_RPC_CSDDATA_MAX_NW2TE_PACKETS		20
#define CFG_RPC_CSDDATA_PKT_SIZE		512

#define CFG_RPC_CSD_START_THRESHOLD		1
#define CFG_RPC_CSD_END_THRESHOLD		    0

#define CFG_RPC_LOG_MAX_PACKETS		 50
#define CFG_RPC_LOG_PKT_SIZE			5000
#define CFG_RPC_LOG_START_THRESHOLD	  1
#define CFG_RPC_LOG_END_THRESHOLD		  0


#define CFG_RPC_SSIPC40_MAX_PACKETS			20
#define CFG_RPC_SSIPC40_PKT_SIZE			512

#define CFG_RPC_SSIPC40_MAX_PACKETS1		20
#define CFG_RPC_SSIPC40_PKT_SIZE1			1024

#define CFG_RPC_SSIPC40_MAX_PACKETS2		10
#define CFG_RPC_SSIPC40_PKT_SIZE2			2048

#define CFG_RPC_SSIPC40_MAX_PACKETS3			0
#define CFG_RPC_SSIPC40_PKT_SIZE3				0	


#define CFG_RPC_SSIPC40_START_THRESHOLD	  1
#define CFG_RPC_SSIPC40_END_THRESHOLD		  0

#endif  // _RPC_IPC_CONFIG_H_

