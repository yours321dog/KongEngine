#include "KongEngine.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "ISceneManager.h"
#include "CFileSystem.h"

using namespace kong;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;

void TestArray()
{
    Array<int> arr;
    arr.SetAll(1);

    printf("size : %d\n", arr.Size());

    for (u32 i = 0; i < 10; i++)
    {
        arr.PushBack(i);
    }

    arr.Insert(38);
    arr.Insert(32, 2);

    for (u32 i = 0; i < arr.Size(); i++)
    {
        printf("%d\t", arr[i]);
    }
    printf("\n");

    arr.Resize(40);
    for (u32 i = 0; i < arr.Size(); i++)
    {
        printf("%d\t", arr[i]);
    }
    printf("\n");
}

void TestS3DVertex()
{
    S3DVertex vtx1;
    S3DVertex vtx2;

    vtx1.pos_ = Vector3Df(1.4f, 5.2f, -1.5f);
    vtx1.normal_ = Vector3Df(4.f, -3.1f, 1.1f);
    vtx1.texcoord_ = Vector2Df(0.4f, 0.1f);
    vtx1.color_ = SColor(100, 111, 12, 42);

    vtx2.pos_ = Vector3Df(1.4f, 5.2f, -1.5f);
    vtx2.normal_ = Vector3Df(4.f, -3.1f, 1.1f);
    vtx2.texcoord_ = Vector2Df(0.4f, 0.1f);
    vtx2.color_ = SColor(100, 111, 12, 42);

    std::cout << (vtx1 == vtx2) << std::endl;

    vtx2.pos_ = Vector3Df(-3.4f, 2.2f, 1.3f);
    vtx2.normal_ = Vector3Df(3.f, 4.f, -1.f);
    vtx2.texcoord_ = Vector2Df(0.2f, 0.4f);
    vtx2.color_ = SColor(43, 53, 112, 2);

    std::cout << (vtx1 == vtx2) << std::endl;

    S3DVertex tmp = vtx1.GetInterpolated(vtx2, 0.1f);
    printf("pos : %f, %f, %f\n", tmp.pos_.x_, tmp.pos_.y_, tmp.pos_.z_);
    printf("normal : %f, %f, %f\n", tmp.normal_.x_, tmp.normal_.y_, tmp.normal_.z_);
    printf("texcoord : %f, %f, %f\n", tmp.texcoord_.x_, tmp.texcoord_.y_, tmp.texcoord_.z_);
    printf("color : %d, %d, %d, %d\n", tmp.color_.GetAlpha(), tmp.color_.GetRed(), tmp.color_.GetGreen(),
        tmp.color_.GetBlue());
}

void TestList()
{
    List<f32> list;
    list.push_back(12.f);
    list.push_front(1.3f);
    list.push_back(32.f);
    list.insert_after(list.begin(), 4.f);

    List<f32>::Iterator it = list.begin();
    for (; it != list.end(); ++it)
    {
        printf("%f\t", *it);
    }
    printf("\n");

    list.clear();
    list.push_back(12.5f);
    it = list.begin();
    for (; it != list.end(); ++it)
    {
        printf("%f\t", *it);
    }
    printf("\n");
}

void TestFileSystem()
{
    IFileSystem *file_sm = new CFileSystem();
    IReadFile *read_file = file_sm->CreateAndOpenFile("E:\\tmp\\test.txt");
    
    char *vals = new char[read_file->GetSize() + 1];
    vals[read_file->GetSize()] = 0;

    read_file->Read(vals, read_file->GetSize());

    std::cout << vals << std::endl;

    IWriteFile *write_file = file_sm->CreateAndWriteFile("E:\\tmp\\test_write.txt");
    write_file->Write(vals, read_file->GetSize());

    delete[] vals;
    delete read_file;
    delete file_sm;
}

