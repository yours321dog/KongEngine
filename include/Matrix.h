// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "KongTypes.h"

namespace kong
{
    namespace core
    {
        template <typename T>
        class Matrix
        {
        public:
            Matrix(T m[4][4]);
            Matrix();
            Matrix(const Matrix &);

            void Zero();
            void Identity();

            T &operator()(int, int);
            Matrix<T> &operator=(const Matrix<T>&);
            Matrix<T> operator+(const Matrix<T>&);
            Matrix<T> operator-(const Matrix<T>&);
            Matrix<T> operator*(const Matrix<T>&);
            Matrix<T> operator*(const T&);

            Vector<T> Apply(const Vector<T> &);

            void Translate(T x, T y, T z);
            void Scale(T x, T y, T z);
            void Rotate(T x, T y, T z, float theta);
            Matrix<T> Transpose();

            Matrix<T> Inverse();

        private:
            T m_[4][4];
        };

        template <typename T>
        Matrix<T>::Matrix()
        {
            Zero();
        }

        template <typename T>
        Matrix<T>::Matrix(T m[4][4])
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    m_[i][j] = m[i][j];
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
                    m_[i][j] = m.m_[i][j];
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
                    m_[i][j] = m.m_[i][j];
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
                    tmp.m_[i][j] = m_[i][j] + m.m_[i][j];
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
                    tmp.m_[i][j] = m_[i][j] - m.m_[i][j];
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
                    tmp.m_[i][j] = m_[i][0] * m.m_[0][j] + m_[i][1] * m.m_[1][j] + m_[i][2] * m.m_[2][j] + m_[i][3] * m.m_[3][j];
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
                    tmp.m_[i][j] = m_[i][j] * scale;
                }
            }
            return tmp;
        }

        template <typename T>
        void Matrix<T>::Zero()
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    m_[i][j] = T(0);
                }
            }
        }

        template <typename T>
        void Matrix<T>::Identity()
        {
            m_[0][0] = m_[1][1] = m_[2][2] = m_[3][3] = T(1);
            m_[0][1] = m_[0][2] = m_[0][3] = T(0);
            m_[1][0] = m_[1][2] = m_[1][3] = T(0);
            m_[2][0] = m_[2][1] = m_[2][3] = T(0);
            m_[3][0] = m_[3][1] = m_[3][2] = T(0);
        }

        template <typename T>
        T& Matrix<T>::operator()(int i, int j)
        {
            return m_[i][j];
        }

        template <typename T>
        Vector<T> Matrix<T>::Apply(const Vector<T>& vec)
        {
            Vector<T> tmp;
            for (int i = 0; i < 4; i++)
            {
                tmp(i) = vec(0) * m_[0][i] + vec(1) * m_[1][i] + vec(2) * m_[2][i] + vec(3) * m_[3][i];
            }
            return tmp;
        }

        template <typename T>
        void Matrix<T>::Translate(T x, T y, T z)
        {
            Identity();
            m_[3][0] = x;
            m_[3][1] = y;
            m_[3][2] = z;
        }

        template <typename T>
        void Matrix<T>::Scale(T x, T y, T z)
        {
            Identity();
            m_[0][0] = x;
            m_[1][1] = y;
            m_[2][2] = z;
        }

        template <typename T>
        void Matrix<T>::Rotate(T x, T y, T z, float theta)
        {
            Identity();
            Vector<T> n(x, y, z);
            //n.Normalize();
            //float qsin = sin(theta);
            //float qcos = cos(theta);
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

            float qsin = (float)sin(theta * 0.5f);
            float qcos = (float)cos(theta * 0.5f);
            Vector<T> vec(x, y, z, 1.0f);
            float w = qcos;
            vec.Normalize();
            x = vec.x * qsin;
            y = vec.y * qsin;
            z = vec.z * qsin;
            m_[0][0] = 1 - 2 * y * y - 2 * z * z;
            m_[1][0] = 2 * x * y - 2 * w * z;
            m_[2][0] = 2 * x * z + 2 * w * y;
            m_[0][1] = 2 * x * y + 2 * w * z;
            m_[1][1] = 1 - 2 * x * x - 2 * z * z;
            m_[2][1] = 2 * y * z - 2 * w * x;
            m_[0][2] = 2 * x * z - 2 * w * y;
            m_[1][2] = 2 * y * z + 2 * w * x;
            m_[2][2] = 1 - 2 * x * x - 2 * y * y;
            m_[0][3] = m_[1][3] = m_[2][3] = 0.0f;
            m_[3][0] = m_[3][1] = m_[3][2] = 0.0f;
            m_[3][3] = 1.0f;
        }

        template <typename T>
        Matrix<T> Matrix<T>::Transpose()
        {
            Matrix<T> tmp;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    tmp.m_[i][j] = m_[j][i];
                }
            }
            return tmp;
        }

        template <typename T>
        Matrix<T> Matrix<T>::Inverse()
        {
            Matrix<T> tmp;
            tmp.Identity();
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    tmp.m_[i][j] = m_[j][i];
                }
            }
            tmp.m_[3][0] = -m_[3][0];
            tmp.m_[3][1] = -m_[3][1];
            tmp.m_[3][2] = -m_[3][2];
            return tmp;
        }

        typedef Matrix<int> Matrixi;
        typedef Matrix<float> Matrixf;

        //! global const identity matrix
        KONG_API extern const Matrixf IdentityMatrix;
    } // end namespace core
} // end namespace kong

#endif