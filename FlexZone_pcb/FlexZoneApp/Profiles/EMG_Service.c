/******************************************************************************
 * Filename:       EMG_Service.c
 *
 * Description:    This file contains the implementation of the service.
 *
 *                 Generated by:
 *                 BDS version: 1.0.2093.0
 *                 Plugin:      Texas Instruments CC26xx BLE SDK v2.1 GATT Server plugin 1.0.5 beta
 *                 Time:        Tue Feb 02 2016 11:27:25 GMT+01:00
 *

 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **********************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include <EMG_Service.h>
#include <string.h>

//#define xdc_runtime_Log_DISABLE_ALL 1  // Add to disable logs from this file
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>

#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
* GLOBAL VARIABLES
*/

// Data_Service Service UUID
CONST uint8_t EMGServiceUUID[ATT_UUID_SIZE] =
{
  EMG_SERVICE_SERV_UUID_BASE128(EMG_SERVICE_SERV_UUID)
};

// String UUID
CONST uint8_t emg_configUUID[ATT_UUID_SIZE] =
{
  EMG_CONFIG_UUID_BASE128(EMG_CONFIG_UUID)
};

// Stream UUID
CONST uint8_t emg_StreamUUID[ATT_UUID_SIZE] =
{
  EMG_STREAM_UUID_BASE128(EMG_STREAM_UUID)
};


/*********************************************************************
 * LOCAL VARIABLES
 */

static EMGServiceCBs_t *pAppCBs = NULL;
static uint8_t emg_icall_rsp_task_id = INVALID_TASK_ID;

/*********************************************************************
* Profile Attributes - variables
*/

// Service declaration
static CONST gattAttrType_t EMGServiceDecl = { ATT_UUID_SIZE, EMGServiceUUID };

// Characteristic "String" Properties (for declaration)
static uint8_t emg_ConfigProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic "String" Value variable
static uint8_t emg_ConfigVal[EMG_CONFIG_LEN] = {0};

// Length of data in characteristic "String" Value variable, initialized to minimal size.
static uint16_t emg_ConfigValLen = EMG_CONFIG_LEN_MIN;


// Characteristic "Stream" Properties (for declaration)
static uint8_t emg_StreamProps = GATT_PROP_NOTIFY | GATT_PROP_WRITE_NO_RSP;

// Characteristic "Stream" Value variable
static uint8_t emg_StreamVal[EMG_STREAM_LEN] = {0};

// Length of data in characteristic "Stream" Value variable, initialized to minimal size.
static uint16_t emg_StreamValLen = EMG_STREAM_LEN_MIN;

// Characteristic "Stream" Client Characteristic Configuration Descriptor
static gattCharCfg_t *emg_StreamConfig;

static char emg_UserStreamString[] = "EMG Data";
static char emg_UserConfigString[] = "EMG Config";

/*********************************************************************
* Profile Attributes - Table
*/

