// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "KongTypes.h"
#include "Vector.h"

namespace kong
{
    namespace core
    {
        template <typename T>
        class Matrix
        {
        public:
            enum INIT_TYPE
            {
                ZERO,
                IDENTITY
            };


            Matrix(T m[4][4]);
            Matrix(u32 init_type = ZERO);
            Matrix(const Matrix &);

            void Zero();
            void Identity();

            T &operator()(int, int);
            const T&operator()(int, int) const;
            T &operator[](int);
            const T&operator[](int) const;
            Matrix<T> &operator=(const Matrix<T>&);
            Matrix<T> operator+(const Matrix<T>&);
            Matrix<T> operator-(const Matrix<T>&);
            Matrix<T> operator*(const Matrix<T>&);
            Matrix<T> operator*(const Matrix<T>&) const;
            Matrix<T> operator*(const T&);
            bool operator==(const Matrix<T>&) const;
            bool operator!=(const Matrix<T>&) const;

            Vector<T> Apply(const Vector<T> &);

            void Translate(T x, T y, T z);
            void Scale(T x, T y, T z);
            void Rotate(T x, T y, T z, f32 theta);
            void Rotate(T x, T y, T z);
            Matrix<T> Transpose() const;

            Matrix<T> Inverse() const;

            Vector3Df GetTranslation() const;

            const T *Pointer() const;

            T *Pointer();

            bool IsIdentity() const;

        private:
            T m_[16];
        };

        template <typename T>
        Matrix<T>::Matrix(u32 init_type)
        {
            if (init_type == IDENTITY)
            {
                Identity();
            }
            else
            {
                Zero();
            }
        }

