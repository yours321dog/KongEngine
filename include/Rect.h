// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __KONG_RECT_H_INCLUDED__
#define __KONG_RECT_H_INCLUDED__

#include "kongTypes.h"
#include "dimension2d.h"
#include "Position2d.h"

namespace kong
{
    namespace core
    {

        //! Rectangle template.
        /** Mostly used by 2D GUI elements and for 2D drawing methods.
        It has 2 positions instead of position and dimension and a fast
        method for collision detection with other rectangles and points.

        Coordinates are (0,0) for top-left corner, and increasing to the right
        and to the bottom.
        */
        template <class T>
        class rect
        {
        public:

            //! Default constructor creating empty rectangle at (0,0)
            rect() : UpperLeftCorner(0, 0), LowerRightCorner(0, 0) {}

            //! Constructor with two corners
            rect(T x, T y, T x2, T y2)
                : UpperLeftCorner(x, y), LowerRightCorner(x2, y2) {}

            //! Constructor with two corners
            rect(const position2d<T>& upperLeft, const position2d<T>& lowerRight)
                : UpperLeftCorner(upperLeft), LowerRightCorner(lowerRight) {}

            //! Constructor with upper left corner and dimension
            template <class U>
            rect(const position2d<T>& pos, const Dimension2d<U>& size)
                : UpperLeftCorner(pos), LowerRightCorner(pos.X + size.Width, pos.Y + size.Height) {}

            //! move right by given numbers
            rect<T> operator+(const position2d<T>& pos) const
            {
                rect<T> ret(*this);
                return ret += pos;
            }

            //! move right by given numbers
            rect<T>& operator+=(const position2d<T>& pos)
            {
                UpperLeftCorner += pos;
                LowerRightCorner += pos;
                return *this;
            }

            //! move left by given numbers
            rect<T> operator-(const position2d<T>& pos) const
            {
                rect<T> ret(*this);
                return ret -= pos;
            }

            //! move left by given numbers
            rect<T>& operator-=(const position2d<T>& pos)
            {
                UpperLeftCorner -= pos;
                LowerRightCorner -= pos;
                return *this;
            }

            //! equality operator
            bool operator==(const rect<T>& other) const
            {
                return (UpperLeftCorner == other.UpperLeftCorner &&
                    LowerRightCorner == other.LowerRightCorner);
            }

            //! inequality operator
            bool operator!=(const rect<T>& other) const
            {
                return (UpperLeftCorner != other.UpperLeftCorner ||
                    LowerRightCorner != other.LowerRightCorner);
            }

            //! compares size of rectangles
            bool operator<(const rect<T>& other) const
            {
                return getArea() < other.getArea();
            }

            //! Returns size of rectangle
            T getArea() const
            {
                return getWidth() * getHeight();
            }

            //! Returns if a 2d point is within this rectangle.
            /** \param pos Position to test if it lies within this rectangle.
            \return True if the position is within the rectangle, false if not. */
            bool isPointInside(const position2d<T>& pos) const
            {
                return (UpperLeftCorner.x_ <= pos.X &&
                    UpperLeftCorner.y_ <= pos.Y &&
                    LowerRightCorner.x_ >= pos.X &&
                    LowerRightCorner.y_ >= pos.Y);
            }

            //! Check if the rectangle collides with another rectangle.
            /** \param other Rectangle to test collision with
            \return True if the rectangles collide. */
            bool isRectCollided(const rect<T>& other) const
            {
                return (LowerRightCorner.y_ > other.UpperLeftCorner.y_ &&
                    UpperLeftCorner.y_ < other.LowerRightCorner.y_ &&
                    LowerRightCorner.x_ > other.UpperLeftCorner.x_ &&
                    UpperLeftCorner.x_ < other.LowerRightCorner.x_);
            }

            //! Clips this rectangle with another one.
            /** \param other Rectangle to clip with */
            void clipAgainst(const rect<T>& other)
            {
                if (other.LowerRightCorner.x_ < LowerRightCorner.x_)
                    LowerRightCorner.x_ = other.LowerRightCorner.x_;
                if (other.LowerRightCorner.y_ < LowerRightCorner.y_)
                    LowerRightCorner.y_ = other.LowerRightCorner.y_;

                if (other.UpperLeftCorner.x_ > UpperLeftCorner.x_)
                    UpperLeftCorner.x_ = other.UpperLeftCorner.x_;
                if (other.UpperLeftCorner.y_ > UpperLeftCorner.y_)
                    UpperLeftCorner.y_ = other.UpperLeftCorner.y_;

                // correct possible invalid rect resulting from clipping
                if (UpperLeftCorner.y_ > LowerRightCorner.y_)
                    UpperLeftCorner.y_ = LowerRightCorner.y_;
                if (UpperLeftCorner.x_ > LowerRightCorner.x_)
                    UpperLeftCorner.x_ = LowerRightCorner.x_;
            }

