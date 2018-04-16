// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __E_SCENE_NODE_TYPES_H_INCLUDED__
#define __E_SCENE_NODE_TYPES_H_INCLUDED__

#include "KongTypes.h"

namespace kong
{
    namespace scene
    {

        //! An enumeration for all types of built-in scene nodes
        /** A scene node type is represented by a four character code
        such as 'cube' or 'mesh' instead of simple numbers, to avoid
        name clashes with external scene nodes.*/
        enum ESCENE_NODE_TYPE
        {
            //! of type CSceneManager (note that ISceneManager is not(!) an ISceneNode)
            ESNT_SCENE_MANAGER = MAKE_KONG_ID('s', 'm', 'n', 'g'),

            //! simple cube scene node
            ESNT_CUBE = MAKE_KONG_ID('c', 'u', 'b', 'e'),

            //! Sphere scene node
            ESNT_SPHERE = MAKE_KONG_ID('s', 'p', 'h', 'r'),

            //! Text Scene Node
            ESNT_TEXT = MAKE_KONG_ID('t', 'e', 'x', 't'),

            //! Water Surface Scene Node
            ESNT_WATER_SURFACE = MAKE_KONG_ID('w', 'a', 't', 'r'),

            //! Terrain Scene Node
            ESNT_TERRAIN = MAKE_KONG_ID('t', 'e', 'r', 'r'),

            //! Sky Box Scene Node
            ESNT_SKY_BOX = MAKE_KONG_ID('s', 'k', 'y', '_'),

            //! Sky Dome Scene Node
            ESNT_SKY_DOME = MAKE_KONG_ID('s', 'k', 'y', 'd'),

            //! Shadow Volume Scene Node
            ESNT_SHADOW_VOLUME = MAKE_KONG_ID('s', 'h', 'd', 'w'),

            //! Octree Scene Node
            ESNT_OCTREE = MAKE_KONG_ID('o', 'c', 't', 'r'),

            //! Mesh Scene Node
            ESNT_MESH = MAKE_KONG_ID('m', 'e', 's', 'h'),

            //! Light Scene Node
            ESNT_LIGHT = MAKE_KONG_ID('l', 'g', 'h', 't'),

            //! Empty Scene Node
            ESNT_EMPTY = MAKE_KONG_ID('e', 'm', 't', 'y'),

            //! Dummy Transformation Scene Node
            ESNT_DUMMY_TRANSFORMATION = MAKE_KONG_ID('d', 'm', 'm', 'y'),

            //! Camera Scene Node
            ESNT_CAMERA = MAKE_KONG_ID('c', 'a', 'm', '_'),

            //! Billboard Scene Node
            ESNT_BILLBOARD = MAKE_KONG_ID('b', 'i', 'l', 'l'),

            //! Animated Mesh Scene Node
            ESNT_ANIMATED_MESH = MAKE_KONG_ID('a', 'm', 's', 'h'),

            //! Particle System Scene Node
            ESNT_PARTICLE_SYSTEM = MAKE_KONG_ID('p', 't', 'c', 'l'),

            //! Quake3 Shader Scene Node
            ESNT_Q3SHADER_SCENE_NODE = MAKE_KONG_ID('q', '3', 's', 'h'),

            //! Quake3 Model Scene Node ( has tag to link to )
            ESNT_MD3_SCENE_NODE = MAKE_KONG_ID('m', 'd', '3', '_'),

            //! Volume Light Scene Node
            ESNT_VOLUME_LIGHT = MAKE_KONG_ID('v', 'o', 'l', 'l'),

            //! Maya Camera Scene Node
            /** Legacy, for loading version <= 1.4.x .irr files */
            ESNT_CAMERA_MAYA = MAKE_KONG_ID('c', 'a', 'm', 'M'),

            //! First Person Shooter Camera
            /** Legacy, for loading version <= 1.4.x .irr files */
            ESNT_CAMERA_FPS = MAKE_KONG_ID('c', 'a', 'm', 'F'),

            //! Unknown scene node
            ESNT_UNKNOWN = MAKE_KONG_ID('u', 'n', 'k', 'n'),

            //! Will match with any scene node when checking types
            ESNT_ANY = MAKE_KONG_ID('a', 'n', 'y', '_')
        };



    } // end namespace scene
} // end namespace irr


#endif

