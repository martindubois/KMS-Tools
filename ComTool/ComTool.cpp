
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      ComTool/ComTool.cpp

#include <KMS/Base.h>

// ===== Includes ===========================================================
#include <KMS/Banner.h>
#include <KMS/Cfg/MetaData.h>
#include <KMS/CLI/Tool.h>
#include <KMS/Com/Port.h>
#include <KMS/Main.h>

// ===== Local ==============================================================
#include "../Common/Version.h"

using namespace KMS;

// Configuration
// //////////////////////////////////////////////////////////////////////////

#define CONFIG_FILE ("ComTool.cfg")

// Class
// //////////////////////////////////////////////////////////////////////////

class Tool final : public CLI::Tool
{

public:

    static const char* DATA_FILE_DEFAULT;

private:

    DI::File mDataFile;

public:

    static const unsigned int FLAG_DISPLAY;
    static const unsigned int FLAG_DUMP;
    static const unsigned int FLAG_EXIT_ON_ERROR;
    static const unsigned int FLAG_FRAME_T;
    static const unsigned int FLAG_TIMESTAMP;
    static const unsigned int FLAG_WRITE;

    Tool();

    void Receive(unsigned int aSize_byte = 0, unsigned int aFlags = 0);

    void ReceiveAndVerify(const void* aIn, unsigned int aInSize_byte, unsigned int aFlags = 0);

    void ResetDataFile();

    void Send(const void* aIn, unsigned int aInSize_byte, unsigned int aFlags = 0);

    // ===== CLI::Tool ==============================================
    virtual void DisplayHelp(FILE* aOut) const;
    virtual int  ExecuteCommand(const char* aC);
    virtual int  Run();

private:

    NO_COPY(Tool);

    Com::Port mPort;

    void DisplayDumpWrite(const void* aIn, unsigned int aInSize_byte, unsigned int aFlags, const char* aOp);

    void ReceiveAndVerify_Hex(const char* aIn, unsigned int aFlags);

    void Send_Hex(const char* aIn, unsigned int aFlags);

};

// Constants
// //////////////////////////////////////////////////////////////////////////

static const Cfg::MetaData MD_DATA_FILE("DataFile = {Name}");

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void CreateTimestamp(char* aOut, unsigned int aOutSize_byte);

static void Dump(FILE* aOut, const void* aIn, unsigned int aInSize_byte);

static unsigned int ToFlags(const char* aIn);

static unsigned int ToFrameT(const void* aIn, unsigned int aInSize_byte, void* aOut, unsigned int aOutSize_byte);

// Entry point
// //////////////////////////////////////////////////////////////////////////

const char* Tool::DATA_FILE_DEFAULT = "";

const unsigned int Tool::FLAG_DISPLAY       = 0x00000001;
const unsigned int Tool::FLAG_DUMP          = 0x00000002;
const unsigned int Tool::FLAG_EXIT_ON_ERROR = 0x00000004;
const unsigned int Tool::FLAG_FRAME_T       = 0x00000008;
const unsigned int Tool::FLAG_TIMESTAMP     = 0x00000010;
const unsigned int Tool::FLAG_WRITE         = 0x00000020;

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-Tools", "ComTool");

    KMS_MAIN_BEGIN;
    {
        Tool lT;

        lConfigurator.AddConfigurable(&lT);

        lConfigurator.ParseFile(File::Folder::EXECUTABLE, CONFIG_FILE);
        lConfigurator.ParseFile(File::Folder::HOME      , CONFIG_FILE);
        lConfigurator.ParseFile(File::Folder::CURRENT   , CONFIG_FILE);

        KMS_MAIN_PARSE_ARGS(aCount, aVector);

        KMS_MAIN_VALIDATE;

        lResult = lT.Run();
    }
    KMS_MAIN_END;

    KMS_MAIN_RETURN;
}

// Public
// //////////////////////////////////////////////////////////////////

Tool::Tool() : mDataFile(nullptr, DATA_FILE_DEFAULT)
{
    mDataFile.SetMode("wb");

    AddEntry("DataFile", &mDataFile, false, &MD_DATA_FILE);

    AddEntry("Port", &mPort, false);
}

