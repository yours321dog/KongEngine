// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "KongTypes.h"
#include "KongMath.h"

namespace kong
{
    namespace core
    {
        template <typename T> class Vector
        {
        public:
            explicit Vector(T x = T(0), T y = T(0), T z = T(0), T w = T(1));
            Vector(const Vector &);
            Vector<T> &operator=(const Vector<T>&);
            Vector<T> operator+(const Vector<T>&) const;
            Vector<T> operator+(const T&) const;
            Vector<T> &operator+=(const Vector<T>&);
            Vector<T> &operator+=(const T&);
            Vector<T> operator-(const Vector<T>&) const;
            Vector<T> operator*(const f32 &) const;
            Vector<T> operator/(const f32 &) const;
            Vector<T> operator-() const;
            Vector<T> &operator*=(const T&);
            Vector<T> &operator/=(const T&);
            Vector<T> &operator*=(const Vector<T>&);
            Vector<T> &operator/=(const Vector<T>&);
            bool operator==(const Vector<T> &) const;
            bool operator!=(const Vector<T> &) const;
            bool operator<(const Vector<T> &) const;
            Vector<T> operator*(const Vector<T>&) const;
            T& operator()(int i);
            const T& operator()(int i) const;
            T Length();
            T X();
            T Y();
            T Z();
            T W();
            void X(T x);
            void Y(T y);
            void Z(T z);
            void W(T w);
            void Set(T x = T(0), T y = T(0), T z = T(0));

            static T DotProduct(const Vector<T> v1, const Vector<T> v2);
            T DotProduct(const Vector<T> &v1) const;
            Vector<T> CrossProduct(const Vector<T> &v1, const Vector<T> &v2);
            Vector<T> CrossProduct(const Vector<T> &vec);
            static Vector<T> Multiply(const Vector<T> &v1, const Vector<T> &v2);
            void Interpolation(const Vector<T> &v1, const Vector<T> &v2, f32 t);
            Vector<T> GetInterpolation(const Vector<T> &vec, f32 t);
            Vector<T> &Normalize();
            void Zero();
            T GetLength() const;
            T GetLengthSQ() const;
            T GetDistanceFromSQ(const Vector<T> &vec) const;
            T GetDistanceFrom(const Vector<T> &vec) const;
            bool Equals(const Vector<T> &other, const T tolerance = T(ROUNDING_ERROR_f32)) const;

            bool IsBetweenPoints(const Vector<T>& begin, const Vector<T>& end) const;

        //private:
            T x_;
            T y_;
            T z_;
            T w_;
        };

        template <typename T>
        Vector<T>::Vector(T x, T y, T z, T w)
            : x_(x), y_(y), z_(z), w_(w)
        {
        }

        template <typename T>
        Vector<T>::Vector(const Vector& vec)
        {
            x_ = vec.x_;
            y_ = vec.y_;
            z_ = vec.z_;
            w_ = vec.w_;
        }

        template <typename T>
        Vector<T>& Vector<T>::operator=(const Vector<T>& vec)
        {
            x_ = vec.x_;
            y_ = vec.y_;
            z_ = vec.z_;
            w_ = vec.w_;

            return *this;
        }

        template <typename T>
        Vector<T> Vector<T>::operator+(const Vector<T>& vec) const
        {
            Vector<T> tmp;
            tmp.x_ = x_ + vec.x_;
            tmp.y_ = y_ + vec.y_;
            tmp.z_ = z_ + vec.z_;
            tmp.w_ = T(1);

            return tmp;
        }

        template <typename T>
        Vector<T> Vector<T>::operator+(const T& val) const
        {
            Vector<T> tmp;
            tmp.x_ = x_ + val;
            tmp.y_ = y_ + val;
            tmp.z_ = z_ + val;
            tmp.w_ = T(1);

            return tmp;
        }

        template <typename T>
        Vector<T>& Vector<T>::operator+=(const Vector<T>& other)
        {
            x_ += other.x_;
            y_ += other.y_;
            z_ += other.z_;
            w_ = T(1);

            return *this;
        }

        template <typename T>
        Vector<T>& Vector<T>::operator+=(const T& val)
        {
            x_ += val;
            y_ += val;
            z_ += val;
            w_ = T(1);

            return *this;
        }

        template <typename T>
        Vector<T> Vector<T>::operator-(const Vector<T>& vec) const
        {
            Vector<T> tmp;
            tmp.x_ = x_ - vec.x_;
            tmp.y_ = y_ - vec.y_;
            tmp.z_ = z_ - vec.z_;
            tmp.w_ = T(1);

            return tmp;
        }

