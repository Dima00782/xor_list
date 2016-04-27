#include "LinkedList.hpp"
#include <memory>
#include <utility>
#include <cassert>
#include <functional>
#include <iostream>

namespace
{
	template <class T>
	Node<T>* find_previous(Node<T>* const start, Node<T>* const target)
	{
		intptr_t previous = 0;
		auto current = start;
		while (current != target)
		{
			Node<T>* tmp = current;
			current = reinterpret_cast<Node<T>*>(previous ^ current->ptrdiff);
			previous = reinterpret_cast<intptr_t>(tmp);
		}

		return reinterpret_cast<Node<T>*>(previous);
	}
	
	template <class T>
	Node<T>* get_next(const Node<T>* const previous, const intptr_t ptrdiff)
	{
		return reinterpret_cast<Node<T>*>(reinterpret_cast<intptr_t>(previous) ^ ptrdiff);
	}

	template <class T>
	void do_next(Node<T>** const previous, Node<T>** const current)
	{
		auto tmp = *current;
		*current = get_next(*previous, (*current)->ptrdiff);
		*previous = tmp;
	}

	template <class T, class Compare>
	Node<T>* partition(
		const Compare& comp,
		Node<T>* const first,
		Node<T>* const first_previous,
		Node<T>* const last)
	{
		auto pivot = last->data;
		auto i = first_previous;
		auto i_previous = i == nullptr ? nullptr : get_next(first, first_previous->ptrdiff);

		auto j = first;
		auto j_previous = first_previous;

		while (j != last)
		{
			if (comp(j->data, pivot) || j->data == pivot)
			{
				if (nullptr == i)
				{
					i = first;
					i_previous = first_previous;
				}
				else
				{
					do_next(&i_previous, &i);
				}
				std::swap(i->data, j->data);
			}

			do_next(&j_previous, &j);
		}

		if (nullptr == i)
		{
			i = first;
			i_previous = first_previous;
		}
		else
		{
			do_next(&i_previous, &i);
		}
		std::swap(i->data, last->data);

		return i;
	}
}


template <class T, class TAllocator>
LinkedList<T, TAllocator>::LinkedList(const std::size_t n, const_reference val, const node_allocator_type& alloc) : LinkedList(alloc)
{
	for (std::size_t i = 0; i < n; ++i) { push_back(val); }
}

template <class T, class TAllocator>
LinkedList<T, TAllocator>::LinkedList(const LinkedList<T, TAllocator>& other)
	: head(other.head)
	, tail(other.tail)
	, _size(other._size)
	, allocator(other.allocator)
{}

template <class T, class TAllocator>
LinkedList<T, TAllocator>::LinkedList(LinkedList<T, TAllocator>&& other)
	: head(other.head)
	, tail(other.tail)
	, _size(other._size)
	, allocator(std::move(other.allocator))
{
	other.head = other.tail = nullptr;
	other._size = 0;
}

template <class T, class TAllocator>
LinkedList<T, TAllocator>::~LinkedList() { clear(); }

template <class T, class TAllocator>
LinkedList<T, TAllocator>& LinkedList<T, TAllocator>::operator=(const LinkedList<T, TAllocator>& right)
{
	if (&right != this)
	{
		LinkedList<T, TAllocator> tmp(right);
		swap(tmp);
	}

	return *this;
}

template <class T>
ConstLinkedListIterator<T>& ConstLinkedListIterator<T>::operator++()
{
	assert(ptr != nullptr);
	do_next(&previous, &ptr);
	return *this;
}

template <class T>
ConstLinkedListIterator<T> ConstLinkedListIterator<T>::operator++(int)
{
	ConstLinkedListIterator tmp(*this);
	++(*this);
	return tmp;
}

template <class T>
ConstLinkedListIterator<T>& ConstLinkedListIterator<T>::operator--()
{
	assert(ptr != nullptr);
	do_next(&ptr, &previous);
	return *this;
}

template <class T>
ConstLinkedListIterator<T> ConstLinkedListIterator<T>::operator--(int)
{
	ConstLinkedListIterator tmp(*this);
	--(*this);
	return tmp;
}

template <class T, class TAllocator>
LinkedList<T, TAllocator>::LinkedList(const node_allocator_type& alloc)
	: head(nullptr)
	, tail(nullptr)
	, _size(0)
	, allocator(alloc)
{}

