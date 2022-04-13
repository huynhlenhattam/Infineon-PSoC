/******************************************************************************
* Huynh Le Nhat Tam
* 6th April 2022
*******************************************************************************/

#include <project.h>
#include "stdio.h"

/* Project Defines */
#define FALSE  0
#define TRUE   1
#define TRANSMIT_BUFFER_SIZE  200

uint32 x = 0;
uint32 Output1; //Variable to store ADC value (in mV)
uint32 AvgOutput = 0; //Average value of ADC values 
uint32 count = 0; //number of samples
uint32 temp = 0;  //the value of the temperature in degree Celsius)
CY_ISR_PROTO(myEOCisr); //EOC: End of Conversion
CY_ISR_PROTO(myTIMERisr);
/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  main() performs following functions:
*  1: Starts the ADC and UART components.
*  2: Checks for ADC end of conversion.  Stores latest result in output
*     if conversion complete.
*  3: Checks for UART input.
*     On 'C' or 'c' received: transmits the last sample via the UART.
*     On 'S' or 's' received: continuously transmits samples as they are completed.
*     On 'X' or 'x' received: stops continuously transmitting samples.
*     On 'E' or 'e' received: transmits a dummy byte of data.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
int main()
{
    
    
    /* Variable to store UART received character */
    uint8 Ch;
    /* Variable used to send emulated data */
    uint8 EmulatedData;
    /* Flags used to store transmit data commands */
    uint8 ContinuouslySendData;
    uint8 SendSingleByte;
    uint8 SendEmulatedData;
    /* Transmit Buffer */
    char TransmitBuffer[TRANSMIT_BUFFER_SIZE];
    
    /* Start the components */
    ADC_DelSig_1_Start();
    UART_1_Start();
    Timer_1_Start();
    
    /* Initialize Variables */
    ContinuouslySendData = FALSE;
    SendSingleByte = FALSE;
    SendEmulatedData = FALSE;
    EmulatedData = 0;
    
    /* Start the ADC conversion */
    ADC_DelSig_1_StartConvert();
    
    /* Send message to verify COM port is connected properly */
    UART_1_PutString("COM Port Open");
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    isr_EOC_StartEx(myEOCisr);
    isr_TIMER_StartEx(myTIMERisr);
    
    for(;;)
    {        
        /* Non-blocking call to get the latest data recieved  */
        Ch = UART_1_GetChar();
        
        /* Set flags based on UART command */
        switch(Ch)
        {
            case 0:
                /* No new data was recieved */
                break;
            case 'C':
            case 'c':
                SendSingleByte = TRUE;
                break;
            case 'S':
            case 's':
                ContinuouslySendData = TRUE;
                break;
            case 'X':
            case 'x':
                ContinuouslySendData = FALSE;
                break;
            case 'E':
            case 'e':
                SendEmulatedData = TRUE;
                break;
            default:
                /* Place error handling code here */
                break;    
        }
        
        
           
            /* Send data based on last UART command */
        
            if(SendSingleByte || ContinuouslySendData)
            {
                /* Format ADC result for transmition */
                sprintf(TransmitBuffer, "The average value of the ADC: %lu mV, Temperature: %lu degree Celsius\r\n", AvgOutput, temp);
                /* Send out the data */
                UART_1_PutString(TransmitBuffer);
                /* Reset the send once flag */
                SendSingleByte = FALSE;
            }
            else if(SendEmulatedData)
            {
                /* Format ADC result for transmition */
                sprintf(TransmitBuffer, "Emulated Data: %x \r\n",EmulatedData);
                /* Send out the data */
                UART_1_PutString(TransmitBuffer);
                EmulatedData++;
                /* Reset the send once flag */
                SendEmulatedData = FALSE;   
            }
        }
    }
//}

//ISR for EOC
CY_ISR(myEOCisr)
{
Output1 = ADC_DelSig_1_CountsTo_mVolts(ADC_DelSig_1_GetResult32());//receive the data from ADC and convert to mV
x += Output1; //sum of ADC values
count += 1; //number of samples
}
//ISR for TIMER
CY_ISR(myTIMERisr)
{
AvgOutput = x/count;
temp = AvgOutput/10; 
Timer_1_ReadStatusRegister();
}

/* [] END OF FILE */
