/* ========================================
 *
 * Huynh Le Nhat Tam
 * Date: 4th April 2022
 * ========================================
*/
#include "project.h"
#include "stdio.h"

char cVal; //int can also be used
char sMyName[20] = "Nhat Tam\n\r"; //20: maximum number of characters in the string
int counter = 0 ; //Initialize the software counter
char count[20];

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    UART_Start();
    UART_PutString(sMyName);
    
    BLUE_Write(1); //LED is on
    
    for(;;)
    {
        /* Place your application code here. */
        cVal = BUTTON_Read();
        BLUE_Write(cVal); //LED is on
        CyDelay(2000); //wait
        BLUE_Write(0); //LED is off
        CyDelay(2000); //wait
        counter += 1; //increment the counter after each LED blink
        sprintf (count,"Counter: %d \n\r",counter); //Assign the value of counter to the the string count (%d for int)
        UART_PutString(count);
    }
}

/* [] END OF FILE */
