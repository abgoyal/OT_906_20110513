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
*   @file   rpc_ser.c
*
*   @brief  This file define the request/respose structure for 
*	serialize/deserialize.
*
****************************************************************************/
#ifndef UNDER_LINUX
#include "stdlib.h"
#endif
#include "rpc_internal_api.h"

#define MAX_MSG_STREAM_SIZE	2048

#ifdef XDR_DETAIL_LOG_ENABLED
#define MAX_LOG_BUFFER_SIZE 4000 //~4.5K
static char log_buf[MAX_LOG_BUFFER_SIZE+100];
#endif


Result_t RPC_SerializeMsg(RPC_InternalMsg_t* rpcMsg, char* stream, UInt32 streamLen, UInt32 *outLen)
{
	XDR xdrs;
	Result_t result = RESULT_OK;
	
	if(DETAIL_LOG_ENABLED)
		xdrmem_create(&xdrs, stream, streamLen, log_buf, MAX_LOG_BUFFER_SIZE, XDR_ENCODE);
	else
		xdrmem_create(&xdrs, stream, streamLen, NULL, 0, XDR_ENCODE);

	result = (xdr_RPC_InternalMsg_t(&xdrs, rpcMsg)) ? RESULT_OK : RESULT_ERROR;

#ifdef XDR_DETAIL_LOG_ENABLED
			if(DETAIL_LOG_ENABLED)
			{
				if(rpcMsg->rootMsg.msgId != MSG_CAPI2_ACK_RSP && (xdrs.x_op == XDR_ENCODE || xdrs.x_op == XDR_DECODE))
				{
					if((strlen(log_buf)+3) < MAX_LOG_BUFFER_SIZE)
						strncat(log_buf,"}\r\n",3);
					_DBG_(Rpc_DebugOutputString(log_buf));
				}
			}
#endif

	*outLen = xdr_getpos(&xdrs);
	xdr_destroy(&xdrs);

	return(result);
}

Result_t RPC_SendMsg(RPC_InternalMsg_t* rpcMsg)
{
	char* stream;
	bool_t ret;
	UInt32 len;
	PACKET_InterfaceType_t rpcInterfaceType;
	PACKET_BufHandle_t bufHandle;
	//coverity[var_decl], "entry" will be inited in function rpc_fast_lookup()
	RPC_InternalXdrInfo_t entry;
	UInt32 maxPktSize =0;
	Result_t result = RESULT_OK;

	ret = rpc_fast_lookup((UInt16)rpcMsg->rootMsg.msgId, &entry);
	if(!ret || entry.xdrInfo == NULL)
	{
		_DBG_(RPC_TRACE("RPC_SerializeMsg: failed!"));
		return RESULT_ERROR;
	}

	if(rpcMsg->msgType == RPC_TYPE_REQUEST)
	{
		rpcMsg->clientIndex = entry.clientIndex;
	}

	rpcInterfaceType = RPC_GetInterfaceType((UInt8)entry.clientIndex);
	maxPktSize = RPC_GetMaxPktSize(rpcInterfaceType, entry.xdrInfo->maxMsgSize);
	maxPktSize = (maxPktSize==0) ? MAX_MSG_STREAM_SIZE : maxPktSize;

	bufHandle = RPC_PACKET_AllocateBuffer (rpcInterfaceType, maxPktSize, 0);
	if(!bufHandle)
		return RESULT_ERROR;

	stream = RPC_PACKET_GetBufferData(bufHandle);
	if(stream == 0)
		return RESULT_ERROR;

	result = RPC_SerializeMsg(rpcMsg, stream, maxPktSize, &len);

	RPC_PACKET_SetBufferLength(bufHandle, len);
	RPC_PACKET_SendData(0, rpcInterfaceType, 0, bufHandle);

	return result;
}


Result_t RPC_SerializeReq(RPC_Msg_t* rpcMsg)
{
	RPC_InternalMsg_t rpcInMsg;

	rpcInMsg.rootMsg = *rpcMsg;
	rpcInMsg.msgType = RPC_TYPE_REQUEST;

	return RPC_SendMsg(&rpcInMsg);
}

Result_t RPC_SerializeRsp(RPC_Msg_t* rpcMsg)
{
	RPC_InternalMsg_t rpcInMsg;

	rpcInMsg.rootMsg = *rpcMsg;
	rpcInMsg.msgType = RPC_TYPE_RESPONSE;

	return RPC_SendMsg(&rpcInMsg);
}


