/**
 * Producer.h contains the class necessary in order to create a Producer object for the Producer-Consumer visualization.
 */
#ifndef PRODUCER_H_
#define PRODUCER_H_

#include <random>
#include <tsgl.h>
#include "Queue.h"
#include "PCThread.h"
using namespace tsgl;

/**
 * Producer class creates a Producer object and inherits from the Thread class.
 * Inheritance: Thread class.
 * Implements the abstract run() method from the Thread class so that the pthread runs the produce() method.
 */
class Producer : public PCThread {
public:
	Producer(); //Default constructor
	~Producer();
	Producer(Queue<Star*> & sharedBuffer, unsigned long id, Canvas & can);  //Size of data to generate	and id of pthread
	void wait();
	void lock();
	void act();
	void unlock();

private:
	Star* nextItem();
	ColorInt randColor();
};

#endif /* PRODUCER_H_ */