        template <typename T>
        Vector<T> Vector<T>::operator*(const f32& f) const
        {
            Vector<T> tmp;
            tmp.x_ = x_ * f;
            tmp.y_ = y_ * f;
            tmp.z_ = z_ * f;
            tmp.w_ = T(1);

            return tmp;
        }

        template <typename T>
        Vector<T> Vector<T>::operator/(const f32& f) const
        {
            Vector<T> tmp;
            tmp.x_ = x_ / f;
            tmp.y_ = y_ / f;
            tmp.z_ = z_ / f;
            tmp.w_ = T(1);

            return tmp;
        }

        template <typename T>
        Vector<T> Vector<T>::operator-() const
        {
            Vector<T> tmp;
            tmp.x_ = -x_;
            tmp.y_ = -y_;
            tmp.z_ = -z_;
            tmp.w_ = w_;

            return tmp;
        }

        template <typename T>
        Vector<T>& Vector<T>::operator*=(const T& val)
        {
            (*this) = (*this) * val;
            return *this;
        }

        template <typename T>
        Vector<T>& Vector<T>::operator/=(const T& val)
        {
            (*this) = (*this) / val;
            return *this;
        }

        template <typename T>
        Vector<T>& Vector<T>::operator*=(const Vector<T> &other)
        {
            x_ *= other.x_;
            y_ *= other.y_;
            z_ *= other.z_;
            return *this;
        }

        template <typename T>
        Vector<T>& Vector<T>::operator/=(const Vector<T>& other)
        {
            x_ /= other.x_;
            y_ /= other.y_;
            z_ /= other.z_;
            return *this;
        }

        template <typename T>
        bool Vector<T>::operator==(const Vector<T>& vec) const
        {
            return core::equals(x_, vec.x_) &&
                core::equals(y_, vec.y_) && 
                core::equals(z_, vec.z_) &&
                core::equals(w_, vec.w_);
        }

        template <typename T>
        bool Vector<T>::operator!=(const Vector<T>& vec) const
        {
            return !core::equals(x_, vec.x_) || 
                !core::equals(y_, vec.y_) ||
                !core::equals(z_, vec.z_) ||
                !core::equals(w_, vec.w_);
        }

        template <typename T>
        bool Vector<T>::operator<(const Vector<T>& other) const
        {
            return 	(x_<other.x_ && !core::equals(x_, other.x_)) ||
                (core::equals(x_, other.x_) && y_<other.y_ && !core::equals(y_, other.y_)) ||
                (core::equals(x_, other.x_) && core::equals(y_, other.y_) && z_<other.z_ && !core::equals(z_, other.z_));
        }

        template <typename T>
        Vector<T> Vector<T>::operator*(const Vector<T>& vec) const
        {
            Vector<T> tmp;
            tmp.x_ = x_ * vec.x_;
            tmp.y_ = y_ * vec.y_;
            tmp.z_ = z_ * vec.z_;

            return tmp;
        }

        template <typename T>
        T& Vector<T>::operator()(int i)
        {
            switch (i)
            {
            default:
            case 0:
                return x_;
            case 1:
                return y_;
            case 2:
                return z_;
            case 3:
                return w_;
            }
        }

        template <typename T>
        const T& Vector<T>::operator()(int i) const
        {
            switch (i)
            {
            default:
            case 0:
                return x_;
            case 1:
                return y_;
            case 2:
                return z_;
            case 3:
                return w_;
            }
        }

        template <typename T>
        T Vector<T>::Length()
        {
            return sqrt(x_ * x_ + y_ * y_ + z_ * z_);
        }

        template <typename T>
        T Vector<T>::X()
        {
            return x_;
        }

        template <typename T>
        T Vector<T>::Y()
        {
            return y_;
        }

        template <typename T>
        T Vector<T>::Z()
        {
            return z_;
        }

        template <typename T>
        T Vector<T>::W()
        {
            return w_;
        }

        template <typename T>
        void Vector<T>::X(T x)
        {
            x_ = x;
        }

        template <typename T>
        void Vector<T>::Y(T y)
        {
            y_ = y;
        }

        template <typename T>
        void Vector<T>::Z(T z)
        {
            z_ = z;
        }

        template <typename T>
        void Vector<T>::W(T w)
        {
            w_ = w;
        }


        template <typename T>
        T Vector<T>::DotProduct(const Vector<T> v1, const Vector<T> v2)
        {
            return v1.x_ * v2.x_ + v1.y_ * v2.y_ + v1.z_ * v2.z_;
        }

