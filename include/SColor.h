// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _SCOLOR_H_
#define _SCOLOR_H_
#include "KongTypes.h"

namespace kong
{
    namespace video
    {
        class SColor
        {
        public:
            SColor()
                : color_(0) {}

            //! Constructs the color from 4 values representing the alpha, red, green and blue component.
            /** Must be values between 0 and 255. */
            SColor(u32 a, u32 r, u32 g, u32 b)
                : color_(((a & 0xff) << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff)) {}

            //! Constructs the color from a 32 bit value. Could be another color.
            SColor(u32 clr)
                : color_(clr) {}

            SColor(const SColor &other)
                : color_(((other.GetAlpha() & 0xff) << 24) | ((other.GetRed() & 0xff) << 16) | ((other.GetGreen() & 0xff) << 8) | (other.GetBlue() & 0xff)) {}

            //! Returns the alpha component of the color.
            /** The alpha component defines how opaque a color is.
            \return The alpha value of the color. 0 is fully transparent, 255 is fully opaque. */
            u32 GetAlpha() const { return color_ >> 24; }

            //! Returns the red component of the color.
            /** \return Value between 0 and 255, specifying how red the color is.
            0 means no red, 255 means full red. */
            u32 GetRed() const { return (color_ >> 16) & 0xff; }

            //! Returns the green component of the color.
            /** \return Value between 0 and 255, specifying how green the color is.
            0 means no green, 255 means full green. */
            u32 GetGreen() const { return (color_ >> 8) & 0xff; }

            //! Returns the blue component of the color.
            /** \return Value between 0 and 255, specifying how blue the color is.
            0 means no blue, 255 means full blue. */
            u32 GetBlue() const { return color_ & 0xff; }

            //! Get average intensity of the color in the range [0,255].
            u32 getAverage() const
            {
                return (GetRed() + GetGreen() + GetBlue()) / 3;
            }

            //! Sets the alpha component of the Color.
            /** The alpha component defines how transparent a color should be.
            \param a The alpha value of the color. 0 is fully transparent, 255 is fully opaque. */
            void SetAlpha(u32 a) { color_ = ((a & 0xff) << 24) | (color_ & 0x00ffffff); }

            //! Sets the red component of the Color.
            /** \param r: Has to be a value between 0 and 255.
            0 means no red, 255 means full red. */
            void SetRed(u32 r) { color_ = ((r & 0xff) << 16) | (color_ & 0xff00ffff); }

            //! Sets the green component of the Color.
            /** \param g: Has to be a value between 0 and 255.
            0 means no green, 255 means full green. */
            void SetGreen(u32 g) { color_ = ((g & 0xff) << 8) | (color_ & 0xffff00ff); }

            //! Sets the blue component of the Color.
            /** \param b: Has to be a value between 0 and 255.
            0 means no blue, 255 means full blue. */
            void setBlue(u32 b) { color_ = (b & 0xff) | (color_ & 0xffffff00); }

            void Set(u32 col) { color_ = col; }

            void Set(u32 a, u32 r, u32 g, u32 b)
            {
                color_ = (((a & 0xff) << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff));
            }

            //! Converts color to OpenGL color format
            /** From ARGB to RGBA in 4 byte components for endian aware
            passing to OpenGL
            \param dest: address where the 4x8 bit OpenGL color is stored. */
            void ToOpenGLColor(u8* dest) const
            {
                *dest = static_cast<u8>(GetRed());
                *++dest = static_cast<u8>(GetGreen());
                *++dest = static_cast<u8>(GetBlue());
                *++dest = static_cast<u8>(GetAlpha());
            }

        private:
            u32 color_;
        };
    } // end namespace video
} // end namespace kong

#endif