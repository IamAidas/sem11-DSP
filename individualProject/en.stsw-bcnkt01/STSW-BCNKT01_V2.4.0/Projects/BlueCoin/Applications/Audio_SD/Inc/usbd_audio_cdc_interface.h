/**
******************************************************************************
* @file    usbd_cdc_interface.h
* @author  SRA
* @version v2.4.0
* @date    23-Apr-2021
* @brief   Header for usbd_cdc_interface.c file.
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
*
* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
*
*        http://www.st.com/software_license_agreement_liberty_v2
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CDC_IF_H
#define __USBD_CDC_IF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_audio_cdc.h"
#include "BlueCoin_audio.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor USARTx/UARTx instance used and associated
   resources */
/* Definition for USARTx clock resources */

#define USB_RxBufferDim                         512

/* Definition for TIMx clock resources */
#define TIMx                             TIM4
#define TIMx_CLK_ENABLE                  __TIM4_CLK_ENABLE
#define TIMx_FORCE_RESET()               __USART3_FORCE_RESET()
#define TIMx_RELEASE_RESET()             __USART3_RELEASE_RESET()

/* Definition for TIMx's NVIC */
#define TIMx_IRQn                        TIM4_IRQn
#define TIM_USB_IRQHandler               TIM4_IRQHandler

/* Periodically, the state of the buffer "UserTxBuffer" is checked.
   The period depends on CDC_POLLING_INTERVAL */
#define CDC_POLLING_INTERVAL             15 /* in ms. The max is 65ms and the min is 1ms */

extern USBD_AUDIO_CDC_ItfTypeDef  USBD_AUDIO_CDC_fops;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint8_t CDC_Fill_Buffer(uint8_t* Buf, uint32_t TotalLen);
uint16_t USB_CheckForNewData(void);
uint8_t* USB_GetRxBuffer(void);
uint8_t* USB_GetTxBuffer(void);

#define AUDIO_REQ_GET_CUR                           0x81
#define AUDIO_REQ_GET_MIN                           0x82
#define AUDIO_REQ_GET_MAX                           0x83
#define AUDIO_REQ_GET_RES                           0x84

int8_t Audio_VolumeCtl(int16_t Volume);
void Send_Audio_to_USB(int16_t * audioData, uint16_t PCMSamples);


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* __USBD_CDC_IF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
