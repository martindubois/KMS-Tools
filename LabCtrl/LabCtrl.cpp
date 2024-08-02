
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      LabCtrl/LabCtrl.cpp

// TEST COVERAGE 2024-07-18 Martin Dubois

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>
#include <KMS/CLI/InstanceList.h>
#include <KMS/CLI/Macros.h>
#include <KMS/CLI/Tool.h>
#include <KMS/DAQ/AnalogInputModule.h>
#include <KMS/DAQ/AnalogOutputModule.h>
#include <KMS/DAQ/DigitalInputModule.h>
#include <KMS/DAQ/DigitalOutputModule.h>
#include <KMS/Main.h>

// ===== Local ==============================================================
#include "../Common/Version.h"

#include "SCPI.h"

#include "Siglent/SPD3303X.h"

using namespace KMS;

// Class
// //////////////////////////////////////////////////////////////////////////

class Tool final : public CLI::Tool
{

public:

    Tool();

    // ===== CLI::Tool ==============================================
    virtual void DisplayHelp(FILE* aOut) const;
    virtual int  ExecuteCommand(CLI::CommandLine* aCmd);

private:

    NO_COPY(Tool);

    void AddInstrument(const char* aName, IInstrument* aInstrument);

    int Cmd_Instrument     (CLI::CommandLine* aCmd);
    int Cmd_Instrument_Info(CLI::CommandLine* aCmd);
    int Cmd_SCPI           (CLI::CommandLine* aCmd);

    CLI::InstanceList<IInstrument> mInstruments;

    DAQ::AnalogInputModule   mAnalogInputs;
    DAQ::AnalogOutputModule  mAnalogOutputs;
    DAQ::DigitalInputModule  mDigitalInputs;
    DAQ::DigitalOutputModule mDigitalOutputs;

    CLI::Macros mMacros;

};

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-Tools", "LabCtrl");

    Net::Thread_Startup();

    KMS_MAIN_BEGIN;
    {
        Tool lT;

        lConfigurator.AddConfigurable(&lT);

        KMS_MAIN_PARSE_ARGS(aCount, aVector);

        KMS_MAIN_VALIDATE;

        lResult = lT.Run();
    }
    KMS_MAIN_END;

    Net::Thread_Cleanup();

    KMS_MAIN_RETURN;
}

// Public
// //////////////////////////////////////////////////////////////////////////

Tool::Tool() : mMacros(this)
{
    mInstruments.SetAllowedCmds(CLI::InstanceList_Base::CMD_LIST | CLI::InstanceList_Base::CMD_SELECT);

    AddModule(&mAnalogInputs);
    AddModule(&mAnalogOutputs);
    AddModule(&mDigitalInputs);
    AddModule(&mDigitalOutputs);

    AddModule(&mMacros);
}

void Tool::DisplayHelp(FILE* aOut) const
{
    fprintf(aOut,
        "Instrument Info\n"
        "Instrument List\n"
        "Instrument Select {Name}\n"
        "SCPI {Name} COM ...\n"
        "SCPI {Name} TCP ...\n");

    CLI::Tool::DisplayHelp(aOut);
}

int Tool::ExecuteCommand(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    auto lCmd = aCmd->GetCurrent();
    int lResult = __LINE__;

    if      (0 == _stricmp(lCmd, "Instrument")) { aCmd->Next(); lResult = Cmd_Instrument(aCmd); }
    else if (0 == _stricmp(lCmd, "SCPI"      )) { aCmd->Next(); lResult = Cmd_SCPI      (aCmd); }
    else
    {
        lResult = CLI::Tool::ExecuteCommand(aCmd);
    }

    return lResult;
}

// Private
// //////////////////////////////////////////////////////////////////////////

void Tool::AddInstrument(const char* aName, IInstrument* aInstrument)
{
    assert(nullptr != aInstrument);

    aInstrument->RetrieveAnalogInputs  (aName, &mAnalogInputs  .mAnalogInputs  );
    aInstrument->RetrieveAnalogOutputs (aName, &mAnalogOutputs .mAnalogOutputs );
    aInstrument->RetrieveDigitalInputs (aName, &mDigitalInputs .mDigitalInputs );
    aInstrument->RetrieveDigitalOutputs(aName, &mDigitalOutputs.mDigitalOutputs);

    mInstruments.Add(aName, aInstrument);
}

int Tool::Cmd_Instrument(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    int lResult = __LINE__;

    auto lCmd = aCmd->GetCurrent();

    if (0 == _stricmp(lCmd, "Info")) { aCmd->Next(); lResult = Cmd_Instrument_Info(aCmd); }
    else
    {
        lResult = mInstruments.ExecuteCommand(aCmd);
    }

    return lResult;
}

int Tool::Cmd_Instrument_Info(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments", aCmd->GetCurrent());

    mInstruments.VerifyInstanceSelected();

    std::cout << "Name : " << mInstruments.GetSelectedName() << "\n";

    mInstruments.GetSelected()->Info();

    std::cout << std::endl;

    return 0;
}

int Tool::Cmd_SCPI(CLI::CommandLine* aCmd)
{
    auto lName = aCmd->GetCurrent(); aCmd->Next();

    Cfg::Configurator lConfigurator;

    auto lSCPI = new SCPI(&lConfigurator);

    char lOut[LINE_LENGTH];

    try
    {
        lSCPI->ParseArguments(aCmd);

        lConfigurator.ParseArguments(aCmd);

        lSCPI->Command(SCPI::StandardCommand::IDN, lOut, sizeof(lOut));
    }
    catch (...)
    {
        delete lSCPI;
        throw;
    }

    Instrument_SCPI* lInstrument;

    if (0 == strncmp(lOut, "Siglent Technologies,SPD3303X", 29)) { lInstrument = new Siglent::SPD3303X(lSCPI, lOut); }
    else
    {
        delete lSCPI;

        KMS_EXCEPTION(RESULT_NOT_IMPLEMENTED, "Unsupported instrument", lOut);
    }

    AddInstrument(lName, lInstrument);

    return 0;
}
