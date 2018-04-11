// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CImage.h"
//#include "irrString.h"
#include "CColorConverter.h"
#include "CBlit.h"
#include <cstring>

namespace kong
{
    namespace video
    {

        //! Constructor of empty image
        CImage::CImage(ECOLOR_FORMAT format, const core::Dimension2d<u32>& size)
            :data_(nullptr), size_(size), format_(format), delete_memory_(true)
        {
            InitData();
        }


        //! Constructor from raw data
        CImage::CImage(ECOLOR_FORMAT format, const core::Dimension2d<u32>& size, void* data,
            bool ownForeignMemory, bool deleteForeignMemory)
            : data_(nullptr), size_(size), format_(format), delete_memory_(deleteForeignMemory)
        {
            if (ownForeignMemory)
            {
                data_ = (u8*)0xbadf00d;
                InitData();
                data_ = (u8*)data;
            }
            else
            {
                data_ = 0;
                InitData();
                memcpy(data_, data, size_.height_ * pitch_);
            }
        }


        //! assumes format and size has been set and creates the rest
        void CImage::InitData()
        {
#ifdef _DEBUG
            //SetDebugName("CImage");
#endif
            bytes_per_pixel_ = GetBitsPerPixelFromFormat(format_) / 8;

            // Pitch should be aligned...
            pitch_ = bytes_per_pixel_ * size_.width_;

            if (!data_)
            {
                delete_memory_ = true;
                data_ = new u8[size_.height_ * pitch_];
            }
        }


        //! destructor
        CImage::~CImage()
        {
            if (delete_memory_)
                delete[] data_;
        }


        //! Returns width and height of image data.
        const core::Dimension2d<u32>& CImage::GetDimension() const
        {
            return size_;
        }


        //! Returns bits per pixel.
        u32 CImage::GetBitsPerPixel() const
        {
            return GetBitsPerPixelFromFormat(format_);
        }


        //! Returns bytes per pixel
        u32 CImage::GetBytesPerPixel() const
        {
            return bytes_per_pixel_;
        }


        //! Returns image data size in bytes
        u32 CImage::GetImageDataSizeInBytes() const
        {
            return pitch_ * size_.height_;
        }


        //! Returns image data size in pixels
        u32 CImage::GetImageDataSizeInPixels() const
        {
            return size_.width_ * size_.height_;
        }


        //! returns mask for red value of a pixel
        u32 CImage::GetRedMask() const
        {
            switch (format_)
            {
            case ECF_A1R5G5B5:
                return 0x1F << 10;
            case ECF_R5G6B5:
                return 0x1F << 11;
            case ECF_R8G8B8:
                return 0x00FF0000;
            case ECF_A8R8G8B8:
                return 0x00FF0000;
            default:
                return 0x0;
            }
        }


        //! returns mask for green value of a pixel
        u32 CImage::GetGreenMask() const
        {
            switch (format_)
            {
            case ECF_A1R5G5B5:
                return 0x1F << 5;
            case ECF_R5G6B5:
                return 0x3F << 5;
            case ECF_R8G8B8:
                return 0x0000FF00;
            case ECF_A8R8G8B8:
                return 0x0000FF00;
            default:
                return 0x0;
            }
        }


        //! returns mask for blue value of a pixel
        u32 CImage::GetBlueMask() const
        {
            switch (format_)
            {
            case ECF_A1R5G5B5:
                return 0x1F;
            case ECF_R5G6B5:
                return 0x1F;
            case ECF_R8G8B8:
                return 0x000000FF;
            case ECF_A8R8G8B8:
                return 0x000000FF;
            default:
                return 0x0;
            }
        }


        //! returns mask for alpha value of a pixel
        u32 CImage::GetAlphaMask() const
        {
            switch (format_)
            {
            case ECF_A1R5G5B5:
                return 0x1 << 15;
            case ECF_R5G6B5:
                return 0x0;
            case ECF_R8G8B8:
                return 0x0;
            case ECF_A8R8G8B8:
                return 0xFF000000;
            default:
                return 0x0;
            }
        }


