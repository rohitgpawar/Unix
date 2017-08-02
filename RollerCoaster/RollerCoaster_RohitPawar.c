/*
-PROGRAM: RollerCoaster.c
-DESCRIPTION: 'N' passengers waiting for a car. 'C' passengers board the car take a ride and unboard the car.
-Last Revised - August/02/2017
-AUTHOR- Rohit Pawar
*/


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define Car_Capacity	4

pthread_mutex_t rollerCoasterMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t boardQueue = PTHREAD_COND_INITIALIZER;
pthread_cond_t unBoardQueue = PTHREAD_COND_INITIALIZER;
pthread_cond_t allAboard = PTHREAD_COND_INITIALIZER;
pthread_cond_t allAshore = PTHREAD_COND_INITIALIZER;


int passengerOnBoardCount = 0, waitingPassengers = 0, maxPassengerInCar = 1, rear = 0, totalPassengers = 1;
long carQueue[Car_Capacity];

//Animates a car when Ride Starts
void TakeRide()
{
	int travel = 0;
	int carDistance = 0;


	while (travel < 3)
	{// To print the car driving right
		printf("\n");
		carDistance = 0;

		while (carDistance < travel)
		{
			printf("\t");
			carDistance++;
		}
		printf(" ____|~\\_");
		printf("\n");
		carDistance = 0;

		while (carDistance < travel)
		{
			printf("\t");
			carDistance++;
		}
		printf("[_ __|_|-]");
		printf("\n");
		carDistance = 0;

		while (carDistance < travel)
		{
			printf("\t");
			carDistance++;
		}
		printf(" (_)   (_)\n");
		sleep(1);
		travel++;
	}


	while (travel > -1)
	{// To print the car driving left
		carDistance = 0;
		printf("\n");

		while (carDistance < travel)
		{
			printf("\t");
			carDistance++;
		}
		printf(" _/~|____");
		printf("\n");
		carDistance = 0;

		while (carDistance < travel)
		{
			printf("\t");
			carDistance++;
		}
		printf("[-|_|_  _]");
		printf("\n");
		carDistance = 0;

		while (carDistance < travel)
		{
			printf("\t");
			carDistance++;
		}
		printf(" (_)   (_)\n");
		sleep(1);
		travel--;
	}

	printf("\n");
}

// Returns true when Car is full
int Carfull()
{
	//Checks if passengerOnBoardCount == maxPassengerInCar or == to max Car_Capacity.
	if (passengerOnBoardCount == Car_Capacity || passengerOnBoardCount == maxPassengerInCar)
		return 1;

	return 0;
}

//Function called when Passenger is boarding the Car, takes passengerThreadNumber as paramater and stores in an array. Prints which passenger is boarding Car.
void BoardCar(long passengerThreadNumber)
{
	//Add new passengerThreadNumber to carQueue as he has boareded the Car.
	carQueue[rear] = passengerThreadNumber;
	rear++; //increment rear to  store next passenger
	passengerOnBoardCount++; //increment passengerOnBoardCount.

	waitingPassengers--; //decrement waiting passenger count.
	printf("\n\t\t\t\tPassenger %ld is on board.\n", passengerThreadNumber); //Print who is boarding on Car.

	if (Carfull())
	{//If car is full signal allAboard.
		pthread_cond_signal(&allAboard);
	}
}

//Function called when Passenger is unBoarding the Car, takes passengerThreadNumber as parameter and prints which passenger is unBoarding.
void UnBoardCar(long passengerThreadNumber)
{
	rear--; // decrement rear to get the last inseter value from carQueue array.

	printf("\nPassenger %ld is Ashore.\n", carQueue[rear]);//Print passenger being unboarded.
	passengerOnBoardCount--;
	if (rear == 0)
	{// If all are unboarded signal allAshore.
		pthread_cond_signal(&allAshore);
	}
}

//Function that deals with operations that Passenger can perform.
void *Passenger(void *threadId)
{
	long threadnumber;
	threadnumber = (long)threadId;

	//Lock Mutex
	pthread_mutex_lock(&rollerCoasterMutex);
	waitingPassengers++;
	printf("\nPassenger %ld waiting...\n", threadnumber);

	//Wait for boardQueue Signal.
	pthread_cond_wait(&boardQueue, &rollerCoasterMutex);

	//call BoardCar after receiveing boardQueue Signal
	BoardCar(threadnumber);

	//Wait for unBoardQueue Signal.
	pthread_cond_wait(&unBoardQueue, &rollerCoasterMutex);

	//call UnBoardCar after receiveing unBoardQueue Signal
	UnBoardCar(threadnumber);

	//Unlock Mutex.
	pthread_mutex_unlock(&rollerCoasterMutex);
	pthread_exit(NULL);
}