template <class T, class TAllocator>
LinkedList<T, TAllocator>::LinkedList(std::initializer_list<value_type> il, const allocator_type& alloc)
	: LinkedList<T, TAllocator>(alloc)
{
	for (auto it = il.begin(); it != il.end(); ++it)
	{
		auto node = create_node_in_tail();
		node->data = *it;
	}
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::swap(LinkedList<T, TAllocator>& other)
{
	std::swap(other._size, _size);
	std::swap(other.allocator, allocator);
	std::swap(other.head, head);
	std::swap(other.tail, tail);
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::push_back(const_reference data)
{
	auto node = create_node_in_tail();
	node->data = data;
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::push_back(T&& data)
{
	auto node = create_node_in_tail();
	node->data = std::move(data);
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::push_front(const_reference data)
{
	auto node = create_node_in_head();
	node->data = data;
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::push_front(T&& data)
{
	auto node = create_node_in_head();
	node->data = std::move(data);
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::pop_front() { pop(&head); }

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::pop_back() { pop(&tail); }

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::clear()
{
	if (nullptr == head)
	{
		return;
	}

	Node<T>* previous = nullptr;
	auto i = head;
	while (nullptr != i)
	{
		auto next = get_next(previous, i->ptrdiff);
		previous = i;
		allocator.destroy(i);
		allocator.deallocate(i, 1);
		i = next;
	}
	_size = 0;
	head = tail = nullptr;
}

template <class T, class TAllocator>
Node<T>* LinkedList<T, TAllocator>::create_node_in_tail()
{
	auto node = create_node();
	if (tail != node)
	{
        insert_after(tail, get_next(static_cast<Node<T>*>(nullptr), tail->ptrdiff), node);
	}
	++_size;

	return node;
}

template <class T, class TAllocator>
Node<T>* LinkedList<T, TAllocator>::create_node()
{
	auto new_node = allocator.allocate(1);
	allocator.construct(new_node);

	new_node->ptrdiff = 0;

	if (head == nullptr)
	{
		head = new_node;
		tail = head;
	}

	return new_node;
}

template <class T, class TAllocator>
Node<T>* LinkedList<T, TAllocator>::create_node_in_head()
{
	auto node = create_node();
	if (head != node)
	{
		insert_before(head, nullptr, node);
	}
	++_size;

	return node;
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::pop(Node<T>** const node)
{
	assert(*node == head || *node == tail);
	assert(*node != nullptr);

	--_size;
	allocator.destroy(*node);
	auto previous = get_next(static_cast<Node<T>*>(nullptr), (*node)->ptrdiff);
	if (nullptr == previous)
	{
		allocator.deallocate(*node, 1);
		head = tail = nullptr;
		return;
	}

	previous->ptrdiff ^= reinterpret_cast<intptr_t>(*node);
	allocator.deallocate(*node, 1);
	*node = previous;
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::insert_before(Node<T>* const pos, Node<T>* const previous, Node<T>* const node)
{
	if (pos == nullptr)
	{
		insert_after(tail, get_next(pos, tail->ptrdiff), node);
		return;
	}

	assert(node != nullptr);
	assert(node != pos);

	Node<T>* next = reinterpret_cast<Node<T>*>(pos->ptrdiff ^ reinterpret_cast<intptr_t>(previous));
	node->ptrdiff = reinterpret_cast<intptr_t>(previous) ^ reinterpret_cast<intptr_t>(pos);
	pos->ptrdiff = reinterpret_cast<intptr_t>(node) ^ reinterpret_cast<intptr_t>(next);
	if (nullptr != previous)
	{
		previous->ptrdiff ^= reinterpret_cast<intptr_t>(pos) ^ reinterpret_cast<intptr_t>(node);
	}

	if (pos == head)
	{
		head = node;
	}
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::insert_after(Node<T>* const pos, Node<T>* const previous, Node<T>* const node)
{
	assert(pos != nullptr);
	assert(node != nullptr);
	assert(node != pos);

	auto next = get_next(previous, pos->ptrdiff);
	node->ptrdiff = reinterpret_cast<intptr_t>(pos) ^ reinterpret_cast<intptr_t>(next);
	pos->ptrdiff = reinterpret_cast<intptr_t>(previous) ^ reinterpret_cast<intptr_t>(node);
	if (nullptr != next)
	{
		next->ptrdiff ^= reinterpret_cast<intptr_t>(pos) ^ reinterpret_cast<intptr_t>(node);
	}

	if (pos == tail)
	{
		tail = node;
	}
}

template <class T, class TAllocator>
Node<T>* LinkedList<T, TAllocator>::unlink(Node<T>* const pos, Node<T>* const previous)
{
	assert(pos != nullptr);

	auto next = get_next(previous, pos->ptrdiff);
	pos->ptrdiff = 0;

	if (previous != nullptr)
	{
		previous->ptrdiff ^= reinterpret_cast<intptr_t>(pos) ^ reinterpret_cast<intptr_t>(next);
	}

	if (next != nullptr)
	{
		next->ptrdiff ^= reinterpret_cast<intptr_t>(pos) ^ reinterpret_cast<intptr_t>(previous);
	}

	if (pos == head)
	{
		head = next;
	}
	if (pos == tail)
	{
		tail = previous;
	}

	return next;
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::sort() noexcept { sort(std::less<T>()); }

template <class T, class TAllocator>
template <class Compare>
void LinkedList<T, TAllocator>::sort(Compare comp) noexcept
{
	Node<T>* null = nullptr;
	sort_range(comp, head, null, tail, get_next(null, tail->ptrdiff));
}

template <class T, class TAllocator>
template <class Compare>
void LinkedList<T, TAllocator>::sort_range(
	const Compare& comp,
	Node<T>* const first,
	Node<T>* const first_previous,
	Node<T>* const last,
	Node<T>* const last_previous) noexcept
{
	auto last_next = (nullptr == last) ? nullptr : get_next(last_previous, last->ptrdiff);
	if (last != nullptr && first != last && first != last_next)
	{
		auto p = partition(comp, first, first_previous, last);
		auto p_previous = find_previous(head, p);
		auto p_next = get_next(p_previous, p->ptrdiff);
		auto p_previous_previous = (nullptr != p_previous) ? get_next(p, p_previous->ptrdiff) : nullptr;

		sort_range(comp, first, first_previous, p_previous, p_previous_previous);
		sort_range(comp, p_next, p, last, last_previous);
	}
}

template <class T, class TAllocator>
typename LinkedList<T, TAllocator>::iterator LinkedList<T, TAllocator>::insert(const_iterator position, const_reference val)
{
	auto node = create_node();
	if (node != head)
	{
		insert_before(position.ptr, position.previous, node);
	}
	node->data = val;
	++_size;
	return iterator(node, find_previous(head, node));
}

template <class T, class TAllocator>
template <class InputIterator>
typename LinkedList<T, TAllocator>::iterator LinkedList<T, TAllocator>::insert(
	const_iterator position, InputIterator first, InputIterator last)
{
	for (auto i = first; i != last; ++i)
	{
		position = insert(position, *i);
	}

	return iterator(position.ptr, position.previous);
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::reverse() noexcept
{
	Node<T>* const null = nullptr;
	auto first = head;
	Node<T>* first_previous = null;
	auto last = tail;
	Node<T>* last_previous = get_next(null, last->ptrdiff);

	while (first != last && first != nullptr && get_next(first_previous, first->ptrdiff) != last)
	{
		std::swap(first->data, last->data);
		do_next(&first_previous, &first);
		do_next(&last, &last_previous);
	}

	if (first != nullptr && get_next(first_previous, first->ptrdiff) == last)
	{
		std::swap(first->data, last->data);
	}
}

template <class T, class TAllocator>
typename LinkedList<T, TAllocator>::iterator LinkedList<T, TAllocator>::erase(const_iterator position)
{
	auto ptr = position.ptr;
	assert(ptr != nullptr);

	Node<T>* next = nullptr;
	const Node<T>* null = nullptr;
	if (head == ptr)
	{
		next = get_next(null, head->ptrdiff);
		next->ptrdiff ^= reinterpret_cast<intptr_t>(head);
		head = next;
	}
	else if (tail == ptr)
	{
		auto previous = get_next(null, tail->ptrdiff);
		previous->ptrdiff ^= reinterpret_cast<intptr_t>(tail);
		tail = previous;
	}
	else
	{
		next = unlink(ptr, find_previous(head, ptr));
	}
	allocator.destroy(ptr);
	allocator.deallocate(ptr, 1);
	--_size;

	return iterator(next, find_previous(head, next));
}

template <class T, class TAllocator>
typename LinkedList<T, TAllocator>::iterator LinkedList<T, TAllocator>::erase(const_iterator first, const_iterator last)
{
	if (first == begin() && last == end())
	{
		clear();
		return end();
	}

	while (first != last)
	{
		first = erase(first);
	}

	return iterator(last.ptr, last.previous);
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::resize(size_type n, const_reference val)
{
	if (_size < n)
	{
		while (_size != n)
		{
			push_back(val);
		}
	}
	else
	{
		while (_size != n)
		{
			pop_back();
		}
	}
}

template <class T, class TAllocator>
template <class InputIterator>
void LinkedList<T, TAllocator>::assign(InputIterator first, InputIterator last)
{
	clear();
	for (auto it = first; it != last; ++it)
	{
		push_back(*it);
	}
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::assign(size_type n, const_reference val)
{
	clear();
	for (size_type i = 0; i < n; ++i)
	{
		push_back(val);
	}
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::assign(std::initializer_list<value_type> il)
{
	assign(il.begin(), il.end());
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::splice(const_iterator position, LinkedList& x) noexcept
{
	if (this == &x || x.empty())
	{
		return;
	}

	splice(position, x, x.begin(), x.end());
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::splice(const_iterator position, LinkedList& x, const_iterator i) noexcept
{
	auto target = i.ptr;
	x.unlink(target, i.previous);
	insert_before(position.ptr, position.previous, target);
	--x._size;
	++_size;
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::splice(const_iterator position, LinkedList& x, const_iterator first, const_iterator last) noexcept
{
	auto i = first.ptr;
	auto i_previous = first.previous;
	while (i != last.ptr)
	{
		auto next = x.unlink(i, i_previous);
		insert_before(position.ptr, position.previous, i);
		position.previous = i;
		i = next;
		--x._size;
		++_size;
	}
}

template <class T, class TAllocator>
template <class BinaryPredicate>
void LinkedList<T, TAllocator>::unique(BinaryPredicate binary_pred)
{
	Node<T>* null = nullptr;

	if (nullptr == head || get_next(null, head->ptrdiff) == nullptr)
	{
		return;
	}

	auto i_previous = null;
	for (auto i = head; i != nullptr;)
	{
		auto next = get_next(i_previous, i->ptrdiff);
		while (next != nullptr && binary_pred(i->data, next->data))
		{
			unlink(next, i);
            allocator.destroy(next);
            allocator.deallocate(next, 1);
			--_size;
			next = get_next(i_previous, i->ptrdiff);
		}

		if (i != nullptr)
		{
			i_previous = i;
			i = next;
		}
	}
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::unique() { unique(std::equal_to<T>()); }

template <class T, class TAllocator>
template <class Compare>
void LinkedList<T, TAllocator>::merge(LinkedList& x, Compare comp) noexcept
{
	if (this == &x)
	{
		return;
	}

	if (empty())
	{
		swap(x);
		return;
	}

	Node<T>* null = nullptr;

	auto i_previous = null;
	auto i = head;

	auto j_previous = null;
	auto j = x.head;
	while (j != nullptr && i != nullptr)
	{
		if (comp(i->data, j->data))
		{
			do_next(&i_previous, &i);
		}
		else
		{
			auto next = x.unlink(j, j_previous);
			insert_before(i, i_previous, j);
			i_previous = j;
			j = next;
			--x._size;
			++_size;
		}
	}

	if (i == nullptr)
	{
		splice(const_iterator(i, i_previous), x, const_iterator(j, j_previous), x.end());
	}
}

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::merge(LinkedList& x) noexcept { merge(x, std::less<T>()); }

template <class T, class TAllocator>
void LinkedList<T, TAllocator>::resize(size_type n) { resize(n, T()); }

template <class T>
ConstLinkedListIterator<T>& ConstLinkedListIterator<T>::operator=(const ConstLinkedListIterator<T>& other)
{
	// we don't have to check assignment to itself
	// in any case we just assign pointers
	ptr = other.ptr;
	previous = other.previous;
	return *this;
}

template <class T, class TAllocator>
typename LinkedList<T, TAllocator>::reference LinkedList<T, TAllocator>::back() noexcept { return tail->data; }

template <class T, class TAllocator>
typename LinkedList<T, TAllocator>::const_reference LinkedList<T, TAllocator>::back() const noexcept { return tail->data; }

template <class T, class TAllocator>
typename LinkedList<T, TAllocator>::reference LinkedList<T, TAllocator>::front() noexcept { return head->data; }

template <class T, class TAllocator>
typename LinkedList<T, TAllocator>::const_reference LinkedList<T, TAllocator>::front() const noexcept { return head->data; }

template <class T, class TAllocator>
typename LinkedList<T, TAllocator>::iterator LinkedList<T, TAllocator>::begin() noexcept { return iterator(head, nullptr); }

template <class T, class TAllocator>
typename LinkedList<T, TAllocator>::const_iterator LinkedList<T, TAllocator>::begin() const noexcept { return const_iterator(head, nullptr); }

template <class T, class TAllocator>
typename LinkedList<T, TAllocator>::iterator LinkedList<T, TAllocator>::end() noexcept { return iterator(nullptr, tail); }

template <class T, class TAllocator>
typename LinkedList<T, TAllocator>::const_iterator LinkedList<T, TAllocator>::end() const noexcept { return const_iterator(nullptr, tail); }

template <class T, class TAllocator>
typename LinkedList<T, TAllocator>::size_type LinkedList<T, TAllocator>::size() const noexcept { return _size; }

template <class T, class TAllocator>
bool LinkedList<T, TAllocator>::empty() const noexcept { return _size == 0; }
