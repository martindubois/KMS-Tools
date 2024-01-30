
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      Launcher/PGeo.cpp

#include <KMS/Base.h>

// ===== Includes ===========================================================
#include <KMS/Banner.h>
#include <KMS/Cfg/MetaData.h>
#include <KMS/DI/Float.h>
#include <KMS/Main.h>

// ===== Local ==============================================================
#include "../Common/Version.h"

using namespace KMS;

// Class
// //////////////////////////////////////////////////////////////////////////

class Tool final : public DI::Dictionary
{

public:

    static const double CONFIDENCE_DEFAULT;
    static const double PROBABILITY_DEFAULT;

    DI::Float<double> mConfidence;
    DI::Float<double> mProbability;

    Tool();

    int Run();

    // ===== DI::Container ==================================================
    virtual void Validate() const;

};

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-Tools", "PGeo");

    KMS_MAIN_BEGIN;
    {
        Tool lT;

        lConfigurator.AddConfigurable(&lT);

        KMS_MAIN_PARSE_ARGS(aCount, aVector);

        KMS_MAIN_VALIDATE;

        lResult = lT.Run();
    }
    KMS_MAIN_END;

    KMS_MAIN_RETURN;
}

// Constants
// //////////////////////////////////////////////////////////////////////////

static const Cfg::MetaData MD_CONFIDENCE ("Confidence = {Value}");
static const Cfg::MetaData MD_PROBABILITY("Probability = {Value}");

// Public
// //////////////////////////////////////////////////////////////////////////

const double Tool::CONFIDENCE_DEFAULT  = 0.99999;
const double Tool::PROBABILITY_DEFAULT = 0.00001;

Tool::Tool()
    : mConfidence (CONFIDENCE_DEFAULT)
    , mProbability(PROBABILITY_DEFAULT)
{
    AddEntry("Confidence" , &mConfidence , false, &MD_CONFIDENCE);
    AddEntry("Probability", &mProbability, false, &MD_PROBABILITY);
}

int Tool::Run()
{
    double       lConfidence = mConfidence;
    unsigned int lCount = 0;

    double lP = mProbability;
    double lQ = 1.0 - lP;

    double lSum = 0.0;

    int lResult = 0;

    while (lConfidence > lSum)
    {
        lCount++;

        double lProbability = pow(lQ, lCount - 1) * lP;
        double lSum_Prev = lSum;

        lSum += lProbability;

        if (lSum_Prev == lSum)
        {
            std::cout << "Cannot compute at the precision required" << std::endl;
            break;
        }

        if (0 == lCount % 100000)
        {
            std::cout << lCount << " tests -> " << lSum * 100.0 << " % confidence" << std::endl;
            lResult = __LINE__;
        }
    }

    std::cout << "\n";
    std::cout << lCount << " tests -> " << lSum * 100.0 << " % confidence" << std::endl;

    return lResult;
}

// ===== DI::Container ======================================================

void Tool::Validate() const
{
    KMS_EXCEPTION_ASSERT(0.0 < mConfidence , RESULT_INVALID_CONFIG, "Confidence cannot be 0", "");
    KMS_EXCEPTION_ASSERT(1.0 > mConfidence , RESULT_INVALID_CONFIG, "Confidence cannot be 1", "");
    KMS_EXCEPTION_ASSERT(0.0 < mProbability, RESULT_INVALID_CONFIG, "Probability cannot be 0", "");
    KMS_EXCEPTION_ASSERT(1.0 > mProbability, RESULT_INVALID_CONFIG, "Probability cannot be 1", "");
}
