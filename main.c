
//----------------------------------------
// BIOS header files
//----------------------------------------
#include <xdc/std.h>  						//mandatory - have to include first, for BIOS types
#include <ti/sysbios/BIOS.h> 				//mandatory - if you call APIs like BIOS_start()
#include <xdc/runtime/Log.h>				//needed for any Log_info() call
#include <xdc/cfg/global.h> 				//header file for statically defined objects/handles


//-----------------------------------------
// MSP430 Header Files
//-----------------------------------------
#include <driverlib.h>
#include <stdlib.h>						//for using rand and srand function
#include <time.h>						//for time(0) used in srand function

//-----------------------------------------
// MSP430 MCLK frequency settings
// Used to set MCLK frequency
// Ratio = MCLK/FLLREF = 8192KHz/32KHz
//-----------------------------------------
#define MCLK_DESIRED_FREQUENCY_IN_KHZ  8000                            // 8MHz
#define MCLK_FLLREF_RATIO              MCLK_DESIRED_FREQUENCY_IN_KHZ / ( UCS_REFOCLK_FREQUENCY / 1024 )    // Ratio = 250

#define GPIO_ALL	GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3| \
					GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7


#define BUFFER_SIZE 10  //Size of the shared buffer
#define MAX_VAL_NUM 10 //Maximum value of randomly generated produced item!

//-----------------------------------------
// additional defines
//-----------------------------------------
#define GREEN_LED GPIO_PORT_P4,GPIO_PIN7
#define RED_LED GPIO_PORT_P1,GPIO_PIN0

//-----------------------------------------
// Prototypes
//-----------------------------------------


//LED_E enum - denoting all possible LEDs in the system
typedef enum
{
	red_e,
	green_e
} LED_E;


/*
 Structure LedBlinksInfo_T, so that each of the producerTask and consumerTask can define
 a local instance of this (each one in its own context function!) and fill it accordingly.
 This Structure is used, in order to communicate between the producerTask/consumerTask and
 the ledSrvTask (which provides LED blinking services) - each one of the
 producerTask/consumerTask fills it with the appropriate LED blinking specification
 (after producing/consuming an item) and "send" it to ledSrvTask. Well, it is not really a
 "sending" operation, but using the ability BIOS is providing us to set/get the Env (Environment)
 of a Task, using Task_setEnv and Task_getEnv API functions (see API Documentation) - recall,
 setting the Task Env is merely giving it a generic pointer (of type Ptr - which is simply a
 void * pointer - hence generic, which could be a pointer to anything!) to some data. The Task
 whose Env was set can then get this Env data pointer using Task_getEnv API function (and cast
 the pointer appropriately!) and then read the data!

 In this case, producerTask/consumerTask fills a local copy of this structure with the
 appropriate LED blinking specification (after producing/consuming an item) and then sending
 the pointer to this structure to ledSrvTask using Task_setEnv API function.
 Then, when ledSrvTask gets to run, it simply reads this Env pointer using Task_getEnv API
 function (casting the result, of course, to type LedBlinksInfo_T *).
 The LED blinking specification is simple:

 	 - producerTask fills the "led" field to be green_e whereas consumerTask fills
 	   the led field to be red_e.

 	 - the "blinksNum" is to be filled with the number of Blinks the LED (specified in the
 	   field "led") should be blinked.
 */
typedef struct
{
	LED_E led;
	Int blinksNum;
}LedBlinksInfo_T;


//The usual hardware_init function
void hardware_init(void);


