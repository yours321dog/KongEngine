// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef _LIST_H_
#define _LIST_H_

#include "KongTypes.h"
#include "KongMath.h"
#include "Allocator.h"

namespace kong
{
    namespace core
    {


        //! Doubly linked list template.
        template <class T>
        class List
        {
        private:

            //! List element node with pointer to previous and next element in the list.
            struct SKListNode
            {
                SKListNode(const T& e) : Next(0), Prev(0), Element(e) {}

                SKListNode* Next;
                SKListNode* Prev;
                T Element;
            };

        public:
            class ConstIterator;

            //! List iterator.
            class Iterator
            {
            public:
                Iterator() : Current(0) {}

                Iterator& operator ++()    { Current = Current->Next; return *this; }
                Iterator& operator --()    { Current = Current->Prev; return *this; }
                Iterator  operator ++(s32) { Iterator tmp = *this; Current = Current->Next; return tmp; }
                Iterator  operator --(s32) { Iterator tmp = *this; Current = Current->Prev; return tmp; }

                Iterator& operator +=(s32 num)
                {
                    if (num > 0)
                    {
                        while (num-- && this->Current != 0) ++(*this);
                    }
                    else
                    {
                        while (num++ && this->Current != 0) --(*this);
                    }
                    return *this;
                }

                Iterator  operator + (s32 num) const { Iterator tmp = *this; return tmp += num; }
                Iterator& operator -=(s32 num) { return (*this) += (-num); }
                Iterator  operator - (s32 num) const { return (*this) + (-num); }

                bool operator ==(const Iterator&      other) const { return Current == other.Current; }
                bool operator !=(const Iterator&      other) const { return Current != other.Current; }
                bool operator ==(const ConstIterator& other) const { return Current == other.Current; }
                bool operator !=(const ConstIterator& other) const { return Current != other.Current; }

#if defined (_MSC_VER) && (_MSC_VER < 1300)
#pragma warning(disable:4284) // infix notation problem when using iterator operator ->
#endif

                T & operator * () { return Current->Element; }
                T * operator ->() { return &Current->Element; }

            private:
                explicit Iterator(SKListNode* begin) : Current(begin) {}

                SKListNode* Current;

                friend class List<T>;
                friend class ConstIterator;
            };

            //! List iterator for const access.
            class ConstIterator
            {
            public:

                ConstIterator() : Current(0) {}
                ConstIterator(const Iterator& iter) : Current(iter.Current)  {}

                ConstIterator& operator ++()    { Current = Current->Next; return *this; }
                ConstIterator& operator --()    { Current = Current->Prev; return *this; }
                ConstIterator  operator ++(s32) { ConstIterator tmp = *this; Current = Current->Next; return tmp; }
                ConstIterator  operator --(s32) { ConstIterator tmp = *this; Current = Current->Prev; return tmp; }

                ConstIterator& operator +=(s32 num)
                {
                    if (num > 0)
                    {
                        while (num-- && this->Current != 0) ++(*this);
                    }
                    else
                    {
                        while (num++ && this->Current != 0) --(*this);
                    }
                    return *this;
                }

                ConstIterator  operator + (s32 num) const { ConstIterator tmp = *this; return tmp += num; }
                ConstIterator& operator -=(s32 num) { return (*this) += (-num); }
                ConstIterator  operator - (s32 num) const { return (*this) + (-num); }

                bool operator ==(const ConstIterator& other) const { return Current == other.Current; }
                bool operator !=(const ConstIterator& other) const { return Current != other.Current; }
                bool operator ==(const Iterator&      other) const { return Current == other.Current; }
                bool operator !=(const Iterator&      other) const { return Current != other.Current; }

                const T & operator * () { return Current->Element; }
                const T * operator ->() { return &Current->Element; }

                ConstIterator & operator =(const Iterator & iterator) { Current = iterator.Current; return *this; }

            private:
                explicit ConstIterator(SKListNode* begin) : Current(begin) {}

                SKListNode* Current;

                friend class Iterator;
                friend class List<T>;
            };

            //! Default constructor for empty list.
            List()
                : first_(0), last_(0), size_(0) {}


            //! Copy constructor.
            List(const List<T>& other) : first_(0), last_(0), size_(0)
            {
                *this = other;
            }


            //! Destructor
            ~List()
            {
                clear();
            }


            //! Assignment operator
            void operator=(const List<T>& other)
            {
                if (&other == this)
                {
                    return;
                }

                clear();

                SKListNode* node = other.first_;
                while (node)
                {
                    push_back(node->Element);
                    node = node->Next;
                }
            }


            //! Returns amount of elements in list.
            /** \return Amount of elements in the list. */
            u32 size() const
            {
                return size_;
            }
            u32 getSize() const
            {
                return size_;
            }


