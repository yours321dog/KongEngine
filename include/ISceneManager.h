// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _ISCENEMANAGER_H_
#define _ISCENEMANAGER_H_
#include "ISceneNode.h"
#include "IVideoDriver.h"

namespace kong
{
    namespace video
    {
        class IVideoDriver;
        class SMaterial;
        //class IImage;
        //class ITexture;
    } // end namespace video

    namespace scene
    {
        class IAnimatedMesh;
        class ICameraSceneNode;
        class ILightSceneNode;
        class IMeshSceneNode;
        class IMeshLoader;
        class IMeshManipulator;
        class ISceneNode;

        //! Enumeration for render passes.
        /** A parameter passed to the registerNodeForRendering() method of the ISceneManager,
        specifying when the node wants to be drawn in relation to the other nodes. */
        enum E_SCENE_NODE_RENDER_PASS
        {
            //! No pass currently active
            ESNRP_NONE = 0,

            //! Camera pass. The active view is set up here. The very first pass.
            ESNRP_CAMERA = 1,

            //! In this pass, lights are transformed into camera space and added to the driver
            ESNRP_LIGHT = 2,

            //! This is used for sky boxes.
            ESNRP_SKY_BOX = 4,

            //! All normal objects can use this for registering themselves.
            /** This value will never be returned by
            ISceneManager::getSceneNodeRenderPass(). The scene manager
            will determine by itself if an object is transparent or solid
            and register the object as SNRT_TRANSPARENT or SNRT_SOLD
            automatically if you call registerNodeForRendering with this
            value (which is default). Note that it will register the node
            only as ONE type. If your scene node has both solid and
            transparent material types register it twice (one time as
            SNRT_SOLID, the other time as SNRT_TRANSPARENT) and in the
            render() method call getSceneNodeRenderPass() to find out the
            current render pass and render only the corresponding parts of
            the node. */
            ESNRP_AUTOMATIC = 24,

            //! Solid scene nodes or special scene nodes without materials.
            ESNRP_SOLID = 8,

            //! Transparent scene nodes, drawn after solid nodes. They are sorted from back to front and drawn in that order.
            ESNRP_TRANSPARENT = 16,

            //! Transparent effect scene nodes, drawn after Transparent nodes. They are sorted from back to front and drawn in that order.
            ESNRP_TRANSPARENT_EFFECT = 32,

            //! Drawn after the solid nodes, before the transparent nodes, the time for drawing shadow volumes
            ESNRP_SHADOW = 64
        };


        class ISceneManager
        {
        public:
            virtual ~ISceneManager() = default;