/*
 Function: Bool insert_item(Int item)

 This function is called from the producerTask (after producerTask generated a random number
 in the value between 1 and MAX_VAL_NUM). This function receives the produced item in the
 parameter "item" and updates it in the shared buffer "buffer".

 Several guidelines:

 1) This function needs to synchronize the access to the shared data using the semaphores:
    emptySlots, mutex and fullSlots (as defined in the lecture notes in the function "Producer"
    - Ch. 8, slide 63). This should prevent the producerTask from Producing an item when buffer
    is full. Again, this should be done according to the algorithm in lecture Notes.

    Function is also doing Error Checking/Handling - therefore, it returns "TRUE" when
    item was successfully added to the shared "buffer" (and "in" and "count" global variables
    are updated accordingly!). Otherwise, if there is something wrong (i.e. Abnormal Behaviour)
    - it should issue a Log message and return FALSE.
    Abnormal behaviour (Unsuccessful Result), in this case, would be to try and write the item
    on an existing (not consumed!) data-item in the buffer (i.e. buffer[in] value is NOT -1:
    recall - an "empty" buffer cell is always marked with -1 - this is done in initialisation
    of the "buffer" (in main function) and also by the consumer's function remove_item, which
    sets the consumed cell value to -1, after consumption is done!).

    The shared data in this case are the global variables defined in the global section: the
    shared "buffer" array, the "in" variable - managing the next empty slot in the
    shared buffer and the "count" variable - holding the current number of items in the
    shared buffer.

    After gaining access to the shared data (according to the Algorithm in the lecture notes),
    insert_item function should perform the following:

    1) Check if  Abnormal behaviour (i.e. buffer[in] value is NOT -1). If Abnormal behaviour
       is encountered, issue a Log message, release all the "taken" semaphores and return FALSE.

    2) If Everything is OK:
       - update the "count" global variable;
       - enter the "item" to the buffer at location "in";
       - then update "in" global variable to hold the position of the next item for "production"
         (recall - buffer is cyclic! Look in the Lab document for further explanation on how to
         do that!);
       - issue a Log message - outputting the value of the currently produced item and the
         up-to-dated number of items in the buffer (i.e. the value of "count" global variable).

    3) Then, release the Semaphores (according to the Algorithm in the lecture notes) and
       return TRUE.
 */
Bool insert_item(Int item);

/*
 Function: Bool remove_item(Int *item);

 This function is called from the consumerTask. This function receives an address of a locally
 defined variable in a certain consumerTask and "consumes" the next available item
 from the shared buffer "buffer" - copying this item to the contents of the address pointed
 by *item (I'm writing certain consumerTask because, after you test the final program with one
 producerTask/consumerTask, you will be asked to add another  producerTask/consumerTask and to
 verify your program works generically without doing any changes (apart from configuring in GUI
 the two additional producerTask/consumerTask).


 Several guidelines:

 1) This function needs to synchronize the access to the shared data using the semaphores:
    fullSlots, mutex and emptySlots (as defined in the lecture notes in the function "Consumer"
    - Ch. 8, slide 63). This should prevent the consumerTask from Consuming an item when buffer
    is empty. Again, this should be done according to the algorithm in lecture Notes.

    Function is also doing Error Checking/Handling - therefore, it returns "TRUE" when
    item was successfully consumed from the shared "buffer" (and "out" and "count" global
    variables are updated accordingly!). Otherwise, if there is something wrong
    (i.e. Abnormal Behaviour) - it should issue a Log message and return FALSE.
    Abnormal behaviour (Unsuccessful Result), in this case, would be to try and remove a
    non-existing item from the shared buffer (i.e. buffer[out] value is -1:
    recall - an "empty" buffer cell is always marked with -1 - this is done in initialisation
    of the "buffer" (in main function) and also by this function, which sets the consumed cell
    value to -1, after consumption is done!).

    The shared data in this case are the global variables defined in the global section: the
    shared "buffer" array, the "out" variable - managing the next full slot in the shared buffer
    and the "count" variable - holding the current number of items in the shared buffer.

    After gaining access to the shared data (according to the Algorithm in the lecture notes),
    remove_item function should perform the following:

    1) Check if  Abnormal behaviour (i.e. buffer[out] value is -1). If Abnormal behaviour
       is encountered, issue a Log message, release all the "taken" semaphores and return FALSE.

    2) If Everything is OK:
       - update the "count" global variable;
       - copy the value from the buffer at location "out" to *item;
	   - mark the consumed cell in the shared buffer as empty (i.e. put there -1);
       - then update "out" global variable to hold the position of the next item for "consumption"
         (recall - buffer is cyclic! Look in the Lab document for further explanation on how to
         do that!);
       - issue a Log message - outputting the value of the currently consumed item and the
         up-to-dated number of items in the buffer (i.e. the value of "count" global variable).


    3) Then, release the Semaphores (according to the Algorithm in the lecture notes) and
       return TRUE.
 */