            //! Clears the list, deletes all elements in the list.
            /** All existing iterators of this list will be invalid. */
            void clear()
            {
                while (first_)
                {
                    SKListNode * next = first_->Next;
                    allocator.destruct(first_);
                    allocator.deallocate(first_);
                    first_ = next;
                }

                //First = 0; handled by loop
                last_ = 0;
                size_ = 0;
            }


            //! Checks for empty list.
            /** \return True if the list is empty and false if not. */
            bool empty() const
            {
                return (first_ == 0);
            }


            //! Adds an element at the end of the list.
            /** \param element Element to add to the list. */
            void push_back(const T& element)
            {
                SKListNode* node = allocator.allocate(1);
                allocator.construct(node, element);

                ++size_;

                if (first_ == 0)
                    first_ = node;

                node->Prev = last_;

                if (last_ != 0)
                    last_->Next = node;

                last_ = node;
            }


            //! Adds an element at the begin of the list.
            /** \param element: Element to add to the list. */
            void push_front(const T& element)
            {
                SKListNode* node = allocator.allocate(1);
                allocator.construct(node, element);

                ++size_;

                if (first_ == 0)
                {
                    last_ = node;
                    first_ = node;
                }
                else
                {
                    node->Next = first_;
                    first_->Prev = node;
                    first_ = node;
                }
            }


            //! Gets first node.
            /** \return A list iterator pointing to the beginning of the list. */
            Iterator begin()
            {
                return Iterator(first_);
            }


            //! Gets first node.
            /** \return A const list iterator pointing to the beginning of the list. */
            ConstIterator begin() const
            {
                return ConstIterator(first_);
            }


            //! Gets end node.
            /** \return List iterator pointing to null. */
            Iterator end()
            {
                return Iterator(0);
            }


            //! Gets end node.
            /** \return Const list iterator pointing to null. */
            ConstIterator end() const
            {
                return ConstIterator(0);
            }


            //! Gets last element.
            /** \return List iterator pointing to the last element of the list. */
            Iterator getLast()
            {
                return Iterator(last_);
            }


            //! Gets last element.
            /** \return Const list iterator pointing to the last element of the list. */
            ConstIterator getLast() const
            {
                return ConstIterator(last_);
            }


            //! Inserts an element after an element.
            /** \param it Iterator pointing to element after which the new element
            should be inserted.
            \param element The new element to be inserted into the list.
            */
            void insert_after(const Iterator& it, const T& element)
            {
                SKListNode* node = allocator.allocate(1);
                allocator.construct(node, element);

                node->Next = it.Current->Next;

                if (it.Current->Next)
                    it.Current->Next->Prev = node;

                node->Prev = it.Current;
                it.Current->Next = node;
                ++size_;

                if (it.Current == last_)
                    last_ = node;
            }


            //! Inserts an element before an element.
            /** \param it Iterator pointing to element before which the new element
            should be inserted.
            \param element The new element to be inserted into the list.
            */
            void insert_before(const Iterator& it, const T& element)
            {
                SKListNode* node = allocator.allocate(1);
                allocator.construct(node, element);

                node->Prev = it.Current->Prev;

                if (it.Current->Prev)
                    it.Current->Prev->Next = node;

                node->Next = it.Current;
                it.Current->Prev = node;
                ++size_;

                if (it.Current == first_)
                    first_ = node;
            }


            //! Erases an element.
            /** \param it Iterator pointing to the element which shall be erased.
            \return Iterator pointing to next element. */
            Iterator erase(Iterator& it)
            {
                // suggest changing this to a const Iterator& and
                // working around line: it.Current = 0 (possibly with a mutable, or just let it be garbage?)

                Iterator returnIterator(it);
                ++returnIterator;

                if (it.Current == first_)
                {
                    first_ = it.Current->Next;
                }
                else
                {
                    it.Current->Prev->Next = it.Current->Next;
                }

                if (it.Current == last_)
                {
                    last_ = it.Current->Prev;
                }
                else
                {
                    it.Current->Next->Prev = it.Current->Prev;
                }

                allocator.destruct(it.Current);
                allocator.deallocate(it.Current);
                it.Current = 0;
                --size_;

                return returnIterator;
            }

            //! Swap the content of this list container with the content of another list
            /** Afterwards this object will contain the content of the other object and the other
            object will contain the content of this object. Iterators will afterwards be valid for
            the swapped object.
            \param other Swap content with this object	*/
            void swap(List<T>& other)
            {
                core::swap(first_, other.first_);
                core::swap(last_, other.last_);
                core::swap(size_, other.size_);
                core::swap(allocator, other.allocator);	// memory is still released by the same allocator used for allocation
            }


        private:

            SKListNode* first_;
            SKListNode* last_;
            u32 size_;
            Allocator<SKListNode> allocator;

        };


    } // end namespace core
}// end namespace irr

#endif