            //! Get pointer to an animateable mesh. Loads the file if not loaded already.
            /**
            * If you want to remove a loaded mesh from the cache again, use removeMesh().
            *  Currently there are the following mesh formats supported:
            *  <TABLE border="1" cellpadding="2" cellspacing="0">
            *  <TR>
            *    <TD>Format</TD>
            *    <TD>Description</TD>
            *  </TR>
            *  <TR>
            *    <TD>3D Studio (.3ds)</TD>
            *    <TD>Loader for 3D-Studio files which lots of 3D packages
            *      are able to export. Only static meshes are currently
            *      supported by this importer.</TD>
            *  </TR>
            *  <TR>
            *    <TD>3D World Studio (.smf)</TD>
            *    <TD>Loader for Leadwerks SMF mesh files, a simple mesh format
            *    containing static geometry for games. The proprietary .STF texture format
            *    is not supported yet. This loader was originally written by Joseph Ellis. </TD>
            *  </TR>
            *  <TR>
            *    <TD>Bliz Basic B3D (.b3d)</TD>
            *    <TD>Loader for blitz basic files, developed by Mark
            *      Sibly. This is the ideal animated mesh format for game
            *      characters as it is both rigidly defined and widely
            *      supported by modeling and animation software.
            *      As this format supports skeletal animations, an
            *      ISkinnedMesh will be returned by this importer.</TD>
            *  </TR>
            *  <TR>
            *    <TD>Cartography shop 4 (.csm)</TD>
            *    <TD>Cartography Shop is a modeling program for creating
            *      architecture and calculating lighting. Irrlicht can
            *      directly import .csm files thanks to the IrrCSM library
            *      created by Saurav Mohapatra which is now integrated
            *      directly in Irrlicht. If you are using this loader,
            *      please note that you'll have to set the path of the
            *      textures before loading .csm files. You can do this
            *      using
            *      SceneManager-&gt;getParameters()-&gt;setAttribute(scene::CSM_TEXTURE_PATH,
            *      &quot;path/to/your/textures&quot;);</TD>
            *  </TR>
            *  <TR>
            *    <TD>COLLADA (.dae, .xml)</TD>
            *    <TD>COLLADA is an open Digital Asset Exchange Schema for
            *        the interactive 3D industry. There are exporters and
            *        importers for this format available for most of the
            *        big 3d packagesat http://collada.org. Irrlicht can
            *        import COLLADA files by using the
            *        ISceneManager::getMesh() method. COLLADA files need
            *        not contain only one single mesh but multiple meshes
            *        and a whole scene setup with lights, cameras and mesh
            *        instances, this loader can set up a scene as
            *        described by the COLLADA file instead of loading and
            *        returning one single mesh. By default, this loader
            *        behaves like the other loaders and does not create
            *        instances, but it can be switched into this mode by
            *        using
            *        SceneManager-&gt;getParameters()-&gt;setAttribute(COLLADA_CREATE_SCENE_INSTANCES, true);
            *        Created scene nodes will be named as the names of the
            *        nodes in the COLLADA file. The returned mesh is just
            *        a dummy object in this mode. Meshes included in the
            *        scene will be added into the scene manager with the
            *        following naming scheme:
            *        "path/to/file/file.dea#meshname". The loading of such
            *        meshes is logged. Currently, this loader is able to


            *        create meshes (made of only polygons), lights, and
            *        cameras. Materials and animations are currently not
            *        supported but this will change with future releases.
            *    </TD>
            *  </TR>
            *  <TR>
            *    <TD>Delgine DeleD (.dmf)</TD>
            *    <TD>DeleD (delgine.com) is a 3D editor and level-editor
            *        combined into one and is specifically designed for 3D
            *        game-development. With this loader, it is possible to
            *        directly load all geometry is as well as textures and
            *        lightmaps from .dmf files. To set texture and
            *        material paths, see scene::DMF_USE_MATERIALS_DIRS and
            *        scene::DMF_TEXTURE_PATH. It is also possible to flip
            *        the alpha texture by setting
            *        scene::DMF_FLIP_ALPHA_TEXTURES to true and to set the
            *        material transparent reference value by setting
            *        scene::DMF_ALPHA_CHANNEL_REF to a float between 0 and
            *        1. The loader is based on Salvatore Russo's .dmf
            *        loader, I just changed some parts of it. Thanks to
            *        Salvatore for his work and for allowing me to use his
            *        code in Irrlicht and put it under Irrlicht's license.
            *        For newer and more enchanced versions of the loader,
            *        take a look at delgine.com.
            *    </TD>
            *  </TR>
            *  <TR>
            *    <TD>DirectX (.x)</TD>
            *    <TD>Platform independent importer (so not D3D-only) for
            *      .x files. Most 3D packages can export these natively
            *      and there are several tools for them available, e.g.
            *      the Maya exporter included in the DX SDK.
            *      .x files can include skeletal animations and Irrlicht
            *      is able to play and display them, users can manipulate
            *      the joints via the ISkinnedMesh interface. Currently,
            *      Irrlicht only supports uncompressed .x files.</TD>
            *  </TR>
            *  <TR>
            *    <TD>Half-Life model (.mdl)</TD>
            *    <TD>This loader opens Half-life 1 models, it was contributed
            *        by Fabio Concas and adapted by Thomas Alten.</TD>
            *  </TR>
            *  <TR>
            *    <TD>Irrlicht Mesh (.irrMesh)</TD>
            *    <TD>This is a static mesh format written in XML, native
            *      to Irrlicht and written by the irr mesh writer.
            *      This format is exported by the CopperCube engine's
            *      lightmapper.</TD>
            *  </TR>
            *  <TR>
            *    <TD>LightWave (.lwo)</TD>
            *    <TD>Native to NewTek's LightWave 3D, the LWO format is well
            *      known and supported by many exporters. This loader will
            *      import LWO2 models including lightmaps, bumpmaps and
            *      reflection textures.</TD>
            *  </TR>
            *  <TR>
            *    <TD>Maya (.obj)</TD>
            *    <TD>Most 3D software can create .obj files which contain
            *      static geometry without material data. The material
            *      files .mtl are also supported. This importer for
            *      Irrlicht can load them directly. </TD>
            *  </TR>
            *  <TR>
            *    <TD>Milkshape (.ms3d)</TD>
            *    <TD>.MS3D files contain models and sometimes skeletal
            *      animations from the Milkshape 3D modeling and animation
            *      software. Like the other skeletal mesh loaders, oints
            *      are exposed via the ISkinnedMesh animated mesh type.</TD>
            *  </TR>
            *  <TR>
            *  <TD>My3D (.my3d)</TD>
            *      <TD>.my3D is a flexible 3D file format. The My3DTools
            *        contains plug-ins to export .my3D files from several
            *        3D packages. With this built-in importer, Irrlicht
            *        can read and display those files directly. This
            *        loader was written by Zhuck Dimitry who also created
            *        the whole My3DTools package. If you are using this
            *        loader, please note that you can set the path of the
            *        textures before loading .my3d files. You can do this
            *        using
            *        SceneManager-&gt;getParameters()-&gt;setAttribute(scene::MY3D_TEXTURE_PATH,
            *        &quot;path/to/your/textures&quot;);
            *        </TD>
            *    </TR>
            *    <TR>
            *      <TD>OCT (.oct)</TD>
            *      <TD>The oct file format contains 3D geometry and
            *        lightmaps and can be loaded directly by Irrlicht. OCT
            *        files<br> can be created by FSRad, Paul Nette's
            *        radiosity processor or exported from Blender using
            *        OCTTools which can be found in the exporters/OCTTools
            *        directory of the SDK. Thanks to Murphy McCauley for
            *        creating all this.</TD>
            *    </TR>
            *    <TR>
            *      <TD>OGRE Meshes (.mesh)</TD>
            *      <TD>Ogre .mesh files contain 3D data for the OGRE 3D
            *        engine. Irrlicht can read and display them directly
            *        with this importer. To define materials for the mesh,
            *        copy a .material file named like the corresponding
            *        .mesh file where the .mesh file is. (For example
            *        ogrehead.material for ogrehead.mesh). Thanks to
            *        Christian Stehno who wrote and contributed this
            *        loader.</TD>
            *    </TR>
            *    <TR>
            *      <TD>Pulsar LMTools (.lmts)</TD>
            *      <TD>LMTools is a set of tools (Windows &amp; Linux) for
            *        creating lightmaps. Irrlicht can directly read .lmts
            *        files thanks to<br> the importer created by Jonas
            *        Petersen. If you are using this loader, please note
            *        that you can set the path of the textures before
            *        loading .lmts files. You can do this using
            *        SceneManager-&gt;getParameters()-&gt;setAttribute(scene::LMTS_TEXTURE_PATH,
            *        &quot;path/to/your/textures&quot;);
            *        Notes for<br> this version of the loader:<br>
            *        - It does not recognise/support user data in the
            *          *.lmts files.<br>
            *        - The TGAs generated by LMTools don't work in
            *          Irrlicht for some reason (the textures are upside
            *          down). Opening and resaving them in a graphics app
            *          will solve the problem.</TD>
            *    </TR>
            *    <TR>
            *      <TD>Quake 3 levels (.bsp)</TD>
            *      <TD>Quake 3 is a popular game by IDSoftware, and .pk3
            *        files contain .bsp files and textures/lightmaps
            *        describing huge prelighted levels. Irrlicht can read
            *        .pk3 and .bsp files directly and thus render Quake 3
            *        levels directly. Written by Nikolaus Gebhardt
            *        enhanced by Dean P. Macri with the curved surfaces
            *        feature. </TD>
            *    </TR>
            *    <TR>
            *      <TD>Quake 2 models (.md2)</TD>
            *      <TD>Quake 2 models are characters with morph target
            *        animation. Irrlicht can read, display and animate
            *        them directly with this importer. </TD>
            *    </TR>
            *    <TR>
            *      <TD>Quake 3 models (.md3)</TD>
            *      <TD>Quake 3 models are characters with morph target
            *        animation, they contain mount points for weapons and body
            *        parts and are typically made of several sections which are
            *        manually joined together.</TD>
            *    </TR>
            *    <TR>
            *      <TD>Stanford Triangle (.ply)</TD>
            *      <TD>Invented by Stanford University and known as the native
            *        format of the infamous "Stanford Bunny" model, this is a
            *        popular static mesh format used by 3D scanning hardware
            *        and software. This loader supports extremely large models
            *        in both ASCII and binary format, but only has rudimentary
            *        material support in the form of vertex colors and texture
            *        coordinates.</TD>
            *    </TR>
            *    <TR>
            *      <TD>Stereolithography (.stl)</TD>
            *      <TD>The STL format is used for rapid prototyping and
            *        computer-aided manufacturing, thus has no support for
            *        materials.</TD>
            *    </TR>
            *  </TABLE>
            *
            *  To load and display a mesh quickly, just do this:
            *  \code
            *  SceneManager->addAnimatedMeshSceneNode(
            *		SceneManager->getMesh("yourmesh.3ds"));
            * \endcode
            * If you would like to implement and add your own file format loader to Irrlicht,
            * see addExternalMeshLoader().
            * \param filename: Filename of the mesh to load.
            * \return Null if failed, otherwise pointer to the mesh.
            * This pointer should not be dropped. See IReferenceCounted::drop() for more information.
            **/
            virtual IAnimatedMesh* getMesh(const io::path& filename) = 0;

