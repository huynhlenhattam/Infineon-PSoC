/* ========================================
 *
 * Huynh Le Nhat Tam
 * Date: 5th April 2022
 * ========================================
*/
#include "project.h"
CY_ISR_PROTO(myBUTTONisr);

uint8 flag = 0;
uint16 val = 19000; //This value corresponds to the angle of the servo (Compare type: Greater => 19000 LOW, 1000 HIGH => 90 degree to the right)
uint16 x = 19000;  //This variable is introduced to make the transition from each angle smooth
int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Clock_1_Start();
    PWM_1_Start();
    PWM_1_WriteCompare(val); //write the compare value
    isr_BUTTON_StartEx(myBUTTONisr);
    for(;;)
    {
        /* Place your application code here. */
        if (flag == 1)
        {
           val = val - 250;
            while( x != val)
            { 
                x = x - 1;
                PWM_1_WriteCompare(x); //write the compare value
            }
           
           if (val < 18000) 
           {val = 19000;} //prevent the value of val from getting too small (Choose the compare mode Greater => the smaller the compare value is, the larger the angle that the servo rotates)
           flag = 0;
            

            
        
        }
    }
        
}

//ISR for BUTTON
CY_ISR(myBUTTONisr)
{
  
//  set a flag that this interrupt is served 
    flag = 1;
//  Clear interrupt source
   
  BUTTON_ClearInterrupt();
}
/* [] END OF FILE */
