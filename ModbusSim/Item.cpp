
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      ModbusSim/Item.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Convert.h>

// ===== Local ==============================================================
#include "Item.h"

using namespace KMS;

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static Modbus::RegisterValue ToRegisterValue(const char* aIn);

// Public
// //////////////////////////////////////////////////////////////////////////

Item::Item() : mFlags(0), mValue(0) {}

// ===== DI::Value ==========================================================

unsigned int Item::Get(char* aOut, unsigned int aOutSize_byte) const
{
    assert(nullptr != aOut);

    // TODO Validate aOutSize_byte

    return sprintf_s(aOut SizeInfoV(aOutSize_byte), "%s;%u;%u", mName.c_str(), mValue, mFlags);
}

void Item::Set(const char* aIn)
{
    auto lRet = Set_Try(aIn);
    KMS_EXCEPTION_ASSERT(lRet, RESULT_INVALID_CONFIG, "Invalid bit information", aIn);
}

bool Item::Set_Try(const char* aIn)
{
    assert(nullptr != aIn);

    char lN[64];
    char lV[64];
    char lF[64];

    unsigned int          lFlags = 0;
    Modbus::RegisterValue lValue = 0;

    switch (sscanf_s(aIn, "%[^;];%[^;];%[^\n\r\t]", lN SizeInfo(lN), lV SizeInfo(lV), lF SizeInfo(lF)))
    {
    case 3: lFlags = Convert::ToUInt32(lF);
    case 2: lValue = ToRegisterValue(lV);
    case 1: break;

    default: return false;
    }

    mFlags = lFlags;
    mName  = lN;
    mValue = lValue;

    return true;
}

// ===== DI::Object =========================================================

Item::~Item() {}

bool Item::Clear()
{
    auto lResult = !mName.empty();

    mFlags = 0;
    mName  = "";
    mValue = 0;

    return lResult;
}

// Static functions
// //////////////////////////////////////////////////////////////////////////

Modbus::RegisterValue ToRegisterValue(const char* aIn)
{
    assert(nullptr != aIn);

    if (0 == _stricmp("false", aIn)) { return 0; }
    if (0 == _stricmp("true" , aIn)) { return 1; }

    if (0 == _stricmp("off", aIn)) { return Modbus::OFF; }
    if (0 == _stricmp("on" , aIn)) { return Modbus::ON; }

    return Convert::ToUInt16(aIn);
}
