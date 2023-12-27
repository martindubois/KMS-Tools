
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      ModbusTool/ModbusTool.cpp

#include <KMS/Base.h>

// ===== Windows ============================================================
#include <winsock2.h>

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>
#include <KMS/Cfg/MetaData.h>
#include <KMS/CLI/Tool.h>
#include <KMS/Main.h>
#include <KMS/Modbus/LinkAndMaster_Cfg.h>
#include <KMS/Net/Socket.h>
#include <KMS/Scope/Channel_Modbus.h>
#include <KMS/WGDI/Scope_Module.h>

// ===== Local ==============================================================
#include "../Common/Version.h"

using namespace KMS;

KMS_RESULT_STATIC(RESULT_MODBUS_ERROR);

// Configuration
// //////////////////////////////////////////////////////////////////////////

#define CONFIG_FILE ("ModbusTool.cfg")


class Tool final : public CLI::Tool
{

private:

    DI::Dictionary mCoils;
    DI::Dictionary mDiscreteInputs;
    DI::Dictionary mHoldingRegisters;
    DI::Dictionary mInputRegisters;

public:

    static int Main(int aCount, const char** aVector);

    Tool();

    void InitMaster(Modbus::Master* aMaster);

    void AddCoil           (const char* aN, uint16_t aA);
    void AddDiscreteInput  (const char* aN, uint16_t aA);
    void AddHoldingRegister(const char* aN, uint16_t aA);
    void AddInputRegister  (const char* aN, uint16_t aA);

    void Connect();

    void Disconnect();

    void Dump(FILE* aOut);

    // ===== Modbus functions ===========================================

    bool ReadCoil(const char* aName);

    bool ReadDiscreteInput(const char* aName);

    uint16_t ReadHoldingRegister(const char* aName);

    uint16_t ReadInputRegister(const char* aName);

    void WriteSingleCoil(const char* aName, bool aValue);

    void WriteSingleRegister(const char* aName, Modbus::RegisterValue aValue);

    // ===== CLI::Tool ==============================================
    virtual void DisplayHelp(FILE* aOut) const;
    virtual int  ExecuteCommand(const char* aC);
    virtual int  Run();

private:

    NO_COPY(Tool);

    void Scope_Channel(Scope::Channel_Modbus* aChannel, const char* aAddrOrName, const DI::Dictionary& aMap);

    WGDI::Scope_Module mScope;

    Modbus::Master* mMaster;

};

// Constants
// //////////////////////////////////////////////////////////////////////////

static const Cfg::MetaData MD_COILS            ("Coils.{Name} = {Address}");
static const Cfg::MetaData MD_DISCRETE_INPUTS  ("DiscreteInputs.{Name} = {Address}");
static const Cfg::MetaData MD_HOLDING_REGISTERS("HoldingRegisters.{Name} = {Address}");
static const Cfg::MetaData MD_INPUT_REGISTERS  ("InputRegisters.{Name} = {Address}");

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static uint16_t ToAddress(const DI::Dictionary& aMap, const char* aName);

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-Tools", "ModbusTool");

    Net::Thread_Startup();

    KMS_MAIN_BEGIN;
    {
        Modbus::LinkAndMaster_Cfg lLAMC(&lConfigurator, Modbus::LinkAndMaster_Cfg::Link::COM);
        Tool                      lT;

        lConfigurator.AddConfigurable(&lT);

        auto lArgStart = lLAMC.ParseArguments(aCount, aVector);

        lT.InitMaster(lLAMC.GetMaster());

        lConfigurator.ParseFile(File::Folder::EXECUTABLE, CONFIG_FILE);
        lConfigurator.ParseFile(File::Folder::HOME, CONFIG_FILE);
        lConfigurator.ParseFile(File::Folder::CURRENT, CONFIG_FILE);

        lConfigurator.ParseArguments(aCount - lArgStart, aVector + lArgStart);

        KMS_MAIN_VALIDATE;

        lResult = lT.Run();
    }
    KMS_MAIN_END;

    Net::Thread_Cleanup();

    KMS_MAIN_RETURN;
}

