#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <cstddef>
#include <memory>
#include <iterator>
#include <initializer_list>
#include <cstdint>

namespace
{
	// see
	// https://en.wikipedia.org/wiki/XOR_linked_list
	// for implementation details

	template <class T>
	struct Node
	{
		T data;
		intptr_t ptrdiff; /* XOR of next and previous node */
	};
}

template < class T, class TAllocator = std::allocator<T> >
class LinkedList;

template <class T>
class ConstLinkedListIterator : public std::iterator<std::bidirectional_iterator_tag, T>
{
public:
	using const_reference = const T&;
	using const_pointer = const T*;

public:
	friend LinkedList<T>;

	ConstLinkedListIterator()
		: previous(nullptr)
		, ptr(nullptr)
	{}

	explicit ConstLinkedListIterator(Node<T>* _ptr, Node<T>* _previous)
		: previous(_previous)
		, ptr(_ptr)
	{}

	ConstLinkedListIterator(const ConstLinkedListIterator& other)
		: previous(other.previous)
		, ptr(other.ptr)
	{}

	ConstLinkedListIterator& operator=(const ConstLinkedListIterator& other);

	ConstLinkedListIterator& operator++();
	ConstLinkedListIterator operator++(int);
	ConstLinkedListIterator& operator--();
	ConstLinkedListIterator operator--(int);

	const_reference operator*() { return ptr->data; }
	const_pointer operator->() { return &ptr->data; }

	bool operator==(const ConstLinkedListIterator& rhs) { return ptr == rhs.ptr; }
	bool operator!=(const ConstLinkedListIterator& rhs) { return !(*this == rhs); }

	virtual ~ConstLinkedListIterator() {}

protected:
	Node<T>* previous;
	Node<T>* ptr;
};

template <class T>
class LinkedListIterator : public ConstLinkedListIterator<T>
{
public:
    using typename ConstLinkedListIterator<T>::reference;
    using typename ConstLinkedListIterator<T>::pointer;

    LinkedListIterator() : ConstLinkedListIterator<T>() {}
	explicit LinkedListIterator(Node<T>* _ptr, Node<T>* _previous) : ConstLinkedListIterator<T>(_ptr, _previous) {}
	LinkedListIterator(const LinkedListIterator<T>& other) : ConstLinkedListIterator<T>(other) {}

    reference operator*() { return ConstLinkedListIterator<T>::ptr->data; }
    pointer operator->() { return &ConstLinkedListIterator<T>::ptr->data; }
};

template <class T, class TAllocator>
class LinkedList
{
public:
	using allocator_type = TAllocator;
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using size_type = std::size_t;
	using difference_type = ptrdiff_t;
	using node_allocator_type = typename std::allocator_traits<TAllocator>::template rebind_alloc< Node<T> >;

	using iterator = LinkedListIterator<T>;
	using const_iterator = ConstLinkedListIterator<T>;

public:
	LinkedList() : LinkedList(node_allocator_type()) {}
	explicit LinkedList(const node_allocator_type& alloc);

	LinkedList(std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type());

	explicit LinkedList(const std::size_t n, const node_allocator_type& alloc = allocator_type()) : LinkedList(n, T()) {}
	LinkedList(const std::size_t n, const_reference val, const node_allocator_type& alloc = allocator_type());

	LinkedList(const LinkedList<T, TAllocator>& other);
	LinkedList(LinkedList<T, TAllocator>&& other);
	
	virtual ~LinkedList();

	LinkedList& operator=(const LinkedList<T, TAllocator>& right);

	void swap(LinkedList<T, TAllocator>& other);

	void push_back(const_reference data);
	void push_back(T&& data);

	void push_front(const_reference data);
	void push_front(T&& data);

	void pop_front();
	void pop_back();

	size_type size() const noexcept;
	bool empty() const noexcept;

	void clear();

	reference back() noexcept;
	const_reference back() const noexcept;

	reference front() noexcept;
	const_reference front() const noexcept;

	iterator begin() noexcept;
	const_iterator begin() const noexcept;

	iterator end() noexcept;
	const_iterator end() const noexcept;

	void sort() noexcept;

	template <class Compare>
	void sort(Compare comp) noexcept;

	template <class Compare>
	void sort_range(
		const Compare& comp,
		Node<T>* const first,
		Node<T>* const first_previous,
		Node<T>* const last,
		Node<T>* const last_previous) noexcept;
	
	
	iterator insert(const_iterator position, const_reference val);

	template <class InputIterator>
	iterator insert(const_iterator position, InputIterator first, InputIterator last);
	
	void reverse() noexcept;
	
	iterator erase(const_iterator position);
	iterator erase(const_iterator first, const_iterator last);
	
	void resize(size_type n);
	void resize(size_type n, const_reference val);

	template <class InputIterator>
	void assign(InputIterator first, InputIterator last);

	void assign(size_type n, const_reference val);
	void assign(std::initializer_list<value_type> il);

	void splice(const_iterator position, LinkedList& x) noexcept;
	void splice(const_iterator position, LinkedList& x, const_iterator i) noexcept;
	void splice(const_iterator position, LinkedList& x, const_iterator first, const_iterator last) noexcept;
	
	template <class BinaryPredicate>
	void unique(BinaryPredicate binary_pred);
	void unique();
	
	template <class Compare>
	void merge(LinkedList& x, Compare comp) noexcept;
	void merge(LinkedList& x) noexcept;

private:
	Node<T>* head;
	Node<T>* tail;
	size_type _size;

	node_allocator_type allocator;

private:
	Node<T>* create_node();

	void insert_before(Node<T>* const pos, Node<T>* const previous, Node<T>* const node);
	void insert_after(Node<T>* const pos, Node<T>* const previous, Node<T>* const node);
	Node<T>* unlink(Node<T>* const pos, Node<T>* const previous);

	Node<T>* create_node_in_tail();
	Node<T>* create_node_in_head();
	void pop(Node<T>** const node);
};

#include "LinkedList-inl.hpp"

#endif /* _LINKED_LIST_H_ */
