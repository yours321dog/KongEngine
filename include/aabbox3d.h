// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_AABBOX_3D_H_INCLUDED__
#define __IRR_AABBOX_3D_H_INCLUDED__

#include "KongMath.h"
#include "plane3d.h"
#include "line3d.h"

namespace kong
{
    namespace core
    {

        //! Axis aligned bounding box in 3d dimensional space.
        /** Has some useful methods used with occlusion culling or clipping.
        */
        template <class T>
        class aabbox3d
        {
        public:

            //! Default Constructor.
            aabbox3d() : MinEdge(-1, -1, -1), MaxEdge(1, 1, 1) {}
            //! Constructor with min edge and max edge.
            aabbox3d(const Vector<T>& min, const Vector<T>& max) : MinEdge(min), MaxEdge(max) {}
            //! Constructor with only one point.
            aabbox3d(const Vector<T>& init) : MinEdge(init), MaxEdge(init * -1) {}
            //! Constructor with min edge and max edge as single values, not vectors.
            aabbox3d(T minx, T miny, T minz, T maxx, T maxy, T maxz) : MinEdge(minx, miny, minz), MaxEdge(maxx, maxy, maxz) {}
            //! Copy constructor
            aabbox3d(const aabbox3d<T> &other) { MinEdge = other.MinEdge; MaxEdge = other.MaxEdge; }

            // operators
            //! Equality operator
            /** \param other box to compare with.
            \return True if both boxes are equal, else false. */
            inline bool operator==(const aabbox3d<T>& other) const { return (MinEdge == other.MinEdge && other.MaxEdge == MaxEdge); }
            //! Inequality operator
            /** \param other box to compare with.
            \return True if both boxes are different, else false. */
            inline bool operator!=(const aabbox3d<T>& other) const { return !(MinEdge == other.MinEdge && other.MaxEdge == MaxEdge); }

            // functions

            //! Resets the bounding box to a one-point box.
            /** \param x X coord of the point.
            \param y Y coord of the point.
            \param z Z coord of the point. */
            void reset(T x, T y, T z)
            {
                MaxEdge.Set(x, y, z);
                MinEdge = MaxEdge;
            }

            //! Resets the bounding box.
            /** \param initValue New box to set this one to. */
            void reset(const aabbox3d<T>& initValue)
            {
                *this = initValue;
            }

            //! Resets the bounding box to a one-point box.
            /** \param initValue New point. */
            void reset(const Vector<T>& initValue)
            {
                MaxEdge = initValue;
                MinEdge = initValue;
            }

            //! Adds a point to the bounding box
            /** The box grows bigger, if point was outside of the box.
            \param p: Point to add into the box. */
            void addInternalPoint(const Vector<T>& p)
            {
                addInternalPoint(p.x_, p.y_, p.z_);
            }

            //! Adds another bounding box
            /** The box grows bigger, if the new box was outside of the box.
            \param b: Other bounding box to add into this box. */
            void addInternalBox(const aabbox3d<T>& b)
            {
                addInternalPoint(b.MaxEdge);
                addInternalPoint(b.MinEdge);
            }

            //! Adds a point to the bounding box
            /** The box grows bigger, if point is outside of the box.
            \param x X coordinate of the point to add to this box.
            \param y Y coordinate of the point to add to this box.
            \param z Z coordinate of the point to add to this box. */
            void addInternalPoint(T x, T y, T z)
            {
                if (x>MaxEdge.x_) MaxEdge.x_ = x;
                if (y>MaxEdge.y_) MaxEdge.y_ = y;
                if (z>MaxEdge.z_) MaxEdge.z_ = z;

                if (x<MinEdge.x_) MinEdge.x_ = x;
                if (y<MinEdge.y_) MinEdge.y_ = y;
                if (z<MinEdge.z_) MinEdge.z_ = z;
            }

            //! Get center of the bounding box
            /** \return Center of the bounding box. */
            Vector<T> getCenter() const
            {
                return (MinEdge + MaxEdge) / 2;
            }

            //! Get extent of the box (maximal distance of two points in the box)
            /** \return Extent of the bounding box. */
            Vector<T> getExtent() const
            {
                return MaxEdge - MinEdge;
            }

            //! Check if the box is empty.
            /** This means that there is no space between the min and max edge.
            \return True if box is empty, else false. */
            bool isEmpty() const
            {
                return MinEdge.equals(MaxEdge);
            }