// Public
// //////////////////////////////////////////////////////////////////////////

Tool::Tool() : mMaster(nullptr)
{
    mCoils           .SetCreator(DI::UInt<uint16_t>::Create);
    mDiscreteInputs  .SetCreator(DI::UInt<uint16_t>::Create);
    mHoldingRegisters.SetCreator(DI::UInt<uint16_t>::Create);
    mInputRegisters  .SetCreator(DI::UInt<uint16_t>::Create);

    AddEntry("Coils"           , &mCoils           , false, &MD_COILS);
    AddEntry("DiscreteInputs"  , &mDiscreteInputs  , false, &MD_DISCRETE_INPUTS);
    AddEntry("HoldingRegisters", &mHoldingRegisters, false, &MD_HOLDING_REGISTERS);
    AddEntry("InputRegisters"  , &mInputRegisters  , false, &MD_INPUT_REGISTERS);

    AddModule(&mScope);
}

void Tool::InitMaster(Modbus::Master* aMaster)
{
    assert(nullptr != aMaster);

    assert(nullptr == mMaster);

    mMaster = aMaster;
}

void Tool::Connect   () { assert(nullptr != mMaster); mMaster->Connect   (); }
void Tool::Disconnect() { assert(nullptr != mMaster); mMaster->Disconnect(); }

void Tool::Dump(FILE* aOut)
{
    assert(nullptr != aOut);

    assert(nullptr != mMaster);

    const DI::UInt<uint16_t>* lAddr;

    fprintf(aOut, "Coils\n");
    for (const DI::Dictionary::Internal::value_type lVT : mCoils.mInternal)
    {
        assert(nullptr != lVT.second);

        lAddr = dynamic_cast<const DI::UInt<uint16_t>*>(lVT.second.Get());
        assert(nullptr != lAddr);

        bool lValB;

        bool lRetB = mMaster->ReadCoil(*lAddr, &lValB);
        KMS_EXCEPTION_ASSERT(lRetB, RESULT_MODBUS_ERROR, "ReadCoil failed", lAddr->Get());

        fprintf(aOut, "    %s\t(%u)\t%s\n", lVT.first.c_str(), lAddr->Get(), lValB ? "true" : "false");
    }

    fprintf(aOut, "Discrete inputs\n");
    for (const DI::Dictionary::Internal::value_type lVT : mDiscreteInputs.mInternal)
    {
        assert(nullptr != lVT.second);

        lAddr = dynamic_cast<const DI::UInt<uint16_t>*>(lVT.second.Get());
        assert(nullptr != lAddr);

        bool lValB;

        bool lRetB = mMaster->ReadDiscreteInput(*lAddr, &lValB);
        KMS_EXCEPTION_ASSERT(lRetB, RESULT_MODBUS_ERROR, "ReadDiscreteInput failed", lAddr->Get());

        fprintf(aOut, "    %s\t(%u)\t%s\n", lVT.first.c_str(), lAddr->Get(), lValB ? "true" : "false");
    }

    fprintf(aOut, "Holding registers\n");
    for (const DI::Dictionary::Internal::value_type lVT : mHoldingRegisters.mInternal)
    {
        assert(nullptr != lVT.second);

        lAddr = dynamic_cast<const DI::UInt<uint16_t>*>(lVT.second.Get());
        assert(nullptr != lAddr);

        Modbus::RegisterValue lValR;

        bool lRetB = mMaster->ReadHoldingRegister(*lAddr, &lValR);
        KMS_EXCEPTION_ASSERT(lRetB, RESULT_MODBUS_ERROR, "ReadHoldingRegister failed", lAddr->Get());

        fprintf(aOut, "    %s\t(%u)\t%u\n", lVT.first.c_str(), lAddr->Get(), lValR);
    }

    fprintf(aOut, "Input registers\n");
    for (const DI::Dictionary::Internal::value_type lVT : mInputRegisters.mInternal)
    {
        assert(nullptr != lVT.second);

        lAddr = dynamic_cast<const DI::UInt<uint16_t>*>(lVT.second.Get());
        assert(nullptr != lAddr);

        Modbus::RegisterValue lValR;

        bool lRetB = mMaster->ReadInputRegister(*lAddr, &lValR);
        KMS_EXCEPTION_ASSERT(lRetB, RESULT_MODBUS_ERROR, "ReadInputRegister failed", lAddr->Get());

        fprintf(aOut, "    %s\t(%u)\t%u\n", lVT.first.c_str(), lAddr->Get(), lValR);
    }
}

