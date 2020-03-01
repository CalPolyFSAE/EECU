/* std lib */
#include <queue> 

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Hardware includes. */
#include "MKE18F16.h"
#include "pin_mux.h"
#include "clock_config.h"

/* MKE includes */
#include "can.h"

using namespace BSP;
using namespace std;

queue <can::CANlight::frame> q1;
queue <can::CANlight::frame> q2;

typedef struct{
	long unsigned int id;
	queue <can::CANlight::frame> q;
} IdAndQueue;

IdAndQueue queueSorterData[] = 
{
IdAndQueue{222, q1},
IdAndQueue{333, q2}
};

void callback(){
	can::CANlight::frame out;
    can::CANlight& can = can::CANlight::StaticClass();
    can::CANlight::frame f = can.readrx(1);

	/* Loop through structs */
	for(unsigned int i =0; i<sizeof(queueSorterData); i++){ 
		if((queueSorterData[i]).id == f.id){
			((queueSorterData[i]).q).push(f);
		}
	}
}

int main( void )
{
    BOARD_InitBootClocks();
    BOARD_InitBootPins();

	can::can_config config;
	can::CANlight::ConstructStatic(&config);
    can::CANlight& can = can::CANlight::StaticClass();

    // can::CANlight::canx_config can0_config;
    // can0_config.baudRate = 500000;
    // can.init(0, &can0_config);

    can::CANlight::canx_config can1_config;
    can1_config.baudRate = 500000;
    can1_config.callback = callback;
    can.init(1, &can1_config);

	// can::CANlight::frame out;
    // out.id = 0x221;
    // out.ext = 1;
    // out.data[0] = 141;

    // can.tx(0, out);

	// can::CANlight::frame f = can.readrx(1);

    // you gotta make a task right here
    
	/* Start the scheduler. */
	//vTaskStartScheduler();

	/* Infinite loop */
	for( ;; );
}

extern "C" {

/*-----------------------------------------------------------*/
void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */
}
/*-----------------------------------------------------------*/

}
