///******************************************************************************
//* Author: Huynh Le Nhat Tam
//* Date: 25th August 2022
//*******************************************************************************/
// See https://www.keil.com/pack/doc/CMSIS/DSP/html/index.html for explanation of RFFT 
#include <project.h>
#include "stdio.h"
#include "arm_math.h"
#include "arm_const_structs.h"

#define TEST_LENGTH_SAMPLES 2048

char uartout[20];
char uart_array[20];

uint32_t fftSize = TEST_LENGTH_SAMPLES;
uint32_t ifftFlag = 0; //flag that selects forward (ifftFlag=0) or inverse (ifftFlag=1) transform
uint32_t doBitReverse = 1; // flag that enables (doBitReverse=1) or disables (doBitReverse=0) bit reversal of output.
// Set doBitReverse ( = 1) for output to be in normal order otherwise output is in bit reversed order
arm_cfft_radix4_instance_f32 S_CFFT; // Instance structure for the floating-point CFFT/CIFFT function
arm_rfft_instance_f32 S; // Instance structure for the floating-point RFFT/RIFFT function.
uint32_t cutoff = 10; // Cutoff frequency: 10 Hz

CY_ISR_PROTO(Buttonisr);





int32_t main(void)
{
  /* Start the components*/
  Opamp_1_Start();
  Opamp_2_Start();
  ADC_DelSig_1_Start();
  UART_1_Start();
  LCD_1_Start();
  
 /* Start the ADC conversion*/
  ADC_DelSig_1_StartConvert();

 /* Initialize the necessary variables for FFT operation (these variables have to be inside main() so that it can be modified by the loop) */
  float32_t testInput[TEST_LENGTH_SAMPLES]; // input data in time domain
// The sample frequency (Fs) of this FFT from ARM is equal to the sample length (L) because the frequency resolution of the bins
// is 1 (frequency resolution = Fs/L)
// If the sample frequency of the input signal (Fs_signal) is different than Fs => f_signal/f_bin = Fs_signal/Fs
// (Ex: The frequency of the signal (f_signal) is 120 Hz and its Fs_signal is 1000 Hz => f_bin (testIndex) = 246 (Fs = 2048))
  float32_t testOutput[TEST_LENGTH_SAMPLES*2]; // output data in frequency domain
  float32_t mag[TEST_LENGTH_SAMPLES]; // this array contains the magnitudes of the complex numbers in testOutput
  arm_status status; // status of the ARM operation (success or error)
  float32_t maxValue; // the largest element in the array mag[TEST_LENGTH_SAMPLES]
  uint32_t  testIndex = 0; // the position that maxValue is in the array mag => The strongest frequency component
  uint32_t speed = 0; 
  status = ARM_MATH_SUCCESS;

   /*Initialize the Interrupt*/
  CyGlobalIntEnable;
  Button_ISR_StartEx(Buttonisr);
  
  int i = 0;
  for(;;)
  {

    if(ADC_DelSig_1_IsEndConversion(ADC_DelSig_1_RETURN_STATUS))
    {
      testInput[i] = ADC_DelSig_1_CountsTo_Volts(ADC_DelSig_1_GetResult32()); // 16-bit single-ended mode => Use the 
    // ADC_DelSig_1_GetResult32() (ADC_DelSig_1_GetResult16() returns only signed 16-bit results, 
    // which allows a maximum positive value of 32767, not 65535) 
    
      if (i == TEST_LENGTH_SAMPLES - 1)
      {
         /* Initialization function for the floating-point RFFT/RIFFT */    
        status = arm_rfft_init_f32(&S, &S_CFFT, fftSize, ifftFlag, doBitReverse); 
                
        /* Process the data through the Real FFT module */
        arm_rfft_f32(&S, testInput, testOutput);
        
        /* Process the data through the Complex Magnitude Module for
  calculating the magnitude at each frequency bin */
        arm_cmplx_mag_f32(testOutput, mag, fftSize/2 + 1);

        /* Calculate maxValue and return corresponding frequency bin value */
        arm_max_f32(mag, fftSize/2 + 1, &maxValue, &testIndex);   
        
        
        while(testIndex < cutoff) // this while loop acts as a high-pass filter
        {
          mag[testIndex] = 0; // Set the magnitude at the frequencies that are lower than cutoff to zero
          arm_max_f32(mag, fftSize/2 + 1, &maxValue, &testIndex); // Re-compute the maxValue and 
        // return corresponding frequency bin value
        }
        
        speed = testIndex/19.51;
                
        sprintf(uartout, "Frequency: %u Hz \r\n", testIndex);
        UART_1_PutString(uartout);
        
        
        
        /* For speed measurement*/
          LCD_1_ClearDisplay(); //Clear the old displayed value
          LCD_1_Position(0,0);
          LCD_1_PrintString("F:"); // Frequency
          LCD_1_Position(0,2);
          LCD_1_PrintU32Number(testIndex);
          LCD_1_Position(0,6);
          LCD_1_PrintString("Hz");
          LCD_1_Position(0,8);
          LCD_1_PrintString(",");
          LCD_1_Position(0,9);
          LCD_1_PrintString("C:"); // Cutoff
          LCD_1_Position(0,11);
          LCD_1_PrintU32Number(cutoff);
          LCD_1_Position(0,14);       
          LCD_1_PrintString("Hz");
          LCD_1_Position(1,0); 
          LCD_1_PrintString("Speed:");
          LCD_1_Position(1,7);
          LCD_1_PrintU32Number(speed);
          LCD_1_Position(1,11);
          LCD_1_PrintString("km/h");
        
        i = 0; // Let i = 0 in order to start a new array for the next waveform
      }
      else
      {
        i += 1; // Increase the index in the array testInput if TEST_LENGTH_SAMPLES is not reached
      }
    }
  }

}

CY_ISR(Buttonisr)
{
  cutoff += 50; // Increase the cutoff frequency when the button is pressed
  Button_ClearInterrupt();
}




/* [] END OF FILE */