            //! Get pointer to an animateable mesh. Loads the file if not loaded already.
            /** Works just as getMesh(const char* filename). If you want to
            remove a loaded mesh from the cache again, use removeMesh().
            \param file File handle of the mesh to load.
            \return NULL if failed and pointer to the mesh if successful.
            This pointer should not be dropped. See
            IReferenceCounted::drop() for more information. */
            virtual IAnimatedMesh* getMesh(io::IReadFile* file) = 0;

            //! Adds a cube scene node
            /** \param size: Size of the cube, uniformly in each dimension.
            \param parent: Parent of the scene node. Can be 0 if no parent.
            \param id: Id of the node. This id can be used to identify the scene node.
            \param position: Position of the space relative to its parent
            where the scene node will be placed.
            \param rotation: Initital rotation of the scene node.
            \param scale: Initial scale of the scene node.
            \return Pointer to the created test scene node. This
            pointer should not be dropped. See IReferenceCounted::drop()
            for more information. */
            virtual IMeshSceneNode* AddCubeSceneNode(f32 size = 10.0f, ISceneNode* parent = nullptr, s32 id = -1,
                const core::Vector3Df& position = core::Vector3Df(0, 0, 0),
                const core::Vector3Df& rotation = core::Vector3Df(0, 0, 0),
                const core::Vector3Df& scale = core::Vector3Df(1.0f, 1.0f, 1.0f)) = 0;

