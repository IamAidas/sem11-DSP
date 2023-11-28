/**
******************************************************************************
* @file    main.c
* @author  SRA
* @version v2.4.0
* @date    23-Apr-2021
* @brief   Audio playback and record main file.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* FatFs includes component */
#include "ff_gen_drv.h"
#include "sd_diskio.h"


FRESULT res;                                          /* FatFs function common result code */
FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
char newLine[] = "\r\n";
volatile uint8_t SD_Log_Enabled = 0;
char SDPath[4]; /* SD card logical drive path */
static volatile uint8_t BUTTONInterrupt = 0;
int index_n=0;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/




/* Private function prototypes -----------------------------------------------*/



/* Private functions ---------------------------------------------------------*/
static void SystemClock_Config(void);
/* Private function prototypes -----------------------------------------------*/
extern uint16_t PDM_Buffer[];
USBD_HandleTypeDef hUSBDDevice;
volatile int16_t temp_buffer[48 * 2];

volatile uint8_t cont=0;
volatile int  index_buff=0;

BSP_AUDIO_Init_t MicParams;
uint16_t PCM_Buffer[AUDIO_CHANNELS * AUDIO_SAMPLING_FREQUENCY / 1000];
uint16_t PDM_Buffer[AUDIO_CHANNELS * AUDIO_SAMPLING_FREQUENCY / 1000 * 64 / 8];

#define WRITE_EACH 16//ms
#define SIZE_BUFF (AUDIO_SAMPLING_FREQUENCY / 1000 * AUDIO_CHANNELS * (WRITE_EACH*2))

int16_t Audio_OUT_Buff[SIZE_BUFF];
uint8_t pHeader[44];


/**
  * @brief  Initialize the wave header file
  * @param  pHeader: Header Buffer to be filled
  * @param  pWaveFormatStruct: Pointer to the wave structure to be filled.
  * @retval 0 if passed, !0 if failed.
  */
static uint32_t WavProcess_HeaderInit()
{
  uint16_t   BitPerSample=16;
  uint16_t   NbrChannels=AUDIO_CHANNELS;
  uint32_t   ByteRate=AUDIO_SAMPLING_FREQUENCY*(BitPerSample/8);
   
  uint32_t   SampleRate=AUDIO_SAMPLING_FREQUENCY;  
  uint16_t   BlockAlign= NbrChannels * (BitPerSample/8);
  
  /* Write chunkID, must be 'RIFF'  ------------------------------------------*/
  pHeader[0] = 'R';
  pHeader[1] = 'I';
  pHeader[2] = 'F';
  pHeader[3] = 'F';
  
  /* Write the file length ----------------------------------------------------*/
  /* The sampling time: this value will be be written back at the end of the 
     recording opearation.  Example: 661500 Btyes = 0x000A17FC, byte[7]=0x00, byte[4]=0xFC */
  pHeader[4] = 0x00;
  pHeader[5] = 0x4C;
  pHeader[6] = 0x1D;
  pHeader[7] = 0x00;
  
  /* Write the file format, must be 'WAVE' -----------------------------------*/
  pHeader[8]  = 'W';
  pHeader[9]  = 'A';
  pHeader[10] = 'V';
  pHeader[11] = 'E';
  
  /* Write the format chunk, must be'fmt ' -----------------------------------*/
  pHeader[12]  = 'f';
  pHeader[13]  = 'm';
  pHeader[14]  = 't';
  pHeader[15]  = ' ';
  
  /* Write the length of the 'fmt' data, must be 0x10 ------------------------*/
  pHeader[16]  = 0x10;
  pHeader[17]  = 0x00;
  pHeader[18]  = 0x00;
  pHeader[19]  = 0x00;
  
  /* Write the audio format, must be 0x01 (PCM) ------------------------------*/
  pHeader[20]  = 0x01;
  pHeader[21]  = 0x00;
  
  /* Write the number of channels, ie. 0x01 (Mono) ---------------------------*/
  pHeader[22]  = NbrChannels;
  pHeader[23]  = 0x00;
  
  /* Write the Sample Rate in Hz ---------------------------------------------*/
  /* Write Little Endian ie. 8000 = 0x00001F40 => byte[24]=0x40, byte[27]=0x00*/
  pHeader[24]  = (uint8_t)((SampleRate & 0xFF));
  pHeader[25]  = (uint8_t)((SampleRate >> 8) & 0xFF);
  pHeader[26]  = (uint8_t)((SampleRate >> 16) & 0xFF);
  pHeader[27]  = (uint8_t)((SampleRate >> 24) & 0xFF);
  
  /* Write the Byte Rate -----------------------------------------------------*/
  pHeader[28]  = (uint8_t)(( ByteRate & 0xFF));                   
  pHeader[29]  = (uint8_t)(( ByteRate >> 8) & 0xFF);
  pHeader[30]  = (uint8_t)(( ByteRate >> 16) & 0xFF);
  pHeader[31]  = (uint8_t)(( ByteRate >> 24) & 0xFF);
  
  /* Write the block alignment -----------------------------------------------*/
  pHeader[32]  = BlockAlign;
  pHeader[33]  = 0x00;
  
  /* Write the number of bits per sample -------------------------------------*/
  pHeader[34]  = BitPerSample;
  pHeader[35]  = 0x00;
  
  /* Write the Data chunk, must be 'data' ------------------------------------*/
  pHeader[36]  = 'd';
  pHeader[37]  = 'a';
  pHeader[38]  = 't';
  pHeader[39]  = 'a';
  
  /* Write the number of sample data -----------------------------------------*/
  /* This variable will be written back at the end of the recording operation */
  pHeader[40]  = 0x00;
  pHeader[41]  = 0x4C;
  pHeader[42]  = 0x1D;
  pHeader[43]  = 0x00;
  
  /* Return 0 if all operations are OK */
  return 0;
}