            //! Moves this rectangle to fit inside another one.
            /** \return True on success, false if not possible */
            bool constrainTo(const rect<T>& other)
            {
                if (other.getWidth() < getWidth() || other.getHeight() < getHeight())
                    return false;

                T diff = other.LowerRightCorner.x_ - LowerRightCorner.x_;
                if (diff < 0)
                {
                    LowerRightCorner.x_ += diff;
                    UpperLeftCorner.x_ += diff;
                }

                diff = other.LowerRightCorner.y_ - LowerRightCorner.y_;
                if (diff < 0)
                {
                    LowerRightCorner.y_ += diff;
                    UpperLeftCorner.y_ += diff;
                }

                diff = UpperLeftCorner.x_ - other.UpperLeftCorner.x_;
                if (diff < 0)
                {
                    UpperLeftCorner.x_ -= diff;
                    LowerRightCorner.x_ -= diff;
                }

                diff = UpperLeftCorner.y_ - other.UpperLeftCorner.y_;
                if (diff < 0)
                {
                    UpperLeftCorner.y_ -= diff;
                    LowerRightCorner.y_ -= diff;
                }

                return true;
            }

            //! Get width of rectangle.
            T getWidth() const
            {
                return LowerRightCorner.x_ - UpperLeftCorner.x_;
            }

            //! Get height of rectangle.
            T getHeight() const
            {
                return LowerRightCorner.y_ - UpperLeftCorner.y_;
            }

            //! If the lower right corner of the rect is smaller then the upper left, the points are swapped.
            void repair()
            {
                if (LowerRightCorner.x_ < UpperLeftCorner.x_)
                {
                    T t = LowerRightCorner.x_;
                    LowerRightCorner.x_ = UpperLeftCorner.x_;
                    UpperLeftCorner.x_ = t;
                }

                if (LowerRightCorner.y_ < UpperLeftCorner.y_)
                {
                    T t = LowerRightCorner.y_;
                    LowerRightCorner.y_ = UpperLeftCorner.y_;
                    UpperLeftCorner.y_ = t;
                }
            }

            //! Returns if the rect is valid to draw.
            /** It would be invalid if the UpperLeftCorner is lower or more
            right than the LowerRightCorner. */
            bool isValid() const
            {
                return ((LowerRightCorner.x_ >= UpperLeftCorner.x_) &&
                    (LowerRightCorner.y_ >= UpperLeftCorner.y_));
            }

            //! Get the center of the rectangle
            position2d<T> getCenter() const
            {
                return position2d<T>(
                    (UpperLeftCorner.x_ + LowerRightCorner.x_) / 2,
                    (UpperLeftCorner.y_ + LowerRightCorner.y_) / 2);
            }

            //! Get the dimensions of the rectangle
            Dimension2d<T> getSize() const
            {
                return Dimension2d<T>(getWidth(), getHeight());
            }


            //! Adds a point to the rectangle
            /** Causes the rectangle to grow bigger if point is outside of
            the box
            \param p Point to add to the box. */
            void addInternalPoint(const position2d<T>& p)
            {
                addInternalPoint(p.X, p.Y);
            }

            //! Adds a point to the bounding rectangle
            /** Causes the rectangle to grow bigger if point is outside of
            the box
            \param x X-Coordinate of the point to add to this box.
            \param y Y-Coordinate of the point to add to this box. */
            void addInternalPoint(T x, T y)
            {
                if (x>LowerRightCorner.x_)
                    LowerRightCorner.x_ = x;
                if (y>LowerRightCorner.y_)
                    LowerRightCorner.y_ = y;

                if (x<UpperLeftCorner.x_)
                    UpperLeftCorner.x_ = x;
                if (y<UpperLeftCorner.y_)
                    UpperLeftCorner.y_ = y;
            }

            //! Upper left corner
            position2d<T> UpperLeftCorner;
            //! Lower right corner
            position2d<T> LowerRightCorner;
        };

        //! Rectangle with float values
        typedef rect<f32> rectf;
        //! Rectangle with int values
        typedef rect<s32> recti;

    } // end namespace core
} // end namespace irr

#endif

