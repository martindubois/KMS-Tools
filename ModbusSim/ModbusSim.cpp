
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      ModbusSim/ModbusSim.cpp

#include "Component.h"

// ===== C ==================================================================
#include <signal.h>

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>
#include <KMS/Cfg/MetaData.h>
#include <KMS/Com/Port.h>
#include <KMS/DI/Array_Sparse.h>
#include <KMS/DI/Dictionary.h>
#include <KMS/Main.h>
#include <KMS/Modbus/Slave_Cfg.h>
#include <KMS/Modbus/Slave_IDevice.h>

// ===== Local ==============================================================
#include "../Common/Version.h"

#include "Item.h"

using namespace KMS;

// Configuration
// //////////////////////////////////////////////////////////////////////////

#define CONFIG_FILE ("ModbusSim.cfg")

// Class
// //////////////////////////////////////////////////////////////////////////

class Tool final : public DI::Dictionary
{

private:

    DI::Array_Sparse mCoils;
    DI::Array_Sparse mDiscreteInputs;
    DI::Array_Sparse mHoldingRegisters;
    DI::Array_Sparse mInputRegisters;

public:

    static const unsigned int FLAG_VERBOSE_CHANGE;
    static const unsigned int FLAG_VERBOSE_READ;
    static const unsigned int FLAG_VERBOSE_WRITE;

    Tool();

    void InitSlave(Modbus::Slave* aSlave);

    // aFlags  FLAG_VERBOSE_READ, FLAG_VERBOSE_WRITE, FLAG_VERBOSE_WRITE_CHANGE
    void AddCoil(const char* aN, Modbus::Address aA, bool aValue, unsigned int aFlags = 0);

    // aFlags  FLAG_VERBOSE_READ
    void AddDiscreteInput(const char* aN, Modbus::Address aA, bool aValue, unsigned int aFlags = 0);

    // aFlags  FLAG_VERBOSE_WRITE, FLAG_VERBOSE_WRITE_CHANGE
    void AddHoldingRegister(const char* aN, Modbus::Address aA, Modbus::RegisterValue aValue, unsigned int aFlags = 0);

    // aFlags  FLAG_VERBOSE_READ
    void AddInputRegister(const char* aN, Modbus::Address aA, Modbus::RegisterValue aValue, unsigned int aFlags = 0);

    int Run();

    void Stop();

private:

    Callback<Tool> ON_READ_COILS;
    Callback<Tool> ON_READ_DISCRETE_INPUTS;
    Callback<Tool> ON_READ_HOLDING_REGISTERS;
    Callback<Tool> ON_READ_INPUT_REGISTERS;
    Callback<Tool> ON_WRITE_SINGLE_COIL;
    Callback<Tool> ON_WRITE_SINGLE_REGISTER;

    unsigned int OnReadCoils           (void* aSender, void* aData);
    unsigned int OnReadDiscreteInputs  (void* aSender, void* aData);
    unsigned int OnReadHoldingRegisters(void* aSender, void* aData);
    unsigned int OnReadInputRegisters  (void* aSender, void* aData);
    unsigned int OnWriteSingleCoil     (void* aSender, void* aData);
    unsigned int OnWriteSingleRegister (void* aSender, void* aData);

    Modbus::Slave* mSlave;

};

// Constants
// //////////////////////////////////////////////////////////////////////////

static const Cfg::MetaData MD_COILS            ("Coils[{Address}] = {Name}[;{Value}][;{Flags}]");
static const Cfg::MetaData MD_DISCRETE_INPUTS  ("DiscreteInputs[{Address}] = {Name}[;{Value}][;{Flags}]");
static const Cfg::MetaData MD_HOLDING_REGISTERS("HoldingRegisters[{Address}] = {Name}[;{Value}][;{Flags}]");
static const Cfg::MetaData MD_INPUT_REGISTERS  ("InputRegisters[{Address}] = {Name}[;{Value}][;{Flags}]");

// Static variable
// //////////////////////////////////////////////////////////////////////////

static Tool* sTool = nullptr;

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

extern "C"
{
    static void OnCtrlC(int aSignal);
}

static DI::Object* CreateItem();

static void TraceKnown(const char* aOp, Modbus::Address aA, const Item& aItem, unsigned int aFlags);

static void TraceUnknown(const char* aOp, Modbus::Address aA, Modbus::RegisterValue aV);

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-Tools", "ModbusSim");

    _crt_signal_t lHandler = nullptr;

    KMS_MAIN_BEGIN;
    {
        Com::Port             lPort;
        Modbus::Slave_IDevice lSl(&lPort);
        Tool                  lT;

        Modbus::Slave_Cfg lCfg(&lSl);

        lT.InitSlave(&lSl);

        lConfigurator.AddConfigurable(&lCfg);
        lConfigurator.AddConfigurable(&lPort);
        lConfigurator.AddConfigurable(&lT);

        lConfigurator.ParseFile(File::Folder::EXECUTABLE, CONFIG_FILE);
        lConfigurator.ParseFile(File::Folder::HOME      , CONFIG_FILE);
        lConfigurator.ParseFile(File::Folder::CURRENT   , CONFIG_FILE);

        KMS_MAIN_PARSE_ARGS(aCount, aVector);

        KMS_MAIN_VALIDATE;

        sTool = &lT;

        lHandler = signal(SIGINT, OnCtrlC);

        std::cout << "Press Ctrl-C to stop" << std::endl;

        lResult = lT.Run();
    }
    KMS_MAIN_END;

    signal(SIGINT, lHandler);

    sTool = nullptr;

    KMS_MAIN_RETURN;
}

