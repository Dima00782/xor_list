#ifndef _LINKED_LIEST_TEST_HPP_
#define _LINKED_LIEST_TEST_HPP_
#include "LinkedList.hpp"

/*
	There are some trivial tests for my LinkedList
	I know about GTest and other test frameworks but for this small project they are too expensive
*/

class LinkedListTest
{
public:
	static void run();
private:
	static void push_back_test();
	static void push_frond_test();
	static void pop_back_test();
	static void pop_frond_test();
	static void sort_test();
	static void splice_test();
	static void merge_test();
	static void insert_test();
	static void reverse_test();
	static void erase_test();
	static void assign_test();
	static void resize_test();
	static void unique_test();
	static void iterators_test();

private:
	static const LinkedList<int> must;
};
#endif /* _LINKED_LIEST_TEST_HPP_ */