        template <typename T>
        T Vector<T>::DotProduct(const Vector<T>& v1) const
        {
            return v1.x_ * x_ + v1.y_ * y_ + v1.z_ * z_;
        }

        template <typename T>
        Vector<T> Vector<T>::CrossProduct(const Vector<T> &v1, const Vector<T> &v2)
        {
            x_ = v1.y_ * v2.z_ - v1.z_ * v2.y_;
            y_ = v1.z_ * v2.x_ - v1.x_ * v2.z_;
            z_ = v1.x_ * v2.y_ - v1.y_ * v2.x_;
            return *this;
        }

        template <typename T>
        Vector<T> Vector<T>::CrossProduct(const Vector<T>& vec)
        {
            Vector tmp;
            tmp.CrossProduct(*this, vec);
            return tmp;
        }

        template <typename T>
        Vector<T> Vector<T>::Multiply(const Vector<T>& v1, const Vector<T>& v2)
        {
            Vector<T> tmp;
            tmp.x_ = v1.x_ * v2.x_;
            tmp.y_ = v1.y_ * v2.y_;
            tmp.z_ = v1.z_ * v2.z_;
            return tmp;
        }


        template <typename T>
        void Vector<T>::Interpolation(const Vector<T> &v1, const Vector<T> &v2, f32 t)
        {
            x_ = lerp<T>(v1.x_, v2.x_, t);
            y_ = lerp<T>(v1.y_, v2.y_, t);
            z_ = lerp<T>(v1.z_, v2.z_, t);
            //w_ = T(1);
            w_ = lerp<T>(v1.w_, v2.w_, t);
        }

        template <typename T>
        Vector<T> Vector<T>::GetInterpolation(const Vector<T>& vec, f32 t)
        {
            Vector<T> tmp;
            tmp.Interpolation(vec, *this, t);
            return tmp;
        }

        template <typename T>
        Vector<T> &Vector<T>::Normalize()
        {
            T length = Length();
            if (length > T(0))
            {
                T inv = T(1) / length;
                x_ *= inv;
                y_ *= inv;
                z_ *= inv;
            }

            return *this;
        }

        template <typename T>
        void Vector<T>::Zero()
        {
            x_ = 0.f;
            y_ = 0.f;
            z_ = 0.f;
            w_ = T(1);
        }

        template <typename T>
        T Vector<T>::GetLength() const
        {
            return core::squareroot(x_ * x_ + y_ * y_ + z_ * z_);
        }

        template <typename T>
        T Vector<T>::GetLengthSQ() const
        {
            return x_ * x_ + y_ * y_ + z_ * z_;
        }

        template <typename T>
        T Vector<T>::GetDistanceFromSQ(const Vector<T>& vec) const
        {
            return Vector<T>(x_ - vec.x_, y_ - vec.y_, z_ - vec.z_).GetLengthSQ();
        }

        template <typename T>
        T Vector<T>::GetDistanceFrom(const Vector<T>& vec) const
        {
            return Vector<T>(x_ - vec.x_, y_ - vec.y_, z_ - vec.z_).GetLength();
        }

        template <typename T>
        bool Vector<T>::Equals(const Vector<T>& other, const T tolerance) const
        {
            return core::equals(x_, other.x_, tolerance) 
                && core::equals(y_, other.y_, tolerance) 
                && core::equals(z_, other.z_, tolerance);
        }

        template <typename T>
        bool Vector<T>::IsBetweenPoints(const Vector<T>& begin, const Vector<T>& end) const
        {
            if (begin.x_ != end.x_)
            {
                return ((begin.x_ <= x_ && x_ <= end.x_) ||
                    (begin.x_ >= x_ && x_ >= end.x_));
            }
            else if (begin.y_ != end.y_)
            {
                return ((begin.y_ <= y_ && y_ <= end.y_) ||
                    (begin.y_ >= y_ && y_ >= end.y_));
            }
            else
            {
                return ((begin.z_ <= z_ && z_ <= end.z_) ||
                    (begin.z_ >= z_ && z_ >= end.z_));
            }
        }

        template <typename T>
        void Vector<T>::Set(T x, T y, T z)
        {
            x_ = x;
            y_ = y;
            z_ = z;
        }

        typedef Vector<f32> Vector2Df;
        typedef Vector<f32> Vector3Df;
        typedef Vector<f32> vector2df;
        typedef Vector<f32> vector3df;
        typedef Vector<s32> Vector2Di;
        typedef Vector<s32> Vector3Di;
        typedef Vector<u8> Coloru8;
        typedef Vector<f32> Colorf;
    }
}

#endif