// ===== Modbus functions ===========================================

bool Tool::ReadCoil(const char* aName)
{
    assert(nullptr != mMaster);

    bool lValB;

    bool lRetB = mMaster->ReadCoil(ToAddress(mCoils, aName), &lValB);
    KMS_EXCEPTION_ASSERT(lRetB, RESULT_MODBUS_ERROR, "ReadCoil failed", aName);

    return lValB;

}

bool Tool::ReadDiscreteInput(const char* aName)
{
    assert(nullptr != mMaster);

    bool lValB;

    bool lRetB = mMaster->ReadDiscreteInput(ToAddress(mDiscreteInputs, aName), &lValB);
    KMS_EXCEPTION_ASSERT(lRetB, RESULT_MODBUS_ERROR, "ReadDiscreteInput failed", aName);

    return lValB;
}

uint16_t Tool::ReadHoldingRegister(const char* aName)
{
    assert(nullptr != mMaster);

    Modbus::RegisterValue lValR;

    bool lRetB = mMaster->ReadHoldingRegister(ToAddress(mHoldingRegisters, aName), &lValR);
    KMS_EXCEPTION_ASSERT(lRetB, RESULT_MODBUS_ERROR, "ReadHoldingRegister failed", aName);

    return lValR;
}

uint16_t Tool::ReadInputRegister(const char* aName)
{
    assert(nullptr != mMaster);

    Modbus::RegisterValue lValR;

    bool lRetB = mMaster->ReadInputRegister(ToAddress(mInputRegisters, aName), &lValR);
    KMS_EXCEPTION_ASSERT(lRetB, RESULT_MODBUS_ERROR, "ReadInputRegister failed", aName);

    return lValR;
}

void Tool::WriteSingleCoil(const char* aName, bool aValue)
{
    assert(nullptr != mMaster);

    bool lRetB = mMaster->WriteSingleCoil(ToAddress(mCoils, aName), aValue);
    KMS_EXCEPTION_ASSERT(lRetB, RESULT_MODBUS_ERROR, "WriteSingleCoil failed", aName);
}

void Tool::WriteSingleRegister(const char* aName, uint16_t aValue)
{
    assert(nullptr != mMaster);

    bool lRetB = mMaster->WriteSingleRegister(ToAddress(mHoldingRegisters, aName), aValue);
    KMS_EXCEPTION_ASSERT(lRetB, RESULT_MODBUS_ERROR, "WriteSingleRegister failed", aName);
}

// ===== CLI::Tool ==========================================================

void Tool::DisplayHelp(FILE* aOut) const
{
    assert(nullptr != aOut);

    fprintf(aOut,
        "Dump\n"
        "ReadCoil {AddrOrNAme}\n"
        "ReadDiscreteInput {AddrOrName}\n"
        "ReadHoldingRegister {AddrOrName}\n"
        "ReadInputRegister {AddrOrName}\n"
        "Scope Channel Coil {AddrOrName}\n"
        "Scope Channel DiscreteInput {AddrOrName}\n"
        "Scope Channel HoldingRegister {AddOrName}\n"
        "Scope Channel InputRegister {AddOrName}\n"
        "WriteSingleCoil {AddrOrName} {false|true}\n"
        "WriteSingleRegister {AddrOrName} {Value}\n");

    CLI::Tool::DisplayHelp(aOut);
}

