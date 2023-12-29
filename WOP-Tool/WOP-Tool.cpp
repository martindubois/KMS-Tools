
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      WOP-Tool/WOP-Tool.cpp

#include <KMS/Base.h>

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>
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
    virtual int  ExecuteCommand(const char* aC);

private:

    void Dump();
    void Dump(unsigned int aInstance);

    void SendRequest(unsigned int aInstance, unsigned int aMask);

    WOP::Link_Port      mLink;
    WOP::Object       * mInstances[255];
    WOP::Object_Dynamic mDynamics[255];
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
    , mSystem(VERSION, 0xffffffff, 0xff)
{
    for (unsigned int i = 0; i < 255; i++)
    {
        mInstances[i] = mDynamics + i;
    }

    mSystem.SetInstances(mInstances, 255);

    AddEntry("Port", &mPort, false);
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

int Tool::ExecuteCommand(const char* aC)
{
    assert(nullptr != aC);

    unsigned int lInstance;
    unsigned int lMask;
    int          lResult = 0;

    if (0 == strcmp(aC, "Connect"))
    {
        if (!mPort.Connect(Dev::Device::FLAG_ACCESS_READ | Dev::Device::FLAG_ACCESS_WRITE))
        {
            KMS_EXCEPTION(RESULT_CONNECT_FAILED, "Connexion failed", "");
        }
    }
    else if (0 == strcmp(aC, "Disconnect")) { mPort.Disconnect(); }
    else if (0 == strcmp(aC, "Dump"      )) { Dump(); }
    else if (0 == strcmp(aC, "Start"     )) { mLink.Start(); }
    else if (0 == strcmp(aC, "Stop"      )) { mLink.Stop(); }
    else if (1 == sscanf_s(aC, "Dump %u", &lInstance)) { Dump(lInstance); }
    else if (2 == sscanf_s(aC, "SendRequest %u %x", &lInstance, &lMask)) { SendRequest(lInstance, lMask); }
    else
    {
        lResult = CLI::Tool::ExecuteCommand(aC);
    }

    return lResult;
}

// Private
// //////////////////////////////////////////////////////////////////////////

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