Bool remove_item(Int *item);


/*
 Function: producerHandler(UArg arg0, UArg arg1)

 This is the handler function for the producerTask (producerTask1) and for all other
 producerTasks added to the system (in this case: producerTask2).

 This function implements the producer behaviour and provides the infrastructure for the
 producer to run forever! Therefore, this function runs in while(TRUE) loop.
 Remember, it must also:

  1) Define a local structure of type LedBlinksInfo_T;

  2) Recall, arg0 of this function holds the unique producerID (you gave it in GUI definition).


 Then the while(TRUE) loop. Every iteration in this loop should perform the following:

 1) Generate a random number between 1 and MAX_VAL_NUM;

 2) Send this number to insert_item function call. Recall, insert_item is the function that
    implements the producer algorithm for 1 item (as defined in the lecture notes) in the
    context of the currently running producerTask (therefore, currently running producerTask
    might block on one of the Semaphore pends in this function - according to the algorithm
    in the lecture notes!);

    You must check the return value from insert_item - if it is FALSE, you must issue a Log
    message about an abnormal behaviour and the producerID of the running producerTask. In
    this case, you must skip the rest of the loop and get back to the beginning of the loop!

 3) If no abnormal behaviour:

  	A. Issue a Log message outputting the producerID and the value of the item produced;

  	B. Set the Env of ledSrvTask (using Task_setEnv API function) to send it the up-to-dated
  	   data for the Led Blink (i.e. sending it a pointer to the local LedBlinksInfo_T structure
  	   that you must also update it with the proper information before that!)

  	C. Release ledSrvTask to work (by posting the Scheduling Constraint Semaphore
  	   ledSrvSchedSem, which ledSrvTask pends on).

  	Now, since ledSrvTask is the highest priority Task in the system, it will immediately get
  	to run and blink the appropriate Led (according to the specification you updated in
  	LedBlinksInfo_T structure and sent it to ledSrvTask's Env).

  	However, what happens if current Task yielded (by timeSharingClk), just after setting the
  	ledSrvTask's Env, but before posting ledSrvSchedSem (i.e. before releasing ledSrvTask)???
  	Some other producerTask/consumerTask may set ledSrvTask's Env with their data -
  	running over the data set by current Task...!!! Reminds you of something???
  	Indeed, this is RACE CONDITION!!!

  	This means that the ENTIRE CODE in sections B & C above is a CRITICAL SECTION!!!! It must,
  	therefore, be protected by a Mutex Semaphore (we named this Mutex Semaphore: setLedEnvMutex).

  4) Go back to the beginning of the while(TRUE) loop;
 */
void producerHandler(UArg arg0, UArg arg1);


/*
 Function: consumerHandler(UArg arg0, UArg arg1)

 This is the handler function for the consumerTask (consumerTask1) and for all other
 consumerTasks added to the system (in this case: consumerTask2).

 This function implements the consumer behaviour and provides the infrastructure for the
 consumer to run forever! Therefore, this function runs in while(TRUE) loop.
 Remember, it must also:

  1) Define a local structure of type LedBlinksInfo_T;

  2) Recall, arg0 of this function holds the unique consumerID (you gave it in GUI definition).
     

 Then the while(TRUE) loop. Every iteration in this loop should perform the following:

 1) Call remove_item function. Recall, remove_item is the function that
    implements the consumer algorithm for 1 item (as defined in the lecture notes) in the
    context of the currently running consumerTask (therefore, currently running consumerTask
    might block on one of the Semaphore pends in this function - according to the algorithm
    in the lecture notes!);

    You must check the return value from remove_item - if it is FALSE, you must issue a Log
    message about an abnormal behaviour and the consumerID of the running consumerTask. In
    this case, you must skip the rest of the loop and get back to the beginning of the loop!

 3) If no abnormal behaviour:

  	A. Issue a Log message outputting the consumerID and the value of the item consumed;

  	B. Set the Env of ledSrvTask (using Task_setEnv API function) to send it the up-to-dated
  	   data for the Led Blink (i.e. sending it a pointer to the local LedBlinksInfo_T structure
  	   that you must also update it with the proper information before that!)

  	C. Release ledSrvTask to work (by posting the Scheduling Constraint Semaphore
  	   ledSrvSchedSem, which ledSrvTask pends on).

  	Now, since ledSrvTask is the highest priority Task in the system, it will immediately get
  	to run and blink the appropriate Led (according to the specification you updated in
  	LedBlinksInfo_T structure and sent it to ledSrvTask's Env).

  	However, what happens if current Task yielded (by timeSharingClk), just after setting the
  	ledSrvTask's Env, but before posting ledSrvSchedSem (i.e. before releasing ledSrvTask)???
  	Some other producerTask/consumerTask may set ledSrvTask's Env with their data -
  	running over the data set by current Task...!!! Reminds you of something???
  	Indeed, this is RACE CONDITION!!!

  	This means that the ENTIRE CODE in sections B & C above is a CRITICAL SECTION!!!! It must,
  	therefore, be protected by a Mutex Semaphore (we named this Mutex Semaphore: setLedEnvMutex).

  4) Go back to the beginning of the while(TRUE) loop;
 */
