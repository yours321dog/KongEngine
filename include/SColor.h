// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _SCOLOR_H_
#define _SCOLOR_H_
#include "KongTypes.h"
#include "KongMath.h"

namespace kong
{
    namespace video
    {
        //! An enum for the color format of textures used by the Irrlicht Engine.
        /** A color format specifies how color information is stored. */
        enum ECOLOR_FORMAT
        {
            //! 16 bit color format used by the software driver.
            /** It is thus preferred by all other irrlicht engine video drivers.
            There are 5 bits for every color component, and a single bit is left
            for alpha information. */
            ECF_A1R5G5B5 = 0,

            //! Standard 16 bit color format.
            ECF_R5G6B5,

            //! 24 bit color, no alpha channel, but 8 bit for red, green and blue.
            ECF_R8G8B8,

            //! Default 32 bit color format. 8 bits are used for every component: red, green, blue and alpha.
            ECF_A8R8G8B8,

            /** Floating Point formats. The following formats may only be used for render target textures. */

            //! 16 bit floating point format using 16 bits for the red channel.
            ECF_R16F,

            //! 32 bit floating point format using 16 bits for the red channel and 16 bits for the green channel.
            ECF_G16R16F,

            //! 64 bit floating point format 16 bits are used for the red, green, blue and alpha channels.
            ECF_A16B16G16R16F,

            //! 32 bit floating point format using 32 bits for the red channel.
            ECF_R32F,

            //! 64 bit floating point format using 32 bits for the red channel and 32 bits for the green channel.
            ECF_G32R32F,

            //! 128 bit floating point format. 32 bits are used for the red, green, blue and alpha channels.
            ECF_A32B32G32R32F,

            //! Unknown color format:
            ECF_UNKNOWN
        };

        //! Creates a 16 bit A1R5G5B5 color
        inline u16 RGBA16(u32 r, u32 g, u32 b, u32 a = 0xFF)
        {
            return (u16)((a & 0x80) << 8 |
                (r & 0xF8) << 7 |
                (g & 0xF8) << 2 |
                (b & 0xF8) >> 3);
        }


        //! Creates a 16 bit A1R5G5B5 color
        inline u16 RGB16(u32 r, u32 g, u32 b)
        {
            return RGBA16(r, g, b);
        }


        //! Creates a 16bit A1R5G5B5 color, based on 16bit input values
        inline u16 RGB16from16(u16 r, u16 g, u16 b)
        {
            return (0x8000 |
                (r & 0x1F) << 10 |
                (g & 0x1F) << 5 |
                (b & 0x1F));
        }


        //! Converts a 32bit (X8R8G8B8) color to a 16bit A1R5G5B5 color
        inline u16 X8R8G8B8toA1R5G5B5(u32 color)
        {
            return (u16)(0x8000 |
                (color & 0x00F80000) >> 9 |
                (color & 0x0000F800) >> 6 |
                (color & 0x000000F8) >> 3);
        }


        //! Converts a 32bit (A8R8G8B8) color to a 16bit A1R5G5B5 color
        inline u16 A8R8G8B8toA1R5G5B5(u32 color)
        {
            return (u16)((color & 0x80000000) >> 16 |
                (color & 0x00F80000) >> 9 |
                (color & 0x0000F800) >> 6 |
                (color & 0x000000F8) >> 3);
        }


        //! Converts a 32bit (A8R8G8B8) color to a 16bit R5G6B5 color
        inline u16 A8R8G8B8toR5G6B5(u32 color)
        {
            return (u16)((color & 0x00F80000) >> 8 |
                (color & 0x0000FC00) >> 5 |
                (color & 0x000000F8) >> 3);
        }


        //! Convert A8R8G8B8 Color from A1R5G5B5 color
        /** build a nicer 32bit Color by extending dest lower bits with source high bits. */
        inline u32 A1R5G5B5toA8R8G8B8(u16 color)
        {
            return (((-((s32)color & 0x00008000) >> (s32)31) & 0xFF000000) |
                ((color & 0x00007C00) << 9) | ((color & 0x00007000) << 4) |
                ((color & 0x000003E0) << 6) | ((color & 0x00000380) << 1) |
                ((color & 0x0000001F) << 3) | ((color & 0x0000001C) >> 2)
                );
        }


        //! Returns A8R8G8B8 Color from R5G6B5 color
        inline u32 R5G6B5toA8R8G8B8(u16 color)
        {
            return 0xFF000000 |
                ((color & 0xF800) << 8) |
                ((color & 0x07E0) << 5) |
                ((color & 0x001F) << 3);
        }


        //! Returns A1R5G5B5 Color from R5G6B5 color
        inline u16 R5G6B5toA1R5G5B5(u16 color)
        {
            return 0x8000 | (((color & 0xFFC0) >> 1) | (color & 0x1F));
        }


        //! Returns R5G6B5 Color from A1R5G5B5 color
        inline u16 A1R5G5B5toR5G6B5(u16 color)
        {
            return (((color & 0x7FE0) << 1) | (color & 0x1F));
        }



        //! Returns the alpha component from A1R5G5B5 color
        /** In Irrlicht, alpha refers to opacity.
        \return The alpha value of the color. 0 is transparent, 1 is opaque. */
        inline u32 getAlpha(u16 color)
        {
            return ((color >> 15) & 0x1);
        }


        //! Returns the red component from A1R5G5B5 color.
        /** Shift left by 3 to get 8 bit value. */
        inline u32 getRed(u16 color)
        {
            return ((color >> 10) & 0x1F);
        }


        //! Returns the green component from A1R5G5B5 color
        /** Shift left by 3 to get 8 bit value. */
        inline u32 getGreen(u16 color)
        {
            return ((color >> 5) & 0x1F);
        }


        //! Returns the blue component from A1R5G5B5 color
        /** Shift left by 3 to get 8 bit value. */
        inline u32 getBlue(u16 color)
        {
            return (color & 0x1F);
        }


        //! Returns the average from a 16 bit A1R5G5B5 color
        inline s32 getAverage(s16 color)
        {
            return ((getRed(color) << 3) + (getGreen(color) << 3) + (getBlue(color) << 3)) / 3;
        }

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
            void SetBlue(u32 b) { color_ = (b & 0xff) | (color_ & 0xffffff00); }

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

            bool operator==(const SColor &other) const
            {
                return color_ == other.color_;
            }

            bool operator!=(const SColor &other) const
            {
                return color_ != other.color_;
            }

            void Interpolation(const SColor &clr1, const SColor &clr2, f32 t)
            {
                t = core::clamp(t, 0.f, 1.f);
                SetAlpha(core::lerp(clr1.GetAlpha(), clr2.GetAlpha(), t));
                SetRed(core::lerp(clr1.GetRed(), clr2.GetRed(), t));
                SetGreen(core::lerp(clr1.GetGreen(), clr2.GetGreen(), t));
                SetBlue(core::lerp(clr1.GetBlue(), clr2.GetBlue(), t));
            }

            //! Calculates a 16 bit A1R5G5B5 value of this color.
            /** \return 16 bit A1R5G5B5 value of this color. */
            u16 ToA1R5G5B5() const
            {
                return A8R8G8B8toA1R5G5B5(color_);
            }

        //private:
            u32 color_;
        };
    } // end namespace video
} // end namespace kong

#endif