class MyEventReceiver : public IEventReceiver
{
public:
    enum MY_KEY_STATE
    {
        EKS_UP,
        EKS_DOWN,
        EKS_PRESSED,
        EKS_RELEASED
    };
    // This is the one method that we have to implement
    virtual bool OnEvent(const SEvent& event)
    {
        // Remember whether each key is down or up
        if (event.EventType == kong::EET_KEY_INPUT_EVENT)
        {
            if (event.KeyInput.PressedDown)
            {
                if (key_state_[event.KeyInput.Key] == EKS_DOWN)
                {
                    key_state_[event.KeyInput.Key] = EKS_PRESSED;
                }
                else
                {
                    key_state_[event.KeyInput.Key] = EKS_DOWN;
                }
            }
            else
            {
                if (key_state_[event.KeyInput.Key] != EKS_UP)
                {
                    key_state_[event.KeyInput.Key] = EKS_RELEASED;
                }
                else
                {
                    key_state_[event.KeyInput.Key] = EKS_UP;
                }
            }
        }

        return false;
    }

    virtual bool StartEventProcess()
    {
        for (u32 i = 0; i < KEY_KEY_CODES_COUNT; ++i)
        {
            if (key_state_[i] == EKS_PRESSED)
            {
                key_state_[i] = EKS_DOWN;
            }
            if (key_state_[i] == EKS_RELEASED)
            {
                key_state_[i] = EKS_UP;
            }
        }

        return true;
    }

    // This is used to check whether a key is being held down
    virtual bool IsKeyDown(EKEY_CODE keyCode) const
    {
        return key_state_[keyCode] == EKS_PRESSED || key_state_[keyCode] == EKS_DOWN;
    }
    virtual bool IsKeyUp(EKEY_CODE keyCode) const
    {
        return key_state_[keyCode] == EKS_UP || key_state_[keyCode] == EKS_RELEASED;
    }

    virtual bool IsKeyRelease(EKEY_CODE keyCode) const
    {
        return key_state_[keyCode] == EKS_RELEASED;
    }

    MyEventReceiver()
    {
        for (u32 i = 0; i < KEY_KEY_CODES_COUNT; ++i)
        {
            key_state_[i] = EKS_UP;
        }
    }

private:
    // We use this array to store the current state of each key
    MY_KEY_STATE key_state_[KEY_KEY_CODES_COUNT];
};