void consumerHandler(UArg arg0, UArg arg1);


/*
 Function: ledSrvTaskHandler(void)

 This is the handling function for ledSrvTask. This is the highest priority Task in the system,
 so when it is scheduled (i.e. not pending on the Scheduling Constraint Semaphore:
 ledSrvSchedSem) - it will occupy the CPU exclusively! The role of this Task is to wait on the
 Scheduling Constraint Semaphore, ledSrvSchedSem, and whenever an event is posted it should:

  - Read its own (SELF!!) environment data, in order to get the Led blinking specification (Recall:
    consumerTask/producerTask updated ledSrvTask's Env with this specification, just before
    posting ledSrvSchedSem). This is done by calling Task_getEnv API function on SELF (Recall: the
    return value of Task_getEnv is of type Ptr (defined in BIOS as void *) - this should be
    casted to LedBlinksInfo_T *, in this case).

  - Blink the Led, according to the Led specification set by consumerTask/producerTask to
   ledSrvTask's Env - this is, in fact, done by simply calling ledToggle function with the
   parameters corresponding to the Led specification read from ledSrvTask's own Env!

  - get back to pend on ledSrvSchedSem Scheduling Constraint Semaphore.

 Of course - you realise that the above functionality should be run in while(TRUE) loop, so that
 ledSrvTask should always be ready to receive "posts" on ledSrvSchedSem Scheduling Constraint
 Semaphore.
 */
void ledSrvTaskHandler(void);


/*
 Function: void ledToggle(uint8_t selectedPort, uint16_t selectedPins, int times)

 This function is very similar to what you implemented in Lab 1: gets LED Port/Pin and times
 and toggles the corresponding LED times times.

 As you recall, a delay of 1/2 a second should be set between ON/OFF states of the Led.
 Note, in this project, you must use the delay function "delay" implemented here - see below
 (This delay function is similar to what you used in Lab 3, only that in this project, the Clock
 Module is set to tick every 500 microseconds, so the ~1/2 second delay value here is set with
 respect to 500 microseconds Clock tick - different to that in Lab3!).

 Recall, "delay" function consumes CPU Cycles for 1/2 second. You must use this function in the
 ledToggle function, rather than Task_sleep function, because Task_sleep function blocks on a
 timer for N milliseconds, and while blocking, another Task (with highest priority in the
 Ready Queue) gets to run, until Task unblocks from sleep! In this project, if you get
 ledSrvTask to release the CPU - you break the system requirements - and might get into
 trouble!!! Therefore, you have no choice, but to use a non-blocking delay here!
 */
void ledToggle(uint8_t selectedPort, uint16_t selectedPins, int times);


/*
 Function: void delay(void)

 The delay function, used in lab 3. This function consumes CPU Cycles for 1/2 second.
 You must use this function in the LED toggling function, rather than Task_sleep function,
 because Task_sleep function blocks on a timer for N milliseconds, and while blocking, another
 Task (with highest priority in the Ready Queue) gets to run, until Task unblocks from sleep!
 In this project, if you get ledSrvTask to release the CPU - you break the system requirements
 - and might get into trouble!!! Therefore, you have no choice, but to use a non-blocking
 delay here!
 */
