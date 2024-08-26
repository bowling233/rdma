#define _GNU_SOURCE
// CPU Frequency: 2.5 GHz
#define CPU_CYCLE_TIME 0.4 // ns

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include "hdr/hdr_histogram.h"

unsigned long long rdtsc()
{
	unsigned int lo, hi;
	__asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
	return ((unsigned long long)hi << 32) | lo;
}

int64_t ticks_to_ns(unsigned long long ticks)
{
	double ns = ticks * CPU_CYCLE_TIME;
	return (int64_t)(ns + 0.5);
}

#ifndef NODE_PADDING
#define NODE_PADDING 0
#endif
typedef struct ListNode ListNode;

struct ListNode
{
	ListNode *next;
	int8_t padding[NODE_PADDING];
};

ListNode * prepare_list(int size)
{
	ListNode *nodes = (ListNode *)malloc(sizeof(ListNode) * size);
	if (nodes == NULL)
	{
		perror("malloc");
		return NULL;
	}

	for (int i = 0; i < size; i++)
	{
		nodes[i].next = &nodes[(i + 1) % size];
	}

	return nodes;
}

int main(void)
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(1, &mask);
	sched_setaffinity(0, sizeof(mask), &mask);

	fprintf(stderr, "Node padding: %d\n", NODE_PADDING);

	ListNode *nodes = prepare_list(UINT16_MAX);
	ListNode *current = nodes;

	unsigned long long start, end;

	struct hdr_histogram *histogram;
	hdr_init(1, INT64_C(3600000000), 3, &histogram);

	for (unsigned long long i = 0; i < UINT16_MAX; i++)
	{
		start = rdtsc();
		for(int j = 0; j < UINT8_MAX; j++)
		{
			current = current->next;
		}
		end = rdtsc();
		hdr_record_value(histogram, ticks_to_ns(end - start));
	}

	hdr_percentiles_print(histogram, stdout, 5, 1.0, CLASSIC);

	return 0;
}
