// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _DIMENSION2D_H_
#define _DIMENSION2D_H_

#include "KongMath.h"

namespace kong
{
    namespace core
    {
        template <class T>
        class Dimension2d
        {
        public:
            Dimension2d() : width_(0), height_(0) {}

            Dimension2d(const T& width, const T& height) : width_(width), height_(height) {}

            //! Use this constructor only where you are sure that the conversion is valid.
            template <class U>
            explicit Dimension2d(const Dimension2d<U>& other) :
                width_(other.width_), height_(other.height_) {}

            template <class U>
            Dimension2d<T> &operator=(const Dimension2d<U> &other)
            {
                width_ = static_cast<T>(other.width_);
                height_ = static_cast<T>(other.height_);
                return *this;
            }

            //! Equality operator
            bool operator==(const Dimension2d<T>& other) const
            {
                return core::equals(width_, other.width_) &&
                    core::equals(height_, other.height_);
            }

            //! Inequality operator
            bool operator!=(const Dimension2d<T>& other) const
            {
                return !(*this == other);
            }

            //! Set to new values
            Dimension2d<T>& set(const T& width, const T& height)
            {
                width_ = width;
                height_ = height;
                return *this;
            }

            //! Divide width and height by scalar
            Dimension2d<T>& operator/=(const T& scale)
            {
                width_ /= scale;
                height_ /= scale;
                return *this;
            }

            //! Divide width and height by scalar
            Dimension2d<T> operator/(const T& scale) const
            {
                return Dimension2d<T>(width_ / scale, height_ / scale);
            }

            //! Multiply width and height by scalar
            Dimension2d<T>& operator*=(const T& scale)
            {
                width_ *= scale;
                height_ *= scale;
                return *this;
            }

            //! Multiply width and height by scalar
            Dimension2d<T> operator*(const T& scale) const
            {
                return Dimension2d<T>(width_*scale, height_*scale);
            }

            //! Add another dimension to this one.
            Dimension2d<T>& operator+=(const Dimension2d<T>& other)
            {
                width_ += other.width_;
                height_ += other.height_;
                return *this;
            }

            //! Add two dimensions
            Dimension2d<T> operator+(const Dimension2d<T>& other) const
            {
                return Dimension2d<T>(width_ + other.width_, height_ + other.height_);
            }

            //! Subtract a dimension from this one
            Dimension2d<T>& operator-=(const Dimension2d<T>& other)
            {
                width_ -= other.width_;
                height_ -= other.height_;
                return *this;
            }

            //! Subtract one dimension from another
            Dimension2d<T> operator-(const Dimension2d<T>& other) const
            {
                return Dimension2d<T>(width_ - other.width_, height_ - other.height_);
            }

            //! Get area
            T getArea() const
            {
                return width_*height_;
            }

            T width_;
            T height_;
        };
    } // end namespace core
} // end namespace kong

#endif