        //! sets a pixel
        void CImage::SetPixel(u32 x, u32 y, const SColor &color, bool blend)
        {
            if (x >= size_.width_ || y >= size_.height_)
                return;

            switch (format_)
            {
            case ECF_A1R5G5B5:
            {
                u16 * dest = (u16*)(data_ + (y * pitch_) + (x << 1));
                *dest = video::A8R8G8B8toA1R5G5B5(color.color_);
            } break;

            case ECF_R5G6B5:
            {
                u16 * dest = (u16*)(data_ + (y * pitch_) + (x << 1));
                *dest = video::A8R8G8B8toR5G6B5(color.color_);
            } break;

            case ECF_R8G8B8:
            {
                u8* dest = data_ + (y * pitch_) + (x * 3);
                dest[0] = (u8)color.GetRed();
                dest[1] = (u8)color.GetGreen();
                dest[2] = (u8)color.GetBlue();
            } break;

            case ECF_A8R8G8B8:
            {
                u32 * dest = (u32*)(data_ + (y * pitch_) + (x << 2));
                *dest = blend ? PixelBlend32(*dest, color.color_) : color.color_;
            } break;
#ifndef _DEBUG
            default:
                break;
#endif
            }
        }


        //! returns a pixel
        SColor CImage::GetPixel(u32 x, u32 y) const
        {
            if (x >= size_.width_ || y >= size_.height_)
                return SColor(0);

            switch (format_)
            {
            case ECF_A1R5G5B5:
                return A1R5G5B5toA8R8G8B8(((u16*)data_)[y*size_.width_ + x]);
            case ECF_R5G6B5:
                return R5G6B5toA8R8G8B8(((u16*)data_)[y*size_.width_ + x]);
            case ECF_A8R8G8B8:
                return ((u32*)data_)[y*size_.width_ + x];
            case ECF_R8G8B8:
            {
                u8* p = data_ + (y * 3)*size_.width_ + (x * 3);
                return SColor(255, p[0], p[1], p[2]);
            }
#ifndef _DEBUG
            default:
                break;
#endif
            }

            return SColor(0);
        }


        //! returns the color format
        ECOLOR_FORMAT CImage::GetColorFormat() const
        {
            return format_;
        }


        //! copies this surface into another at given position
        void CImage::CopyTo(IImage* target, const core::position2d<s32>& pos)
        {
            Blit(BLITTER_TEXTURE, target, 0, &pos, this, 0, 0);
        }


        //! copies this surface partially into another at given position
        void CImage::CopyTo(IImage* target, const core::position2d<s32>& pos, const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect)
        {
            Blit(BLITTER_TEXTURE, target, clipRect, &pos, this, &sourceRect, 0);
        }


        //! copies this surface into another, using the alpha mask, a cliprect and a color to add with
        void CImage::CopyToWithAlpha(IImage* target, const core::position2d<s32>& pos, const core::rect<s32>& sourceRect, const SColor &color, const core::rect<s32>* clipRect)
        {
            // color blend only necessary on not full spectrum aka. color.color_ != 0xFFFFFFFF
            Blit(color.color_ == 0xFFFFFFFF ? BLITTER_TEXTURE_ALPHA_BLEND : BLITTER_TEXTURE_ALPHA_COLOR_BLEND,
                target, clipRect, &pos, this, &sourceRect, color.color_);
        }


        //! copies this surface into another, scaling it to the target image size
        // note: this is very very slow.
        void CImage::CopyToScaling(void* target, u32 width, u32 height, ECOLOR_FORMAT format, u32 pitch)
        {
            if (!target || !width || !height)
                return;

            const u32 bpp = GetBitsPerPixelFromFormat(format) / 8;
            if (0 == pitch)
                pitch = width*bpp;

            if (format_ == format && size_.width_ == width && size_.height_ == height)
            {
                if (pitch == pitch_)
                {
                    memcpy(target, data_, height*pitch);
                    return;
                }
                else
                {
                    u8* tgtpos = (u8*)target;
                    u8* srcpos = data_;
                    const u32 bwidth = width*bpp;
                    const u32 rest = pitch - bwidth;
                    for (u32 y = 0; y<height; ++y)
                    {
                        // copy scanline
                        memcpy(tgtpos, srcpos, bwidth);
                        // clear pitch
                        memset(tgtpos + bwidth, 0, rest);
                        tgtpos += pitch;
                        srcpos += pitch_;
                    }
                    return;
                }
            }

            const f32 sourceXStep = (f32)size_.width_ / (f32)width;
            const f32 sourceYStep = (f32)size_.height_ / (f32)height;
            s32 yval = 0, syval = 0;
            f32 sy = 0.0f;
            for (u32 y = 0; y<height; ++y)
            {
                f32 sx = 0.0f;
                for (u32 x = 0; x<width; ++x)
                {
                    CColorConverter::convert_viaFormat(data_ + syval + ((s32)sx)*bytes_per_pixel_, format_, 1, ((u8*)target) + yval + (x*bpp), format);
                    sx += sourceXStep;
                }
                sy += sourceYStep;
                syval = ((s32)sy)*pitch_;
                yval += pitch;
            }
        }


