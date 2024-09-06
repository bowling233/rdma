#define _GNU_SOURCE
#include <x86gprintrin.h>
#define rdtsc() __rdtsc()
unsigned int ui;
#define rdtscp() __rdtscp(&ui)
// CPU Frequency: 2.5 GHz
#define CPU_CYCLE_TIME 0.4 // ns

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <sched.h>
#include "hdr/hdr_histogram.h"

inline double ticks_to_ns(unsigned long long ticks)
{
	double ns = ticks * CPU_CYCLE_TIME;
	return ns + 0.5;
}

// User-defined literals
auto constexpr operator""_B(unsigned long long int n) { return n; }
auto constexpr operator""_KB(unsigned long long int n) { return n * 1024; }
auto constexpr operator""_M(unsigned long long int n) { return n * 1000 * 1000; }

// Cache line size: 64 bytes for x86-64, 128 bytes for A64 ARMs
const auto kCachelineSize = 64_B;
// Memory page size. Default page size is 4 KB
const auto kPageSize = 4_KB;

// Singly linked list node with padding
struct ListNode
{
	ListNode *next;
	std::byte padding[kPageSize];
};
//
// Traverse the list of nodes.
//
// @param head
//   Pointer to the list head.
// @param num_ops
//   Number of operations to perform.
//
static auto traverse_list(ListNode *head, size_t num_ops)
{
	while (num_ops--)
		head = head->next;
	return head;
}

//
// Benchmark memory latency using a list.
//
// @param state.range(0)
//   Memory block size in KB to benchmark.
//
static void memory_latency_list(unsigned size)
{
	const auto mem_block_size = operator""_KB(size);
	// Each memory access fetches a cache line
	const auto num_nodes = mem_block_size / kCachelineSize;
	// std::cout << "Number of nodes: " << num_nodes << std::endl;
	assert(num_nodes > 0);

	// Allocate a contiguous list of nodes for an iteration
	std::vector<ListNode> list(num_nodes);
	// Make a cycle of the list nodes
	for (size_t i = 0; i < list.size() - 1; i++)
		list[i].next = &list[i + 1];
	list[list.size() - 1].next = &list[0];

	unsigned long long start, end;
	struct hdr_histogram *histogram;
	hdr_init(1, INT64_C(100000), 3, &histogram);

	const auto num_ops = 10000;
	for (int i = 0; i < 100; i++)
	{
		start = rdtsc();
		auto last_node = traverse_list(&list[0], num_ops);
		end = rdtsc();
		asm volatile("" : "+m,r"(last_node) : : "memory");
		// std::cout << "Memory latency: " << ticks_to_ns(end - start)/num_ops << " ns" << std::endl;
		hdr_record_value(histogram, (int64_t)(ticks_to_ns(end - start) / num_ops));
	}
	// std::cout << "Memory latency: " << ticks_to_ns(end - start)/num_ops << " ns" << std::endl;
	hdr_percentiles_print(histogram, stdout, 5, 1.0, CLASSIC);
}

int main(int argc, char *argv[])
{
	// check for argument
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <size in KB>" << std::endl;
		return 1;
	}

	// bind to core
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(1, &mask);
	sched_setaffinity(0, sizeof(mask), &mask);

	memory_latency_list(atoi(argv[1]));

	return 0;
}