static gattAttribute_t EMG_ServiceAttrTbl[] =
{
  // Data_Service Service Declaration
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID },
    GATT_PERMIT_READ,
    0,
    (uint8_t *)&EMGServiceDecl
  },
    // String Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &emg_ConfigProps
    },
      // Config Characteristic Value
      {
        { ATT_UUID_SIZE, emg_configUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        emg_ConfigVal
      },
	  // config CUD
		{
		  { ATT_BT_UUID_SIZE, charUserDescUUID },
		  GATT_PERMIT_READ,
		  0,
		  (uint8_t *)&emg_UserConfigString
		},

    // Stream Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &emg_StreamProps
    },
      // Stream Characteristic Value
      {
        { ATT_UUID_SIZE, emg_StreamUUID },
        GATT_PERMIT_WRITE,
        0,
        emg_StreamVal
      },
      // Stream CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *)&emg_StreamConfig
      },

	  // Stream CUD
		{
		  { ATT_BT_UUID_SIZE, charUserDescUUID },
		  GATT_PERMIT_READ,
		  0,
		  (uint8_t *)&emg_UserStreamString
		},
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t EMG_Service_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                           uint16_t maxLen, uint8_t method );
static bStatus_t EMG_Service_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                            uint8_t *pValue, uint16_t len, uint16_t offset,
                                            uint8_t method );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t EMG_ServiceCBs =
{
  EMG_Service_ReadAttrCB,  // Read callback function pointer
  EMG_Service_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*
 * EMGService_AddService- Initializes the DataService service by registering
 *          GATT attributes with the GATT server.
 *
 *    rspTaskId - The ICall Task Id that should receive responses for Indications.
 */
extern bStatus_t EMGService_AddService( uint8_t rspTaskId )
{
  uint8_t status;

  // Allocate Client Characteristic Configuration table
  emg_StreamConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( emg_StreamConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, emg_StreamConfig );
  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService( EMG_ServiceAttrTbl,
                                        GATT_NUM_ATTRS( EMG_ServiceAttrTbl ),
                                        GATT_MAX_ENCRYPT_KEY_SIZE,
                                        &EMG_ServiceCBs );
  Log_info1("Registered service, %d attributes", (IArg)GATT_NUM_ATTRS( EMG_ServiceAttrTbl ));
  emg_icall_rsp_task_id = rspTaskId;

  return ( status );
}

/*
 * EMGService_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
bStatus_t EMGService_RegisterAppCBs( EMGServiceCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    pAppCBs = appCallbacks;
    Log_info1("Registered callbacks to application. Struct %p", (IArg)appCallbacks);
    return ( SUCCESS );
  }
  else
  {
    Log_warning0("Null pointer given for app callbacks.");
    return ( FAILURE );
  }
}

/*
 * EMGService_SetParameter - Set a DataService parameter.
 *
 *    param - Profile parameter ID
 *    len   - length of data to write
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate
 *            data type (example: data type of uint16_t will be cast to
 *            uint16_t pointer).
 */
bStatus_t EMGService_SetParameter( uint8_t param, uint16_t len, void *value )
{
  bStatus_t ret = SUCCESS;
  uint8_t  *pAttrVal;
  uint16_t *pValLen;
  uint16_t valMinLen;
  uint16_t valMaxLen;
  uint8_t sendNotiInd = FALSE;
  gattCharCfg_t *attrConfig;
  uint8_t needAuth;

  switch ( param )
  {
    case EMG_CONFIG_ID:
      pAttrVal  =  emg_ConfigVal;
      pValLen   = &emg_ConfigValLen;
      valMinLen =  EMG_CONFIG_LEN_MIN;
      valMaxLen =  EMG_CONFIG_LEN;
      Log_info2("SetParameter : %s len: %d", (IArg)"String", (IArg)len);
      break;

    case EMG_STREAM_ID:
      pAttrVal  =  emg_StreamVal;
      pValLen   = &emg_StreamValLen;
      valMinLen =  EMG_STREAM_LEN_MIN;
      valMaxLen =  EMG_STREAM_LEN;
      sendNotiInd = TRUE;
      attrConfig  = emg_StreamConfig;
      needAuth    = FALSE; // Change if authenticated link is required for sending.
      Log_info2("SetParameter : %s len: %d", (IArg)"Stream", (IArg)len);
      break;

    default:
      Log_error1("SetParameter: Parameter #%d not valid.", (IArg)param);
      return INVALIDPARAMETER;
  }

  // Check bounds, update value and send notification or indication if possible.
  if ( len <= valMaxLen && len >= valMinLen )
  {
    memcpy(pAttrVal, value, len);
    *pValLen = len; // Update length for read and get.

    if (sendNotiInd)
    {
      Log_info2("Trying to send noti/ind: connHandle %x, %s",
                (IArg)attrConfig[0].connHandle,
                (IArg)((attrConfig[0].value==0)?"\x1b[33mNoti/ind disabled\x1b[0m" :
                       (attrConfig[0].value==1)?"Notification enabled" :
                                                "Indication enabled"));
      // Try to send notification.
      GATTServApp_ProcessCharCfg( attrConfig, pAttrVal, needAuth,
                                  EMG_ServiceAttrTbl, GATT_NUM_ATTRS( EMG_ServiceAttrTbl ),
                                  emg_icall_rsp_task_id,  EMG_Service_ReadAttrCB);
    }
  }
  else
  {
    Log_error3("Length outside bounds: Len: %d MinLen: %d MaxLen: %d.", (IArg)len, (IArg)valMinLen, (IArg)valMaxLen);
    ret = bleInvalidRange;
  }

  return ret;
}


/*
 * EMGService_GetParameter - Get a DataService parameter.
 *
 *    param - Profile parameter ID
 *    len   - pointer to a variable that contains the maximum length that can be written to *value.
              After the call, this value will contain the actual returned length.
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate
 *            data type (example: data type of uint16_t will be cast to
 *            uint16_t pointer).
 */
bStatus_t EMGService_GetParameter( uint8_t param, uint16_t *len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case EMG_CONFIG_ID:
      *len = MIN(*len, emg_ConfigValLen);
      memcpy(value, emg_ConfigVal, *len);
      Log_info2("GetParameter : %s returning %d bytes", (IArg)"String", (IArg)*len);
      break;

    case EMG_STREAM_ID:
      *len = MIN(*len, emg_StreamValLen);
      memcpy(value, emg_StreamVal, *len);
      Log_info2("GetParameter : %s returning %d bytes", (IArg)"Stream", (IArg)*len);
      break;

    default:
      Log_error1("GetParameter: Parameter #%d not valid.", (IArg)param);
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}

/*********************************************************************
 * @internal
 * @fn          EMG_Service_findCharParamId
 *
 * @brief       Find the logical param id of an attribute in the service's attr table.
 *
 *              Works only for Characteristic Value attributes and
 *              Client Characteristic Configuration Descriptor attributes.
 *
 * @param       pAttr - pointer to attribute
 *
 * @return      uint8_t paramID (ref Data_Service.h) or 0xFF if not found.
 */
static uint8_t EMG_Service_findCharParamId(gattAttribute_t *pAttr)
{
  // Is this a Client Characteristic Configuration Descriptor?
  if (ATT_BT_UUID_SIZE == pAttr->type.len && GATT_CLIENT_CHAR_CFG_UUID == *(uint16_t *)pAttr->type.uuid)
    return EMG_Service_findCharParamId(pAttr - 1); // Assume the value attribute precedes CCCD and recurse

  // Is this attribute in "String"?
  else if ( ATT_UUID_SIZE == pAttr->type.len && !memcmp(pAttr->type.uuid, emg_configUUID, pAttr->type.len))
    return EMG_CONFIG_ID;

  // Is this attribute in "Stream"?
  else if ( ATT_UUID_SIZE == pAttr->type.len && !memcmp(pAttr->type.uuid, emg_StreamUUID, pAttr->type.len))
    return EMG_STREAM_ID;

  else
    return 0xFF; // Not found. Return invalid.
}

/*********************************************************************
 * @fn          EMG_Service_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t EMG_Service_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                       uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                       uint16_t maxLen, uint8_t method )
{
  bStatus_t status = SUCCESS;
  uint16_t valueLen;
  uint8_t paramID = 0xFF;

  // Find settings for the characteristic to be read.
  paramID = EMG_Service_findCharParamId( pAttr );
  switch ( paramID )
  {
    case EMG_CONFIG_ID:
      valueLen = emg_ConfigValLen;

      Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                 (IArg)"String",
                 (IArg)connHandle,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for String can be inserted here */
      break;

    case EMG_STREAM_ID:
      valueLen = emg_StreamValLen;

      Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                 (IArg)"Stream",
                 (IArg)connHandle,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for Stream can be inserted here */
      break;

    default:
      Log_error0("Attribute was not found.");
      return ATT_ERR_ATTR_NOT_FOUND;
  }
  // Check bounds and return the value
  if ( offset > valueLen )  // Prevent malicious ATT ReadBlob offsets.
  {
    Log_error0("An invalid offset was requested.");
    status = ATT_ERR_INVALID_OFFSET;
  }
  else
  {
    *pLen = MIN(maxLen, valueLen - offset);  // Transmit as much as possible
    memcpy(pValue, pAttr->pValue + offset, *pLen);
  }

  return status;
}

