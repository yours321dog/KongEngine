#include "KongEngine.h"

using namespace kong;

using namespace core;

int main()
{
    KongDevice *device = CreateDevice(Dimension2d<u32>(640, 480), 16,
        false, false, false);

    if (!device)
    {
        return 1;
    }

    while (device->run())
    {
    }

    return 0;
}