void TestWindow()
{
    MyEventReceiver receiver;

    KongDevice *device = CreateDevice(Dimension2d<u32>(640, 480), 16,
        false, false, false, &receiver);

    if (!device)
    {
        return;
    }

    IVideoDriver *driver = device->GetVideoDriver();
    ISceneManager *smr = device->GetSceneManager();

    smr->AddPerspectiveCameraSceneNode(nullptr, Vector3Df(0.f, 0.f, -2.f), Vector3Df(0.f, 1.f, 0.f), Vector3Df(0.f, 0.f, 0.f));
    ISceneNode *node = smr->AddCubeSceneNode(1.f, nullptr, -1, Vector3Df(0.0f, 0.0f, -0.0f), Vector3Df(0.f, 0.f, 0.f), Vector3Df(1.f, 1.f, 1.f));
    //node->SetMaterialTexture(0, driver->GetTexture("../../materials/saber1.jpg"));
    node->SetMaterialTexture(0, driver->GetTexture("../../materials/hon_eye_ref.tga"));
    //node->SetRenderingMode(video::ERM_WIREFRAME);
    node->EnableDrawBoundingBox(true);

    ILightSceneNode *light_node = smr->AddLightSceneNode(nullptr, Vector3Df(2.f, 2.f, -2.f));
    SLight light_data = light_node->GetLightData();
    light_data.ambient_color_ = SColorf(0.1f, 0.1f, 0.1f, 1.f);
    light_data.type_ = ELT_DIRECTIONAL;
    //light_data.type_ = ELT_SPOT;
    light_node->SetLightData(light_data);
    light_node->SetRotation(Vector3Df(-PI/4, PI / 4, 0.f));

    Vector3Df node_pos(0.f, 0.f, 0.f);
    Vector3Df node_rot(0.f, 0.f, 0.f);
    const f32 movement = 0.05;
    const f32 rotate_factor = 0.05;
    bool shadow_on = false;

    while (device->run())
    {
        driver->BeginScene();

        if (receiver.IsKeyDown(kong::KEY_KEY_W))
        {
            node_pos.y_ += movement;
        }
        else if (receiver.IsKeyDown(kong::KEY_KEY_S))
        {
            node_pos.y_ -= movement;
        }

        if (receiver.IsKeyDown(kong::KEY_KEY_A))
        {
            node_pos.x_ -= movement;
        }
        else if (receiver.IsKeyDown(kong::KEY_KEY_D))
        {
            node_pos.x_ += movement;
        }

        if (receiver.IsKeyDown(kong::KEY_KEY_Q))
        {
            node_pos.z_ -= movement;
        }
        else if (receiver.IsKeyDown(kong::KEY_KEY_E))
        {
            node_pos.z_ += movement;
        }

        if (receiver.IsKeyDown(kong::KEY_SPACE))
        {
            node_pos = Vector3Df(0.f, 0.f, 0.f);
            node_rot = Vector3Df(0.f, 0.f, 0.f);
        }

        if (receiver.IsKeyDown(kong::KEY_KEY_U))
        {
            node_rot.x_ -= rotate_factor;
        }
        else if (receiver.IsKeyDown(kong::KEY_KEY_J))
        {
            node_rot.x_ += rotate_factor;
        }

        if (receiver.IsKeyDown(kong::KEY_KEY_I))
        {
            node_rot.y_ -= rotate_factor;
        }
        else if (receiver.IsKeyDown(kong::KEY_KEY_K))
        {
            node_rot.y_ += rotate_factor;
        }

        if (receiver.IsKeyDown(kong::KEY_KEY_O))
        {
            node_rot.z_ -= rotate_factor;
        }
        else if (receiver.IsKeyDown(kong::KEY_KEY_L))
        {
            node_rot.z_ += rotate_factor;
        }

        if (receiver.IsKeyRelease(kong::KEY_KEY_1))
        {
            smr->EnableShadow(shadow_on);
            shadow_on = ~shadow_on;
        }


        node->SetPosition(node_pos);
        node->SetRotation(node_rot);

        //driver->Draw3DLine(Vector3Df(0.f, 0.f, 0.f), Vector3Df(1.f, 1.f, 1.f));
        //smr->DrawAll();
        smr->DrawAllDeferred();

        driver->EndScene();
    }
}

