/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://www.github.com/FreeRTOS
 *
 * 1 tab == 4 spaces!
 */

/* FreeRTOS kernel includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <stdio.h>

#include "platform/ee290c_oscibear.h"

/* Priorities used by the tasks. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue.  The 200ms value is converted
to ticks using the pdMS_TO_TICKS() macro. */
#define mainQUEUE_SEND_FREQUENCY_MS			pdMS_TO_TICKS( 1000 )

/* The maximum number items the queue can hold.  The priority of the receiving
task is above the priority of the sending task, so the receiving task will
preempt the sending task and remove the queue items each time the sending task
writes to the queue.  Therefore the queue will never have more than one item in
it at any time, and even with a queue length of 1, the sending task will never
find the queue full. */
#define mainQUEUE_LENGTH					( 1 )

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/*-----------------------------------------------------------*/

typedef struct {
	const char *msg;
	char is_first;
} ping_pong_task_s;

static inline uint32_t
cycle_counter(void) {
	uint32_t c;
	__asm__ volatile ("csrr %0, cycle" : "=r" (c));
	return c;
}

static void pingPongTask( void *pvParameters ) {
	ping_pong_task_s *ppt = (ping_pong_task_s *)pvParameters;
	char recv = !ppt->is_first;
	TickType_t xNextWakeTime = xTaskGetTickCount();
	const char *task_name = pcTaskGetName(xTaskGetCurrentTaskHandle());
	char buf [40];
	uint32_t cycles;

	while (1) {
		if (recv) {
			const char *recv_ptr = NULL;
			xQueueReceive(xQueue, (void *)&recv_ptr, portMAX_DELAY);
			cycles = cycle_counter();
			snprintf(buf, sizeof(buf), "[%s] RX: %s", task_name, recv_ptr);
			vSendString(buf);
			vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );
		} else {
			snprintf(buf, sizeof(buf), "[%s] TX...", task_name);
			vSendString(buf);
			xQueueSend(xQueue, &ppt->msg, 0);
			vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );
		}

		recv = !recv;
	}
}


/*-----------------------------------------------------------*/

int main_pingPong( void )
{
	vSendString( "[main] Hello FreeRTOS <3");

	/* Create the queue. */
	xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint32_t ) );

	if( xQueue != NULL )
	{
		/* Start the two tasks as described in the comments at the top of this
		file. */
		ping_pong_task_s *ts1 = pvPortMalloc(sizeof(ping_pong_task_s)); 
		ts1->msg = "Ping";
		ts1->is_first = 0x1;

		ping_pong_task_s *ts2 = pvPortMalloc(sizeof(ping_pong_task_s)); 
		ts2->msg = "Pong";
		ts2->is_first = 0x0;
		xTaskCreate( pingPongTask, "Task 1", configMINIMAL_STACK_SIZE, (void *)ts1,
					mainQUEUE_RECEIVE_TASK_PRIORITY, NULL );

		xTaskCreate( pingPongTask, "Task 2", configMINIMAL_STACK_SIZE, (void *)ts2,
					mainQUEUE_SEND_TASK_PRIORITY, NULL );
	}

	vTaskStartScheduler();

	return 0;
}
