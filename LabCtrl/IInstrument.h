
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      LabCtrl/IInstrument.h

#pragma once

// ===== Import/Includes ====================================================
#include <KMS/CLI/InstanceList.h>
#include <KMS/DAQ/AnalogInput.h>
#include <KMS/DAQ/AnalogOutput.h>
#include <KMS/DAQ/DigitalInput.h>
#include <KMS/DAQ/DigitalOutput.h>

class IInstrument
{

public:

    virtual void Info() = 0;

    virtual void RetrieveAnalogInputs  (const char* aName, KMS::CLI::InstanceList<KMS::DAQ::AnalogInput  >* aList) = 0;
    virtual void RetrieveAnalogOutputs (const char* aName, KMS::CLI::InstanceList<KMS::DAQ::AnalogOutput >* aList) = 0;
    virtual void RetrieveDigitalInputs (const char* aName, KMS::CLI::InstanceList<KMS::DAQ::DigitalInput >* aList) = 0;
    virtual void RetrieveDigitalOutputs(const char* aName, KMS::CLI::InstanceList<KMS::DAQ::DigitalOutput>* aList) = 0;

};
