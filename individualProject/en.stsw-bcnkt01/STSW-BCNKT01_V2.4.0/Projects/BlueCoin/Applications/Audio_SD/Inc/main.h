/**
******************************************************************************
* @file    main.h
* @author  SRA
* @version v2.4.0
* @date    23-Apr-2021
* @brief   Header for main.c module
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stm32F4xx_hal.h"

#include "BlueCoin.h"

#include "BlueCoin_audio.h"

#include "BlueCoin_sd.h"

#include "usbd_desc.h"
#include "usbd_audio_cdc.h"
#include "usbd_audio_cdc_interface.h"

#define PCM_TEMP_BUFF_SIZE  AUDIO_SAMPLING_FREQUENCY/1000*1

/* Exported Defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Error_Handler( void );
void RTC_TimeRegulate( uint8_t hh, uint8_t mm, uint8_t ss );
void test_passed(void);
void test_failed(void);
void usbPrintMsg(uint8_t* Buf);
void SW_Task1_Callback(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
