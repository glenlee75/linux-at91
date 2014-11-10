
#include "NMI_OsWrapper/include/NMI_OSWrapper.h"

#ifdef CONFIG_NMI_MSG_QUEUE_FEATURE


/*!
*  @author		syounan
*  @date		1 Sep 2010
*  @note		copied from FLO glue implementatuion
*  @version		1.0
*/
NMI_ErrNo NMI_MsgQueueCreate(NMI_MsgQueueHandle* pHandle,
			tstrNMI_MsgQueueAttrs* pstrAttrs)
{
	tstrNMI_SemaphoreAttrs strSemAttrs;
	NMI_SemaphoreFillDefault(&strSemAttrs);
	strSemAttrs.u32InitCount = 0;
	
	if( (NMI_SemaphoreCreate(&pHandle->hSem, &strSemAttrs) == NMI_SUCCESS)
		&& (NMI_SemaphoreCreate(&pHandle->strCriticalSection, NMI_NULL) == NMI_SUCCESS))
	{
	
		pHandle->pstrMessageList = NULL;
		pHandle->u32ReceiversCount = 0;
		pHandle->bExiting = NMI_FALSE;

		return NMI_SUCCESS;
	}
	else
	{
		return NMI_FAIL;
	}
}

/*!
*  @author		syounan
*  @date		1 Sep 2010
*  @note		copied from FLO glue implementatuion
*  @version		1.0
*/
NMI_ErrNo NMI_MsgQueueDestroy(NMI_MsgQueueHandle* pHandle,
			tstrNMI_MsgQueueAttrs* pstrAttrs)
{

	pHandle->bExiting = NMI_TRUE;

	// Release any waiting receiver thread.
	while(pHandle->u32ReceiversCount > 0)
	{
		NMI_SemaphoreRelease(&(pHandle->hSem), NMI_NULL);	
		pHandle->u32ReceiversCount--;
	}

	NMI_SemaphoreDestroy(&pHandle->hSem, NMI_NULL);
	NMI_SemaphoreDestroy(&pHandle->strCriticalSection, NMI_NULL);
	
	while(pHandle->pstrMessageList != NULL)
	{
		Message * pstrMessge = pHandle->pstrMessageList->pstrNext;
		NMI_FREE(pHandle->pstrMessageList);
		pHandle->pstrMessageList = pstrMessge;	
	}

	return NMI_SUCCESS;
}

/*!
*  @author		syounan
*  @date		1 Sep 2010
*  @note		copied from FLO glue implementatuion
*  @version		1.0
*/
NMI_ErrNo NMI_MsgQueueSend(NMI_MsgQueueHandle* pHandle,
			const void * pvSendBuffer, NMI_Uint32 u32SendBufferSize,
			tstrNMI_MsgQueueAttrs* pstrAttrs)
{
	NMI_ErrNo s32RetStatus = NMI_SUCCESS;

	Message * pstrMessage = NULL;
	
	if( (pHandle == NULL) || (u32SendBufferSize == 0) || (pvSendBuffer == NULL) )
	{
		NMI_ERRORREPORT(s32RetStatus, NMI_INVALID_ARGUMENT);
	}

	if(pHandle->bExiting == NMI_TRUE)
	{
		NMI_ERRORREPORT(s32RetStatus, NMI_FAIL);
	}

	NMI_SemaphoreAcquire(&pHandle->strCriticalSection, NMI_NULL);
	
	/* construct a new message */
	pstrMessage = NMI_NEW(Message, 1);
	NMI_NULLCHECK(s32RetStatus, pstrMessage);
	pstrMessage->u32Length = u32SendBufferSize;
	pstrMessage->pstrNext = NULL;
	pstrMessage->pvBuffer = NMI_MALLOC(u32SendBufferSize);
	NMI_NULLCHECK(s32RetStatus, pstrMessage->pvBuffer);
	NMI_memcpy(pstrMessage->pvBuffer, pvSendBuffer, u32SendBufferSize);
	

	/* add it to the message queue */
	if(pHandle->pstrMessageList == NULL)
	{
		pHandle->pstrMessageList  = pstrMessage;
	}
	else
	{
		Message * pstrTailMsg = pHandle->pstrMessageList;
		while(pstrTailMsg->pstrNext != NULL)
		{
			pstrTailMsg = pstrTailMsg->pstrNext;
		}
		pstrTailMsg->pstrNext = pstrMessage;
	}	
	
	NMI_SemaphoreRelease(&pHandle->strCriticalSection, NMI_NULL);

	NMI_SemaphoreRelease(&pHandle->hSem, NMI_NULL);
	
	NMI_CATCH(s32RetStatus)
	{
		/* error occured, free any allocations */
		if(pstrMessage != NULL)
		{
			if(pstrMessage->pvBuffer != NULL)
			{
				NMI_FREE(pstrMessage->pvBuffer);
			}
			NMI_FREE(pstrMessage);
		}
	}

	return s32RetStatus; 
}