/* Deserialize the response stream passed by the IPC */
Result_t RPC_DeserializeMsg( char* stream, UInt32 stream_len, ResultDataBuffer_t *dataBuf)
{
	XDR *xdrs = NULL;
	RPC_InternalMsg_t* rpcMsg = NULL;
	Result_t result = RESULT_OK;

	if(dataBuf)
	{
		xdrs = &dataBuf->xdrs;
		rpcMsg = &dataBuf->rsp;
		/* Deserialize the request/response */

		memset(rpcMsg, 0, sizeof(RPC_InternalMsg_t));
		
		if(DETAIL_LOG_ENABLED)
			xdrmem_create( xdrs, stream, stream_len, log_buf, MAX_LOG_BUFFER_SIZE, XDR_DECODE );
		else
			xdrmem_create( xdrs, stream, stream_len, NULL, 0, XDR_DECODE );

		result = (xdr_RPC_InternalMsg_t( xdrs, rpcMsg)) ? RESULT_OK : RESULT_ERROR; 

#ifdef XDR_DETAIL_LOG_ENABLED
			if(DETAIL_LOG_ENABLED)
			{
				if(rpcMsg->rootMsg.msgId != MSG_CAPI2_ACK_RSP && (xdrs->x_op == XDR_ENCODE || xdrs->x_op == XDR_DECODE))
				{
					if((strlen(log_buf)+3) < MAX_LOG_BUFFER_SIZE)
						strncat(log_buf,"}\r\n",3);
					_DBG_(Rpc_DebugOutputString(log_buf));
				}
			}
#endif

	}
	else
	{
		result = RESULT_ERROR;
	}

	/* The AP side want to free up the deserialized buffer. */
	return(result);
}


void RPC_SYSFreeResultDataBuffer(ResultDataBufHandle_t dataBufHandle)
{
	ResultDataBuffer_t* dataBuf = (ResultDataBuffer_t*)dataBufHandle;

	if(dataBuf && dataBuf->refCount > 0)
	{
		--(dataBuf->refCount);
		if(dataBuf->refCount == 0)
	{
		/* Free up the memory allocated during deserialization of the pointers */
		dataBuf->xdrs.x_op = XDR_FREE;
		xdr_RPC_InternalMsg_t( &dataBuf->xdrs, &dataBuf->rsp );
		capi2_free(dataBuf);
	}
}
	else
	{
		_DBG_(RPC_TRACE("ERROR: failed! RPC_SYSFreeResultDataBuffer ref=%d", (dataBuf)?dataBuf->refCount:-1));
	}
}


Result_t RPC_SendAck(UInt32 tid, UInt8 clientID, RPC_ACK_Result_t ackResult, UInt32 ackUsrData, UInt8 clientIndex)
{
	RPC_InternalMsg_t rpcInMsg;
	RPC_Ack_t			RPC_Ack_Rsp;

	/* message specific */
	RPC_Ack_Rsp.ackResult = ackResult;
	RPC_Ack_Rsp.ackUsrData = ackUsrData;


	rpcInMsg.rootMsg.msgId = MSG_CAPI2_ACK_RSP;
	rpcInMsg.rootMsg.tid = tid;
	rpcInMsg.rootMsg.clientID = clientID;
	rpcInMsg.rootMsg.dataBuf = &RPC_Ack_Rsp;
	rpcInMsg.clientIndex = clientIndex;

	rpcInMsg.msgType = RPC_TYPE_ACK;

	return RPC_SendMsg(&rpcInMsg);
}

Result_t RPC_SendAckForRequest(ResultDataBufHandle_t handle, UInt32 ackUsrData)
{
	ResultDataBuffer_t* dataBuf = (ResultDataBuffer_t*)handle;
	if(dataBuf)
	{
		return RPC_SendAck(dataBuf->rsp.rootMsg.tid, dataBuf->rsp.rootMsg.clientID, ACK_SUCCESS, ackUsrData, dataBuf->rsp.clientIndex);
	}
	return RESULT_ERROR;
}

Boolean RPC_IsValidMsg(MsgType_t msgID)
{
	RPC_InternalXdrInfo_t entry;
	return rpc_fast_lookup((UInt16)msgID, &entry);
}

UInt32 RPC_GetMsgPayloadSize(MsgType_t msgID)
{
	RPC_InternalXdrInfo_t entry;
	return (rpc_fast_lookup((UInt16)msgID, &entry))?entry.xdrInfo->xdr_size:0;
}