void delay(void);


/*
 Function: void tsClockHandler(void)

 This is the handler function for the "time-sharing" Clock object: timeSharingClk.

 timeSharingClk is set to elapse every 1/2 millisecond (500 microseconds), and forcefully
 get the running Task to yield (You should know how to do that by now). If you set the Clock
 Module's "Tick period" to 500 microseconds (and NOT the default 1000 microseconds), what
 should be timeSharingClk "Initial timeout" and "period" set to in GUI?
 */
void tsClockHandler(void);


/*
 Function: initArray(volatile Int *arr, Int size)

 This function initialises the shared buffer array to hold -1 in ALL its cells.
 Recall, -1 marks an empty cell in the shared buffer! Therefore, initially ALL cells are empty!

 Of course, this function must be invoked from main function to initialise the shared
 buffer array - before BIOS kernel objects start running!
 */
void initArray(volatile Int *arr, Int size);



//-----------------------------------------
// Globals
//-----------------------------------------


/*
 The shared buffer array.
 */
volatile Int buffer[BUFFER_SIZE];

/*
 This global variable always manages the next empty slot in the shared buffer - see the
 description of the functions: insert_item & remove_item.
 */
volatile Int in = 0;

/*
 This global variable always manages the next full slot in the shared buffer - see the
 description of the functions: insert_item & remove_item.
 */
volatile Int out = 0;

/*
 This global variable always manages the count of current full slots in the shared buffer -
 see the description of the functions: insert_item & remove_item.
 */
volatile Int count = 0;


//---------------------------------------------------------------------------
// main()
//---------------------------------------------------------------------------
void main(void)
{
	/*
	 Remember to do all necessary initialisations here.
	 */
	Semaphore_reset(emptySlots, BUFFER_SIZE);	// initialize the semaphore value to the buffer size, so it will behave in a generic way, means if buffer size changes so as the emptySlots semaphore initial value.
	initArray(buffer, BUFFER_SIZE);				// initialize the bounded buffer's cells to -1 which means empty array

	hardware_init();							// init hardware via Xware

	BIOS_start(); 								// As it says, start the BIOS
}

/*
 * Function: hardware_init
 * Description: initialize hardware of the system to prevent some bugs that can accour from un-initializing those ports and etc.
 * Input: void
 * Output: void
 * Algorithm: Using system's api's to initialize hardware.
*/
void hardware_init(void)					//called by main
{
	// Disable the Watchdog Timer (important, as this is enabled by default)
	WDT_A_hold( WDT_A_BASE );


    // Set MCLK frequency to 8192 KHz
	// First, set DCO FLLREF to 32KHz = REF0
	UCS_initClockSignal(UCS_FLLREF, UCS_REFOCLK_SELECT, UCS_CLOCK_DIVIDER_1);
	// Second, Set Ratio and Desired MCLK Frequency = 8192KHz and initialize DCO
	UCS_initFLLSettle(MCLK_DESIRED_FREQUENCY_IN_KHZ, MCLK_FLLREF_RATIO);

	// Set GPIO ports to low-level outputs
    GPIO_setAsOutputPin( GPIO_PORT_P1, GPIO_ALL );
    GPIO_setAsOutputPin( GPIO_PORT_P2, GPIO_ALL );
    GPIO_setAsOutputPin( GPIO_PORT_P3, GPIO_ALL );
    GPIO_setAsOutputPin( GPIO_PORT_P4, GPIO_ALL );
    GPIO_setAsOutputPin( GPIO_PORT_P5, GPIO_ALL );
    GPIO_setAsOutputPin( GPIO_PORT_P6, GPIO_ALL );
    GPIO_setAsOutputPin( GPIO_PORT_PJ, GPIO_ALL );

    GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_ALL );
    GPIO_setOutputLowOnPin( GPIO_PORT_P2, GPIO_ALL );
    GPIO_setOutputLowOnPin( GPIO_PORT_P3, GPIO_ALL );
    GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_ALL );
    GPIO_setOutputLowOnPin( GPIO_PORT_P5, GPIO_ALL );
    GPIO_setOutputLowOnPin( GPIO_PORT_P6, GPIO_ALL );
    GPIO_setOutputLowOnPin( GPIO_PORT_PJ, GPIO_ALL );


    // Configure I/O's that we're using (redundant, based on the previous settings)
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0 );
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0 );
}

