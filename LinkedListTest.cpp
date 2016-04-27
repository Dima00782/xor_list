#include "LinkedListTest.hpp"
#include <cassert>
#include <algorithm>
#include <iterator>
#include <iostream>

namespace
{
	template <class T>
	bool equal(const LinkedList<T>& a, const LinkedList<T>& b)
	{
		if (a.size() != b.size())
		{
			return false;
		}

		auto i = a.begin();
		auto j = b.begin();
		for (; i != a.end();)
		{
			if (*i != *j)
			{
				return false;
			}

			++i;
			++j;
		}

		return true;
	}
}

const LinkedList<int> LinkedListTest::must = { 1, 2, 3 };

void LinkedListTest::run()
{
	push_back_test();
	push_frond_test();
	pop_back_test();
	pop_frond_test();
	sort_test();
	splice_test();
	merge_test();
	insert_test();
	reverse_test();
	erase_test();
	assign_test();
	resize_test();
	unique_test();
	iterators_test();
	std::cout << "All test passed" << std::endl;
}

void LinkedListTest::push_back_test()
{
	LinkedList<int> list;
	list.push_back(1);
	list.push_back(2);
	list.push_back(3);

	assert(equal(list, must));
}

void LinkedListTest::push_frond_test()
{
	const LinkedList<int> must = { 3, 2, 1 };

	LinkedList<int> list;
	list.push_front(1);
	list.push_front(2);
	list.push_front(3);

	assert(equal(list, must));
}

void LinkedListTest::pop_back_test()
{
	LinkedList<int> list = {1, 2, 3, 4, 5};
	list.pop_back();
	list.pop_back();

	assert(equal(list, must));
}

void LinkedListTest::pop_frond_test()
{
	LinkedList<int> list = { -1, 0, 1, 2, 3 };
	list.pop_front();
	list.pop_front();

	assert(equal(list, must));
}

void LinkedListTest::sort_test()
{
	// check all permutatuion on three elements
	int values[] = { 1, 2, 3 };

	LinkedList<int> list = { 1, 2, 3 };
	do {
		list.assign({ values[0], values[1], values[2] });
		list.sort();
		assert(equal(list, must));
	} while (std::next_permutation(values, values + 3));	
}

void LinkedListTest::splice_test()
{
	const LinkedList<int> must = { 2, 4, 6, 8, 1, 3, 5, 7 };
	LinkedList<int> l1 = { 1, 3, 5, 7 };
	LinkedList<int> l2 = { 2, 4, 6, 8 };

	l1.splice(l1.begin(), l2);
	assert(equal(l1, must));
}

void LinkedListTest::merge_test()
{
	const LinkedList<int> must = { 1, 2, 3, 4, 5, 6, 7, 8 };

	LinkedList<int> l1 = { 1, 3, 5, 7 };
	LinkedList<int> l2 = { 2, 4, 6, 8 };
	l1.merge(l2);
	assert(equal(l1, must));
}

void LinkedListTest::insert_test()
{
	const LinkedList<int> reverse = { 3, 2, 1 };

	LinkedList<int> list;
	list.insert(list.begin(), reverse.begin(), reverse.end());

	assert(equal(list, must));
}

void LinkedListTest::reverse_test()
{
	const LinkedList<int> must_odd = { 1, 2, 3, 4, 5 };
	const LinkedList<int> must_even = { 1, 2, 3, 4 };

	LinkedList<int> list = { 5, 4, 3, 2, 1 };
	list.reverse();
	assert(equal(list, must_odd));

	list.assign({ 4, 3, 2, 1 });
	list.reverse();
	assert(equal(list, must_even));
}

void LinkedListTest::erase_test()
{
	LinkedList<int> list = { -1, 0, 1, 2, 3 };
	auto i = list.begin();
	auto j = i;
	std::advance(j, 2);
	list.erase(i,  j);

	assert(equal(list, must));
}

void LinkedListTest::assign_test()
{
	LinkedList<int> list;
	list.assign({ 1, 2, 3 });
	assert(equal(list, must));
}

void LinkedListTest::resize_test()
{
	LinkedList<int> list = { 1, 2, 3, 4, 5, 6, 7 };
	list.resize(3);
	assert(equal(list, must));

	list.assign({1, 2});
	list.resize(3, 3);
	assert(equal(list, must));
}

void LinkedListTest::unique_test()
{
	LinkedList<int> list = { 1, 1, 1, 2, 2, 3, 3, 3 };
	list.unique();

	assert(equal(list, must));
}

void LinkedListTest::iterators_test()
{
	LinkedList<int> list = { 10, 20, 30 };
	for (auto& x : list)
	{
		x /= 10;
	}

	assert(equal(list, must));
}
