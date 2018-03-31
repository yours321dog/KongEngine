// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "KongMath.h"

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

            u32 Size() const;
            bool Empty() const;
            
            T &operator[](s32 i);
            T &operator[](s32 i) const;
            void SetAll(const T val);

            void PushBack(const T &val);
            void Insert(const T &val, u32 index = 0);

            T *Pointer();
            const T *ConstPointer() const;

        private:
            void Reallocted(u32 times = 2);

            u32 size_;
            u32 allocated_;
            T *data_;
        };

        template <typename T>
        Array<T>::Array() :
            allocated_(32), size_(0)
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
    } // end namespace core
} // end namespace kong

#endif