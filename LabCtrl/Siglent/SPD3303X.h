
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      LabCtrl/Siglent/SPD3303X.h

#pragma once

// ===== Import/Includes ====================================================
#include <KMS/DAQ/IAnalogInputs.h>
#include <KMS/DAQ/IAnalogOutputs.h>
#include <KMS/DAQ/IDigitalInputs.h>
#include <KMS/DAQ/IDigitalOutputs.h>

// ===== Local ==============================================================
#include "../Instrument_SCPI.h"

namespace Siglent
{

    class SPD3303X : public Instrument_SCPI, public KMS::DAQ::IAnalogInputs, public KMS::DAQ::IAnalogOutputs, public KMS::DAQ::IDigitalInputs, public KMS::DAQ::IDigitalOutputs
    {

    public:

        SPD3303X(SCPI* aSCPI, const char* aIdentification);

        // ===== Instrument =================================================
        virtual void Info();
        virtual void RetrieveAnalogInputs  (const char* aName, KMS::CLI::InstanceList<KMS::DAQ::AnalogInput  >* aList);
        virtual void RetrieveAnalogOutputs (const char* aName, KMS::CLI::InstanceList<KMS::DAQ::AnalogOutput >* aList);
        virtual void RetrieveDigitalInputs (const char* aName, KMS::CLI::InstanceList<KMS::DAQ::DigitalInput >* aList);
        virtual void RetrieveDigitalOutputs(const char* aName, KMS::CLI::InstanceList<KMS::DAQ::DigitalOutput>* aList);

        // ===== KMS::DAQ::IAnalogInputs ====================================
        virtual KMS::DAQ::AnalogValue     AI_Read    (KMS::DAQ::Id aId);
        virtual KMS::DAQ::AnalogValue_Raw AI_Read_Raw(KMS::DAQ::Id aId);

        // ===== KMS::DAQ::IAnalogOutputs ===================================
        virtual KMS::DAQ::AnalogValue     AO_Get      (KMS::DAQ::Id aId) const;
        virtual KMS::DAQ::AnalogValue_Raw AO_Get_Raw  (KMS::DAQ::Id aId) const;
        virtual void                      AO_Write    (KMS::DAQ::Id aId, KMS::DAQ::AnalogValue     aValue);
        virtual void                      AO_Write_Raw(KMS::DAQ::Id aId, KMS::DAQ::AnalogValue_Raw aValue);

        // ===== KMS::DAQ::IDigitalInputs ===================================
        virtual bool DI_Read(KMS::DAQ::Id aId);

        // ===== KMS::DAQ::IDigitalOutputs ==================================
        virtual void DO_Clear(KMS::DAQ::Id aId);
        virtual bool DO_Get  (KMS::DAQ::Id aId) const;
        virtual void DO_Set  (KMS::DAQ::Id aId, bool aValue);

    private:

        uint16_t ReadStatus() const;

        char mAddress[32];
        char mGateway[32];
        char mNetMask[32];
        char mVersion[32];

    };

}