        //! copies this surface into another, scaling it to the target image size
        // note: this is very very slow.
        void CImage::CopyToScaling(IImage* target)
        {
            if (!target)
                return;

            const core::Dimension2d<u32>& targetSize = target->GetDimension();

            if (targetSize == size_)
            {
                CopyTo(target);
                return;
            }

            CopyToScaling(target->Lock(), targetSize.width_, targetSize.height_, target->GetColorFormat());
            target->Unlock();
        }


        //! copies this surface into another, scaling it to fit it.
        void CImage::CopyToScalingBoxFilter(IImage* target, s32 bias, bool blend)
        {
            const core::Dimension2d<u32> destSize = target->GetDimension();

            const f32 sourceXStep = (f32)size_.width_ / (f32)destSize.width_;
            const f32 sourceYStep = (f32)size_.height_ / (f32)destSize.height_;

            target->Lock();

            s32 fx = core::ceil32(sourceXStep);
            s32 fy = core::ceil32(sourceYStep);
            f32 sx;
            f32 sy;

            sy = 0.f;
            for (u32 y = 0; y != destSize.height_; ++y)
            {
                sx = 0.f;
                for (u32 x = 0; x != destSize.width_; ++x)
                {
                    target->SetPixel(x, y,
                        GetPixelBox(core::floor32(sx), core::floor32(sy), fx, fy, bias), blend);
                    sx += sourceXStep;
                }
                sy += sourceYStep;
            }

            target->Unlock();
        }


        //! fills the surface with given color
        void CImage::Fill(const SColor &color)
        {
            u32 c;

            switch (format_)
            {
            case ECF_A1R5G5B5:
                c = color.ToA1R5G5B5();
                c |= c << 16;
                break;
            case ECF_R5G6B5:
                c = video::A8R8G8B8toR5G6B5(color.color_);
                c |= c << 16;
                break;
            case ECF_A8R8G8B8:
                c = color.color_;
                break;
            case ECF_R8G8B8:
            {
                u8 rgb[3];
                CColorConverter::convert_A8R8G8B8toR8G8B8(&color, 1, rgb);
                const u32 size = GetImageDataSizeInBytes();
                for (u32 i = 0; i<size; i += 3)
                {
                    memcpy(data_ + i, rgb, 3);
                }
                return;
            }
            break;
            default:
                // TODO: Handle other formats
                return;
            }
            memset32(data_, c, GetImageDataSizeInBytes());
        }


        //! get a filtered pixel
        inline SColor CImage::GetPixelBox(s32 x, s32 y, s32 fx, s32 fy, s32 bias) const
        {
            SColor c;
            s32 a = 0, r = 0, g = 0, b = 0;

            for (s32 dx = 0; dx != fx; ++dx)
            {
                for (s32 dy = 0; dy != fy; ++dy)
                {
                    c = GetPixel(core::s32_min(x + dx, size_.width_ - 1),
                        core::s32_min(y + dy, size_.height_ - 1)
                        );

                    a += c.GetAlpha();
                    r += c.GetRed();
                    g += c.GetGreen();
                    b += c.GetBlue();
                }

            }

            s32 sdiv = s32_log2_s32(fx * fy);

            a = core::s32_clamp((a >> sdiv) + bias, 0, 255);
            r = core::s32_clamp((r >> sdiv) + bias, 0, 255);
            g = core::s32_clamp((g >> sdiv) + bias, 0, 255);
            b = core::s32_clamp((b >> sdiv) + bias, 0, 255);

            c.Set(a, r, g, b);
            return c;
        }


    } // end namespace video
} // end namespace irr
