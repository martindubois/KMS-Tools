
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      ModbusSim/Item.h

#pragma once

// ===== C++ ================================================================
#include <string>

// ===== Import/Includes ====================================================
#include <KMS/DI/Value.h>
#include <KMS/Modbus/Modbus.h>

class Item : public KMS::DI::Value
{

public:

    Item();

    // ===== DI::Value ======================================================
    virtual unsigned int Get(char* aOut, unsigned int aOutSize_byte) const;
    virtual void Set(const char* aIn);
    virtual bool Set_Try(const char* aIn);

    // ===== DI::Object =====================================================
    virtual ~Item();
    virtual bool Clear();

    unsigned int               mFlags;
    std::string                mName;
    KMS::Modbus::RegisterValue mValue;

};
