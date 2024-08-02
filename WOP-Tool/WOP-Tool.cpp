
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      WOP-Tool/WOP-Tool.cpp

#include <KMS/Base.h>

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>
#include <KMS/CLI/Macros.h>
#include <KMS/CLI/Tool.h>
#include <KMS/Com/Port.h>
#include <KMS/Main.h>
#include <KMS/WOP/Link_Port.h>
#include <KMS/WOP/Object_Dynamic.h>
#include <KMS/WOP/System.h>

// ===== Local ==============================================================
#include "../Common/Version.h"

using namespace KMS;

// Configuration
// //////////////////////////////////////////////////////////////////////////

#define CONFIG_FILE ("WOP-Tool.cfg")

// Class
// //////////////////////////////////////////////////////////////////////////

class Tool final : public CLI::Tool
{

public:

    Tool();

    Com::Port mPort;

    // ===== CLI::Tool ==============================================
    virtual void DisplayHelp(FILE* aOut) const;
    virtual int  ExecuteCommand(CLI::CommandLine* aCmd);

private:

    int Cmd_Connect    (CLI::CommandLine* aCmd);
    int Cmd_Disconnect (CLI::CommandLine* aCmd);
    int Cmd_Dump       (CLI::CommandLine* aCmd);
    int Cmd_SendRequest(CLI::CommandLine* aCmd);
    int Cmd_Start      (CLI::CommandLine* aCmd);
    int Cmd_Stop       (CLI::CommandLine* aCmd);

    void Dump();
    void Dump(unsigned int aInstance);

    void SendRequest(unsigned int aInstance, unsigned int aMask);

    WOP::Link_Port      mLink;
    WOP::Object       * mInstances[255];
    WOP::Object_Dynamic mDynamics[255];
    CLI::Macros         mMacros;
    WOP::System         mSystem;

};

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-Tools", "WOP-Tool");

    KMS_MAIN_BEGIN;
    {
        Tool lT;

        lConfigurator.AddConfigurable(&lT);

        lConfigurator.ParseFile(File::Folder::CURRENT, CONFIG_FILE);

        KMS_MAIN_PARSE_ARGS(aCount, aVector);

        KMS_MAIN_VALIDATE;

        lResult = lT.Run();
    }
    KMS_MAIN_END;

    KMS_MAIN_RETURN;
}

// Public
// //////////////////////////////////////////////////////////////////////////

Tool::Tool()
    : mLink(&mSystem, &mPort)
    , mMacros(this)
    , mSystem(VERSION, 0xffffffff, 0xff)
{
    for (unsigned int i = 0; i < 255; i++)
    {
        mInstances[i] = mDynamics + i;
    }

    mSystem.SetInstances(mInstances, 255);

    Ptr_OF<DI::Object> lEntry;

    lEntry.Set(&mPort, false); AddEntry("Port", lEntry);

    mPort.SetConnectFlags(Dev::Device::FLAG_ACCESS_READ | Dev::Device::FLAG_ACCESS_WRITE);

    AddModule(&mMacros);
}

// ===== CLI::Tool ==========================================================

void Tool::DisplayHelp(FILE* aOut) const
{
    assert(nullptr != aOut);

    fprintf(aOut,
        "Connect\n"
        "Disconnect\n"
        "Dump [Instance]\n"
        "SendRequest {Instance} {Mask_Hex}\n"
        "Start\n"
        "Stop\n");

    CLI::Tool::DisplayHelp(aOut);
}

int Tool::ExecuteCommand(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    int lResult = __LINE__;

    auto lCmd = aCmd->GetCurrent();

    if      (0 == _stricmp(lCmd, "Connect"    )) { aCmd->Next(); lResult = Cmd_Connect    (aCmd); }
    else if (0 == _stricmp(lCmd, "Disconnect" )) { aCmd->Next(); lResult = Cmd_Disconnect (aCmd); }
    else if (0 == _stricmp(lCmd, "Dump"       )) { aCmd->Next(); lResult = Cmd_Dump       (aCmd); }
    else if (0 == _stricmp(lCmd, "SendRequest")) { aCmd->Next(); lResult = Cmd_SendRequest(aCmd); }
    else if (0 == _stricmp(lCmd, "Start"      )) { aCmd->Next(); lResult = Cmd_Start      (aCmd); }
    else if (0 == _stricmp(lCmd, "Stop"       )) { aCmd->Next(); lResult = Cmd_Stop       (aCmd); }
    else
    {
        lResult = CLI::Tool::ExecuteCommand(aCmd);
    }

    return lResult;
}

// Private
// //////////////////////////////////////////////////////////////////////////

int Tool::Cmd_Connect(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    if (!mPort.Connect())
    {
        KMS_EXCEPTION(RESULT_CONNECT_FAILED, "Connexion failed", "");
    }

    return 0;
}

int Tool::Cmd_Disconnect(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    mPort.Disconnect();

    return 0;
}

int Tool::Cmd_Dump(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    if (aCmd->IsAtEnd())
    {
        Dump();
    }
    else
    {
        auto lInstance = Convert::ToUInt32(aCmd->GetCurrent()); aCmd->Next();

        KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

        Dump(lInstance);
    }

    return 0;
}

int Tool::Cmd_SendRequest(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    auto lInstance = Convert::ToUInt32(aCmd->GetCurrent()                         ); aCmd->Next();
    auto lMask     = Convert::ToUInt32(aCmd->GetCurrent(), KMS::Radix::HEXADECIMAL); aCmd->Next();

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    SendRequest(lInstance, lMask);

    return 0;
}

int Tool::Cmd_Start(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    mLink.Start();

    return 0;
}

int Tool::Cmd_Stop(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    mLink.Stop();

    return 0;
}

void Tool::Dump()
{
    for (unsigned int i = 0; i < 255; i++)
    {
        Dump(i);
    }

    mSystem.Dump();
}

void Tool::Dump(unsigned int aInstance)
{
    if (0 < mDynamics[aInstance].GetDataSize_byte())
    {
        std::cout << "Object_Dynamic " << aInstance << "\n";

        mDynamics[aInstance].Dump();
    }
}

void Tool::SendRequest(unsigned int aInstance, unsigned int aMask)
{
    if (0xff <= aInstance)
    {
        mSystem.SendRequest(aMask);
    }
    else
    {
        mInstances[aInstance]->SendRequest(aMask);
    }
}