            //! Adds a plane scene node
            /** \param size: Size of the plane, uniformly in each dimension.
            \param parent: Parent of the scene node. Can be 0 if no parent.
            \param id: Id of the node. This id can be used to identify the scene node.
            \param position: Position of the space relative to its parent
            where the scene node will be placed.
            \param rotation: Initital rotation of the scene node.
            \param scale: Initial scale of the scene node.
            \return Pointer to the created test scene node. This
            pointer should not be dropped. See IReferenceCounted::drop()
            for more information. */
            virtual IMeshSceneNode* AddPlaneSceneNode(f32 size = 10.0f, ISceneNode* parent = nullptr, s32 id = -1,
                const core::Vector3Df& position = core::Vector3Df(0, 0, 0),
                const core::Vector3Df& rotation = core::Vector3Df(0, 0, 0),
                const core::Vector3Df& scale = core::Vector3Df(1.0f, 1.0f, 1.0f)) = 0;

            //! Adds a scene node for rendering a static mesh.
            /** \param mesh: Pointer to the loaded static mesh to be displayed.
            \param parent: Parent of the scene node. Can be NULL if no parent.
            \param id: Id of the node. This id can be used to identify the scene node.
            \param position: Position of the space relative to its parent where the
            scene node will be placed.
            \param rotation: Initital rotation of the scene node.
            \param scale: Initial scale of the scene node.
            \param alsoAddIfMeshPointerZero: Add the scene node even if a 0 pointer is passed.
            \return Pointer to the created scene node.
            This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
            virtual IMeshSceneNode* AddMeshSceneNode(IMesh* mesh, ISceneNode* parent = 0, s32 id = -1,
                const core::vector3df& position = core::vector3df(0, 0, 0),
                const core::vector3df& rotation = core::vector3df(0, 0, 0),
                const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f),
                bool alsoAddIfMeshPointerZero = false) = 0;

            //! Adds a camera scene node to the scene graph and sets it as active camera.
            /** This camera does not react on user input like for example the one created with
            addCameraSceneNodeFPS(). If you want to move or animate it, use animators or the
            ISceneNode::setPosition(), ICameraSceneNode::setTarget() etc methods.
            By default, a camera's look at position (set with setTarget()) and its scene node
            rotation (set with setRotation()) are independent. If you want to be able to
            control the direction that the camera looks by using setRotation() then call
            ICameraSceneNode::bindTargetAndRotation(true) on it.
            \param position: Position of the space relative to its parent where the camera will be placed.
            \param up
            \param lookat: Position where the camera will look at. Also known as target.
            \param parent: Parent scene node of the camera. Can be null. If the parent moves,
            the camera will move too.
            \param id: id of the camera. This id can be used to identify the camera.
            \param makeActive Flag whether this camera should become the active one.
            Make sure you always have one active camera.
            \return Pointer to interface to camera if successful, otherwise 0.
            This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
            virtual ICameraSceneNode* AddCameraSceneNode(ISceneNode* parent = nullptr,
                const core::Vector3Df& position = core::Vector3Df(0, 0, 0),
                const core::Vector3Df& up = core::Vector3Df(0, 1, 0),
                const core::Vector3Df& lookat = core::Vector3Df(0, 0, 100),
                s32 id = -1, bool make_active = true) = 0;

            //! Adds a camera scene node to the scene graph and sets it as active camera.
            /** This camera does not react on user input like for example the one created with
            addCameraSceneNodeFPS(). If you want to move or animate it, use animators or the
            ISceneNode::setPosition(), ICameraSceneNode::setTarget() etc methods.
            By default, a camera's look at position (set with setTarget()) and its scene node
            rotation (set with setRotation()) are independent. If you want to be able to
            control the direction that the camera looks by using setRotation() then call
            ICameraSceneNode::bindTargetAndRotation(true) on it.
            \param position: Position of the space relative to its parent where the camera will be placed.
            \param up
            \param lookat: Position where the camera will look at. Also known as target.
            \param parent: Parent scene node of the camera. Can be null. If the parent moves,
            the camera will move too.
            \param id: id of the camera. This id can be used to identify the camera.
            \param makeActive Flag whether this camera should become the active one.
            Make sure you always have one active camera.
            \return Pointer to interface to camera if successful, otherwise 0.
            This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
            virtual ICameraSceneNode* AddPerspectiveCameraSceneNode(ISceneNode* parent = nullptr,
                const core::Vector3Df& position = core::Vector3Df(0, 0, 0),
                const core::Vector3Df& up = core::Vector3Df(0, 1, 0),
                const core::Vector3Df& lookat = core::Vector3Df(0, 0, 100),
                s32 id = -1, bool make_active = true) = 0;

            //! Adds a orthogonal camera scene node to the scene graph and sets it as active camera.
            /** This camera does not react on user input like for example the one created with
            addCameraSceneNodeFPS(). If you want to move or animate it, use animators or the
            ISceneNode::setPosition(), ICameraSceneNode::setTarget() etc methods.
            By default, a camera's look at position (set with setTarget()) and its scene node
            rotation (set with setRotation()) are independent. If you want to be able to
            control the direction that the camera looks by using setRotation() then call
            ICameraSceneNode::bindTargetAndRotation(true) on it.
            \param position: Position of the space relative to its parent where the camera will be placed.
            \param up
            \param lookat: Position where the camera will look at. Also known as target.
            \param parent: Parent scene node of the camera. Can be null. If the parent moves,
            the camera will move too.
            \param id: id of the camera. This id can be used to identify the camera.
            \param makeActive Flag whether this camera should become the active one.
            Make sure you always have one active camera.
            \return Pointer to interface to camera if successful, otherwise 0.
            This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
            virtual ICameraSceneNode* AddOrthogonalCameraSceneNode(ISceneNode* parent = nullptr,
                const core::Vector3Df& position = core::Vector3Df(0, 0, 0),
                const core::Vector3Df& up = core::Vector3Df(0, 1, 0),
                const core::Vector3Df& lookat = core::Vector3Df(0, 0, 100),
                s32 id = -1, bool make_active = true) = 0;

            //! Adds a dynamic light scene node to the scene graph.
            /** The light will cast dynamic light on all
            other scene nodes in the scene, which have the material flag video::MTF_LIGHTING
            turned on. (This is the default setting in most scene nodes).
            \param parent: Parent scene node of the light. Can be null. If the parent moves,
            the light will move too.
            \param position: Position of the space relative to its parent where the light will be placed.
            \param color: Diffuse color of the light. Ambient or Specular colors can be set manually with
            the ILightSceneNode::getLightData() method.
            \param radius: Radius of the light.
            \param id: id of the node. This id can be used to identify the node.
            \return Pointer to the interface of the light if successful, otherwise NULL.
            This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
            virtual ILightSceneNode* AddLightSceneNode(ISceneNode* parent = 0,
                const core::vector3df& position = core::vector3df(0, 0, 0),
                video::SColorf color = video::SColorf(1.0f, 1.0f, 1.0f),
                f32 radius = 100.0f, s32 id = -1) = 0;

            //! Adds a scene node to the scene by name
            /** \return Pointer to the scene node added by a factory
            This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
            virtual ISceneNode* AddSceneNode(const char* scene_node_name, ISceneNode* parent = nullptr) = 0;

            //! Sets the currently active camera.
            /** The previous active camera will be deactivated.
            \param camera: The new camera which should be active. */
            virtual void SetActiveCamera(ICameraSceneNode* camera) = 0;