void Tool::Receive(unsigned int aSize_byte, unsigned int aFlags)
{
    assert(LINE_LENGTH > aSize_byte);

    char lData[LINE_LENGTH];

    unsigned int lSize_byte;

    if (0 == aSize_byte)
    {
        lSize_byte = mPort.Read(lData, sizeof(lData), 0);
    }
    else
    {
        lSize_byte = mPort.Read(lData, aSize_byte, Com::Port::FLAG_READ_ALL);
    }

    lData[lSize_byte] = '\0';

    if (0 < lSize_byte)
    {
        std::cout << Console::Color::GREEN;

        DisplayDumpWrite(lData, lSize_byte, aFlags, "Receive");

        std::cout << Console::Color::WHITE << std::flush;
    }
}

void Tool::ReceiveAndVerify(const void* aIn, unsigned int aInSize_byte, unsigned int aFlags)
{
    assert(0 < aInSize_byte);
    assert(LINE_LENGTH > aInSize_byte);

    char lIn[LINE_LENGTH];
    unsigned int lInSize_byte;

    if (0 != (aFlags & FLAG_FRAME_T))
    {
        lInSize_byte = ToFrameT(aIn, aInSize_byte, lIn, sizeof(lIn));
    }
    else
    {
        lInSize_byte = aInSize_byte;
        memcpy(lIn, aIn, lInSize_byte);
    }

    char lData[LINE_LENGTH];

    auto lSize_byte = mPort.Read(lData, lInSize_byte, Com::Port::FLAG_READ_ALL);

    lData[lSize_byte] = '\0';

    auto lMatch = (lInSize_byte == lSize_byte);
    if (lMatch)
    {
        lMatch = 0 == memcmp(lIn, lData, lSize_byte);
    }

    if (lMatch)
    {
        std::cout << Console::Color::GREEN;

        DisplayDumpWrite(lData, lSize_byte, aFlags, "Receive and verify PASSED");

        std::cout << Console::Color::WHITE << std::flush;
    }
    else
    {
        std::cout << Console::Color::RED;

        DisplayDumpWrite(lData, lSize_byte, aFlags | FLAG_DUMP | FLAG_TIMESTAMP, "Receive and verify FAILED");

        std::cout << Console::Color::WHITE << std::flush;
    }
}

void Tool::Send(const void* aIn, unsigned int aInSize_byte, unsigned int aFlags)
{
    assert(0 < aInSize_byte);
    assert(LINE_LENGTH > aInSize_byte);

    char lIn[LINE_LENGTH];
    unsigned int lInSize_byte;

    if (0 != (aFlags & FLAG_FRAME_T))
    {
        lInSize_byte = ToFrameT(aIn, aInSize_byte, lIn, sizeof(lIn));
    }
    else
    {
        lInSize_byte = aInSize_byte;
        memcpy(lIn, aIn, lInSize_byte);
    }

    lIn[lInSize_byte] = '\0';

    mPort.Write(lIn, lInSize_byte);

    std::cout << Console::Color::BLUE;

    DisplayDumpWrite(lIn, lInSize_byte, aFlags, "Send");

    std::cout << Console::Color::WHITE << std::flush;
}

// ===== CLI::Tool ==================================================

void Tool::DisplayHelp(FILE* aFile) const
{
    assert(nullptr != aFile);

    fprintf(aFile,
        "ClearDTR\n"
        "ClearRTS\n"
        "Connect\n"
        "Disconnect\n"
        "Receive [Flags] [Size_byte]\n"
        "ReceiveAndVerify {Flags} {Expected}\n"
        "ReceiveAndVerify_Hex {Flags} {Expected}\n"
        "Send {Flags} {Data}\n"
        "Send_Hex {Flags} {Data}\n"
        "SetDTR\n"
        "SetRTS\n"
        "StartReceive [Flags]\n"
        "Status\n");

    CLI::Tool::DisplayHelp(aFile);
}

