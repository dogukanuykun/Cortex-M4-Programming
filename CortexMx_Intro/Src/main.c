/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include<stdio.h>
#include<stdint.h>

/* This function executes in THREAD MODE of the processor */
int main(void)
{

#if 0
	__asm volatile("LDR R1,=#0X20001000");
	__asm volatile("LDR R2,=#0X20001004");
	__asm volatile("LDR R0,[R1]");
	__asm volatile("LDR R1,[R2]");
	__asm volatile("ADD R0,R0,R1");
	__asm volatile("STR R0,[R2]");
#endif

	int val = 50;
	__asm volatile("MOV R0,%0": :"r"(val));

	uint32_t control_reg;
	__asm volatile("MRS %0,CONTROL":"=r"(control_reg));

	uint32_t var1=2, var2=4;
	__asm volatile("MOV %0,%1": "=r"(var2) : "r"(var1));

	uint32_t p1, *p2;
	p2 = (uint32_t*)0x20000008;

	__asm volatile("LDR %0,[%1]": "=r"(p1) : "r"(p2) );

	for(;;);
}



