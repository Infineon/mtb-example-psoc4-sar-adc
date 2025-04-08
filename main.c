/******************************************************************************
* File Name:   main.c
*
* Description: This example demonstrates the use of a SAR ADC resource of
                PSoC 4. The ADC is configured to measure inputs from
                different sources and display results through a uart terminal.
*
* Related Document: See README.md
*
*
*******************************************************************************
* Copyright 2021-2025, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/
#include "cy_pdl.h"
#include "cybsp.h"
#include <stdio.h>


cy_stc_scb_uart_context_t CYBSP_DEBUG_UART_context;


/* Function to Initialize UART for Debugging */
static void Initialize_UART_Debug(void);
static void Print_ADC(uint8_t adc_ch, int adc_value);
/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function for CM0 Plus CPU. It does the following functions -
*    1. Initialize debug UART
*    2. Initialize and enable the Opamp in follower mode
*    3. Initialize and enable the SAR ADC
*    4. Trigger periodic conversions for measuring input voltages and temperature
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    /*Variables to store analog to digital conversion results*/
    int adcResult0, adcResult1, adcResult2, adcResult3;

    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }


/* We do not have CTBM in Yellowstone Device*/
#ifndef COMPONENT_PSOC4100TP
    /*Initialize the OPAMP0 with device configurator generated structure*/
    result = Cy_CTB_Init(CTBM0, &pass_0_ctb_0_config);
    if (result != CY_CTB_SUCCESS)
    {
        CY_ASSERT(0);
    }
    /*Enable Opamp0*/
    Cy_CTB_Enable(CTBM0);
#endif
    /* Initialize the SAR ADC with the device configurator generated structure*/
    result = Cy_SAR_Init(SAR0, &pass_0_sar_0_config);
    if (result != CY_SAR_SUCCESS)
    {
        CY_ASSERT(0);
    }


    /* Initialize to Debug UART port */
    Initialize_UART_Debug();
    /* Enable the SAR ADC */
    Cy_SAR_Enable(SAR0);

    /* Print message */
    Cy_SCB_UART_PutString(CYBSP_DEBUG_UART_HW,"\x1b[2J\x1b[;H");
    Cy_SCB_UART_PutString(CYBSP_DEBUG_UART_HW,"-----------------------------------------------------------\r\n");
    Cy_SCB_UART_PutString(CYBSP_DEBUG_UART_HW,"PSoC 4 MCU: SAR ADC BASIC\r\n");
    Cy_SCB_UART_PutString(CYBSP_DEBUG_UART_HW,"-----------------------------------------------------------\r\n\n");
    for (;;)
    {
        /* Start the continuous conversion */
        Cy_SAR_StartConvert(SAR0, CY_SAR_START_CONVERT_SINGLE_SHOT);
        /* Wait till the sample is ready */
        Cy_SAR_IsEndConversion(SAR0, CY_SAR_WAIT_FOR_RESULT);
        /* Get the result from Input 0 */
        adcResult0 = Cy_SAR_GetResult16(SAR0, 0);
        /* Get the result from Input 1 */
        adcResult1 = Cy_SAR_GetResult16(SAR0, 1);
        /* Get the result from Input 2 */
        adcResult2 = Cy_SAR_GetResult16(SAR0, 2);
        /* Get the result from Input 3 */
        adcResult3 = Cy_SAR_GetResult16(SAR0, 3);
        /*Print Input 0 result through UART*/
        Print_ADC(0,Cy_SAR_CountsTo_mVolts(SAR0, 0, adcResult0));
        /*Print Input 1 result through UART*/
        Print_ADC(1, Cy_SAR_CountsTo_mVolts(SAR0, 1, adcResult1));
        /*Print Input 2 result through UART*/
        Print_ADC(2, Cy_SAR_CountsTo_mVolts(SAR0, 2, adcResult2));
        /*Print Input 3 result through UART*/
        Print_ADC(3, Cy_SAR_CountsTo_degreeC(SAR0, 3, adcResult3));
        /*Delay between conversions*/
        Cy_SysLib_Delay(1000);
    }
}

static void Initialize_UART_Debug(void)
{
    Cy_SCB_UART_Init(CYBSP_DEBUG_UART_HW , &CYBSP_DEBUG_UART_config, &CYBSP_DEBUG_UART_context);
    Cy_SCB_UART_Enable(CYBSP_DEBUG_UART_HW);
}
static void Print_ADC(uint8_t adc_ch, int adc_value)
{
    char buff[100];
    switch(adc_ch)
    {
        case 0:
            /*Update buffer for Input 0 result*/
            sprintf(buff,"ADC Result Channel 0= %d mV\r\n",adc_value);
            break;
        case 1:
            /*Update buffer for Input 1 result*/
            sprintf(buff,"ADC Result Channel 1= %d mV\r\n",adc_value);
            break;
        case 2:
            /*Update buffer for Input 2 result*/
            sprintf(buff,"ADC Result Channel 2= %d mV\r\n",adc_value);
            break;
        case 3:
            /*Update buffer for Input 2 result*/
            sprintf(buff,"Die Temperature= %d degC\r\n\n",adc_value);
            break;
    }
    /*Print Updated buffer on UART port*/
    Cy_SCB_UART_PutString(CYBSP_DEBUG_UART_HW,buff);
}
/* [] END OF FILE */
