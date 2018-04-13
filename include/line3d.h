// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_LINE_3D_H_INCLUDED__
#define __IRR_LINE_3D_H_INCLUDED__

#include "KongTypes.h"
#include "Vector.h"

namespace kong
{
    namespace core
    {

        //! 3D line between two points with intersection methods.
        template <class T>
        class line3d
        {
        public:

            //! Default constructor
            /** line from (0,0,0) to (1,1,1) */
            line3d() : start(0, 0, 0), end(1, 1, 1) {}
            //! Constructor with two points
            line3d(T xa, T ya, T za, T xb, T yb, T zb) : start(xa, ya, za), end(xb, yb, zb) {}
            //! Constructor with two points as vectors
            line3d(const Vector<T>& start, const Vector<T>& end) : start(start), end(end) {}

            // operators

            line3d<T> operator+(const Vector<T>& point) const { return line3d<T>(start + point, end + point); }
            line3d<T>& operator+=(const Vector<T>& point) { start += point; end += point; return *this; }

            line3d<T> operator-(const Vector<T>& point) const { return line3d<T>(start - point, end - point); }
            line3d<T>& operator-=(const Vector<T>& point) { start -= point; end -= point; return *this; }

            bool operator==(const line3d<T>& other) const
            {
                return (start == other.start && end == other.end) || (end == other.start && start == other.end);
            }
            bool operator!=(const line3d<T>& other) const
            {
                return !(start == other.start && end == other.end) || (end == other.start && start == other.end);
            }

            // functions
            //! Set this line to a new line going through the two points.
            void setLine(const T& xa, const T& ya, const T& za, const T& xb, const T& yb, const T& zb)
            {
                start.set(xa, ya, za); end.set(xb, yb, zb);
            }
            //! Set this line to a new line going through the two points.
            void setLine(const Vector<T>& nstart, const Vector<T>& nend)
            {
                start.set(nstart); end.set(nend);
            }
            //! Set this line to new line given as parameter.
            void setLine(const line3d<T>& line)
            {
                start.set(line.start); end.set(line.end);
            }

            //! Get length of line
            /** \return Length of line. */
            T getLength() const { return start.getDistanceFrom(end); }

            //! Get squared length of line
            /** \return Squared length of line. */
            T getLengthSQ() const { return start.getDistanceFromSQ(end); }

            //! Get middle of line
            /** \return Center of line. */
            Vector<T> getMiddle() const
            {
                return (start + end) / (T)2;
            }

            //! Get vector of line
            /** \return vector of line. */
            Vector<T> getVector() const
            {
                return end - start;
            }

            //! Check if the given point is between start and end of the line.
            /** Assumes that the point is already somewhere on the line.
            \param point The point to test.
            \return True if point is on the line between start and end, else false.
            */
            bool isPointBetweenStartAndEnd(const Vector<T>& point) const
            {
                return point.IsBetweenPoints(start, end);
            }

            //! Get the closest point on this line to a point
            /** \param point The point to compare to.
            \return The nearest point which is part of the line. */
            Vector<T> getClosestPoint(const Vector<T>& point) const
            {
                Vector<T> c = point - start;
                Vector<T> v = end - start;
                T d = (T)v.GetLength();
                v /= d;
                T t = v.DotProduct(c);

                if (t < (T)0.0)
                    return start;
                if (t > d)
                    return end;

                v *= t;
                return start + v;
            }

            //! Check if the line intersects with a shpere
            /** \param sorigin: Origin of the shpere.
            \param sradius: Radius of the sphere.
            \param outdistance: The distance to the first intersection point.
            \return True if there is an intersection.
            If there is one, the distance to the first intersection point
            is stored in outdistance. */
            bool getIntersectionWithSphere(Vector<T> sorigin, T sradius, f64& outdistance) const
            {
                const Vector<T> q = sorigin - start;
                T c = q.GetLength();
                T v = q.DotProduct(getVector().Normalize());
                T d = sradius * sradius - (c*c - v*v);

                if (d < 0.0)
                    return false;

                outdistance = v - core::squareroot(d);
                return true;
            }

            // member variables

            //! Start point of line
            Vector<T> start;
            //! End point of line
            Vector<T> end;
        };

        //! Typedef for an f32 line.
        typedef line3d<f32> line3df;
        //! Typedef for an integer line.
        typedef line3d<s32> line3di;

    } // end namespace core
} // end namespace irr

#endif