int Tool::ExecuteCommand(const char* aC)
{
    char lA[NAME_LENGTH];
    char lB[NAME_LENGTH];
    char lC[NAME_LENGTH];
    char lD[NAME_LENGTH];

    switch (sscanf_s(aC, "%[^ \n\r\t] %[^ \n\r\t] %[^ \n\r\t] %[^ \n\r\t]", lA SizeInfo(lA), lB SizeInfo(lB), lC SizeInfo(lC), lD SizeInfo(lD)))
    {
    case 1:
        if (0 == strcmp("Dump", lA)) { Dump(stdout); return 0; }
        break;

    case 2:
        if (0 == strcmp("ReadCoil", lA))
        {
            bool lRet = ReadCoil(lB);
            std::cout << lRet << std::endl;
            return 0;
        }
        if (0 == strcmp("ReadDiscreteInput", lA))
        {
            bool lRet = ReadDiscreteInput(lB);
            std::cout << lRet << std::endl;
            return 0;
        }
        if (0 == strcmp("ReadHoldingRegister", lA))
        {
            uint16_t lRet = ReadHoldingRegister(lB);
            std::cout << lRet << std::endl;
            return 0;
        }
        if (0 == strcmp("ReadInputRegister", lA))
        {
            uint16_t lRet = ReadInputRegister(lB);
            std::cout << lRet << std::endl;
            return 0;
        }
        break;

    case 3:
        if (0 == strcmp("WriteSingleCoil"    , lA)) { WriteSingleCoil    (lB, Convert::ToBool  (lC)); return 0; }
        if (0 == strcmp("WriteSingleRegister", lA)) { WriteSingleRegister(lB, Convert::ToUInt16(lC)); return 0; }
        break;

    case 4:
        if ((0 == strcmp("Scope", lA)) && (0 == strcmp("Channel", lB)))
        {
            if (0 == strcmp("Coil"           , lC)) { Scope_Channel(new Scope::Channel_Modbus_Coil           (), lD, mCoils           ); return 0; }
            if (0 == strcmp("DiscreteInput"  , lC)) { Scope_Channel(new Scope::Channel_Modbus_DiscreteInput  (), lD, mDiscreteInputs  ); return 0; }
            if (0 == strcmp("HoldingRegister", lC)) { Scope_Channel(new Scope::Channel_Modbus_HoldingRegister(), lD, mHoldingRegisters); return 0; }
            if (0 == strcmp("InputRegister"  , lC)) { Scope_Channel(new Scope::Channel_Modbus_InputRegister  (), lD, mInputRegisters  ); return 0; }
        }
        break;
    }

    return CLI::Tool::ExecuteCommand(aC);
}

int Tool::Run()
{
    Connect();

    auto lRet = CLI::Tool::Run();

    Disconnect();

    return lRet;
}

// Private
// //////////////////////////////////////////////////////////////////////////

void Tool::Scope_Channel(Scope::Channel_Modbus* aChannel, const char* aAddrOrName, const DI::Dictionary& aMap)
{
    assert(nullptr != aChannel);

    aChannel->Init(mMaster, ToAddress(aMap, aAddrOrName));

    mScope.AddChannel(aChannel);
}

// Static functions
// //////////////////////////////////////////////////////////////////////////

uint16_t ToAddress(const DI::Dictionary& aMap, const char* aName)
{
    assert(nullptr != aName);

    uint16_t lResult;

    auto lObject = aMap.GetEntry_R(aName);
    if (nullptr == lObject)
    {
        lResult = Convert::ToUInt16(aName);
    }
    else
    {
        auto lAddr = dynamic_cast<const DI::UInt<uint16_t>*>(lObject);
        assert(nullptr != lAddr);

        lResult = *lAddr;
    }

    return lResult;
}
