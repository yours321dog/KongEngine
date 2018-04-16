// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_LIGHT_H_INCLUDED__
#define __S_LIGHT_H_INCLUDED__

#include "SColor.h"
#include "Vector.h"

namespace kong
{
    namespace video
    {

        //! Enumeration for different types of lights
        enum E_LIGHT_TYPE
        {
            //! point light, it has a position in space and radiates light in all directions
            ELT_POINT,
            //! spot light, it has a position in space, a direction, and a limited cone of influence
            ELT_SPOT,
            //! directional light, coming from a direction from an infinite distance
            ELT_DIRECTIONAL,

            //! Only used for counting the elements of this enum
            ELT_COUNT
        };

        //! Names for light types
        const c8* const LightTypeNames[] =
        {
            "Point",
            "Spot",
            "Directional",
            nullptr
        };

        //! structure for holding data describing a dynamic point light.
        /** Irrlicht supports point lights, spot lights, and directional lights.
        */
        class SLight
        {
        public:
            SLight() : ambient_color_(0.f, 0.f, 0.f), diffuse_color_(1.f, 1.f, 1.f),
            specular_color_(1.f, 1.f, 1.f), attenuation_(1.f, 0.f, 0.f),
            outer_cone_(45.f), inner_cone_(0.f), falloff_(2.f),
            position_(0.f, 0.f, 0.f), direction_(0.f, 0.f, 1.f),
            radius_(100.f), type_(ELT_POINT), cast_shadows_(true)
            {}

            //! Ambient color emitted by the light
            SColorf ambient_color_;

            //! Diffuse color emitted by the light.
            /** This is the primary color you want to set. */
            SColorf diffuse_color_;

            //! Specular color emitted by the light.
            /** For details how to use specular highlights, see SMaterial::Shininess */
            SColorf specular_color_;

            //! Attenuation factors (constant, linear, quadratic)
            /** Changes the light strength fading over distance.
            Can also be altered by setting the radius, Attenuation will change to
            (0,1.f/radius,0). Can be overridden after radius was set. */
            core::vector3df attenuation_;

            //! The angle of the spot's outer cone. Ignored for other lights.
            f32 outer_cone_;

            //! The angle of the spot's inner cone. Ignored for other lights.
            f32 inner_cone_;

            //! The light strength's decrease between Outer and Inner cone.
            f32 falloff_;

            //! Read-ONLY! Position of the light.
            /** If Type is ELT_DIRECTIONAL, it is ignored. Changed via light scene node's position. */
            core::vector3df position_;

            //! Read-ONLY! Direction of the light.
            /** If Type is ELT_POINT, it is ignored. Changed via light scene node's rotation. */
            core::vector3df direction_;

            //! Read-ONLY! Radius of light. Everything within this radius will be lighted.
            f32 radius_;

            //! Read-ONLY! Type of the light. Default: ELT_POINT
            E_LIGHT_TYPE type_;

            //! Read-ONLY! Does the light cast shadows?
            bool cast_shadows_ : 1;
        };

    } // end namespace video
} // end namespace irr

#endif

