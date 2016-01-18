#include <stm32f0xx.h>
#include <stm32f0xx_rcc.h>
#include <stm32f0xx_adc.h>
#include <stm32f0xx_dma.h>
#include <stm32f0xx_gpio.h>
#include <stm32f0xx_usart.h>
#define CUSTOM_FILE_IO 1
#include <stdio.h>
#include <math.h>
#include <string.h>

#define sqr(x) (x*x)

//how many averaging cycles
#define AVG_PER 100
//100 = every 5 seconds

//measurements
//1000= 45.60 sec  =  0.0456
//10= 0.7789
// time=AVG_PER*(measurement+calculation) + printout
//10 (0.04s +calculation)+printout=0.7789
//1000 (0.04s +calculation)+printout=45.60
//
//10*calc=0.3789 - printout
//1000*calc=5.60 - printout
//
//calc=0.005919
//printout=0.3197

//power to energy conversion
#define ENE_FACTOR ((0.32+AVG_PER*(0.04+0.0059)) / 3600000)

//samples per second on all channels ~ 166ksmps - all those measured
#define SAMPLES_PER_SECOND 27785
#define DEFAULT_SAMPLES_50HZ 555
#define DEFAULT_SAMPLES_50HZ_FLOAT 555.54

//how many samples to collect roughly 2xDEFAULT_SAMPLES_50HZ
#define SAMPLES 1120

//samples for one period - 20ms
unsigned int SAM_PER=DEFAULT_SAMPLES_50HZ;
unsigned int Q_SAM;

//transformers conversion factors 0,2,4 - current 1,3,5 - voltage) - measured
float FACTORS[6]={0.020, 0.211, 0.020, 0.211, 0.020, 0.211};
//float FACTORS[6]={1, 1, 1, 1, 1, 1}; //for testing

//voltage to current phase offsets (in samples count between) - measured
//int CT_PH[6]={0,0,0,0,0,0}; //for testing
int CT_PH[6]={36,0,36,0,42,0};

//equals minimum of CT_PH if there is one below 0
#define PH_OFS 0




//sequence of DMA channels for A0...A5 inputs
unsigned char DMA_SEQ[6]={1,2,3,4,0,5};

#define ADC1_DR_Address               0x40012440
__IO uint16_t DMA_BUF[6*SAMPLES];

void setup_ADC(void)
{
  ADC_InitTypeDef     ADC_InitStructure;
  DMA_InitTypeDef     DMA_InitStructure;

  ADC_DeInit(ADC1);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

  /* DMA1 Channel1 Config */
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DMA_BUF;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 6*SAMPLES;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  DMA_Cmd(DMA1_Channel1, ENABLE);
  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_OneShot );
  ADC_DMACmd(ADC1, ENABLE);

  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_ChannelConfig(ADC1, ADC_Channel_0 ,  ADC_SampleTime_71_5Cycles);
  ADC_ChannelConfig(ADC1, ADC_Channel_1 ,  ADC_SampleTime_71_5Cycles);
  ADC_ChannelConfig(ADC1, ADC_Channel_4 ,  ADC_SampleTime_71_5Cycles);
  ADC_ChannelConfig(ADC1, ADC_Channel_8 ,  ADC_SampleTime_71_5Cycles);
  ADC_ChannelConfig(ADC1, ADC_Channel_10 , ADC_SampleTime_71_5Cycles);
  ADC_ChannelConfig(ADC1, ADC_Channel_11 , ADC_SampleTime_71_5Cycles);

  ADC_GetCalibrationFactor(ADC1);
  ADC_Cmd(ADC1, ENABLE);
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN));
  ADC_StartOfConversion(ADC1);
}


void USARTInit(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2,  GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3,  GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 1000000;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART2, ENABLE);
    USART_Cmd(USART1, ENABLE);

}

