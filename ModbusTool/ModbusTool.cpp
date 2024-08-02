
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023-2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      ModbusTool/ModbusTool.cpp

#include <KMS/Base.h>

// ===== Windows ============================================================
#include <winsock2.h>

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>
#include <KMS/Cfg/MetaData.h>
#include <KMS/CLI/Macros.h>
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

// Class
// //////////////////////////////////////////////////////////////////////////

class Tool final : public CLI::Tool
{

private:

    DI::Dictionary mCoils;
    DI::Dictionary mDiscreteInputs;
    DI::Dictionary mHoldingRegisters;
    DI::Dictionary mInputRegisters;

public:

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
    virtual int  ExecuteCommand(CLI::CommandLine* aCmd);
    virtual int  Run();

private:

    NO_COPY(Tool);

    int Cmd_Dump                 (CLI::CommandLine* aCmd);
    int Cmd_ReadCoil             (CLI::CommandLine* aCmd);
    int Cmd_ReadDiscreteInput    (CLI::CommandLine* aCmd);
    int Cmd_ReadHoldingRegister  (CLI::CommandLine* aCmd);
    int Cmd_ReadInputRegister    (CLI::CommandLine* aCmd);
    int Cmd_Scope                (CLI::CommandLine* aCmd);
    int Cmd_Scope_Coil           (CLI::CommandLine* aCmd);
    int Cmd_Scope_DiscreteInput  (CLI::CommandLine* aCmd);
    int Cmd_Scope_HoldingRegister(CLI::CommandLine* aCmd);
    int Cmd_Scope_InputRegister  (CLI::CommandLine* aCmd);
    int Cmd_WriteSingleCoil      (CLI::CommandLine* aCmd);
    int Cmd_WriteSingleRegister  (CLI::CommandLine* aCmd);

    void Scope_Channel(Scope::Channel_Modbus* aChannel, const char* aAddrOrName, const DI::Dictionary& aMap);

    // Modules
    CLI::Tool          mMacros;
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
        Modbus::LinkAndMaster_Cfg lLAMC(&lConfigurator, Stream::StreamType::COM);
        Tool                      lT;

        lConfigurator.AddConfigurable(&lT);

        CLI::CommandLine lCmd(aCount, aVector);

        lLAMC.ParseArguments(&lCmd);

        lT.InitMaster(lLAMC.GetMaster());

        lConfigurator.ParseFile(File::Folder::EXECUTABLE, CONFIG_FILE);
        lConfigurator.ParseFile(File::Folder::HOME, CONFIG_FILE);
        lConfigurator.ParseFile(File::Folder::CURRENT, CONFIG_FILE);

        lConfigurator.ParseArguments(&lCmd);

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

    Ptr_OF<DI::Object> lEntry;

