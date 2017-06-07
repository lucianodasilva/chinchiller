#pragma once
#ifndef _forward_list_h_
#define _forward_list_h_

#include <stdlib.h>

#include "utility.h"
#include "type_traits.h"

namespace common {

    template < class _t >
    struct _forward_node {
        _t                      _data;
        _forward_node < _t > *  _next = nullptr;
    };

    template < class _t >
    struct forward_iterator {
        _forward_node < _t > * _node;
    };

    template < class _t >
    inline forward_iterator < _t > operator ++ (const forward_iterator < _t > & it) {
        return { it._node->_next };
    }

    template < class _t >
    inline _t & operator * (forward_iterator < _t > & it) {
        return it._node->_data;
    }

    template < class _t >
    inline bool operator == (forward_iterator < _t > const & v1, forward_iterator < _t > const & v2 ) {
        return 
            (!v1._node && !v2._node) ||
                (
                    (v1._node && v2._node) && 
                    (v1._node->_data == v2._node->_data) && 
                    (v1._node->_next == v2._node->_next)
                );
    }

    template <class _t>
    inline bool operator != (forward_iterator< _t > const & v1, forward_iterator< _t > const & v2)
    {
        return !(v1 == v2);
    }

    template < class _t >
    class forward_list {
    public:
        // types:
        using value_type = _t;

        using reference = value_type &;
        using const_reference = value_type const &;

        using pointer = value_type *;
        using const_pointer = value_type const *;

        using size_type = size_t;
        
        using iterator = forward_iterator < _t >;
        using const_iterator = iterator const;

        // construct/copy/destroy:
        explicit forward_list() = default;

        explicit forward_list(size_type n) {
            resize (n);
        }
    
        forward_list(size_type n, const_reference value) {
            resize (n, value);
        }

        template <class _it_t>
        forward_list(_it_t first, _it_t last) : forward_list() {
            assign(first, last);
        }

        forward_list(const forward_list & x);

        forward_list(forward_list && x) noexcept;

        ~forward_list() {
            clear();
        }

        forward_list & operator=(const forward_list & x);
        forward_list & operator=(forward_list && x) noexcept;

        template <class _it_t>
        void assign(_it_t first, _it_t last) {
            _node_t prev { _t{}, _head };

            _node_t * last_assigned = assign_sequence(&prev, _head, first, last);
            erase_sequence(last_assigned->_next);

            last_assigned->_next = nullptr;

            if (!_head)
                _head = prev._next;
        }

        void assign(size_type n, const_reference t) {
            _node_t prev { _t{}, _head };

            _node_t * last_assigned = assign_sequence(&prev, _head, n, t);
            erase_sequence (last_assigned->_next);

            last_assigned->_next = nullptr;

            if (!_head)
                _head = prev._next;
        }

        // iterators:
        iterator begin() noexcept {
            return { _head };
        }

        const_iterator begin() const noexcept {
            return { _head };
        }

        iterator end() noexcept {
            return { nullptr };
        }

        const_iterator end() const noexcept {
            return { nullptr };
        }

        const_iterator cbegin() noexcept {
            return { _head };
        }

        const_iterator cend() noexcept {
            return { nullptr };
        }

        // capacity:
        bool empty() const noexcept {
            return _head == nullptr;
        }

        // element access:
        reference front() {
            return _head->data;
        }

        const_reference front() const {
            return _head->data;
        }

        // modifiers:
        template <class ... _args_tv>
        void emplace_front(_args_tv && ... args);

        void push_front(const_reference x) {
            _head = new _node_t { x, _head };
        }

        void push_front(_t && x) {
            _head = new _node_t { x, move(_head) };
        }

        void pop_front() {
            if (_head) {
                auto * t = _head;
                _head = _head->_next;
                delete t;
            }
        }

/*
        template <class... _args_tv>
        iterator emplace_after(const_iterator position, _args_tv && ... args);

        iterator insert_after(const_iterator position, const_reference x);
        iterator insert_after(const_iterator position, _t && x);

        iterator insert_after(const_iterator position, size_type n, const_reference x);

        template <class _it_t>
        iterator insert_after(const_iterator position, _it_t first,
                              _it_t last);

        iterator erase_after(const_iterator position);
        iterator erase_after(const_iterator position, iterator last);
*/