        template <typename T>
        Matrix<T>::Matrix(T m[4][4])
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    m_[i * 4 + j] = m[i * 4 + j];
                }
            }
        }

        template <typename T>
        Matrix<T>::Matrix(const Matrix& m)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    m_[i * 4 + j] = m.m_[i * 4 + j];
                }
            }
        }

        template <typename T>
        Matrix<T>& Matrix<T>::operator=(const Matrix<T>& m)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    m_[i * 4 + j] = m.m_[i * 4 + j];
                }
            }
            return *this;
        }

        template <typename T>
        Matrix<T> Matrix<T>::operator+(const Matrix<T>& m)
        {
            Matrix<T> tmp;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    tmp.m_[i * 4 + j] = m_[i * 4 + j] + m.m_[i * 4 + j];
                }
            }
            return tmp;
        }

        template <typename T>
        Matrix<T> Matrix<T>::operator-(const Matrix<T>& m)
        {
            Matrix<T> tmp;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    tmp.m_[i * 4 + j] = m_[i * 4 + j] - m.m_[i * 4 + j];
                }
            }
            return tmp;
        }

        template <typename T>
        Matrix<T> Matrix<T>::operator*(const Matrix<T>& m)
        {
            Matrix<T> tmp;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    tmp.m_[i * 4 + j] = m_[i * 4 + 0] * m.m_[0 * 4 + j] + m_[i * 4 + 1] * m.m_[1 * 4 + j]
                        + m_[i * 4 + 2] * m.m_[2 * 4 + j] + m_[i * 4 + 3] * m.m_[3 * 4 + j];
                }
            }
            return tmp;
        }

        template <typename T>
        Matrix<T> Matrix<T>::operator*(const Matrix<T>&) const
        {
            Matrix<T> tmp;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    tmp.m_[i * 4 + j] = m_[i * 4 + 0] * m.m_[0 * 4 + j] + m_[i * 4 + 1] * m.m_[1 * 4 + j]
                        + m_[i * 4 + 2] * m.m_[2 * 4 + j] + m_[i * 4 + 3] * m.m_[3 * 4 + j];
                }
            }
            return tmp;
        }

        template <typename T>
        Matrix<T> Matrix<T>::operator*(const T& scale)
        {
            Matrix<T> tmp;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    tmp.m_[i * 4 + j] = m_[i * 4 + j] * scale;
                }
            }
            return tmp;
        }

        template <typename T>
        bool Matrix<T>::operator==(const Matrix<T>& other) const
        {
            for (int i = 0; i < 16; i++)
            {
                if (m_[i] != other.m_[i])
                {
                    return false;
                }
            }
            return true;
        }

        template <typename T>
        bool Matrix<T>::operator!=(const Matrix<T>& other) const
        {
            for (int i = 0; i < 16; i++)
            {
                if (m_[i] == other.m_[i])
                {
                    return false;
                }
            }
            return true;
        }

        template <typename T>
        void Matrix<T>::Zero()
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    m_[i * 4 + j] = T(0);
                }
            }
        }

        template <typename T>
        void Matrix<T>::Identity()
        {
            Zero();
            m_[0] = m_[5] = m_[10] = m_[15] = T(1);
        }

        template <typename T>
        T& Matrix<T>::operator()(int i, int j)
        {
            return m_[i * 4 + j];
        }

        template <typename T>
        const T& Matrix<T>::operator()(int i, int j) const
        {
            return m_[i * 4 + j];
        }

        template <typename T>
        T& Matrix<T>::operator[](int i)
        {
            return m_[i];
        }

        template <typename T>
        const T& Matrix<T>::operator[](int i) const
        {
            return m_[i];
        }

        template <typename T>
        Vector<T> Matrix<T>::Apply(const Vector<T>& vec)
        {
            Vector<T> tmp;
            for (int i = 0; i < 4; i++)
            {
                tmp(i) = vec(0) * m_[0 * 4 + i] + vec(1) * m_[1 * 4 + i] + vec(2) * m_[2 * 4 + i] + vec(3) * m_[3 * 4 + i];
            }
            return tmp;
        }

        template <typename T>
        void Matrix<T>::Translate(T x, T y, T z)
        {
            //Identity();
            m_[3 * 4 + 0] = x;
            m_[3 * 4 + 1] = y;
            m_[3 * 4 + 2] = z;
        }

        template <typename T>
        void Matrix<T>::Scale(T x, T y, T z)
        {
            //Identity();
            m_[0] = x;
            m_[5] = y;
            m_[10] = z;
        }

        template <typename T>
        void Matrix<T>::Rotate(T x, T y, T z, f32 theta)
        {
            //Identity();
            Vector<T> n(x, y, z);
            //n.Normalize();
            //f32 qsin = sin(theta);
            //f32 qcos = cos(theta);
            //T nx = n.X();
            //T ny = n.Y();
            //T nz = n.Z();
            //m_[0][0] = nx * (1 - qcos) + qcos;
            //m_[0][1] = nx * ny * (1 - qcos) - nz * qsin;
            //m_[0][2] = nx * nz * (1 - qcos) + ny * qsin;
            //m_[1][0] = nx * ny * (1 - qcos) + nz * qsin;
            //m_[1][1] = ny * ny * (1 - qcos) + qcos;
            //m_[1][2] = ny * nz * (1 - qcos) - nx * qsin;
            //m_[2][0] = nx * nz * (1 - qcos) - ny * qsin;
            //m_[2][1] = ny * nz * (1 - qcos) + nx * qsin;
            //m_[2][2] = nz * nz * (1 - qcos) + qcos;

            f32 qsin = (f32)sin(theta * 0.5f);
            f32 qcos = (f32)cos(theta * 0.5f);
            Vector<T> vec(x, y, z, 1.0f);
            f32 w = qcos;
            vec.Normalize();
            x = vec.x * qsin;
            y = vec.y * qsin;
            z = vec.z * qsin;
            m_[0 * 4 + 0] = 1 - 2 * y * y - 2 * z * z;
            m_[1 * 4 + 0] = 2 * x * y - 2 * w * z;
            m_[2 * 4 + 0] = 2 * x * z + 2 * w * y;
            m_[0 * 4 + 1] = 2 * x * y + 2 * w * z;
            m_[1 * 4 + 1] = 1 - 2 * x * x - 2 * z * z;
            m_[2 * 4 + 1] = 2 * y * z - 2 * w * x;
            m_[0 * 4 + 2] = 2 * x * z - 2 * w * y;
            m_[1 * 4 + 2] = 2 * y * z + 2 * w * x;
            m_[2 * 4 + 2] = 1 - 2 * x * x - 2 * y * y;
            m_[0 * 4 + 3] = m_[1 * 4 + 3] = m_[2 * 4 + 3] = 0.0f;
            m_[3 * 4 + 0] = m_[3 * 4 + 1] = m_[3 * 4 + 2] = 0.0f;
            m_[3 * 4 + 3] = 1.0f;
        }

        template <typename T>
        void Matrix<T>::Rotate(T x, T y, T z)
        {
            //Identity();

            const f64 cr = cos(x);
            const f64 sr = sin(x);
            const f64 cp = cos(y);
            const f64 sp = sin(y);
            const f64 cy = cos(z);
            const f64 sy = sin(z);

            m_[0 * 4 + 0] = static_cast<T>(cp * cy);
            m_[1 * 4 + 0] = static_cast<T>(cp * sy);
            m_[2 * 4 + 0] = static_cast<T>(-sp);

            const f64 srsp = sr*sp;
            const f64 crsp = cr*sp;

            m_[0 * 4 + 1] = static_cast<T>(srsp * cy - cr * sy);
            m_[1 * 4 + 1] = static_cast<T>(srsp * sy + cr * cy);
            m_[2 * 4 + 1] = static_cast<T>(sr * cp);

            m_[0 * 4 + 2] = static_cast<T>(crsp * cy + sr * sy);
            m_[1 * 4 + 2] = static_cast<T>(crsp * sy - sr * cy);
            m_[2 * 4 + 2] = static_cast<T>(cr * cp);
        }

        template <typename T>
        Matrix<T> Matrix<T>::Transpose() const
        {
            Matrix<T> tmp;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    tmp.m_[i * 4 + j] = m_[j * 4 + i];
                }
            }
            return tmp;
        }

        template <typename T>
        Matrix<T> Matrix<T>::Inverse() const
        {
            Matrix<T> tmp;
            tmp.Identity();
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    tmp.m_[i * 4 + j] = m_[j * 4 + i];
                }
            }
            tmp.m_[3 * 4 + 0] = -m_[3 * 4 + 0];
            tmp.m_[3 * 4 + 1] = -m_[3 * 4 + 1];
            tmp.m_[3 * 4 + 2] = -m_[3 * 4 + 2];
            return tmp;
        }

        template <typename T>
        Vector3Df Matrix<T>::GetTranslation() const
        {
            return Vector3Df(m_[3 * 4 + 0], m_[3 * 4 + 1], m_[3 * 4 + 2]);
        }

        template <typename T>
        const T* Matrix<T>::Pointer() const
        {
            return static_cast<const T*>(m_);
        }

        template <typename T>
        T* Matrix<T>::Pointer()
        {
            return static_cast<T*>(m_);
        }

        template <typename T>
        bool Matrix<T>::IsIdentity() const
        {
            Matrix<T> tmp(IDENTITY);
            return (*this) == tmp;
        }

        typedef Matrix<int> Matrixi;
        typedef Matrix<f32> Matrixf;

        //! global const identity matrix
        KONG_API extern const Matrixf identity_matrix;
    } // end namespace core
} // end namespace kong

#endif