/*
 * Function: insert_item
 * Description: insert item into the buffer.
 * Input: Int item - hold the random integer sent by the producerHandler.
 * Output: Bool - return true if succesfuly insert and item which is a normal behavior, false otherwise which is abnormal behavior of the system.
 * Algorithm: Works as the producer algorithm work as given in lecture 8 page 63 but in such manner that it returns true if succesfuly inserts an
 * 			  item which is a normal behavior and issue a log about it and release semaphores, false otherwise which is abnormal behavior of the
 * 			  system and issue a log about it and release all "taken" semaphores.
*/
Bool insert_item(Int item) {
	/* Semaphores pend */
	Semaphore_pend(emptySlots, BIOS_WAIT_FOREVER); // pend emptySlots Counting Sem
	Semaphore_pend(mutex, BIOS_WAIT_FOREVER); // pend Mutex Sem

	/* Critical Section */
	if(buffer[in] != -1) { 			// if trying to insert item into non empty slot.
		Log_info0("ERROR! Can't insert an item into a non-empty slot.\n"); //error log
		/* End of Critical Section */
		Semaphore_post(mutex); // post Mutex Sem
		Semaphore_post(fullSlots); // post fullSlots Counting Sem
		return FALSE;
	} else {
		count = (count + 1);
		buffer[in] = item;
		in = (in + 1)%BUFFER_SIZE; // cyclic buffer implementation so the next in won't surpass the index boundry of the buffer.
		Log_info2("Success! buffer produced item is: %d, number of currently full slots is: %d\n", item, count); //success log
		/* End of Critical Section */
		Semaphore_post(mutex); // post Mutex Sem
		Semaphore_post(fullSlots); // post fullSlots Counting Sem
		return TRUE;
	}
}

/*
 * Function: remove_item
 * Description: removes an item from the bounded buffer.
 * Input: pointer to int which it name is item and will hold removed item.
 * Output: Bool - TRUE or FALSE depend on the if the buffer in this place eg. out is == -1 if yes
 * 		   then FALSE, TRUE otherwise.
 * Algorithm: pended on mutex and fullSlots semaphores, then when enter the critic sec. check if buffer
 * 			  in the current out position is empty if it is can't consume return FALSE issue compitable
 * 			  Log msg and post mutex and fullSlots.
*/
Bool remove_item(Int *item) {
	/* Semaphores pend */
	Semaphore_pend(fullSlots, BIOS_WAIT_FOREVER); // pend emptySlots Counting Sem
	Semaphore_pend(mutex, BIOS_WAIT_FOREVER); // pend Mutex Sem

	/* Critical Section */
	if(buffer[out] == -1) { 			// if trying to remove item from an empty slot.
		Log_info0("ERROR! Can't remove an item from an empty slot.\n"); //error log
		/* End of Critical Section */

		Semaphore_post(mutex); // post Mutex Sem
		Semaphore_post(emptySlots); // post emptySlots Counting Sem
		return FALSE;
	} else {
		count = (count - 1);
		*item = buffer[out];
		buffer[out] = -1;
		out = (out + 1)%BUFFER_SIZE; // cyclic buffer implementation so the next in won't surpass the index boundry of the buffer.
		Log_info2("Success! buffer consumed item is: %d, number of currently full slots is: %d\n", *item, count); //success log
		/* End of Critical Section */

		Semaphore_post(mutex); // post Mutex Sem
		Semaphore_post(emptySlots); // post emptySlots Counting Sem
		return TRUE;
	}
}