        void swap(forward_list & x) {
            auto th = _head;
            _head = x._head;
            x._head = th;
        }

        void resize(size_type sz) {
            resize (sz, _t {});
        }

        void resize(size_type sz, const_reference c) {

            // find size at end or erase
            size_type count = 0;

            _node_t pre_head = { _t{}, _head };

            _node_t * prev = &pre_head;
            _node_t * node = _head;

            while (node && sz > 0) {
                ++count;
                --sz;
                prev = node;
                node = node->_next;
            }

            if (!node) {
                assign_sequence(prev, node, sz, c);

                if (_head)
                    _head = prev->_next;
            } else {
                erase_sequence (prev->_next);
                prev->_next = nullptr;
            }
        }

        void clear() noexcept {
            erase_sequence (_head);
            _head = nullptr;
        }

        // forward_list operations:

/*
        void splice_after(const_iterator position, forward_list<T, Allocator> &x);
        void splice_after(const_iterator position, forward_list<T, Allocator> &&x);
        void splice_after(const_iterator position, forward_list<T, Allocator> &x,
                          const_iterator i);
        void splice_after(const_iterator position, forward_list<T, Allocator> &&x,
                          const_iterator i);
        void splice_after(const_iterator position, forward_list<T, Allocator> &x,
                          const_iterator first, const_iterator last);
        void splice_after(const_iterator position, forward_list<T, Allocator> &&x,
                          const_iterator first, const_iterator last);
*/

        void remove(const_reference value) {
            remove_if ([&](const_reference a) { return value == a; });
        }

        template <class _predicate_t>
        void remove_if(_predicate_t pred) {
            if (!_head)
                return;

            _node_t 
                * cursor = _head,
                * prev = nullptr;

            do { 
                if (pred(cursor->_data)) {
                    _node_t * del = cursor;

                    (prev ? prev->_next : _head) = cursor->_next;
                    cursor = cursor->_next;

                    delete del;
                } else {
                    prev = cursor;
                    cursor = cursor->_next;
                }
            } while (cursor);
        }

/*
        void unique();

        template <class _binary_predicate_t>
        void unique(_binary_predicate_t binary_pred);

        void merge(forward_list & x);
        void merge(forward_list && x);

        template <class _comparer_t>
        void merge(forward_list & x, _comparer_t comp);

        template <class _comparer_t>
        void merge(forward_list && x, _comparer_t comp);

        void sort();

        template <class _comparer_t>
        void sort(_comparer_t comp);

        void reverse() noexcept;
*/

    private:

        using _node_t = _forward_node < _t >;

        _node_t        
            * _head = nullptr;

        template < class _it_t >
        static inline _node_t * assign_sequence (_node_t * prev, _node_t * node, _it_t first, _it_t last) {
            while (first != last) {
                if (node) {
                    node->_data = *first;
                } else {
                    node = new _node_t { *first, nullptr };
                }

                prev->_next = node;
                prev = node;
                node = node->_next;
                ++first;
            }

            return prev;
        }

        template < class _it_t >
        static inline _node_t * assign_sequence (_node_t * prev, _node_t * node, size_type count, const_reference v) {
            while (count > 0) {
                if (node) {
                    node->_data = v;
                } else {
                    node = new _node_t { v, nullptr };
                }

                prev->_next = node;
                prev = node;
                node = node->_next;
                --count;
            }

            return prev;
        }

        inline _node_t * find_index (size_t i) const {
            auto * c = _head;

            while (i > 0 && c) {
                c = c->_next;
                --i;
            }

            return c;
        }

        inline static void erase_sequence (_node_t * n) {
            while (n) {
                auto * p = n;
                n = n->_next;
                delete p;
            }
        }

        inline size_type size() const {
            size_type n = 0;

            auto * c = _head;

            while (c) {
                ++n;
                c = c->_next;
            }

            return n;
        }

    };

}

#endif