void GPIOinit(){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

int main(void){
	int i,j,len,z=0;
	int xx=1;
	unsigned int chk=0;

	uint16_t dc_offset;

	unsigned long sum;

	long l;
	float x;
	float freq,last_freq=DEFAULT_SAMPLES_50HZ_FLOAT;
	float freq_sum;
	float v1,v2;
	float kwh=0;

	float rms_cur[3];
	float rms_vol[3];
	float real_pwr[3];
	float react_pwr[3];

	GPIOinit();
	USARTInit();


	for (z=0;z<3;z++){
			rms_cur[z]=0;
			rms_vol[z]=0;
			real_pwr[z]=0;
			react_pwr[z]=0;
	}
	freq_sum=0;

	while(1){
		setup_ADC();
		while((DMA_GetFlagStatus(DMA1_FLAG_TC1)) == RESET );
		DMA_ClearFlag(DMA1_FLAG_TC1);


		//look at 4.2.1
		//SLAA577G
		//http://www.ti.com/lit/an/slaa577g/slaa577g.pdf

		//first sample sometimes is corrupted so i=1
		//calculate DC offset as average of voltage channels
		sum=0;
		for (z=1;z<6;z+=2) for(i=1;i<SAM_PER;i++) sum+=DMA_BUF[(i*6)+DMA_SEQ[z]];
		sum/=3*SAM_PER;
		dc_offset=sum;

		freq=0;
		for (z=1;z<6;z+=2){
			l=0;
			for(i=2;i<SAMPLES;i++) if ((DMA_BUF[((i-1)*6)+DMA_SEQ[z]]<=dc_offset)&&(DMA_BUF[(i*6)+DMA_SEQ[z]]>=dc_offset)){
				if (l) {
					SAM_PER=i-l+1;
					v1=(DMA_BUF[((i-1)*6)+DMA_SEQ[z]]-dc_offset);
					v2=(DMA_BUF[(i*6)+DMA_SEQ[z]]-dc_offset);
					if (freq==0) {
							if ((v2-v1)>0) freq=(float)i+(float) (-v1 / (v2-v1)) - x;
							else freq=(float)i -x;
					}
					z=6;
					break;//this will make execution time variable a little
				}
				else {
					l=i;
					v1=(DMA_BUF[((i-1)*6)+DMA_SEQ[z]]-dc_offset);
					v2=(DMA_BUF[(i*6)+DMA_SEQ[z]]-dc_offset);
					if ((v2-v1)>0) x=(float)i+(float) (-v1 / (v2-v1));
					else x=(float)i;
				}
			}
		}
		if ((SAM_PER<500)||(SAM_PER>600)||(freq==0)) { //it CAN happen occasionally
				SAM_PER=DEFAULT_SAMPLES_50HZ;
				freq=last_freq;
				//continue; // continue or not
				//"continue" means cycle time would be different, and a possible problem when no power supply
				//not to "continue" corrupts frequency measurement but every cycle has same execution time;
		}
		last_freq=freq;

		freq=SAMPLES_PER_SECOND/freq;
		freq_sum+=freq;

		Q_SAM=SAM_PER/4;

		//CORRECT CT_PHASE
		//offset currents
		for (z=0;z<6;z+=2)	for(i=0;i<SAM_PER+Q_SAM;i++) DMA_BUF[(i*6)+DMA_SEQ[z]]=DMA_BUF[((i+CT_PH[z]+PH_OFS)*6)+DMA_SEQ[z]];
		//offset voltages
		for (z=1;z<6;z+=2)	for(i=0;i<SAM_PER+Q_SAM;i++) DMA_BUF[(i*6)+DMA_SEQ[z]]=DMA_BUF[((i+PH_OFS)*6)+DMA_SEQ[z]];

		//calculate RMS current
		for (z=0;z<3;z++){
					l=0;
					for(i=1;i<SAM_PER;i++) l+=sqr(((long)DMA_BUF[(i*6)+DMA_SEQ[z*2]]-dc_offset));
					x=(float)FACTORS[z*2]*sqrt(l / SAM_PER);
					rms_cur[z]+=x;
		}
		//calculate RMS voltage
		for (z=0;z<3;z++){
					l=0;
					for(i=1;i<SAM_PER;i++) l+=sqr((DMA_BUF[(i*6)+DMA_SEQ[(z*2)+1]]-dc_offset));
					l/=SAM_PER;
					x=(float)FACTORS[(z*2)+1] * sqrt(l);
					rms_vol[z]+=x;
		}
		//calculate real power
		for (z=0;z<3;z++){
					l=0;
					for(i=1;i<SAM_PER;i++) l+=(DMA_BUF[(i*6)+DMA_SEQ[(z*2)+1]]-dc_offset)*(DMA_BUF[(i*6)+DMA_SEQ[z*2]]-dc_offset);
					x=(float)l * (float)FACTORS[(z*2)+1] * (float)FACTORS[z*2] / SAM_PER;
					real_pwr[z]+=x;
		}
		//calculate reactive power - 90 degree voltage phase shift
		for (z=0;z<3;z++){
					l=0;
					for(i=1;i<SAM_PER;i++) l+=(DMA_BUF[((i+Q_SAM)*6)+DMA_SEQ[(z*2)+1]]-dc_offset)*(DMA_BUF[(i*6)+DMA_SEQ[z*2]]-dc_offset);
					x=(float)l * (float)FACTORS[(z*2)+1] * (float)FACTORS[z*2] / SAM_PER;
					react_pwr[z]+=x;
		}

		//output
		if (xx%AVG_PER==0){
			printf("\n\nDATA_START\n");
			for (z=1;z<6;z+=2){
					  printf("\"volt%d\": [",z>>1);
					  for (i=1;i<SAM_PER-1;i+=4) printf("[%d,%.1f],",i,(float)FACTORS[z]*(DMA_BUF[(i*6)+DMA_SEQ[z]]-dc_offset));
					  i=SAM_PER-1; printf("[%d,%.1f]",i,(float)FACTORS[z]*(DMA_BUF[(i*6)+DMA_SEQ[z]]-dc_offset));
					  printf("],\n");
			}
			for (z=0;z<6;z+=2){
					  printf("\"cur%d\": [",z>>1);
					  for (i=1;i<SAM_PER-1;i+=4) printf("[%d,%.2f],",i,(float)FACTORS[z]*(DMA_BUF[(i*6)+DMA_SEQ[z]]-dc_offset));
					  i=SAM_PER-1; printf("[%d,%.2f]",i,(float)FACTORS[z]*(DMA_BUF[(i*6)+DMA_SEQ[z]]-dc_offset));
					  printf("],\n");
			}
			for (z=0;z<3;z++){
				rms_cur[z]/=AVG_PER;
				rms_vol[z]/=AVG_PER;
				real_pwr[z]/=AVG_PER;
				react_pwr[z]/=AVG_PER;
				kwh+=real_pwr[z]*ENE_FACTOR;
			}


			for (z=0;z<3;z++){
				 printf("\"rms_cur%d\": %.2f,\n",z,rms_cur[z]);
				 printf("\"rms_vol%d\": %.1f,\n",z,rms_vol[z]);
				 printf("\"real_pwr%d\": %.1f,\n",z,real_pwr[z]);
				 printf("\"react_pwr%d\": %.1f,\n",z,react_pwr[z]);
				 printf("\"apa_pwr%d\": %.1f,\n",z,rms_cur[z]*rms_vol[z]);
			}
			printf("\"kwh\": %.5f,\n",kwh);
			printf("\"freq\": %.3f,\n",freq_sum/AVG_PER);
			printf("\"dc_offset\": %d\n",dc_offset);
			printf("DATA_END\n\n");
			for (z=0;z<3;z++){
						rms_cur[z]=0;
						rms_vol[z]=0;
						real_pwr[z]=0;
						react_pwr[z]=0;
			}
			freq_sum=0;
		}
		xx++;


	}


}
