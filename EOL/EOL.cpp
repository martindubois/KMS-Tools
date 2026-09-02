
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      LabCtrl/LabCtrl.cpp

#include <KMS/Base.h>

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>
#include <KMS/Console/Color.h>
#include <KMS/Exception.h>

// ===== Local ==============================================================
#include "../Common/Version.h"

using namespace KMS;

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static int Count(const char* aInFile);

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-Tools", "EOL");

    int lResult = __LINE__;

    try
    {
        switch (aCount)
        {
        case 2: lResult = Count(aVector[1]); break;
         
        default:
            std::cout << Console::Color::RED;
            std::cout << "USER ERROR  Invalid command line\n";
            std::cout << Console::Color::WHITE;
            std::cout << "Usage: EOL.exe{InputFileName}\n" << std::endl;
        }
    }
    KMS_CATCH_RESULT(lResult);

    return lResult;
}

// Static functions
// //////////////////////////////////////////////////////////////////////////

// --> INIT <--+
//      |      |
//      +---> CR
enum class State
{
    INIT,
    CR,
};

int Count(const char* aInFile)
{
    assert(nullptr != aInFile);

    FILE* lIn;

    auto lRet = fopen_s(&lIn, aInFile, "rb");
    KMS_EXCEPTION_ASSERT(0 == lRet, RESULT_OPEN_FAILED, "Cannot open input file", aInFile);

    char   lBuffer[1024];
    size_t lLength;
    State  lState = State::INIT;

    size_t lCariageReturn = 0;
    size_t lNewLine       = 0;
    size_t lNull          = 0;
    size_t lSpace         = 0;
    size_t lTab           = 0;

    size_t lCharacters = 0;

    size_t lCR    = 0;
    size_t lDOS   = 0;
    size_t lLinux = 0;

    while (0 < (lLength = fread(lBuffer, sizeof(char), sizeof(lBuffer) / sizeof(char), lIn)))
    {
        for (size_t i = 0; i < lLength; i++)
        {
            lCharacters++;

            bool lInit = false;

            switch (lBuffer[i])
            {
            case '\0': lNull ++; lInit = true; break;
            case '\t': lTab  ++; lInit = true; break;
            case ' ' : lSpace++; lInit = true; break;

            case '\r':
                lCariageReturn++;
                switch (lState)
                {
                case State::INIT: lState = State::CR; break;

                case State::CR: lCR++; std::cout << "(" << lDOS << ")" << std::endl; break;

                default: assert(false);
                }
                break;

            case '\n':
                lNewLine++;
                switch (lState)
                {
                case State::INIT: lLinux++; break;

                case State::CR: lDOS++; lState = State::INIT; break;

                default: assert(false);
                }
                break;

            default: lInit = true;
            }

            if (lInit)
            {
                switch (lState)
                {
                case State::INIT: break;

                case State::CR: lCR++; lState = State::INIT; break;

                default: assert(false);
                }
            }
        }
    }

    lRet = fclose(lIn);
    assert(0 == lRet);

    auto lOther = lCharacters - lCariageReturn - lNewLine - lNull - lSpace - lTab;

    printf("End of line\n");
    printf("    %5llu  CR\n"   , lCR);
    printf("    %5llu  DOS\n"  , lDOS);
    printf("    %5llu  Linux\n", lLinux);
    printf("\n");

    printf("Character\n");
    printf("    %6llu  carriage return\n", lCariageReturn);
    printf("    %6llu  new line\n"       , lNewLine);
    printf("    %6llu  null\n"           , lNull);
    printf("    %6llu  space\n"          , lSpace);
    printf("    %6llu  tab\n"            , lTab);
    printf("    %6llu  other\n"          , lOther);
    printf("    ======\n");
    printf("    %6llu  total\n"          , lCharacters);
    printf("\n");

    return 0;
}