// Public
// //////////////////////////////////////////////////////////////////////////

const unsigned int Tool::FLAG_VERBOSE_CHANGE = 0x00000001;
const unsigned int Tool::FLAG_VERBOSE_READ   = 0x00000002;
const unsigned int Tool::FLAG_VERBOSE_WRITE  = 0x00000004;

Tool::Tool()
    : ON_READ_COILS            (this, &Tool::OnReadCoils)
    , ON_READ_DISCRETE_INPUTS  (this, &Tool::OnReadDiscreteInputs)
    , ON_READ_HOLDING_REGISTERS(this, &Tool::OnReadHoldingRegisters)
    , ON_READ_INPUT_REGISTERS  (this, &Tool::OnReadInputRegisters)
    , ON_WRITE_SINGLE_COIL     (this, &Tool::OnWriteSingleCoil)
    , ON_WRITE_SINGLE_REGISTER (this, &Tool::OnWriteSingleRegister)
    , mSlave(nullptr)
{
    mCoils           .SetCreator(CreateItem);
    mDiscreteInputs  .SetCreator(CreateItem);
    mHoldingRegisters.SetCreator(CreateItem);
    mInputRegisters  .SetCreator(CreateItem);

    AddEntry("Coils"           , &mCoils           , false, &MD_COILS);
    AddEntry("DiscreteInputs"  , &mDiscreteInputs  , false, &MD_DISCRETE_INPUTS);
    AddEntry("HoldingRegisters", &mHoldingRegisters, false, &MD_HOLDING_REGISTERS);
    AddEntry("InputRegisters"  , &mInputRegisters  , false, &MD_INPUT_REGISTERS);
}

void Tool::InitSlave(Modbus::Slave* aSlave)
{
    assert(nullptr != aSlave);

    mSlave = aSlave;

    mSlave->mOnReadCoils            = &ON_READ_COILS;
    mSlave->mOnReadDiscreteInputs   = &ON_READ_DISCRETE_INPUTS;
    mSlave->mOnReadHoldingRegisters = &ON_READ_HOLDING_REGISTERS;
    mSlave->mOnReadInputRegisters   = &ON_READ_INPUT_REGISTERS;
    mSlave->mOnWriteSingleCoil      = &ON_WRITE_SINGLE_COIL;
    mSlave->mOnWriteSingleRegister  = &ON_WRITE_SINGLE_REGISTER;
}

int Tool::Run()
{
    assert(nullptr != mSlave);

    if (!mSlave->Connect())
    {
        return __LINE__;
    }

    mSlave->Run();

    return 0;
}

void Tool::Stop() { assert(nullptr != mSlave); mSlave->Stop(); }

// Private
// //////////////////////////////////////////////////////////////////////////

// ===== Callbacks ==========================================================

unsigned int Tool::OnReadCoils(void*, void* aData)
{
    assert(nullptr != aData);

    auto lData = reinterpret_cast<Modbus::Slave::MsgData*>(aData);

    for (unsigned int i = 0; i < lData->mQty; i++)
    {
        auto lValue = false;

        auto lObject = mCoils.GetEntry_R(lData->mStartAddr + i);
        if (nullptr == lObject)
        {
            TraceUnknown("Read Coil", lData->mStartAddr + i, Modbus::OFF);
        }
        else
        {
            auto lItem = dynamic_cast<const Item*>(lObject);
            assert(nullptr != lItem);

            TraceKnown("Read Coil", lData->mStartAddr + i, *lItem, FLAG_VERBOSE_READ);

            lValue = lItem->mValue;
        }

        Modbus::WriteBit(lData->mBuffer, 0, i, lValue);
    }

    return 0;
}

unsigned int Tool::OnReadDiscreteInputs(void*, void* aData)
{
    assert(nullptr != aData);

    auto lData = reinterpret_cast<Modbus::Slave::MsgData*>(aData);

    for (unsigned int i = 0; i < lData->mQty; i++)
    {
        auto lValue = false;

        auto lObject = mDiscreteInputs.GetEntry_R(lData->mStartAddr + i);
        if (nullptr == lObject)
        {
            TraceUnknown("Read Discrete Input", lData->mStartAddr + i, Modbus::OFF);
        }
        else
        {
            auto lItem = dynamic_cast<const Item*>(lObject);
            assert(nullptr != lItem);

            TraceKnown("Read Discrete Input", lData->mStartAddr + i, *lItem, FLAG_VERBOSE_READ);

            lValue = lItem->mValue;
        }

        Modbus::WriteBit(lData->mBuffer, 0, i, lValue);
    }

    return 0;
}

