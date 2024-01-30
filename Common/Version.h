
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      Common/Version.h

#pragma once

// ===== Includes ===========================================================
#ifdef __cplusplus
    #include <KMS/Version.h>
#endif

// Constants
// //////////////////////////////////////////////////////////////////////////

#define VERSION_RC  0,0,1,0
#define VERSION_STR "0.0.1.0"

#ifdef __cplusplus
    KMS_VERSION("front-end");
#endif