/*
 * Function: producerHandler
 * Description: generic producer which for every module which is a producer use it.
 * Input: UArg arg0 - holds the producer unique id , UArg arg1 - nothing set to 0.
 * Output: void
 * Algorithm: structure of PLPE - prolog loop process epilog. while(1) loop to make this task run forever to simulate time sharing
 * 			  system which we are not allowed to assume any timings about threads. the algo set the struct and items to init in the
 * 			  start of his context, sets green_led, producerId, ledBlinksInfo then while loop forever, define and init int variable
 * 			  named randNum using rand() functions to hold the value of the random item to be insert into the buffer, check success insert operation if TRUE issue Logs
 * 			  and wait to enter critical section for ledBlinking service then post it to make so LedSrvTask in RQ preemt the running
 * 			  task, and blink, Otherwise issue suitable Log of fail and continue to next iteration.
 *
*/
void producerHandler(UArg arg0, UArg arg1) {
	/* Prolog */
	LedBlinksInfo_T ledBlinksInfo;
	int producerId = (int)arg0;

	while(1) {
		/*Process*/
		srand(time(0)); // set the seed to be timer(0).
		int randNum = (rand()%MAX_VAL_NUM); // generate random number between 0 to MAX_VAL_NUM.
		Bool success = insert_item(randNum); // insert item to the bounded buffer.
		if(success) {
			Log_info2("The producer task with id = %d successfuly inserted the item = %d to the buffer.\n", producerId, randNum); //success log
			ledBlinksInfo.blinksNum = randNum;  // init struct member blinkNum to be according to the randNum variable.
			ledBlinksInfo.led = green_e;		// init struct member led to be according to the correct LED_E enum green_e/red_e.
			Semaphore_pend(setLedEnvMutex, BIOS_WAIT_FOREVER); // pend setLedEnvMutex Sem
			/* Cricital Section */
			Task_setEnv(ledSrvTask, (Ptr)&ledBlinksInfo); //set the task enviorments of ledSrvTask for internally thread communication in the system.
			Semaphore_post(ledSrvSchedSem); // post ledSrvSchedSem
			/* End of Critical Section */
			Semaphore_post(setLedEnvMutex); // post setLedEnvMutex Sem
		} else {
			Log_info1("ERROR! Producer task with id = %d failed to insert an item to the buffer.\n", producerId); //error log
			continue;
		}
	}
	/* Epilog */
}

/*
 * Function: consumerHandler
 * Description: generic consumer which for every module who use it with additional blinking
 * 				mechanism and struct to hold information.
 * Input: UArg arg0 - holds the consumer unique id , UArg arg1 - nothing set to 0.
 * Output: void.
 * Algorithm: structure of PLPE - prolog loop process epilog. while(1) loop to make this task run forever to simulate time sharing
 * 			  system which we are not allowed to assume any timings about threads. the algo set the struct and items to init in the
 * 			  start of his context, sets red_led, consumerId, ledBlinksInfo then while loop forever, define and init int variable
 * 			  named item to hold the value of the item removed from the buffer, check success remove operation if TRUE issue Logs
 * 			  and wait to enter critical section for ledBlinking service then post it to make so LedSrvTask in RQ preemt the running
 * 			  task, and blink, Otherwise issue suitable Log of fail and continue to next iteration.
*/
void consumerHandler(UArg arg0, UArg arg1) {
	/* Prolog */
	LedBlinksInfo_T ledBlinksInfo;
	int consumerId = (int)arg0;

	while(1) {
		/* Process */
		int item = 0;						// define new variable to hold the removed item
		Bool success = remove_item(&item);	// remove an item from the bounded buffer.
		if(success) {
			Log_info2("The consumer task with id = %d successfuly removed the item = %d from the buffer.\n", consumerId, item); //success log
			ledBlinksInfo.blinksNum = item;  	// init struct member blinkNum to be according to the item variable.
			ledBlinksInfo.led = red_e;		// init struct member led to be according to the correct LED_E enum green_e/red_e.
			Semaphore_pend(setLedEnvMutex, BIOS_WAIT_FOREVER); // pend setLedEnvMutex Sem
			/* Cricital Section */
			Task_setEnv(ledSrvTask, (Ptr)&ledBlinksInfo); //set the task enviorments of ledSrvTask for internally thread communication in the system.
			Semaphore_post(ledSrvSchedSem); // post ledSrvSchedSem
			/* End of Critical Section */
			Semaphore_post(setLedEnvMutex); // post setLedEnvMutex Sem
		} else {
			Log_info1("ERROR! Consumer task with id = %d failed to remove an item from the buffer.\n", consumerId); //error log
			continue;
		}
	}
	/* Epilog */
}