/**
  * @brief  Initialize the wave header file
  * @param  pHeader: Header Buffer to be filled
  * @param  pWaveFormatStruct: Pointer to the wave structure to be filled.
  * @retval 0 if passed, !0 if failed.
  */
static uint32_t WavProcess_HeaderUpdate(uint32_t len)
{
  /* Write the file length ----------------------------------------------------*/
  /* The sampling time: this value will be be written back at the end of the 
     recording opearation.  Example: 661500 Btyes = 0x000A17FC, byte[7]=0x00, byte[4]=0xFC */
  pHeader[4] = (uint8_t)(len);
  pHeader[5] = (uint8_t)(len >> 8);
  pHeader[6] = (uint8_t)(len >> 16);
  pHeader[7] = (uint8_t)(len >> 24);
  /* Write the number of sample data -----------------------------------------*/
  /* This variable will be written back at the end of the recording operation */
  len -=44;
  pHeader[40] = (uint8_t)(len); 
  pHeader[41] = (uint8_t)(len >> 8);
  pHeader[42] = (uint8_t)(len >> 16);
  pHeader[43] = (uint8_t)(len >> 24); 
  /* Return 0 if all operations are OK */
  return 0;
}

/**
  * @brief  Start SD-Card demo
  * @param  None
  * @retval None
  */
void DATALOG_SD_Init(void)
{
  char SDPath[4];
  
  if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
  {
    /* Register the file system object to the FatFs module */
    if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) != FR_OK)
    {
      /* FatFs Initialization Error */
      while(1)
      {
        BSP_LED_On(LED1);
        HAL_Delay(500);
        BSP_LED_Off(LED1);
        HAL_Delay(100);
      }
    }
  }
}

/**
  * @brief  Start SD-Card demo
  * @param  None
  * @retval None
  */
