
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      LabCtrl/SCPI.cpp

// TEST COVERAGE 2024-07-18 Martin Dubois

// NOT TESTED  COM port

#include "Component.h"

// ===== C ==================================================================
#include <stdlib.h>

// ===== Local ==============================================================
#include "SCPI.h"

using namespace KMS;

// Constants
// //////////////////////////////////////////////////////////////////////////

static const char* STANDARD_COMMANDS[static_cast<unsigned int>(SCPI::StandardCommand::QTY)] =
{
    "*IDN?\n",
    "*RCL",
    "*SAV",
};

// Public
// //////////////////////////////////////////////////////////////////////////

SCPI::SCPI(Cfg::Configurator* aConfigurator) : Stream_Cfg(aConfigurator, Stream::StreamType::TCP)
{
    mSocket.mRemoteAddress.Set("192.168.0.110:5025");
}

unsigned int SCPI::Command(StandardCommand aCmd, char* aOut, unsigned int aOutSize_byte)
{
    assert(StandardCommand::QTY > aCmd);

    auto lIn = STANDARD_COMMANDS[static_cast<unsigned int>(aCmd)];

    return Command(lIn, aOut, aOutSize_byte);
}

unsigned int SCPI::Command(const char* aIn, char* aOut, unsigned int aOutSize_byte)
{
    auto lStream = GetStream();
    assert(nullptr != lStream);

    if (!lStream->IsConnected())
    {
        lStream->Connect();
    }

    unsigned int lInSize_byte = static_cast<unsigned int>(strlen(aIn));

    lStream->Write(aIn, lInSize_byte);

    unsigned int lResult = 0;

    if (0 < aOutSize_byte)
    {
        assert(nullptr != aOut);

        memset(aOut, 0, aOutSize_byte);

        lResult = lStream->Read(aOut, aOutSize_byte, 0);

        if ('\n' == aOut[lResult - 1])
        {
            lResult--;
            aOut[lResult] = '\0';
        }
    }

    return lResult;
}

void SCPI::Info()
{
    auto lStream = GetStream();
    assert(nullptr != lStream);

    auto lDevice = dynamic_cast<Dev::IDevice*>(lStream);
    if (nullptr != lDevice)
    {
        std::cout << "Connected through a COM port\n";
    }
    else
    {
        auto lSocket = dynamic_cast<Net::Socket_Client*>(lStream);
        assert(nullptr != lSocket);

        std::cout << "Connected through a TCP socket\n";

        std::cout << "    Local address   : " << lSocket->mLocalAddress.GetString() << "\n";
        std::cout << "    Local TCP port  : " << lSocket->GetLocalPort() << "\n";
        std::cout << "    Remove address  : " << lSocket->mRemoteAddress.GetString() << "\n";
        std::cout << "    Keep a live     : " << (lSocket->mKeepALive ? "Enabled" : "Disabled") << "\n";
        std::cout << "    No delay        : " << (lSocket->mNoDelay   ? "Enabled" : "Disabled") << "\n";
        std::cout << "    Receive timeout : " << lSocket->mReceiveTimeout_ms << " ms\n";
        std::cout << "    Reuse address   : " << (lSocket->mReuseAddr ? "Enabled" : "Disabled") << "\n";
        std::cout << "    Send timeout    : " << lSocket->mSendTimeout_ms << " ms\n";
    }
}