/*
 * Function: ledSrvTaskHandler
 * Description: Serves as the ledSrvTask Handle ISR function and his goal to make led blinks according to
 * 				the info sent by the other tasks.
 * Input: void.
 * Output: void.
 * Algorithm: Retrieve data from the task enviorment, data about the blinksNum and led type: green/red,
 * 			  Implements PLPE (Prolog, Loop, Process, Epilog) Structure,
 * 			  while loop  for ever,
 * 			  inside loop - semaphore pend on ledSrvSchedSem to conditionally ulitize the highest priority
 * 			  task (ledSrvTask) in the system,
 * 			  in proccess means currently running preemt other tasks call ledToggle which make the led blink
 * 			  according to the data from the ledBlinksInfo.
*/
void ledSrvTaskHandler(void) {
	/* Prolog */

	while(1) {
		Semaphore_pend(ledSrvSchedSem, BIOS_WAIT_FOREVER); // pend on ledSrvSchedSem
		LedBlinksInfo_T *ledBlinksInfo = (LedBlinksInfo_T *)Task_getEnv(Task_self()); // cast because pointer returned is of type Ptr which is void* in TI-RTOS

		/* Process */
		if(ledBlinksInfo->led == green_e) {
			ledToggle(GREEN_LED, ledBlinksInfo->blinksNum);
		} else if (ledBlinksInfo->led == red_e) {
			ledToggle(RED_LED, ledBlinksInfo->blinksNum);
		}
	}
	/* Epilog */
}

/*
 * Function: delay
 * Description : make delay, cpu burning cycles.
 * Input:  uint8_t selectedPort - pass ports, uint16_t selectedPins - pass pins, int times - holds number
 * 		   of how many times the led will blink.
 * Output: void.
 * Algorithm:  A simple use of API function _delay_cycles and pass the right amount of CPU cycles according to
 * 			   tick resolution and clock ticks period to set it as defined in the project instructions.
*/
void delay(void)
{
	// ~1/2 second delay providing Clock Module is set to tick every 500 microseconds
	// (i.e every 1/2 milisecond, rather than every 1 milisecond).
	__delay_cycles(1024000);
}

/*
 * Function: ledToggle
 * Description : make the correct led blinks.
 * Input:  uint8_t selectedPort - pass ports, uint16_t selectedPins - pass pins, int times - holds number
 * 		   of how many times the led will blink.
 * Output: void.
 * Algorithm:  A simple for loop that for each index places until times variable value multiply by 2 call
 * 			   delay, call toggle led output pin function with port pins arguments and delay again to
 * 			   create the wrtie timing for the led to blink.
*/
void ledToggle(uint8_t selectedPort, uint16_t selectedPins, int times) {
	int i = 0;
	for(i = 0 ; i < 2*times ; i = i + 1) {	// Run 2 times - 1 for turn off/on and 1 for turn on/off.
		delay(); // 1/2 sec delay, consuming CPU cycles.
		GPIO_toggleOutputOnPin(selectedPort, selectedPins);
		delay(); // 1/2 sec delay, consuming CPU cycles.
	}
	GPIO_setOutputLowOnPin(selectedPort, selectedPins); //Ensure the LED is turned off when finish blinking.
}

/*
 * Function: tsClockHandler
 * Description: Elapse every 1/2 miliseconds and yield the currently running task to simulate time sharing system. Doing short "jobs"
 * 				to release the system for external and internal responsivness, For that reason we are only doing yield to the current
 * 				running task/thread.
 * Input: void
 * Output: void
 * Algorithm: Use the function Task_yield() according to the settings provided by the clock module - period in clock ticks and etc.
*/
void tsClockHandler(void) {
	Task_yield();
}

/*
 * Function: initArray
 * Description : Initializes all the elements in a given array to -1.
 * Input:  an array arr of type Int and its size also Int.
 * Output: void
 * Algorithm:  A simple for loop that for each index places -1 in its value.
*/
void initArray(volatile Int *arr, Int size) {
	int i = 0;
	for(i = 0 ; i < size ; i++) {
		arr[i] = -1;
	}
}