uint8_t DATALOG_SD_Log_Enable(void)
{
  static uint16_t sdcard_file_counter = 0;
 
  uint32_t byteswritten; /* written byte count */
  char file_name[30] = {0};
  
  WavProcess_HeaderInit();
  
  sprintf(file_name, "%s%.3d%s", "BlueCoin_Log_N", sdcard_file_counter, ".wav");
  sdcard_file_counter++;

  if(f_open(&MyFile, (char const*)file_name, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
  {
    return 0;
  }
  
  if(f_write(&MyFile, pHeader, sizeof(pHeader), (void *)&byteswritten) != FR_OK)
  {
    return 0;
  }
  return 1;
}

/**
  * @brief  Disable SDCard Log
  * @param  None
  * @retval None
  */
void DATALOG_SD_Log_Disable(void)
{
  uint32_t len;
  uint32_t byteswritten;                     /* File write/read counts */

  
  len = f_size(&MyFile);
  WavProcess_HeaderUpdate(len);

    /* Update the data length in the header of the recorded Wave */    
  f_lseek(&MyFile, 0);
  
  /* Parse the wav file header and extract required information */
  
  f_write(&MyFile, pHeader, 44, (void*)&byteswritten);
  
  /* Close file and unmount MyFilesystem */
    
  f_close(&MyFile);
}

/**
  * @brief  Write New Line to file
  * @param  None
  * @retval None
  */
void DATALOG_SD_NewLine(void)
{
  uint32_t byteswritten; /* written byte count */
  f_write(&MyFile, (const void*)&newLine, 2, (void *)&byteswritten);
}


int main(void)
{

  /* STM32F4xx HAL library initialization:
  - Configure the Flash prefetch, instruction and Data caches
  - Configure the Systick to generate an interrupt each 1 msec
  - Set NVIC Group Priority to 4
  - Global MSP (MCU Support Package) initialization
  */
  HAL_Init();
  
  /* Configure the system clock to 180 MHz */
  SystemClock_Config();
  
  /* Initialize LED */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);
  BSP_LED_Init(LED5);
  BSP_LED_Init(LED6);
  BSP_LED_Init(LED7);
  BSP_LED_Init(LED8);
  
  BSP_PB_Init(BUTTON_2, BUTTON_MODE_EXTI);

  HAL_NVIC_SetPriority((IRQn_Type)EXTI1_IRQn, 0xFF, 0); 
  HAL_NVIC_EnableIRQ((IRQn_Type)EXTI1_IRQn); 

  HAL_Delay(100);
  BSP_SD_Detect_Init();

  while(!BSP_SD_IsDetected())
  {
    /* Go to Sleep */
    __WFI();
  }

  HAL_Delay(200);
  DATALOG_SD_Init();
    HAL_Delay(200);

  /* Configure Audio Input peripheral - I2S*/  
  MicParams.BitsPerSample = 16;
  MicParams.ChannelsNbr = AUDIO_CHANNELS;
  MicParams.Device = AUDIO_IN_DIGITAL_MIC;
  MicParams.SampleRate = AUDIO_SAMPLING_FREQUENCY;
  MicParams.Volume = AUDIO_VOLUME_INPUT;
  
  BSP_AUDIO_IN_Init(BSP_AUDIO_IN_INSTANCE, &MicParams);   

  /* Start Microphone acquisition */
  BSP_AUDIO_IN_Record(BSP_AUDIO_IN_INSTANCE, (uint8_t *) PDM_Buffer, 0);

  BSP_LED_On(LED2);
  
  while (1)
  {
    if(BUTTONInterrupt)
    {
      BUTTONInterrupt = 0;
      if (SD_Log_Enabled)
      {
        SD_Log_Enabled = 0;
        DATALOG_SD_Log_Disable();
        BSP_LED_Off(LED4);
      }
      else
      {
        while(SD_Log_Enabled != 1)
        {
          if(DATALOG_SD_Log_Enable())
          {
            SD_Log_Enabled = 1;
          }
          else
          {
            DATALOG_SD_Log_Disable();
          }
        }
      }
    }
  }
  
}

/**
* @brief Throws Highest priority interrupt
* @param None
* @retval None
*/
void SW_Task1_Start(void)
{ 
  HAL_NVIC_SetPendingIRQ(EXTI1_IRQn); 
}


/**
* @brief Highest priority interrupt handler routine
* @param None
* @retval None
*/
void SW_Task1_Callback(void)
{ 
  FRESULT s;
  uint32_t byteswritten;                     /* File write/read counts */
  
  
  /* Check Push Button Event  */
  if (SD_Log_Enabled)
  { 
    s=f_write(&MyFile, &(((uint8_t *)Audio_OUT_Buff)[index_buff]), SIZE_BUFF, (void *)&byteswritten);
    if(s != FR_OK)
      BSP_LED_On(LED4);
    
  }
}


void AudioProcess(void)
{
  /*for L4 PDM to PCM conversion is performed in hardware by DFSDM peripheral*/
  uint16_t index = 0;
  static uint16_t OUT_Buff_lvl = 0;
  uint32_t msTick, msTickPrev = 0;
  
  BSP_AUDIO_IN_PDMToPCM(BSP_AUDIO_IN_INSTANCE,(uint16_t * )PDM_Buffer, PCM_Buffer);
  
  for (index = 0; index < AUDIO_SAMPLING_FREQUENCY/1000 ; index++)
  {

    Audio_OUT_Buff[OUT_Buff_lvl + 0] = PCM_Buffer[index * AUDIO_CHANNELS + 0];// + PCM_Buffer[index * AUDIO_CHANNELS + 3];
    Audio_OUT_Buff[OUT_Buff_lvl + 1] = PCM_Buffer[index * AUDIO_CHANNELS + 1];//+ PCM_Buffer[index * AUDIO_CHANNELS + 2];

    Audio_OUT_Buff[OUT_Buff_lvl + 2] = PCM_Buffer[index * AUDIO_CHANNELS + 2];// + PCM_Buffer[index * AUDIO_CHANNELS + 3];
    Audio_OUT_Buff[OUT_Buff_lvl + 3] = PCM_Buffer[index * AUDIO_CHANNELS + 3];//+ PCM_Buffer[index * AUDIO_CHANNELS + 2];
    
    OUT_Buff_lvl = (OUT_Buff_lvl + 4)%SIZE_BUFF;
  }
  
  if (SD_Log_Enabled)
  {
    //led toggle
    msTick = HAL_GetTick();
    if(msTick % 20 == 0 && msTickPrev != msTick)
    {
      msTickPrev = msTick;
      
      BSP_LED_Toggle(LED4);
    }
    
    //first half
    if(OUT_Buff_lvl == SIZE_BUFF/2)
    {
      index_buff=0;
      SW_Task1_Start(); 
      
    }    
    //second half
    else if (OUT_Buff_lvl == 0)
    {
      index_buff= SIZE_BUFF;
      SW_Task1_Start(); 
    }    
    
    
  }
  
}


/**
* @brief  Half Transfer user callback, called by BSP functions.
* @param  None
* @retval None
*/
void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance)
{
  AudioProcess();

}

