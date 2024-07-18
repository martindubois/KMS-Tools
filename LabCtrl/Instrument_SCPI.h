
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      LabCtrl/Instrument.h

#pragma once

// ===== Local ==============================================================
#include "IInstrument.h"

class SCPI;

class Instrument_SCPI : public IInstrument
{

public:

    Instrument_SCPI(SCPI* aSCPI, const char* aIdentification);

    // ===== IInstrument ====================================================
    virtual void Info();

protected:

    SCPI* mSCPI;

private:

    std::string mIdentification;

};