void TestObjLoad()
{
    MyEventReceiver receiver;

    KongDevice *device = CreateDevice(Dimension2d<u32>(800, 600), 16,
        false, false, false, &receiver);

    if (!device)
    {
        return;
    }

    IVideoDriver *driver = device->GetVideoDriver();
    ISceneManager *smr = device->GetSceneManager();

    smr->AddPerspectiveCameraSceneNode(nullptr, Vector3Df(0.0f, 0.4f, -0.9f), Vector3Df(0.f, 1.f, 0.f), Vector3Df(0.f, 0.f, 0.f));
    //smr->AddOrthogonalCameraSceneNode(nullptr, Vector3Df(0.f, 0.f, -1.f), Vector3Df(0.f, 1.f, 0.f), Vector3Df(0.f, 0.f, 0.f));
    //IMesh * mesh = smr->getMesh("../../materials/honoka_noel.obj");
    //IMesh * mesh = smr->getMesh("../../materials/honoka_noel/source/Honoka_noel_.obj");
    //IMesh * mesh = smr->getMesh("../../materials/misaki_dress_sr/misaki.obj");
    //IMesh * mesh = smr->getMesh("../../materials/misaki_pinchos/Normal/Ponytail/misaki.obj");
    IMesh * mesh = smr->getMesh("../../materials/Misaki_Pemole/Models/Hairstyle A/misaki.obj");
    //IMesh * mesh = smr->getMesh("../../materials/saber_q.obj");
    //ISceneNode *node = smr->AddMeshSceneNode(mesh, nullptr, -1, Vector3Df(0.0f, 0.0f, -0.0f), Vector3Df(0.f, 0.f, 0.f), Vector3Df(0.0003f, 0.0003f, 0.0003f));
    ISceneNode *node = smr->AddMeshSceneNode(mesh, nullptr, -1, Vector3Df(0.0f, 0.0f, -0.0f), Vector3Df(0.f, 0.f, 0.f), Vector3Df(1.f, 1.f, 1.f));
    node->NormalizeVertice();
    f32 distance = 1.f;
    ISceneNode *bottom_plane_node = smr->AddPlaneSceneNode(distance * 5.f, nullptr, -1, Vector3Df(0.0f, 0.f, distance), Vector3Df(0.f, 0.f, 0.f), Vector3Df(1.f, 1.f, 1.f));
    ISceneNode *back_plane_node = smr->AddPlaneSceneNode(distance * 5.f, nullptr, -1, Vector3Df(0.0f, -0.5f, 0.0f), Vector3Df(-PI / 2, 0.f, 0.f), Vector3Df(1.f, 1.f, 1.f));
    //node->SetMaterialTexture(0, driver->GetTexture("../../materials/saber1.jpg"));
    //node->EnableDrawBoundingBox(true);

    ILightSceneNode *light_node = smr->AddLightSceneNode(nullptr, Vector3Df(0.f, 0.0f, -6.f));
    SLight light_data = light_node->GetLightData();
    light_data.ambient_color_ = SColorf(0.4f, 0.4f, 0.4f, 1.f);
    light_data.diffuse_color_ = SColorf(0.8f, 0.8f, 0.8f, 1.f);
    light_data.specular_color_ = SColorf(0.4f, 0.4f, 0.4f, 1.f);
    light_data.type_ = ELT_DIRECTIONAL;
    //light_data.type_ = ELT_SPOT;
    //light_data.type_ = ELT_POINT;
    light_node->SetLightData(light_data);
    light_node->SetRotation(Vector3Df(-PI / 8, PI / 4, 0.f));

    Vector3Df node_pos(0.f, 0.f, 0.f);
    Vector3Df node_rot(0.f, PI, 0.f);
    f32 movement = 0.05;
    f32 rotate_factor = 0.05;
    bool shadow_on = true;
    smr->EnableShadow(shadow_on);

    while (device->run())
    {
        driver->BeginScene();

        if (receiver.IsKeyDown(kong::KEY_KEY_Q))
        {
            node_pos.y_ += movement;
        }
        else if (receiver.IsKeyDown(kong::KEY_KEY_E))
        {
            node_pos.y_ -= movement;
        }

        if (receiver.IsKeyDown(kong::KEY_KEY_A))
        {
            node_pos.x_ -= movement;
        }
        else if (receiver.IsKeyDown(kong::KEY_KEY_D))
        {
            node_pos.x_ += movement;
        }

        if (receiver.IsKeyDown(kong::KEY_KEY_S))
        {
            node_pos.z_ -= movement;
        }
        else if (receiver.IsKeyDown(kong::KEY_KEY_W))
        {
            node_pos.z_ += movement;
        }

        if (receiver.IsKeyDown(kong::KEY_SPACE))
        {
            node_pos = Vector3Df(0.f, 0.f, 0.f);
            node_rot = Vector3Df(0.f, PI, 0.f);
        }

        if (receiver.IsKeyDown(kong::KEY_KEY_U))
        {
            node_rot.x_ -= rotate_factor;
        }
        else if (receiver.IsKeyDown(kong::KEY_KEY_J))
        {
            node_rot.x_ += rotate_factor;
        }

        if (receiver.IsKeyDown(kong::KEY_KEY_I))
        {
            node_rot.y_ -= rotate_factor;
        }
        else if (receiver.IsKeyDown(kong::KEY_KEY_K))
        {
            node_rot.y_ += rotate_factor;
        }

        if (receiver.IsKeyDown(kong::KEY_KEY_O))
        {
            node_rot.z_ -= rotate_factor;
        }
        else if (receiver.IsKeyDown(kong::KEY_KEY_L))
        {
            node_rot.z_ += rotate_factor;
        }

        if (receiver.IsKeyRelease(kong::KEY_KEY_1))
        {
            shadow_on = !shadow_on;
            smr->EnableShadow(shadow_on);
        }

        node->SetPosition(node_pos);
        node->SetRotation(node_rot);

        //driver->Draw3DLine(Vector3Df(0.f, 0.f, 0.f), Vector3Df(1.f, 1.f, 1.f));
        smr->DrawAll();

        driver->EndScene();
    }
}