            //! Sets the currently active camera.
            virtual ICameraSceneNode *GetActiveCamera() = 0;

            //! Get the current color of shadows.
            virtual video::SColor GetShadowColor() const = 0;

            //! Set the current color of shadows.
            virtual void SetShadowColor(video::SColor color) = 0;

            //! Enable shadows.
            virtual void EnableShadow(bool flag = true) = 0;

            //! Draws all the scene nodes.
            /** This can only be invoked between
            IVideoDriver::beginScene() and IVideoDriver::endScene(). Please note that
            the scene is not only drawn when calling this, but also animated
            by existing scene node animators, culling of scene nodes is done, etc. */
            virtual void DrawAll() = 0;

            //! Draws all the scene nodes with deferred render.
            /** This can only be invoked between
            IVideoDriver::beginScene() and IVideoDriver::endScene(). Please note that
            the scene is not only drawn when calling this, but also animated
            by existing scene node animators, culling of scene nodes is done, etc. */
            virtual void DrawAllDeferred() = 0;

            //! Clears the whole scene.
            /** All scene nodes are removed. */
            virtual void Clear() = 0;

            //! Get video driver
            virtual video::IVideoDriver *GetVideoDriver() const = 0;

            //! Registers a node for rendering it at a specific time.
            /** This method should only be used by SceneNodes when they get a
            ISceneNode::OnRegisterSceneNode() call.
            \param node: Node to register for drawing. Usually scene nodes would set 'this'
            as parameter here because they want to be drawn.
            \param pass: Specifies when the node wants to be drawn in relation to the other nodes.
            For example, if the node is a shadow, it usually wants to be drawn after all other nodes
            and will use ESNRP_SHADOW for this. See scene::E_SCENE_NODE_RENDER_PASS for details.
            \return scene will be rendered ( passed culling ) */
            virtual u32 RegisterNodeForRendering(ISceneNode* node,
                E_SCENE_NODE_RENDER_PASS pass = ESNRP_AUTOMATIC) = 0;

            //! Get pointer to the mesh manipulator.
            /** \return Pointer to the mesh manipulator
            This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
            virtual IMeshManipulator* GetMeshManipulator() = 0;

            //! Set main light for shadow rendering
            /** \param light_node: main light for shadow rendering*/
            virtual void SetMainLight(const ILightSceneNode *light_node) = 0;
        };
    }
}

#endif