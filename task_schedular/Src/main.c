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

#include <stdint.h>
#include<stdio.h>
#include<main.h>
#include<led.h>

void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);

void init_systick_timer(uint32_t tick_hz);
void init_tasks_stack(void);
__attribute__((naked)) void init_schedular_stack(uint32_t sched_top_of_stack);
void enable_processor_fault(void);
__attribute__((naked)) void switch_sp_to_psp(void);
uint32_t get_psp_value(void);
uint32_t get_psp_value(void);
void update_next_task();

uint32_t psp_of_tasks[MAX_TASKS] = {
		T1_STACK_START,
		T2_STACK_START,
		T3_STACK_START,
		T4_STACK_START
};

uint32_t task_handlers[MAX_TASKS] = {

};


uint8_t current_task = 0; // task1 is running

int main(void)
{
	enable_processor_fault();

	init_schedular_stack(SCHED_STACK_START);

	task_handlers[0] = (uint32_t)task1_handler;
	task_handlers[1] = (uint32_t)task2_handler;
	task_handlers[2] = (uint32_t)task3_handler;
	task_handlers[3] = (uint32_t)task4_handler;

	init_tasks_stack();

	led_init_all();

	init_systick_timer(TICK_HZ);

	switch_sp_to_psp();

	task1_handler();
    /* Loop forever */
	for(;;);
}

void task1_handler(void){
	while(1){
		led_on(LED_GREEN);
		delay(DELAY_COUNT_1S);
		led_off(LED_GREEN);
		delay(DELAY_COUNT_1S);
	}
}

void task2_handler(void){
	while(1){
		led_on(LED_ORANGE);
		delay(DELAY_COUNT_500MS);
		led_off(LED_ORANGE);
		delay(DELAY_COUNT_500MS);
		}
}
void task3_handler(void){
	while(1){
		led_on(LED_BLUE);
		delay(DELAY_COUNT_250MS);
		led_off(LED_BLUE);
		delay(DELAY_COUNT_250MS);
		}
}
void task4_handler(void){
	while(1){
		led_on(LED_RED);
		delay(DELAY_COUNT_125MS);
		led_off(LED_RED);
		delay(DELAY_COUNT_125MS);
		}
}


void init_systick_timer(uint32_t tick_hz){
	uint32_t *pSRVR = (uint32_t*)0xE000E014; // SysTick Reload Value Register
	uint32_t *pSCSR = (uint32_t*)0xE000E010; // SysTick Control and Status Register
	uint32_t count_value = (SYSTICK_TIM_CLK / tick_hz)-1;

	//Clear the value of SVR
	*pSRVR &= ~(0x00FFFFFFFF);

	//Load the value into SVR
	*pSRVR |= count_value;

	//do some settings
	*pSCSR |= (1 << 1); // Enables SysTick exception request
	*pSCSR |= (1 << 2); // Indicates the clock source, processor clock source

	//enable systick
	*pSCSR |= (1 << 0);//enables the counter

}

__attribute__((naked)) void init_schedular_stack(uint32_t sched_top_of_stack){
	//__asm volatile("MSR MSP,R0");
	__asm volatile("MSR MSP,%0": : "r"(sched_top_of_stack) : );
	__asm volatile("BX LR");
}


void init_tasks_stack(void){

	uint32_t *pPSP;
	for (int i = 0; i < MAX_TASKS; i++) {
		pPSP = (uint32_t*)psp_of_tasks[i];

		pPSP--;
		*pPSP = DUMMY_XPSR;//0x01000000

		pPSP--; //PC
		*pPSP = task_handlers[i];//0x01000000

		pPSP--; //LR
		*pPSP = 0xFFFFFFFD;//0x01000000

		for (int j = 0; j < 13; j++) {
			pPSP--; //R0 - R12
			*pPSP = 0;
		}


		psp_of_tasks[i] = (uint32_t)pPSP;


	}

}


void enable_processor_fault(void){
	uint32_t *pSHCSR = (uint32_t*)0xE000ED24;

	*pSHCSR |= (1 << 16);//mem manage
	*pSHCSR |= (1 << 17);//bus fault
	*pSHCSR |= (1 << 18);//usage fault
}


uint32_t get_psp_value(void){
	return psp_of_tasks[current_task];
}

void save_psp_value(uint32_t stack_psp_value){
	psp_of_tasks[current_task] = stack_psp_value;
}

void update_next_task(){
	current_task++;
	current_task %= MAX_TASKS;
}

__attribute__((naked)) void switch_sp_to_psp(void){
	//1. initialize the PSP with TASK1 stack start
	//get the value if psp of current_task
	__asm volatile("PUSH {LR}");//preserved LR which connects back to main
	__asm volatile("BL get_psp_value");
	__asm volatile("MSR PSP, R0");//initialize psp
	__asm volatile("POP {LR}");//pops back LR value

	//2. change SP to PSP using CONTROL register
	__asm volatile("MOV R0, #0x02");
	__asm volatile("MSR CONTROL, R0");
	__asm volatile("BX LR");

}



__attribute__((naked)) void SysTick_Handler(void){
	//Save the context of current task
	__asm volatile("MRS R0,PSP");
	__asm volatile("STMDB R0!,{R4-R11}");

	__asm volatile("PUSH {LR}");
	__asm volatile("BL save_psp_value");

	//Retrive the context of next task
	__asm volatile("BL update_next_task");
	__asm volatile("BL get_psp_value");
	__asm volatile("LDMIA R0,{R4-R11}");
	__asm volatile("MSR PSP,R0");
	__asm volatile("POP {LR}");

	__asm volatile("BX LR");

}

//Implement fault handlers
void HardFault_Handler(void){
	printf("Exception: HardFault\n");
	while(1);
}
void MemManage_Handler(void){
	printf("Exception: MemManage\n");
	while(1);
}
void BusFault_Handler(void){
	printf("Exception: BusFault\n");
	while(1);
}


