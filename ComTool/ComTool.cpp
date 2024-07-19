
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023-2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      ComTool/ComTool.cpp

#include <KMS/Base.h>

// ===== Includes ===========================================================
#include <KMS/Banner.h>
#include <KMS/Cfg/MetaData.h>
#include <KMS/CLI/CommandLine.h>
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
    virtual int  ExecuteCommand(CLI::CommandLine* aCmd);
    virtual int  Run();

private:

    NO_COPY(Tool);

    Com::Port mPort;

    int Cmd_ClearDTR              (CLI::CommandLine* aCmd);
    int Cmd_ClearRTS              (CLI::CommandLine* aCmd);
    int Cmd_Connect               (CLI::CommandLine* aCmd);
    int Cmd_Disconnect            (CLI::CommandLine* aCmd);
    int Cmd_Receive               (CLI::CommandLine* aCmd);
    int Cmd_ReceiveAndVerify      (CLI::CommandLine* aCmd);
    int Cmd_ReceiveAndVerify_ASCII(CLI::CommandLine* aCmd);
    int Cmd_ReceiveAndVerify_Hex  (CLI::CommandLine* aCmd);
    int Cmd_Send                  (CLI::CommandLine* aCmd);
    int Cmd_Send_ASCII            (CLI::CommandLine* aCmd);
    int Cmd_Send_Hex              (CLI::CommandLine* aCmd);
    int Cmd_SetDTR                (CLI::CommandLine* aCmd);
    int Cmd_SetRTS                (CLI::CommandLine* aCmd);
    int Cmd_Status                (CLI::CommandLine* aCmd);

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

    Ptr_OF<DI::Object> lEntry;

    lEntry.Set(&mDataFile, false); AddEntry("DataFile", lEntry, &MD_DATA_FILE);

    lEntry.Set(&mPort, false); AddEntry("Port", lEntry);

    mPort.SetConnectFlags(Dev::Device::FLAG_ACCESS_READ | Dev::Device::FLAG_ACCESS_WRITE);
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
        "ReceiveAndVerify ASCII {Flags} {Expected}\n"
        "ReceiveAndVerify Hex {Flags} {Expected}\n"
        "Send ASCII {Flags} {Data}\n"
        "Send Hex {Flags} {Data}\n"
        "SetDTR\n"
        "SetRTS\n"
        "Status\n");

    CLI::Tool::DisplayHelp(aFile);
}

int Tool::ExecuteCommand(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    int lResult = __LINE__;

    auto lCmd = aCmd->GetCurrent();

    if      (0 == _stricmp(lCmd, "ClearDTR"        )) { aCmd->Next(); lResult = Cmd_ClearDTR        (aCmd); }
    else if (0 == _stricmp(lCmd, "ClearRTS"        )) { aCmd->Next(); lResult = Cmd_ClearRTS        (aCmd); }
    else if (0 == _stricmp(lCmd, "Connect"         )) { aCmd->Next(); lResult = Cmd_Connect         (aCmd); }
    else if (0 == _stricmp(lCmd, "Disconnect"      )) { aCmd->Next(); lResult = Cmd_Disconnect      (aCmd); }
    else if (0 == _stricmp(lCmd, "Receive"         )) { aCmd->Next(); lResult = Cmd_Receive         (aCmd); }
    else if (0 == _stricmp(lCmd, "ReceiveAndVerify")) { aCmd->Next(); lResult = Cmd_ReceiveAndVerify(aCmd); }
    else if (0 == _stricmp(lCmd, "Send"            )) { aCmd->Next(); lResult = Cmd_Send            (aCmd); }
    else if (0 == _stricmp(lCmd, "SetDTR"          )) { aCmd->Next(); lResult = Cmd_SetDTR          (aCmd); }
    else if (0 == _stricmp(lCmd, "SetRTS"          )) { aCmd->Next(); lResult = Cmd_SetRTS          (aCmd); }
    else if (0 == _stricmp(lCmd, "Status"          )) { aCmd->Next(); lResult = Cmd_Status          (aCmd); }
    else
    {
        lResult = CLI::Tool::ExecuteCommand(aCmd);
    }

    return lResult;
}

int Tool::Run()
{
    if (!mPort.Connect())
    {
        KMS_EXCEPTION(RESULT_CONNECT_FAILED, "Connexion failed", "");
    }

    return CLI::Tool::Run();
}

// Private
// //////////////////////////////////////////////////////////////////////////

int Tool::Cmd_ClearDTR(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    mPort.SetDTR(false);

    return 0;
}

int Tool::Cmd_ClearRTS(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    mPort.SetRTS(false);

    return 0;
}

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

int Tool::Cmd_Receive(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    unsigned int lFlags     = 0;
    unsigned int lSize_byte = 0;

    if (!aCmd->IsAtEnd())
    {
        lFlags     =          ToFlags (aCmd->GetCurrent()); aCmd->Next();
        lSize_byte = Convert::ToUInt32(aCmd->GetCurrent()); aCmd->Next();

        KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());
    }

    Receive(lSize_byte, lFlags);

    return 0;
}

int Tool::Cmd_ReceiveAndVerify(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    int lResult = __LINE__;

    auto lCmd = aCmd->GetCurrent();

    if      (0 == _stricmp(lCmd, "ASCII")) { aCmd->Next(); lResult = Cmd_ReceiveAndVerify_ASCII(aCmd); }
    else if (0 == _stricmp(lCmd, "Hex"  )) { aCmd->Next(); lResult = Cmd_ReceiveAndVerify_Hex  (aCmd); }

    return lResult;
}

int Tool::Cmd_ReceiveAndVerify_ASCII(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    auto lFlags = ToFlags(aCmd->GetCurrent()); aCmd->Next();
    auto lData  =         aCmd->GetCurrent() ; aCmd->Next();

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    ReceiveAndVerify(lData, static_cast<unsigned int>(strlen(lData)), lFlags);

    return 0;
}

int Tool::Cmd_ReceiveAndVerify_Hex(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    auto lFlags = ToFlags(aCmd->GetCurrent()); aCmd->Next();
    auto lData  =         aCmd->GetCurrent() ; aCmd->Next();

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    ReceiveAndVerify_Hex(lData, lFlags);
    
    return 0;
}

int Tool::Cmd_Send(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    int lResult = __LINE__;

    auto lCmd = aCmd->GetCurrent();

    if      (0 == _stricmp(lCmd, "ASCII")) { aCmd->Next(); lResult = Cmd_Send_ASCII(aCmd); }
    else if (0 == _stricmp(lCmd, "Hex"  )) { aCmd->Next(); lResult = Cmd_Send_Hex  (aCmd); }

    return lResult;
}

int Tool::Cmd_Send_ASCII(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    auto lFlags = ToFlags(aCmd->GetCurrent()); aCmd->Next();
    auto lData  =         aCmd->GetCurrent() ; aCmd->Next();

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    Send(lData, static_cast<unsigned int>(strlen(lData)), lFlags);

    return 0;
}

int Tool::Cmd_Send_Hex(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    auto lFlags = ToFlags(aCmd->GetCurrent()); aCmd->Next();
    auto lData  =         aCmd->GetCurrent() ; aCmd->Next();

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    Send_Hex(lData, lFlags);

    return 0;
}

int Tool::Cmd_SetDTR(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    mPort.SetDTR(true);

    return 0;
}

int Tool::Cmd_SetRTS(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    mPort.SetRTS(true);

    return 0;
}

int Tool::Cmd_Status(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    std::cout << mPort << std::endl;

    return 0;
}

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