int Tool::ExecuteCommand(const char* aC)
{
    char lData [LINE_LENGTH];
    char lFlags[LINE_LENGTH];
    int  lResult = 0;
    unsigned int lSize_byte = 0;

    if      (0 == strcmp(aC, "ClearDTR"  )) { mPort.SetDTR(false); }
    else if (0 == strcmp(aC, "ClearRTS"  )) { mPort.SetRTS(false); }
    else if (0 == strcmp(aC, "Connect"   ))
    {
        if (!mPort.Connect(Dev::Device::FLAG_ACCESS_READ | Dev::Device::FLAG_ACCESS_WRITE))
        {
            KMS_EXCEPTION(RESULT_CONNECT_FAILED, "Connexion failed", "");
        }
    }
    else if (0 == strcmp(aC, "Disconnect")) { mPort.Disconnect(); }
    else if (0 == strcmp(aC, "Receive"   )) { Receive(0, 0); }
    else if (0 == strcmp(aC, "SetDTR"    )) { mPort.SetDTR(true); }
    else if (0 == strcmp(aC, "SetRTS"    )) { mPort.SetRTS(true); }
    else if (0 == strcmp(aC, "Status"    )) { std::cout << mPort << std::endl; }
    else if (2 == sscanf_s(aC, "ReceiveAndVerify_Hex %[0A-Z_|] %[^\n\r\t]", lFlags SizeInfo(lFlags), &lData SizeInfo(lData)))
    {
        ReceiveAndVerify_Hex(lData, ToFlags(lFlags));
    }
    else if (2 == sscanf_s(aC, "ReceiveAndVerify %[0A-Z_|] %[^\n\r\t]"    , lFlags SizeInfo(lFlags), &lData SizeInfo(lData)))
    {
        ReceiveAndVerify(lData, static_cast<unsigned int>(strlen(lData)), ToFlags(lFlags));
    }
    else if (1 <= sscanf_s(aC, "Receive %[0A-Z_|] %u"                     , lFlags SizeInfo(lFlags), &lSize_byte))
    {
        Receive(lSize_byte, ToFlags(lFlags));
    }
    else if (2 == sscanf_s(aC, "Send_Hex %[0A-Z_|] %[^\n\r\t]"            , lFlags SizeInfo(lFlags), &lData SizeInfo(lData)))
    {
        Send_Hex(lData, ToFlags(lFlags));
    }
    else if (2 == sscanf_s(aC, "Send %[0A-Z_|] %[^\n\r\t]"                , lFlags SizeInfo(lFlags), &lData SizeInfo(lData)))
    {
        Send(lData, static_cast<unsigned int>(strlen(lData)), ToFlags(lFlags));
    }
    else
    {
        lResult = CLI::Tool::ExecuteCommand(aC);
    }

    return lResult;
}

int Tool::Run()
{
    if (!mPort.Connect(Dev::Device::FLAG_ACCESS_READ | Dev::Device::FLAG_ACCESS_WRITE))
    {
        KMS_EXCEPTION(RESULT_CONNECT_FAILED, "Connexion failed", "");
    }

    return CLI::Tool::Run();
}

// Private
// //////////////////////////////////////////////////////////////////////////

void Tool::DisplayDumpWrite(const void* aIn, unsigned int aInSize_byte, unsigned int aFlags, const char* aOp)
{
    assert(nullptr != aIn);
    assert(nullptr != aOp);

    char lNow[NAME_LENGTH];

    CreateTimestamp(lNow, sizeof(lNow));

    if (0 != (aFlags & FLAG_DISPLAY))
    {
        if (0 != (aFlags & FLAG_TIMESTAMP)) { std::cout << aOp << " " << lNow << "\n"; }

        char lDisplay[LINE_LENGTH];

        Convert::ToDisplay(reinterpret_cast<const char*>(aIn), aInSize_byte, lDisplay, sizeof(lDisplay));

        std::cout << lDisplay << "\n";
    }

    if (0 != (aFlags & FLAG_DUMP))
    {
        if ((0 != (aFlags & FLAG_TIMESTAMP)) && (0 == (aFlags & FLAG_DISPLAY))) { std::cout << aOp << " " << lNow << "\n"; }

        Dump(stdout, aIn, aInSize_byte);
    }

    if ((0 != (aFlags & FLAG_WRITE)) && (nullptr != mDataFile.Get()))
    {
        fprintf(mDataFile, "%s %s\n", aOp, lNow);

        Dump(mDataFile, aIn, aInSize_byte);
    }
}