/**
* @brief  Transfer Complete user callback, called by BSP functions.
* @param  None
* @retval None
*/
void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance)
{
  AudioProcess();

}


/**
* @brief  EXTI line detection callbacks
* @param  GPIO_Pin: Specifies the pins connected EXTI line
* @retval None
*/
void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin )
{
  if(GPIO_Pin == BUTTON_2_PIN)
  {
    BUTTONInterrupt = 1;
  }
  else if(GPIO_Pin == SD_DETECT_GPIO_PIN)
  {
    BSP_SD_IsDetected();
  }
}

/**
* @brief  System Clock Configuration
* @param  None
* @retval None
*/
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is
  clocked below the maximum system frequency, to update the voltage scaling value
  regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  /* generate 48Mhz for SD card clock */
  RCC_PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SDIO | RCC_PERIPHCLK_CK48;
  RCC_PeriphClkInitStruct.SdioClockSelection = RCC_SDIOCLKSOURCE_CK48;
  RCC_PeriphClkInitStruct.Clk48ClockSelection = RCC_CK48CLKSOURCE_PLLSAIP;
  RCC_PeriphClkInitStruct.PLLSAI.PLLSAIM = 16;
  RCC_PeriphClkInitStruct.PLLSAI.PLLSAIN = 384;
  RCC_PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV8;
  HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInitStruct);
  
  
  /*Select Main PLL output as USB clock source */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CK48;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CK48CLKSOURCE_PLLQ;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
  clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif
void Error_Handler(void)
{
//  PRINTF("Error_Handler\n");
  /* Blink LED2 */
  while(1)
  {
    BSP_LED_Toggle(LED2);
    HAL_Delay(100);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