/*!
*  @author		syounan
*  @date		1 Sep 2010
*  @note		copied from FLO glue implementatuion
*  @version		1.0
*/
NMI_ErrNo NMI_MsgQueueRecv(NMI_MsgQueueHandle* pHandle, 
			void * pvRecvBuffer, NMI_Uint32 u32RecvBufferSize, 
			NMI_Uint32* pu32ReceivedLength,
			tstrNMI_MsgQueueAttrs* pstrAttrs)
{

	Message * pstrMessage;
	NMI_ErrNo s32RetStatus = NMI_SUCCESS;

	if( (pHandle == NULL) || (u32RecvBufferSize == 0) 
		|| (pvRecvBuffer == NULL) || (pu32ReceivedLength == NULL) )
	{
		NMI_ERRORREPORT(s32RetStatus, NMI_INVALID_ARGUMENT);
	}

	if(pHandle->bExiting == NMI_TRUE)
	{
		NMI_ERRORREPORT(s32RetStatus, NMI_FAIL);
	}
	
	NMI_SemaphoreAcquire(&pHandle->strCriticalSection, NMI_NULL);
	pHandle->u32ReceiversCount++;
	NMI_SemaphoreRelease(&pHandle->strCriticalSection, NMI_NULL);

	s32RetStatus = NMI_SemaphoreAcquire(&(pHandle->hSem), NMI_NULL);
	NMI_ERRORCHECK(s32RetStatus);

	if(pHandle->bExiting)
	{
		NMI_ERRORREPORT(s32RetStatus, NMI_FAIL);
	}

	NMI_SemaphoreAcquire(&pHandle->strCriticalSection, NMI_NULL);
	
	pstrMessage = pHandle->pstrMessageList;
	if(pstrMessage == NULL)
	{
		NMI_SemaphoreRelease(&pHandle->strCriticalSection, NMI_NULL);
		NMI_ERRORREPORT(s32RetStatus, NMI_FAIL);
	}
	/* check buffer size */
	if(u32RecvBufferSize < pstrMessage->u32Length)
	{
		NMI_SemaphoreRelease(&pHandle->strCriticalSection, NMI_NULL);
		NMI_SemaphoreRelease(&pHandle->hSem, NMI_NULL);
		NMI_ERRORREPORT(s32RetStatus, NMI_BUFFER_OVERFLOW);
	}

	/* consume the message */
	pHandle->u32ReceiversCount--;
	NMI_memcpy(pvRecvBuffer, pstrMessage->pvBuffer, pstrMessage->u32Length);
	*pu32ReceivedLength = pstrMessage->u32Length;

	pHandle->pstrMessageList = pstrMessage->pstrNext;
	
	NMI_FREE(pstrMessage->pvBuffer);
	NMI_FREE(pstrMessage);	
	
	NMI_SemaphoreRelease(&pHandle->strCriticalSection, NMI_NULL);

	NMI_CATCH(s32RetStatus)
	{
	}
	return s32RetStatus;
}

#endif
