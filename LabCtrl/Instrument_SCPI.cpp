
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      LabCtrl/Instrument_SCPI.cpp

// TEST COVERAGE 2024-07-18 Martin Dubois

#include "Component.h"

// ===== Local ==============================================================
#include "Instrument_SCPI.h"
#include "SCPI.h"

// Public
// //////////////////////////////////////////////////////////////////////////

Instrument_SCPI::Instrument_SCPI(SCPI* aSCPI, const char* aIdentification)
    : mIdentification(aIdentification)
    , mSCPI(aSCPI)
{
    assert(nullptr != aSCPI);
    assert(nullptr != aIdentification);
}

// ===== IInstrument ========================================================

void Instrument_SCPI::Info()
{
    std::cout << "Identification : " << mIdentification << "\n";

    mSCPI->Info();
}