            //! Get the volume enclosed by the box in cubed units
            T getVolume() const
            {
                const Vector<T> e = getExtent();
                return e.x_ * e.y_ * e.z_;
            }

            //! Get the surface area of the box in squared units
            T getArea() const
            {
                const Vector<T> e = getExtent();
                return 2 * (e.x_*e.y_ + e.x_*e.z_ + e.y_*e.z_);
            }

            //! Stores all 8 edges of the box into an array
            /** \param edges: Pointer to array of 8 edges. */
            void getEdges(Vector<T> *edges) const
            {
                const core::Vector<T> middle = getCenter();
                const core::Vector<T> diag = middle - MaxEdge;

                /*
                Edges are stored in this way:
                Hey, am I an ascii artist, or what? :) niko.
                /3--------/7
                / |       / |
                /  |      /  |
                1---------5   |
                |  /2- - -|- -6
                | /       |  /
                |/        | /
                0---------4/
                */

                edges[0].set(middle.x_ + diag.x_, middle.y_ + diag.y_, middle.z_ + diag.z_);
                edges[1].set(middle.x_ + diag.x_, middle.y_ - diag.y_, middle.z_ + diag.z_);
                edges[2].set(middle.x_ + diag.x_, middle.y_ + diag.y_, middle.z_ - diag.z_);
                edges[3].set(middle.x_ + diag.x_, middle.y_ - diag.y_, middle.z_ - diag.z_);
                edges[4].set(middle.x_ - diag.x_, middle.y_ + diag.y_, middle.z_ + diag.z_);
                edges[5].set(middle.x_ - diag.x_, middle.y_ - diag.y_, middle.z_ + diag.z_);
                edges[6].set(middle.x_ - diag.x_, middle.y_ + diag.y_, middle.z_ - diag.z_);
                edges[7].set(middle.x_ - diag.x_, middle.y_ - diag.y_, middle.z_ - diag.z_);
            }

            //! Repairs the box.
            /** Necessary if for example MinEdge and MaxEdge are swapped. */
            void repair()
            {
                T t;

                if (MinEdge.x_ > MaxEdge.x_)
                {
                    t = MinEdge.x_; MinEdge.x_ = MaxEdge.x_; MaxEdge.x_ = t;
                }
                if (MinEdge.y_ > MaxEdge.y_)
                {
                    t = MinEdge.y_; MinEdge.y_ = MaxEdge.y_; MaxEdge.y_ = t;
                }
                if (MinEdge.z_ > MaxEdge.z_)
                {
                    t = MinEdge.z_; MinEdge.z_ = MaxEdge.z_; MaxEdge.z_ = t;
                }
            }

            //! Calculates a new interpolated bounding box.
            /** d=0 returns other, d=1 returns this, all other values blend between
            the two boxes.
            \param other Other box to interpolate between
            \param d Value between 0.0f and 1.0f.
            \return Interpolated box. */
            aabbox3d<T> getInterpolated(const aabbox3d<T>& other, f32 d) const
            {
                f32 inv = 1.0f - d;
                return aabbox3d<T>((other.MinEdge*inv) + (MinEdge*d),
                    (other.MaxEdge*inv) + (MaxEdge*d));
            }

            //! Determines if a point is within this box.
            /** Border is included (IS part of the box)!
            \param p: Point to check.
            \return True if the point is within the box and false if not */
            bool isPointInside(const Vector<T>& p) const
            {
                return (p.x_ >= MinEdge.x_ && p.x_ <= MaxEdge.x_ &&
                    p.y_ >= MinEdge.y_ && p.y_ <= MaxEdge.y_ &&
                    p.z_ >= MinEdge.z_ && p.z_ <= MaxEdge.z_);
            }

            //! Determines if a point is within this box and not its borders.
            /** Border is excluded (NOT part of the box)!
            \param p: Point to check.
            \return True if the point is within the box and false if not. */
            bool isPointTotalInside(const Vector<T>& p) const
            {
                return (p.x_ > MinEdge.x_ && p.x_ < MaxEdge.x_ &&
                    p.y_ > MinEdge.y_ && p.y_ < MaxEdge.y_ &&
                    p.z_ > MinEdge.z_ && p.z_ < MaxEdge.z_);
            }

