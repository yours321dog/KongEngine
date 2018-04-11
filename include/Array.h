// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "KongMath.h"
#include "Heapsort.h"

namespace kong
{
    namespace core
    {
        template<typename T>
        class Array
        {
        public:
            Array();
            Array(const Array<T> &arr);
            Array<T> &operator=(const Array<T> &arr);

            ~Array();

            void Resize(u32 size);

            void Reallocate(u32 allocated);

            u32 Size() const;
            bool Empty() const;
            
            T &operator[](s32 i);
            T &operator[](s32 i) const;
            void SetAll(const T val);
            void Clear();

            void PushBack(const T &val);
            void Insert(const T &val, u32 index = 0);

            T *Pointer();
            const T *ConstPointer() const;

            void Sort();

            s32 BinarySearch(const T &element);
            s32 BinarySearch(const T &element) const;

            s32 BinarySearch(const T &element, s32 left, s32 right) const;
            s32 LinearSearch(const T& element) const;

        private:
            void Reallocted(u32 times = 2);

            u32 size_;
            u32 allocated_;
            T *data_;
            bool free_when_destroy_;
        };

        template <typename T>
        Array<T>::Array() :
            size_(0), allocated_(32), free_when_destroy_(false)
        {
            data_ = new T[allocated_];
        }

        template <typename T>
        Array<T>::Array(const Array<T>& arr)
        {
            while (allocated_ < arr.size_)
            {
                Reallocted(arr.size_ / allocated_ + (arr.size_ % allocated_ > 0));
            }

            size_ = arr.size_;

            for (int i = 0; i < size_; i++)
            {
                data_[i] = arr.data_[i];
            }

            free_when_destroy_ = arr.free_when_destroy_;
        }

        template <typename T>
        Array<T>& Array<T>::operator=(const Array<T>& arr)
        {
            while (allocated_ < arr.size_)
            {
                Reallocted(arr.size_ / allocated_ + (arr.size_ % allocated_ > 0));
            }

            size_ = arr.size_;

            for (int i = 0; i < size_; i++)
            {
                data_[i] = arr.data_[i];
            }

            free_when_destroy_ = arr.free_when_destroy_;
            return *this;
        }

        template <typename T>
        Array<T>::~Array()
        {
            delete[] data_;
        }

        template <typename T>
        void Array<T>::Resize(u32 size)
        {
            while (allocated_ < size)
            {
                Reallocted(size / allocated_ + (size % allocated_ > 0));
            }

            size_ = size;
        }

        template <typename T>
        void Array<T>::Reallocate(u32 allocated)
        {
            if (allocated_ < allocated)
            {
                Reallocted(allocated / allocated_ + (allocated % allocated_ > 0));
            }
        }

        template <typename T>
        void Array<T>::Reallocted(u32 times)
        {
            if (times > 1)
            {
                allocated_ *= times;
                T *new_data = new T[allocated_];

                for (int i = 0; i < size_; i++)
                {
                    new_data[i] = data_[i];
                }

                delete[] data_;
                data_ = new_data;
            }
        }

        template <typename T>
        u32 Array<T>::Size() const
        {
            return size_;
        }

        template <typename T>
        bool Array<T>::Empty() const
        {
            return size_ == 0;
        }

        template <typename T>
        T &Array<T>::operator[](s32 i)
        {
            if (i > 0 && i < size_)
            {
                return data_[i];
            }

            return data_[0];
        }

        template <typename T>
        T &Array<T>::operator[](s32 i) const
        {
            if (i > 0 && i < size_)
            {
                return data_[i];
            }

            return data_[0];
        }

        template <typename T>
        void Array<T>::SetAll(const T val)
        {
            for (int i = 0; i < size_; i++)
            {
                data_[i] = val;
            }
        }

        template <typename T>
        void Array<T>::Clear()
        {
            size_ = 0;
        }

        template <typename T>
        void Array<T>::PushBack(const T& val)
        {
            if (size_ == allocated_)
            {
                Reallocted();
            }

            data_[size_++] = val;
        }

        template <typename T>
        void Array<T>::Insert(const T& val, u32 index)
        {
            const u32 min_size = min_(index, size_);

            if (size_ == allocated_)
            {
                allocated_ *= 2;
                T *new_data = new T[allocated_];
                
                for (int i = 0; i < min_size; i++)
                {
                    new_data[i] = data_[i];
                }
                new_data[min_size] = val;
                for (int i = min_size; i < size_; i++)
                {
                    new_data[i + 1] = data_[i];
                }

                delete[] data_;
                data_ = new_data;
            }
            else
            {
                for (int i = size_; i > min_size; i--)
                {
                    data_[i] = data_[i - 1];
                }
                data_[min_size] = val;
            }

            size_++;
        }

        template <typename T>
        T* Array<T>::Pointer()
        {
            return data_;
        }

        template <typename T>
        const T* Array<T>::ConstPointer() const
        {
            return data_;
        }

        template <typename T>
        void Array<T>::Sort()
        {
            Heapsort(data_, size_);
        }

        template <typename T>
        s32 Array<T>::BinarySearch(const T& element)
        {
            Sort();
            return BinarySearch(element, 0, size_ - 1);
        }

        template <typename T>
        s32 Array<T>::BinarySearch(const T& element) const
        {
            return LinearSearch(element, 0, used - 1);
        }

        template <typename T>
        s32 Array<T>::BinarySearch(const T& element, s32 left, s32 right) const
        {
            if (!size_)
                return -1;

            s32 m;

            do
            {
                m = (left + right) >> 1;

                if (element < data_[m])
                    right = m - 1;
                else
                    left = m + 1;

            } while ((element < data_[m] || data_[m] < element) && left <= right);
            // this last line equals to:
            // " while((element != array[m]) && left<=right);"
            // but we only want to use the '<' operator.
            // the same in next line, it is "(element == array[m])"


            if (!(element < data_[m]) && !(data_[m] < element))
                return m;

            return -1;
        }

        template <typename T>
        s32 Array<T>::LinearSearch(const T& element) const
        {
            for (u32 i = 0; i < size_; i++)
            {
                if (element == data_[i])
                {
                    return static_cast<s32>(i);
                }
            }
            return -1;
        }
    } // end namespace core
} // end namespace kong

#endif