void Tool::ReceiveAndVerify_Hex(const char* aIn, unsigned int aFlags)
{
    uint8_t lData[LINE_LENGTH];

    auto lSize_byte = Convert::ToUInt8Array(aIn, " ", "", lData, sizeof(lData));

    ReceiveAndVerify(lData, lSize_byte, aFlags);
}

void Tool::Send_Hex(const char* aIn, unsigned int aFlags)
{
    uint8_t lData[LINE_LENGTH];

    auto lSize_byte = Convert::ToUInt8Array(aIn, " ", "", lData, sizeof(lData));

    Send(lData, lSize_byte, aFlags);
}

// Static functions
// //////////////////////////////////////////////////////////////////////////

void CreateTimestamp(char* aOut, unsigned int aOutSize_byte)
{
    assert(nullptr != aOut);

    SYSTEMTIME lTime;

    GetLocalTime(&lTime);

    sprintf_s(aOut SizeInfoV(aOutSize_byte), "%u-%u-%u %u:%u:%u.%03u",
        lTime.wYear, lTime.wMonth, lTime.wDay, lTime.wHour, lTime.wMinute, lTime.wSecond, lTime.wMilliseconds);
}

void Dump(FILE* aOut, const void* aIn, unsigned int aInSize_byte)
{
    assert(nullptr != aOut);
    assert(nullptr != aIn);

    auto lIn = static_cast<const uint8_t*>(aIn);

    for (unsigned int i = 0; i < aInSize_byte; i++)
    {
        fprintf(aOut, " %02x", lIn[i]);
    }

    fprintf(aOut, "\n");
}

unsigned int ToFrameT(const void* aIn, unsigned int aInSize_byte, void* aOut, unsigned int aOutSize_byte)
{
    assert(nullptr != aIn);
    assert(nullptr != aOut);

    KMS_EXCEPTION_ASSERT(aInSize_byte + 4 <= aOutSize_byte, RESULT_OUTPUT_TOO_SHORT, "The output buffer is too short", aInSize_byte);

    auto lOut = reinterpret_cast<uint8_t*>(aOut);
    unsigned int lResult_byte = 0;

    lOut[lResult_byte] = 0x7e; lResult_byte++;
    memcpy(lOut + lResult_byte, aIn, aInSize_byte); lResult_byte += aInSize_byte;

    uint16_t lCRC = 0xffff;

    for (unsigned int i = 0; i < lResult_byte; i++)
    {
        uint8_t lByte = lOut[i];

        lByte ^= lCRC;
        lByte ^= lByte << 4;

        lCRC = ((lByte << 8) | (0xff & (lCRC >> 8))) ^ (lByte >> 4) ^ (lByte << 3);
    }

    memcpy(lOut + lResult_byte, &lCRC, sizeof(lCRC)); lResult_byte += sizeof(lCRC);
    lOut[lResult_byte] = 0x7f; lResult_byte++;

    return lResult_byte;
}

unsigned int ToFlags(const char* aIn)
{
    unsigned int lResult = 0;

    if (NULL != strstr(aIn, "DISPLAY"      )) { lResult |= Tool::FLAG_DISPLAY; }
    if (NULL != strstr(aIn, "DUMP"         )) { lResult |= Tool::FLAG_DUMP; }
    if (NULL != strstr(aIn, "EXIT_ON_ERROR")) { lResult |= Tool::FLAG_EXIT_ON_ERROR; }
    if (NULL != strstr(aIn, "FRAME_T"      )) { lResult |= Tool::FLAG_FRAME_T; }
    if (NULL != strstr(aIn, "TIMESTAMP"    )) { lResult |= Tool::FLAG_TIMESTAMP; }
    if (NULL != strstr(aIn, "WRITE"        )) { lResult |= Tool::FLAG_WRITE; }

    return lResult;
}
