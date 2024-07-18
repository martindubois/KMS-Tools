
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      LabCtrl/SCPI.h

#pragma once

// ===== Import/Includes ====================================================
#include <KMS/Cfg/Configurator.h>
#include <KMS/Stream/Stream_Cfg.h>

class SCPI : public KMS::Stream::Stream_Cfg
{

public:

    enum class StandardCommand
    {
        IDN,
        RCL,
        SAV,

        QTY
    };

    SCPI(KMS::Cfg::Configurator* aConfigurator);

    unsigned int Command(StandardCommand aCmd, char* aOut, unsigned int aOutSize_byte);

    unsigned int Command(StandardCommand aCmd, const char* aArgs, char* aOut, unsigned int aOutSize_byte);

    unsigned int Command(const char* aIn, char* aOut, unsigned int aOutSize_byte);

    void Info();

};