//Function to perform operations for Car Load.
void CarLoad(long threadnumber)
{
	int i = 0;
	for (i = 0; i < maxPassengerInCar; i++)
	{
		//Lock Mutex.
		pthread_mutex_lock(&rollerCoasterMutex);

		//Send boardQueue Signal.
		pthread_cond_signal(&boardQueue);

		//UnLock Mutex.
		pthread_mutex_unlock(&rollerCoasterMutex);
		sleep(1);
	}
}

//Function to perform operations for Car UnLoad.
void CarUnLoad(long threadnumber)
{
	int i = 0;
	for (i = 0; i < maxPassengerInCar; i++)
	{
		//Lock Mutex.
		pthread_mutex_lock(&rollerCoasterMutex);

		//Send unBoardQueue Signal.
		pthread_cond_signal(&unBoardQueue);

		//UnLock Mutex.
		pthread_mutex_unlock(&rollerCoasterMutex);
		sleep(1);
	}
}

//Function to perform operations for Car Run.
void *CarRun(void *threadId)
{
	while (waitingPassengers > 0)
	{// While even a single passenger is wating for car we need to service him.
		long threadnumber = (long)threadId;
		while (waitingPassengers < maxPassengerInCar)
		{
			sleep(1);
		}
		printf("\n\n!! Car has arrived lets start boarding now !! \n\n");
		sleep(2);

		if (waitingPassengers >= maxPassengerInCar)
		{//If waitingPassengers is enough i.e. more than or eual to maxPassengerInCar then Load start loading car.
			CarLoad(threadnumber);
		}

		//Lock Mutex
		pthread_mutex_lock(&rollerCoasterMutex);
		while (!Carfull())
			pthread_cond_wait(&allAboard, &rollerCoasterMutex); //Wait for the signal of allAboard

																//Unlock Mutex
		pthread_mutex_unlock(&rollerCoasterMutex); //
		printf("\n\n!! Car is full. Lets go for a ride with %d passengers. !!\n\n", maxPassengerInCar);
		sleep(1);
		TakeRide();
		printf("\n\n!! Ride completed lets start unboarding !! \n\n");
		sleep(2);

		//Start Unloading car at this point.
		CarUnLoad(threadnumber);

		//Lock Mutex
		pthread_mutex_lock(&rollerCoasterMutex);
		while (rear > 0)
			pthread_cond_wait(&allAshore, &rollerCoasterMutex);// Wait for allAshore signal.

															   //Unlock Mutex
		pthread_mutex_unlock(&rollerCoasterMutex);

	}
	pthread_exit(NULL);
}



int main()
{
	//Preset Message.
	printf("\n********************************************************************************************************************************************\n");
	printf("\nWelcome to RollerCoaster. Please select maximum number of passengers in multiple of 4 as car capacity for this ride is set to 4 passengers.\nRide will only sart when Car is full.We dont want to leave anyone unplayed so please cooporate and enter passangers in multiple of 4.\n");
	printf("\n********************************************************************************************************************************************\n");

	//Enter Car Capacity
	printf("\nEnter Car Capacity: ");
	scanf("%d", &maxPassengerInCar); // Store car capacity in maxPassengerInCar
	if (maxPassengerInCar < 0)
		printf("\n\n!!!!!!!\tCar capacity cannot be negative. Please enter a valid Car Capacity. !!!!!!!\n\n");
	
	
	while ((totalPassengers % maxPassengerInCar) != 0)
	{//makes sure entered totalPassenger number is multiple of maxPassengerInCar.
		printf("\nEnter total number of Passengers: ");
		scanf("%d", &totalPassengers);
		if ((totalPassengers % maxPassengerInCar) != 0)
			printf("\n\n!!!!!!!\tPlease enter Passengers in multiple of maxPassengerInCar (%d). \n!!!!!!!\tWe don't want anyone left behind do We.?\n\n",maxPassengerInCar);
	}


	pthread_t threads[totalPassengers + 1]; // creates array of threads.
	long passengerCount;


	for (passengerCount = 0; passengerCount < totalPassengers; passengerCount++) {
		//Create all Passenger threads.
		if (pthread_create(&threads[passengerCount], NULL, Passenger, (void *)passengerCount + 1))
		{
			printf("Error occured while creating Passenger thread %ld", passengerCount + 1);
			exit(-1);
		}
	}

	sleep(1);
	//Create Car thread.(Single thread)
	pthread_create(&threads[passengerCount], NULL, CarRun, (void *)passengerCount + 1);


	//Join the threads to main thread to start execution.
	for (passengerCount = 0; passengerCount < totalPassengers + 1; passengerCount++) {
		pthread_join(threads[passengerCount], NULL);
	}

	printf("\n!!!!!!!!!!!!!!!! HOPE YOU HAD A GOOD RIDE. PLEASE DO VISIT AGAIN !!!!!!!!!!!!\n\n");
	pthread_exit(NULL);
}