void TestDrawImage()
{
    MyEventReceiver receiver;

    KongDevice *device = CreateDevice(Dimension2d<u32>(640, 480), 16,
        false, false, false, &receiver);

    if (!device)
    {
        return;
    }

    IVideoDriver *driver = device->GetVideoDriver();
    ISceneManager *smr = device->GetSceneManager();

    //smr->AddPerspectiveCameraSceneNode(nullptr, Vector3Df(0.f, 0.f, -2.f), Vector3Df(0.f, 1.f, 0.f), Vector3Df(0.f, 0.f, 0.f));
    //ISceneNode *node = smr->AddCubeSceneNode(1.f, nullptr, -1, Vector3Df(0.0f, 0.0f, -0.0f), Vector3Df(0.f, 0.f, 0.f), Vector3Df(1.f, 1.f, 1.f));

    Vector3Df node_pos(0.f, 0.f, 0.f);
    f32 movement = 0.05;

    ITexture *texture = driver->GetTexture("../../materials/saber1.jpg");

    while (device->run())
    {
        driver->BeginScene();

        //driver->Draw3DLine(Vector3Df(0.f, 0.f, 0.f), Vector3Df(1.f, 1.f, 1.f));
        //smr->DrawAll();

        driver->Draw2DImage(texture, Vector2Df(-0.5, -0.5), rect<f32>(0.f, 0.f, 1.f, 1.f));

        driver->EndScene();
    }
}

void TestReplace()
{
    std::string str1("abcdefabdef");
    int idx = 0;
    while ((idx = str1.find('a')) >= 0)
    {
        str1.replace(idx, 1, "s");
    }

    std::cout << str1.c_str() << std::endl;
}

void TestFindLastOf()
{
    std::string str("c:ad\\dd");
    const std::size_t found = str.find_last_of("/\\");
    std::cout << found << "   " << str.substr(0, found).c_str() << std::endl;
}

void TestMatrix()
{
    Matrixf mat1(Matrixf::IDENTITY), mat2(Matrixf::IDENTITY), mat3(Matrixf::IDENTITY);
    mat1.Rotate(PI / 2, PI / 2, 0);
    mat2.Rotate(PI / 2, 0.f, 0.f);
    mat3.Rotate(0.f, PI / 2, 0);
    Matrixf mat4 = mat2 * mat3;
    Matrixf mat5 = mat3 * mat2;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%f\t", mat1(i, j));
        }
        printf("\n");
    }
    printf("\n");
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%f\t", mat4(i, j));
        }
        printf("\n");
    }
    printf("\n");
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%f\t", mat5(i, j));
        }
        printf("\n");
    }
}

int main()
{
    //TestArray();
    //TestS3DVertex();
    //TestList();
    //TestWindow();
    TestObjLoad();
    //TestDrawImage();
    //TestReplace();
    //TestFindLastOf();
    //TestFileSystem();
    //TestMatrix();
    system("pause");
    return 0;
}