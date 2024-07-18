
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      LabCtrl/Siglent_SPD3303X.h

// TEST COVERAGE 2024-07-18 Martin Dubois

#include "Component.h"

// ===== Local ==============================================================
#include "SCPI.h"

#include "Siglent/SPD3303X.h"

using namespace KMS;

// Constants
// //////////////////////////////////////////////////////////////////////////

#define ID_CH1_I       (0)
#define ID_CH1_V       (1)
#define ID_CH2_I       (2)
#define ID_CH2_V       (3)

#define ID_CH1_P       (4)
#define ID_CH2_P       (5)
#define ID_ERROR       (6)

#define ID_MODE        (4)

#define ID_CH1_ENABLE  (1)
#define ID_CH2_ENABLE  (2)
#define ID_CH3_ENABLE  (3)

#define ID_CH1_CC      (0)
#define ID_CH2_CC      (1)
#define ID_DHCP        (2)

#define I_MAX_A (3.2)

#define V_MAX_V (32.0)

namespace Siglent
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    SPD3303X::SPD3303X(SCPI* aSCPI, const char* aIdentification) : Instrument_SCPI(aSCPI, aIdentification)
    {
        assert(nullptr != aSCPI);

        mSCPI->Command("GATEaddr?\n"      , mGateway, sizeof(mGateway));
        mSCPI->Command("MASKaddr?\n"      , mNetMask, sizeof(mNetMask));
        mSCPI->Command("IPaddr?\n"        , mAddress, sizeof(mAddress));
        mSCPI->Command("SYSTem:VERSion?\n", mVersion, sizeof(mVersion));
    }

    // ===== Instrument =====================================================

    void SPD3303X::Info()
    {
        std::cout << "Type             : SPD3303X / SPD3303X-E\n";
        std::cout << "Firmware version : " << mVersion << "\n";
        std::cout << "IPv4 Configuration\n";
        std::cout << "    Address  : " << mAddress << "\n";
        std::cout << "    Net Mask : " << mNetMask << "\n";
        std::cout << "    Geteway  : " << mGateway << "\n";

        Instrument_SCPI::Info();
    }

    void SPD3303X::RetrieveAnalogInputs(const char* aName, KMS::CLI::InstanceList<KMS::DAQ::AnalogInput  >* aList)
    {
        assert(nullptr != aList);

        char lName[NAME_LENGTH];

        sprintf_s(lName, "%s.Ch1.I", aName);
        aList->Add(lName, new DAQ::AnalogInput(this, ID_CH1_I));

        sprintf_s(lName, "%s.Ch2.I", aName);
        aList->Add(lName, new DAQ::AnalogInput(this, ID_CH2_I));

        sprintf_s(lName, "%s.Ch1.V", aName);
        aList->Add(lName, new DAQ::AnalogInput(this, ID_CH1_V));

        sprintf_s(lName, "%s.Ch2.V", aName);
        aList->Add(lName, new DAQ::AnalogInput(this, ID_CH2_V));

        sprintf_s(lName, "%s.Ch1.P", aName);
        aList->Add(lName, new DAQ::AnalogInput(this, ID_CH1_P));

        sprintf_s(lName, "%s.Ch2.P", aName);
        aList->Add(lName, new DAQ::AnalogInput(this, ID_CH2_P));

        sprintf_s(lName, "%s.Error", aName);
        aList->Add(lName, new DAQ::AnalogInput(this, ID_ERROR));
    }

    void SPD3303X::RetrieveAnalogOutputs(const char* aName, KMS::CLI::InstanceList<KMS::DAQ::AnalogOutput >* aList)
    {
        assert(nullptr != aList);

        char lName[NAME_LENGTH];

        sprintf_s(lName, "%s.Ch1.I", aName);
        aList->Add(lName, new DAQ::AnalogOutput(this, ID_CH1_I));

        sprintf_s(lName, "%s.Ch2.I", aName);
        aList->Add(lName, new DAQ::AnalogOutput(this, ID_CH2_I));

        sprintf_s(lName, "%s.Ch1.V", aName);
        aList->Add(lName, new DAQ::AnalogOutput(this, ID_CH1_V));

        sprintf_s(lName, "%s.Ch2.V", aName);
        aList->Add(lName, new DAQ::AnalogOutput(this, ID_CH2_V));

        sprintf_s(lName, "%s.Mode", aName);
        aList->Add(lName, new DAQ::AnalogOutput(this, ID_MODE));
    }

    void SPD3303X::RetrieveDigitalInputs(const char* aName, KMS::CLI::InstanceList<KMS::DAQ::DigitalInput >* aList)
    {
        assert(nullptr != aList);

        char lName[NAME_LENGTH];

        sprintf_s(lName, "%s.Ch1.CC", aName);
        aList->Add(lName, new DAQ::DigitalInput(this, ID_CH1_CC));

        sprintf_s(lName, "%s.Ch2.CC", aName);
        aList->Add(lName, new DAQ::DigitalInput(this, ID_CH2_CC));

        sprintf_s(lName, "%s.DHCP", aName);
        aList->Add(lName, new DAQ::DigitalInput(this, ID_DHCP));
    }

    void SPD3303X::RetrieveDigitalOutputs(const char* aName, KMS::CLI::InstanceList<KMS::DAQ::DigitalOutput>* aList)
    {
        assert(nullptr != aList);

        char lName[NAME_LENGTH];

        sprintf_s(lName, "%s.Ch1.Enable", aName);
        aList->Add(lName, new DAQ::DigitalOutput(this, ID_CH1_ENABLE));

        sprintf_s(lName, "%s.Ch2.Enable", aName);
        aList->Add(lName, new DAQ::DigitalOutput(this, ID_CH2_ENABLE));

        sprintf_s(lName, "%s.Ch3.Enable", aName);
        aList->Add(lName, new DAQ::DigitalOutput(this, ID_CH3_ENABLE));
    }

    // ===== DAQ::IAnalogInputs =============================================

    DAQ::AnalogValue SPD3303X::AI_Read(DAQ::Id aId)
    {
        assert(nullptr != mSCPI);

        const char* lCmd = nullptr;

        char lOut[32];

        switch (aId)
        {
        case ID_CH1_I: lCmd = "MEASure:CURRent? CH1\n"; break;
        case ID_CH1_P: lCmd = "MEASure:POWEr? CH1\n"  ; break;
        case ID_CH1_V: lCmd = "MEASure:VOLTage? CH1\n"; break;
        case ID_CH2_I: lCmd = "MEASure:CURRent? CH2\n"; break;
        case ID_CH2_P: lCmd = "MEASure:POWEr? CH2\n"  ; break;
        case ID_CH2_V: lCmd = "MEASure:VOLTage? CH2\n"; break;

        case ID_ERROR:
            mSCPI->Command("SYSTem:ERRor?\n", lOut, sizeof(lOut));

            return strtod(lOut, nullptr);

        default: assert(false);
        }

        mSCPI->Command(lCmd, lOut, sizeof(lOut));

        return Convert::ToDouble(lOut);
    }

    DAQ::AnalogValue_Raw SPD3303X::AI_Read_Raw(DAQ::Id aId) { assert(false); return 0; }

    // ===== DAQ::IAnalogOutputs ============================================

    DAQ::AnalogValue SPD3303X::AO_Get(DAQ::Id aId) const
    {
        assert(nullptr != mSCPI);

        const char* lCmd = nullptr;

        uint16_t lStatus;

        switch (aId)
        {
        case ID_CH1_I: lCmd = "CH1:CURRent?\n"; break;
        case ID_CH1_V: lCmd = "CH1:VOLTage?\n"; break;
        case ID_CH2_I: lCmd = "CH2:CURRent?\n"; break;
        case ID_CH2_V: lCmd = "CH2:VOLTage?\n"; break;

        case ID_MODE:
            lStatus = ReadStatus();

            return (lStatus & 0x000c) >> 2;

        default: assert(false);
        }

        char lOut[32];

        mSCPI->Command(lCmd, lOut, sizeof(lOut));

        return Convert::ToDouble(lOut);
    }

    DAQ::AnalogValue_Raw SPD3303X::AO_Get_Raw(DAQ::Id aId) const { assert(false); return 0; }

    void SPD3303X::AO_Write(DAQ::Id aId, DAQ::AnalogValue aValue)
    {
        assert(nullptr != mSCPI);

        char lCmd[LINE_LENGTH];

        switch (aId)
        {
        case ID_CH1_I:
            KMS_EXCEPTION_ASSERT(0.0     <= aValue, RESULT_INVALID_VALUE, "Too low" , "");
            KMS_EXCEPTION_ASSERT(I_MAX_A >= aValue, RESULT_INVALID_VALUE, "Too high", "");
            sprintf_s(lCmd, "CH1:CURRent %.3lf\n", aValue);
            break;

        case ID_CH1_V:
            KMS_EXCEPTION_ASSERT(0.0     <= aValue, RESULT_INVALID_VALUE, "Too low" , "");
            KMS_EXCEPTION_ASSERT(V_MAX_V >= aValue, RESULT_INVALID_VALUE, "Too high", "");
            sprintf_s(lCmd, "CH1:VOLTage %.3lf\n", aValue);
            break;

        case ID_CH2_I:
            KMS_EXCEPTION_ASSERT(0.0     <= aValue, RESULT_INVALID_VALUE, "Too low" , "");
            KMS_EXCEPTION_ASSERT(I_MAX_A >= aValue, RESULT_INVALID_VALUE, "Too high", "");
            sprintf_s(lCmd, "CH2:CURRent %.3lf\n", aValue);
            break;

        case ID_CH2_V:
            KMS_EXCEPTION_ASSERT(0.0     <= aValue, RESULT_INVALID_VALUE, "Too low" , "");
            KMS_EXCEPTION_ASSERT(V_MAX_V >= aValue, RESULT_INVALID_VALUE, "Too high", "");
            sprintf_s(lCmd, "CH2:VOLTage %.3lf\n", aValue);
            break;

        case ID_MODE:
            KMS_EXCEPTION_ASSERT((0.0 == aValue) || (1.0 == aValue) || (2.0 == aValue), RESULT_INVALID_VALUE, "Invalid mode", "");
            sprintf_s(lCmd, "OUTPut:TRACK %u\n", static_cast<unsigned int>(aValue)); break;

        default: assert(false);
        }

        mSCPI->Command(lCmd, nullptr, 0);
    }

    void SPD3303X::AO_Write_Raw(DAQ::Id aId, DAQ::AnalogValue_Raw aValue) { assert(false); }

    // ===== DAQ::IDigitalInpus =============================================

    bool SPD3303X::DI_Read(DAQ::Id aId)
    {
        assert(nullptr != mSCPI);

        uint16_t lMask;

        switch (aId)
        {
        case ID_CH1_CC: lMask = 0x0001; break;
        case ID_CH2_CC: lMask = 0x0002; break;

        case ID_DHCP:
            char lOut[32];

            mSCPI->Command("DHCP?\n", lOut, sizeof(lOut));
            return 0 == _stricmp(lOut, "DHCP:ON");

        default: assert(false);
        }

        auto lStatus = ReadStatus();

        return 0 != (lStatus & lMask);
    }

    // ===== DAQ::IDigitalOutpus ============================================

    void SPD3303X::DO_Clear(DAQ::Id aId) { DO_Set(aId, false); }

    bool SPD3303X::DO_Get(DAQ::Id aId) const
    {
        assert(nullptr != mSCPI);

        uint16_t lMask;

        switch (aId)
        {
        case ID_CH1_ENABLE: lMask = 0x0010; break;
        case ID_CH2_ENABLE: lMask = 0x0020; break;

        case ID_CH3_ENABLE: return false;

        default: assert(false);
        }

        auto lStatus = ReadStatus();

        return 0 != (lStatus & lMask);
    }

    void SPD3303X::DO_Set(DAQ::Id aId, bool aValue)
    {
        assert(nullptr != mSCPI);

        char lCmd[LINE_LENGTH];

        switch (aId)
        {
        case ID_CH1_ENABLE : sprintf_s(lCmd, "OUTPut CH1,%s\n", aValue ? "ON" : "OFF"); break;
        case ID_CH2_ENABLE : sprintf_s(lCmd, "OUTPut CH2,%s\n", aValue ? "ON" : "OFF"); break;
        case ID_CH3_ENABLE : sprintf_s(lCmd, "OUTPut CH3,%s\n", aValue ? "ON" : "OFF"); break;

        default: assert(false);
        }

        mSCPI->Command(lCmd, nullptr, 0);
    }

    // Private
    // //////////////////////////////////////////////////////////////////////

    uint16_t SPD3303X::ReadStatus() const
    {
        assert(nullptr != mSCPI);

        char lOut[32];

        mSCPI->Command("SYSTem:STATus?\n", lOut, sizeof(lOut));

        return Convert::ToUInt16(lOut);
    }

}
