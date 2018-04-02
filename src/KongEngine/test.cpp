#include "KongEngine.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>

using namespace kong;
using namespace core;
using namespace video;

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
    printf("pos : %f, %f, %f\n", tmp.pos_.x, tmp.pos_.y, tmp.pos_.z);
    printf("normal : %f, %f, %f\n", tmp.normal_.x, tmp.normal_.y, tmp.normal_.z);
    printf("texcoord : %f, %f, %f\n", tmp.texcoord_.x, tmp.texcoord_.y, tmp.texcoord_.z);
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

int main()
{
    //TestArray();
    //TestS3DVertex();
    TestList();

    system("pause");
    return 0;
}