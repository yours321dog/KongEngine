// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "CKongDeviceStub.h"

kong::CKongDeviceStub::CKongDeviceStub(const SKongCreationParameters &param)
    : create_params_(param), close_(false)
{
}

kong::CKongDeviceStub::~CKongDeviceStub()
{
}