    lEntry.Set(&mCoils           , false); AddEntry("Coils"           , lEntry, &MD_COILS);
    lEntry.Set(&mDiscreteInputs  , false); AddEntry("DiscreteInputs"  , lEntry, &MD_DISCRETE_INPUTS);
    lEntry.Set(&mHoldingRegisters, false); AddEntry("HoldingRegisters", lEntry, &MD_HOLDING_REGISTERS);
    lEntry.Set(&mInputRegisters  , false); AddEntry("InputRegisters"  , lEntry, &MD_INPUT_REGISTERS);

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

int Tool::ExecuteCommand(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    int lResult = __LINE__;

    auto lCmd = aCmd->GetCurrent();

    if      (0 == _stricmp(lCmd, "Dump"               )) { aCmd->Next(); lResult = Cmd_Dump               (aCmd); }
    else if (0 == _stricmp(lCmd, "ReadCoil"           )) { aCmd->Next(); lResult = Cmd_ReadCoil           (aCmd); }
    else if (0 == _stricmp(lCmd, "ReadDiscreteInput"  )) { aCmd->Next(); lResult = Cmd_ReadDiscreteInput  (aCmd); }
    else if (0 == _stricmp(lCmd, "ReadHoldingRegister")) { aCmd->Next(); lResult = Cmd_ReadHoldingRegister(aCmd); }
    else if (0 == _stricmp(lCmd, "ReadInputRegister"  )) { aCmd->Next(); lResult = Cmd_ReadInputRegister  (aCmd); }
    else if (0 == _stricmp(lCmd, "Scope"              )) { aCmd->Next(); lResult = Cmd_Scope              (aCmd); }
    else if (0 == _stricmp(lCmd, "WriteSingleCoil"    )) { aCmd->Next(); lResult = Cmd_WriteSingleCoil    (aCmd); }
    else if (0 == _stricmp(lCmd, "WriteSingleRegister")) { aCmd->Next(); lResult = Cmd_WriteSingleRegister(aCmd); }
    else
    {
        lResult = CLI::Tool::ExecuteCommand(aCmd);
    }

    return lResult;
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

int Tool::Cmd_Dump(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    Dump(stdout);

    return 0;
}

int Tool::Cmd_ReadCoil(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    auto lName = aCmd->GetCurrent(); aCmd->Next();

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    auto lRet = ReadCoil(lName);

    std::cout << lRet << std::endl;

    return 0;
}

int Tool::Cmd_ReadDiscreteInput(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    auto lName = aCmd->GetCurrent(); aCmd->Next();

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    auto lRet = ReadDiscreteInput(lName);

    std::cout << lRet << std::endl;

    return 0;
}

int Tool::Cmd_ReadHoldingRegister(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    auto lName = aCmd->GetCurrent(); aCmd->Next();

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    auto lRet = ReadHoldingRegister(lName);

    std::cout << lRet << std::endl;

    return 0;
}

int Tool::Cmd_ReadInputRegister(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    auto lName = aCmd->GetCurrent(); aCmd->Next();

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    auto lRet = ReadInputRegister(lName);

    std::cout << lRet << std::endl;

    return 0;
}

int Tool::Cmd_Scope(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    int lResult = __LINE__;

    auto lCmd = aCmd->GetCurrent();

    if      (0 == _stricmp(lCmd, "Coil"           )) { aCmd->Next(); lResult = Cmd_Scope_Coil           (aCmd); }
    else if (0 == _stricmp(lCmd, "DiscreteInput"  )) { aCmd->Next(); lResult = Cmd_Scope_DiscreteInput  (aCmd); }
    else if (0 == _stricmp(lCmd, "HoldingRegister")) { aCmd->Next(); lResult = Cmd_Scope_HoldingRegister(aCmd); }
    else if (0 == _stricmp(lCmd, "InputRegister"  )) { aCmd->Next(); lResult = Cmd_Scope_InputRegister  (aCmd); }
    {
        aCmd->Reset();

        lResult = CLI::Tool::ExecuteCommand(aCmd);
    }

    return lResult;
}

int Tool::Cmd_Scope_Coil(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    auto lName = aCmd->GetCurrent(); aCmd->Next();

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    Scope_Channel(new Scope::Channel_Modbus_Coil(), lName, mCoils);
    
    return 0;
}

int Tool::Cmd_Scope_DiscreteInput(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    auto lName = aCmd->GetCurrent(); aCmd->Next();

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    Scope_Channel(new Scope::Channel_Modbus_DiscreteInput(), lName, mDiscreteInputs);
    
    return 0;
}

int Tool::Cmd_Scope_HoldingRegister(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    auto lName = aCmd->GetCurrent(); aCmd->Next();

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    Scope_Channel(new Scope::Channel_Modbus_HoldingRegister(), lName, mHoldingRegisters);
    
    return 0;
}

int Tool::Cmd_Scope_InputRegister(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    auto lName = aCmd->GetCurrent(); aCmd->Next();

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    Scope_Channel(new Scope::Channel_Modbus_InputRegister(), lName, mInputRegisters);
    
    return 0;
}

int Tool::Cmd_WriteSingleCoil(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    auto lName  =                 aCmd->GetCurrent() ; aCmd->Next();
    auto lValue = Convert::ToBool(aCmd->GetCurrent()); aCmd->Next();

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    WriteSingleCoil(lName, lValue);
    
    return 0;
}

int Tool::Cmd_WriteSingleRegister(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    auto lName  =                   aCmd->GetCurrent() ; aCmd->Next();
    auto lValue = Convert::ToUInt16(aCmd->GetCurrent()); aCmd->Next();

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    WriteSingleRegister(lName, lValue);
    
    return 0;
}

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
