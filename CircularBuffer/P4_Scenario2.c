/*
Name: Holly Hardin
DuckID: hollyh
Assignment: CIS 415 Project 2
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#define NUMTOPICS 1
#define MAXENTRIES 400
#define QUACKSIZE 140

// Referenced from Joe Sventek
// https://github.com/jsventek/ADTs/blob/master/src/tsbqueue.c
#define LOCK(topicqueue) &((topicqueue)->topic_lock)

struct topicentry
{
	int entrynum;
	struct timeval timestamp;
	int pubID;
	char message[QUACKSIZE];
};

struct topicqueue
{
	pthread_mutex_t topic_lock;
	struct topicentry circular_buffer[MAXENTRIES];
	int tail;
	int head;
	int topic_counter;
};

int enqueue(struct topicqueue *q, struct topicentry *e)
{
	// Lock the queue's mutex
	pthread_mutex_lock(LOCK(q));

	// If the queue is full, return -1
	if (((q->head + 1) % MAXENTRIES) == q->tail)
	{
		// Unlock the queue's mutex
		pthread_mutex_unlock(LOCK(q));

		return -1;
	}

	// Increase queue's topic counter
	q->topic_counter += 1;

	// Update the topic entry's variables
	e->entrynum = q->topic_counter;
	gettimeofday(&(e->timestamp), NULL);
	e->pubID = 0;

	// Insert topic entry into queue and update the queue's head
	q->circular_buffer[q->head] = *e;
	q->head = (q->head + 1) % MAXENTRIES;

	// Unlock the queue's mutex 
	pthread_mutex_unlock(LOCK(q));
	return 0;
}

int dequeue(struct topicqueue *q)
{
	// Lock the queue's mutex
	pthread_mutex_lock(LOCK(q));

	// If the queue is empty, return -1
	if (q->head == q->tail)
	{
		// Unlock the queue's mutex
		pthread_mutex_unlock(LOCK(q));

		// Failed to dequeue
		return -1;
	}
 
	struct timeval end;
	gettimeofday(&end, NULL);
	if ((end.tv_sec - q->circular_buffer[q->tail].timestamp.tv_sec) < 15)
	{
		// Unlock the queue's mutex
		pthread_mutex_unlock(LOCK(q));

		// Failed to dequeue
		return -1;
	}

	// Remove topic entry from queue and update the queue's tail
	q->tail = (q->tail + 1) % MAXENTRIES;

	// Unlock the queue's mutex
	pthread_mutex_unlock(LOCK(q));

	printf("Clean-up Thread 3 deleted topic entry 1\n");
	printf("Message was %lu old\n", end.tv_sec - q->circular_buffer[q->tail-1].timestamp.tv_sec);
	printf("Next entry added will be placed at index %d\n", q->head);
	printf("Next entry removed will come from index %d\n", q->tail);

	// Successfully dequeued
	return 1;
}

int get_entry(struct topicentry *t, struct topicqueue *q, int last_entry)
{
        if (q->head == q->tail)
        {
                return last_entry * -1;
        }

        return last_entry;
}

void print_topic_queue(struct topicqueue *q)
{
	// Print the topic's head and tail values
	printf("Next entry added will be placed at index %d\n", q->head);
	printf("Next entry removed will come from index %d\n", q->tail);
	printf("Highest topic number is %d\n", q->topic_counter);
	printf("Message said '%s'\n", q->circular_buffer[q->topic_counter-1].message);
	printf("Posted %lu\n", q->circular_buffer[q->topic_counter-1].timestamp.tv_sec);
	printf("Entry number %d\n", q->topic_counter);
}

void* pub(void *pub_info)
{
	// Create a pointer to the parameter
	struct topicqueue *q = pub_info;

	// Create topic entries
	struct topicentry t;
	int i;
	strcpy(t.message, "Season's Greetings");

	// Post 99 entries
	for (i = 0; i < 99; i++)
	{
		int indicator;
		indicator = enqueue(q, &t);
		while (indicator == -1)
		{
			indicator = enqueue(q, &t);
		}
		printf("Publisher Thread 1 posted new entry in topic %d\n", i);
		print_topic_queue(q);
	}

	// Sleep for 10 seconds
	sleep(10);

	// Post 5 entries
	for (i = 0; i < 5; i++)
	{
		int indicator;
		indicator = enqueue(q, &t);
		while (indicator == -1)
		{
			indicator = enqueue(q, &t);
		}
		printf("Publisher Thread 1 posted new entry in topic %d\n", i);
	}

	// Post 20 entries, 1 entry per second
	for (i = 0; i < 20; i++)
	{
		int indicator;
		indicator = enqueue(q, &t);
		while (indicator == -1)
		{
			indicator = enqueue(q, &t);
		}
		printf("Publisher Thread 1 posted new entry in topic %d\n", i);
		sleep(1);
	}

	return NULL;
}

void *sub(void *sub_info)
{
	// Create a pointer to the parameter
	struct topicqueue *q = sub_info;

	// Create a pointer to the entry
	struct topicentry *e = &(q->circular_buffer[0]);

	// Read 6 posts every second
	int i;
	int done = 0;
	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL);
	gettimeofday(&end, NULL);
	while (!done)
	{
		for (i = 0; i < 6; i++)
		{
			int indicator;
			indicator = get_entry(e, q, q->topic_counter);
			gettimeofday(&start, NULL);
			while ((indicator < 0) || (!done))
			{
				indicator = get_entry(e, q, q->topic_counter);
				gettimeofday(&end, NULL);
				if ((end.tv_sec - start.tv_sec) > 10)
				{
					done = 1;
				}
			}
			printf("Subscriber Thread 2 read an entry from topic %d\n", i);
			print_topic_queue(q);
		}
		sleep(1);
	}

	return NULL;
}

void *del(void *del_info)
{
	// Create a pointer to the parameter
	struct topicqueue *q = del_info;

	// Remove entries that are at least 15 seconds old
	int indicator;
	int counter = 0;
	while (1)
	{
		sleep(1);
		indicator = dequeue(q);
		if (indicator == 1)
		{
			counter = 0;
		}
		else
		{
			counter++;
			if (counter >= 30)
			{
				break;
			}
		}
	}

	return NULL;
}

int main()
{
	// Initialize queue
	struct topicqueue q[NUMTOPICS];
	int i;
	for (i = 0; i < NUMTOPICS; i++)
	{
		q[i].head = 0;
		q[i].tail = 0;
		q[i].topic_counter = 0;
	}

	// Initialize the queue's mutex locks
	for (i = 0; i < NUMTOPICS; i++)
	{
		if (pthread_mutex_init(&(q[i].topic_lock), NULL) != 0)
		{
			printf("mutex init failed\n");
		}
	}

	// Thread variables
	int num_pubs = 4;
	int error;
	pthread_t publisher[num_pubs];
	int num_subs = 5;
	int error2;
	pthread_t subscriber[num_subs];
	int error3;
	pthread_t delete;

	// Create the threads
	for (i = 0; i < num_pubs; i++)
	{
		error = pthread_create(&publisher[i], NULL, &pub, q);
		if (error != 0)
		{
			printf("Thread can't be created\n");
		}
	}
	for (i = 0; i < num_subs; i++)
	{
		error2 = pthread_create(&subscriber[i], NULL, &sub, q);
		if (error2 != 0)
		{
			printf("Thread can't be created\n");
		}
	}
	error3 = pthread_create(&delete, NULL, &del, q);
	if (error3 != 0)
	{
		printf("Thread can't be created\n");
	}

	// Wait for the threads to finish
	for (i = 0; i < num_pubs; i++)
	{
		pthread_join(publisher[i], NULL);
	}
	for (i = 0; i < num_subs; i++)
	{
		pthread_join(subscriber[i], NULL);
	}
	pthread_join(delete, NULL);

	// Destory the queue's mutex locks
	for (i = 0; i < NUMTOPICS; i++)
	{
		pthread_mutex_destroy(&(q->topic_lock));
	}

	return 0;
}