/*********************************************************************
 * @fn      EMG_Service_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t EMG_Service_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                        uint8_t *pValue, uint16_t len, uint16_t offset,
                                        uint8_t method )
{
  bStatus_t status  = SUCCESS;
  uint8_t   paramID = 0xFF;
  uint8_t   changeParamID = 0xFF;
  uint16_t writeLenMin;
  uint16_t writeLenMax;
  uint16_t *pValueLenVar;

  // See if request is regarding a Client Characterisic Configuration
  if (ATT_BT_UUID_SIZE == pAttr->type.len && GATT_CLIENT_CHAR_CFG_UUID == *(uint16_t *)pAttr->type.uuid)
  {
    Log_info3("WriteAttrCB (CCCD): param: %d connHandle: %d %s",
              (IArg)EMG_Service_findCharParamId(pAttr),
              (IArg)connHandle,
              (IArg)(method == GATT_LOCAL_WRITE?"- restoring bonded state":"- OTA write"));

    // Allow notification and indication, but do not check if really allowed per CCCD.
    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                             offset, GATT_CLIENT_CFG_NOTIFY |
                                                     GATT_CLIENT_CFG_INDICATE );
    if (SUCCESS == status && pAppCBs && pAppCBs->pfnCfgChangeCb)
       pAppCBs->pfnCfgChangeCb( connHandle, EMG_SERVICE_SERV_UUID,
                                EMG_Service_findCharParamId(pAttr), pValue, len );

     return status;
  }

  // Find settings for the characteristic to be written.
  paramID = EMG_Service_findCharParamId( pAttr );
  switch ( paramID )
  {
    case EMG_CONFIG_ID:
      writeLenMin  = EMG_CONFIG_LEN_MIN;
      writeLenMax  = EMG_CONFIG_LEN;
      pValueLenVar = &emg_ConfigValLen;

      Log_info5("WriteAttrCB : %s connHandle(%d) len(%d) offset(%d) method(0x%02x)",
                 (IArg)"String",
                 (IArg)connHandle,
                 (IArg)len,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for String can be inserted here */
      break;

    case EMG_STREAM_ID:
      writeLenMin  = EMG_STREAM_LEN_MIN;
      writeLenMax  = EMG_STREAM_LEN;
      pValueLenVar = &emg_StreamValLen;

      Log_info5("WriteAttrCB : %s connHandle(%d) len(%d) offset(%d) method(0x%02x)",
                 (IArg)"Stream",
                 (IArg)connHandle,
                 (IArg)len,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for Stream can be inserted here */
      break;

    default:
      Log_error0("Attribute was not found.");
      return ATT_ERR_ATTR_NOT_FOUND;
  }
  // Check whether the length is within bounds.
  if ( offset >= writeLenMax )
  {
    Log_error0("An invalid offset was requested.");
    status = ATT_ERR_INVALID_OFFSET;
  }
  else if ( offset + len > writeLenMax )
  {
    Log_error0("Invalid value length was received.");
    status = ATT_ERR_INVALID_VALUE_SIZE;
  }
  else if ( offset + len < writeLenMin && ( method == ATT_EXECUTE_WRITE_REQ || method == ATT_WRITE_REQ ) )
  {
    // Refuse writes that are lower than minimum.
    // Note: Cannot determine if a Reliable Write (to several chars) is finished, so those will
    //       only be refused if this attribute is the last in the queue (method is execute).
    //       Otherwise, reliable writes are accepted and parsed piecemeal.
    Log_error0("Invalid value length was received.");
    status = ATT_ERR_INVALID_VALUE_SIZE;
  }
  else
  {
    // Copy pValue into the variable we point to from the attribute table.
    memcpy(pAttr->pValue + offset, pValue, len);

    // Only notify application and update length if enough data is written.
    //
    // Note: If reliable writes are used (meaning several attributes are written to using ATT PrepareWrite),
    //       the application will get a callback for every write with an offset + len larger than _LEN_MIN.
    // Note: For Long Writes (ATT Prepare + Execute towards only one attribute) only one callback will be issued,
    //       because the write fragments are concatenated before being sent here.
    if ( offset + len >= writeLenMin )
    {
      changeParamID = paramID;
      *pValueLenVar = offset + len; // Update data length.
    }
  }

  // Let the application know something changed (if it did) by using the
  // callback it registered earlier (if it did).
  if (changeParamID != 0xFF)
    if ( pAppCBs && pAppCBs->pfnChangeCb )
      pAppCBs->pfnChangeCb( connHandle, EMG_SERVICE_SERV_UUID, paramID, pValue, len+offset ); // Call app function from stack task context.

  return status;
}