            //! Check if this box is completely inside the 'other' box.
            /** \param other: Other box to check against.
            \return True if this box is completly inside the other box,
            otherwise false. */
            bool isFullInside(const aabbox3d<T>& other) const
            {
                return (MinEdge.x_ >= other.MinEdge.x_ && MinEdge.y_ >= other.MinEdge.y_ && MinEdge.z_ >= other.MinEdge.z_ &&
                    MaxEdge.x_ <= other.MaxEdge.x_ && MaxEdge.y_ <= other.MaxEdge.y_ && MaxEdge.z_ <= other.MaxEdge.z_);
            }

            //! Determines if the axis-aligned box intersects with another axis-aligned box.
            /** \param other: Other box to check a intersection with.
            \return True if there is an intersection with the other box,
            otherwise false. */
            bool intersectsWithBox(const aabbox3d<T>& other) const
            {
                return (MinEdge.x_ <= other.MaxEdge.x_ && MinEdge.y_ <= other.MaxEdge.y_ && MinEdge.z_ <= other.MaxEdge.z_ &&
                    MaxEdge.x_ >= other.MinEdge.x_ && MaxEdge.y_ >= other.MinEdge.y_ && MaxEdge.z_ >= other.MinEdge.z_);
            }

            //! Tests if the box intersects with a line
            /** \param line: Line to test intersection with.
            \return True if there is an intersection , else false. */
            bool intersectsWithLine(const line3d<T>& line) const
            {
                return intersectsWithLine(line.getMiddle(), line.getVector().normalize(),
                    (T)(line.getLength() * 0.5));
            }

            //! Tests if the box intersects with a line
            /** \param linemiddle Center of the line.
            \param linevect Vector of the line.
            \param halflength Half length of the line.
            \return True if there is an intersection, else false. */
            bool intersectsWithLine(const Vector<T>& linemiddle,
                const Vector<T>& linevect, T halflength) const
            {
                const Vector<T> e = getExtent() * (T)0.5;
                const Vector<T> t = getCenter() - linemiddle;

                if ((fabs(t.x_) > e.x_ + halflength * fabs(linevect.x_)) ||
                    (fabs(t.y_) > e.y_ + halflength * fabs(linevect.y_)) ||
                    (fabs(t.z_) > e.z_ + halflength * fabs(linevect.z_)))
                    return false;

                T r = e.y_ * (T)fabs(linevect.z_) + e.z_ * (T)fabs(linevect.y_);
                if (fabs(t.y_*linevect.z_ - t.z_*linevect.y_) > r)
                    return false;

                r = e.x_ * (T)fabs(linevect.z_) + e.z_ * (T)fabs(linevect.x_);
                if (fabs(t.z_*linevect.x_ - t.x_*linevect.z_) > r)
                    return false;

                r = e.x_ * (T)fabs(linevect.y_) + e.y_ * (T)fabs(linevect.x_);
                if (fabs(t.x_*linevect.y_ - t.y_*linevect.x_) > r)
                    return false;

                return true;
            }

            //! Classifies a relation with a plane.
            /** \param plane Plane to classify relation to.
            \return Returns ISREL3D_FRONT if the box is in front of the plane,
            ISREL3D_BACK if the box is behind the plane, and
            ISREL3D_CLIPPED if it is on both sides of the plane. */
            EIntersectionRelation3D classifyPlaneRelation(const plane3d<T>& plane) const
            {
                Vector<T> nearPoint(MaxEdge);
                Vector<T> farPoint(MinEdge);

                if (plane.Normal.x_ > (T)0)
                {
                    nearPoint.x_ = MinEdge.x_;
                    farPoint.x_ = MaxEdge.x_;
                }

                if (plane.Normal.y_ > (T)0)
                {
                    nearPoint.y_ = MinEdge.y_;
                    farPoint.y_ = MaxEdge.y_;
                }

                if (plane.Normal.z_ > (T)0)
                {
                    nearPoint.z_ = MinEdge.z_;
                    farPoint.z_ = MaxEdge.z_;
                }

                if (plane.Normal.dotProduct(nearPoint) + plane.D > (T)0)
                    return ISREL3D_FRONT;

                if (plane.Normal.dotProduct(farPoint) + plane.D > (T)0)
                    return ISREL3D_CLIPPED;

                return ISREL3D_BACK;
            }

            //! The near edge
            Vector<T> MinEdge;

            //! The far edge
            Vector<T> MaxEdge;
        };

        //! Typedef for a f32 3d bounding box.
        typedef aabbox3d<f32> aabbox3df;
        //! Typedef for an integer 3d bounding box.
        typedef aabbox3d<s32> aabbox3di;

    } // end namespace core
} // end namespace irr

#endif