unsigned int Tool::OnReadHoldingRegisters(void*, void* aData)
{
    assert(nullptr != aData);

    auto lData = reinterpret_cast<Modbus::Slave::MsgData*>(aData);

    for (unsigned int i = 0; i < lData->mQty; i++)
    {
        Modbus::RegisterValue lValue = 0;

        auto lObject = mHoldingRegisters.GetEntry_R(lData->mStartAddr + i);
        if (nullptr == lObject)
        {
            TraceUnknown("Read Holding Register", lData->mStartAddr + i, 0);
        }
        else
        {
            auto lItem = dynamic_cast<const Item*>(lObject);
            assert(nullptr != lItem);

            TraceKnown("Read Holding Register", lData->mStartAddr + i, *lItem, FLAG_VERBOSE_READ);

            lValue = lItem->mValue;
        }

        Modbus::WriteUInt16(lData->mBuffer, sizeof(Modbus::RegisterValue) * i, lValue);
    }

    return 0;
}

unsigned int Tool::OnReadInputRegisters(void*, void* aData)
{
    assert(nullptr != aData);

    auto lData = reinterpret_cast<Modbus::Slave::MsgData*>(aData);

    for (unsigned int i = 0; i < lData->mQty; i++)
    {
        Modbus::RegisterValue lValue = 0;

        auto lObject = mInputRegisters.GetEntry_R(lData->mStartAddr + i);
        if (nullptr == lObject)
        {
            TraceUnknown("Read Input Register", lData->mStartAddr + i, 0);
        }
        else
        {
            auto lItem = dynamic_cast<const Item*>(lObject);
            assert(nullptr != lItem);

            TraceKnown("Read Input Register", lData->mStartAddr, *lItem, FLAG_VERBOSE_READ);

            lValue = lItem->mValue;
        }

        Modbus::WriteUInt16(lData->mBuffer, sizeof(Modbus::RegisterValue) * i, lValue);
    }

    return 0;
}

unsigned int Tool::OnWriteSingleCoil(void*, void* aData)
{
    assert(nullptr != aData);

    auto lData = reinterpret_cast<Modbus::Slave::MsgData*>(aData);

    auto lValue = Modbus::ReadUInt16(lData->mBuffer, 0);

    auto lObject = mCoils.GetEntry_RW(lData->mStartAddr);
    if (nullptr == lObject)
    {
        TraceUnknown("Write Single Coil", lData->mStartAddr, lValue);
    }
    else
    {
        auto lBool = Modbus::ON == lValue;
        unsigned int lFlags = FLAG_VERBOSE_WRITE;

        auto lItem = dynamic_cast<Item*>(lObject);
        assert(nullptr != lItem);

        if ((0 != lItem->mValue) != lBool)
        {
            lItem->mValue = lBool;
            lFlags |= FLAG_VERBOSE_CHANGE;
        }

        TraceKnown("Write Single Coil", lData->mStartAddr, *lItem, lFlags);
    }

    return 0;
}

unsigned int Tool::OnWriteSingleRegister(void*, void* aData)
{
    assert(nullptr != aData);

    auto lData = reinterpret_cast<Modbus::Slave::MsgData*>(aData);

    auto lValue = Modbus::ReadUInt16(lData->mBuffer, 0);

    auto lObject = mHoldingRegisters.GetEntry_RW(lData->mStartAddr);
    if (nullptr == lObject)
    {
        TraceUnknown("Write Single Register", lData->mStartAddr, lValue);
    }
    else
    {
        unsigned int lFlags = FLAG_VERBOSE_WRITE;

        auto lItem = dynamic_cast<Item*>(lObject);
        assert(nullptr != lItem);

        if (lItem->mValue != lValue)
        {
            lItem->mValue = lValue;
            lFlags |= FLAG_VERBOSE_CHANGE;
        }

        TraceKnown("Write Single Register", lData->mStartAddr, *lItem, lFlags);
    }

    return 0;
}

// Static functions
// //////////////////////////////////////////////////////////////////////////

void OnCtrlC(int aSignal)
{
    assert(SIGINT == aSignal);

    assert(nullptr != sTool);

    std::cout << "Ctrl-C" << std::endl;

    sTool->Stop();
}

DI::Object* CreateItem() { return new Item; }

void TraceKnown(const char* aOp, Modbus::Address aA, const Item& aItem, unsigned int aFlags)
{
    assert(nullptr != aOp);

    if (0 != (aItem.mFlags & aFlags))
    {
        std::cout << aOp << " " << aItem.mName.c_str() << " (" << aA << ") = " << aItem.mValue << std::endl;
    }
}

void TraceUnknown(const char* aOp, Modbus::Address aA, Modbus::RegisterValue aV)
{
    assert(nullptr != aOp);

    std::cout << Console::Color::RED;
    std::cout << aOp << " at " << aA << " = " << aV;
    std::cout << Console::Color::WHITE << std::endl;
}
