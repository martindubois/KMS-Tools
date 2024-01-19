
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      ComTool/Launcher.cpp

#include <KMS/Base.h>

// ===== Includes ===========================================================
#include <KMS/Banner.h>
#include <KMS/Cfg/MetaData.h>
#include <KMS/DI/NetAddressRange.h>
#include <KMS/DI/UInt.h>
#include <KMS/HTTP/ReactApp.h>
#include <KMS/HTTP/Transaction.h>
#include <KMS/Main.h>
#include <KMS/Proc/Browser.h>
#include <KMS/Proc/Process.h>

// ===== Local ==============================================================
#include "../Common/Version.h"

using namespace KMS;

// Class
// //////////////////////////////////////////////////////////////////////////

class Tool final : public DI::Dictionary
{

public:

    DI::String_Expand mExec;
    DI::String        mPage;
    DI::String        mTitle;

    Tool();

    ~Tool();

    int Run();

    Proc::Browser  mBrowser;
    HTTP::ReactApp mReactApp;

private:

    NO_COPY(Tool);

    const Callback<Tool> ON_DETACH;
    const Callback<Tool> ON_EXIT;
    const Callback<Tool> ON_GET_EXIT_CODE;
    const Callback<Tool> ON_LAUNCH;
    const Callback<Tool> ON_TERMINATE;

    unsigned int OnDetach     (void* aSender, void* aData);
    unsigned int OnExit       (void* aSender, void* aData);
    unsigned int OnGetExitCode(void* aSender, void* aData);
    unsigned int OnLaunch     (void* aSender, void* aData);
    unsigned int OnTerminate  (void* aSender, void* aData);

    void Detach();
    void Exit  ();

    void Process_Delete();

    Proc::Process* mProcess;

};

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-Tools", "Launcher");

    KMS_MAIN_BEGIN;
    {
        Tool lT;

        lConfigurator.AddConfigurable(&lT);
        lConfigurator.AddConfigurable(&lT.mBrowser);
        lConfigurator.AddConfigurable(&lT.mReactApp);
        lConfigurator.AddConfigurable(&lT.mReactApp.mFileServer);
        lConfigurator.AddConfigurable(&lT.mReactApp.mServer.mSocket);

        KMS_MAIN_PARSE_ARGS(aCount, aVector);

        KMS_MAIN_VALIDATE;

        lResult = lT.Run();
    }
    KMS_MAIN_END;

    KMS_MAIN_RETURN;
}

// Constants
// //////////////////////////////////////////////////////////////////////////

static const Cfg::MetaData MD_EXEC ("Exec = {Exec}");
static const Cfg::MetaData MD_PAGE ("Page = {Page}");
static const Cfg::MetaData MD_TITLE("Title = {Title}");

// Public
// //////////////////////////////////////////////////////////////////////////

Tool::Tool()
    : ON_DETACH       (this, &Tool::OnDetach)
    , ON_EXIT         (this, &Tool::OnExit)
    , ON_GET_EXIT_CODE(this, &Tool::OnGetExitCode)
    , ON_LAUNCH       (this, &Tool::OnLaunch)
    , ON_TERMINATE(this, &Tool::OnTerminate)
{
    AddEntry("Exec" , &mExec , false, &MD_EXEC);
    AddEntry("Page" , &mPage , false, &MD_PAGE);
    AddEntry("Title", &mTitle, false, &MD_TITLE);

    mReactApp.mServer.mSocket.mAllowedRanges.AddEntry(new DI::NetAddressRange("127.0.0.1"), true);

    mReactApp.AddFunction("/API/Detach"     , &ON_DETACH);
    mReactApp.AddFunction("/API/Exit"       , &ON_EXIT);
    mReactApp.AddFunction("/API/GetExitCode", &ON_GET_EXIT_CODE);
    mReactApp.AddFunction("/API/Launch"     , &ON_LAUNCH);
    mReactApp.AddFunction("/API/Terminate"  , &ON_TERMINATE);

    mReactApp.mServer.mSocket.SetLocalPort(8080);
}

Tool::~Tool() { Process_Delete(); }

int Tool::Run()
{
    mReactApp.mServer.mThread.Start();

    mBrowser.Open(mReactApp.mServer, "", mTitle);

    mBrowser.Wait(60 * 60 * 1000);

    mReactApp.mServer.mThread.StopAndWait(60 * 1000);

    return 0;
}

// Private
// //////////////////////////////////////////////////////////////////////////

static const DI::String E_BAD_REQUEST  ("Bad request");
static const DI::String E_INVALID_STATE("Invalid state");

static const DI::String R_ERROR("Error");
static const DI::String R_OK   ("OK");

static const DI::String S_INVALID("Invalid");
static const DI::String S_RUNNING("Running");
static const DI::String S_STOPPED("Stopped");

// ----- Response -----------------------------------------------------------
// Error   Present if Result is "Error"
// Result  "Error" or "OK"
unsigned int Tool::OnDetach(void* aSender, void* aData)
{
    assert(nullptr != aData);

    auto lTransaction = reinterpret_cast<HTTP::Transaction*>(aData);

    auto lData = new DI::Dictionary;

    lTransaction->SetResponseData(lData);

    if (nullptr != mProcess)
    {
        Detach();

        lData->AddConstEntry("Result", &R_OK);
    }
    else
    {
        lData->AddConstEntry("Error" , &E_INVALID_STATE);
        lData->AddConstEntry("Result", &R_ERROR);
    }

    return 0;
}

// ----- Response -----------------------------------------------------------
// Result  "OK"
unsigned int Tool::OnExit(void* aSender, void* aData)
{
    assert(nullptr != aData);

    auto lTransaction = reinterpret_cast<HTTP::Transaction*>(aData);

    auto lData = new DI::Dictionary;

    lTransaction->SetResponseData(lData);

    Exit();

    lData->AddConstEntry("Result", &R_OK);

    return 0;
}

// ----- Response -----------------------------------------------------------
// Error     Present if Result is "Error"
// ExitCode  Present if Result is "OK" and State is "Stopped"
// Result    "Error" or "OK"
// State     "Invalid", "Running" or "Stopped"
unsigned int Tool::OnGetExitCode(void* aSender, void* aData)
{
    assert(nullptr != aData);

    auto lTransaction = reinterpret_cast<HTTP::Transaction*>(aData);

    auto lData = new DI::Dictionary;

    lTransaction->SetResponseData(lData);

    if (nullptr != mProcess)
    {
        lData->AddConstEntry("Result", &R_OK);

        if (mProcess->IsRunning())
        {
            lData->AddConstEntry("State", &S_RUNNING);
        }
        else
        {
            auto lExitCode = mProcess->GetExitCode();

            lData->AddEntry("ExitCode", new DI::UInt<uint32_t>(lExitCode), true);
            lData->AddConstEntry("State", &S_STOPPED);

            Process_Delete();
        }
    }
    else
    {
        lData->AddConstEntry("Error" , &E_INVALID_STATE);
        lData->AddConstEntry("Result", &R_ERROR);
        lData->AddConstEntry("State" , &S_INVALID);
    }

    return 0;
}

// ----- Request ------------------------------------------------------------
// Arguments         Optional, Array of string
// Detach            Optional
// Exit              Optional
// WorkingDirectory  Optional
// ----- Response -----------------------------------------------------------
// Error   Present when Result is "Error"
// Result  "Error" or "OK"
unsigned int Tool::OnLaunch(void* aSender, void* aData)
{
    assert(nullptr != aData);

    bool lLaunch = false;

    auto lTransaction = reinterpret_cast<HTTP::Transaction*>(aData);

    auto lData = new DI::Dictionary;

    lTransaction->SetResponseData(lData);

    auto lData = dynamic_cast<DI::Dictionary*>(lTransaction->GetRequestData());
    if (nullptr != lData)
    {
        auto lDetach = nullptr != lData->GetEntry_R("Detach");
        auto lExit   = nullptr != lData->GetEntry_R("Exit");

        mProcess = new Proc::Process(File::Folder::NONE, mExec);

        auto lArguments = lData->GetEntry_R<DI::Array>("Arguments");
        if (nullptr != lArguments)
        {
            lLaunch = true;

            auto lWorkingDirectory = lData->GetEntry_R<DI::String>("WorkingDirectory");
            if (nullptr != lWorkingDirectory)
            {
                mProcess->SetWorkingDirectory(lWorkingDirectory->Get());
            }

            for (auto lElement : lArguments->mInternal)
            {
                auto lArgument = dynamic_cast<DI::String*>(lElement.Get());
                if (nullptr == lArgument)
                {
                    lLaunch = false;
                    break;
                }

                mProcess->AddArgument(lArgument.Get());
            }

            if (lLaunch)
            {
                mProcess->Start();

                if (lDetach)
                {
                    Detach();
                }

                if (lExit)
                {
                    Exit();
                }

                lData->AddConstEntry("Result", &R_OK);
            }
        }
    }

    if (!lLaunch)
    {
        Process_Delete();

        lData->AddConstEntry("Error" , &E_BAD_REQUEST);
        lData->AddConstEntry("Result", &R_ERROR);
    }

    return 0;
}

// ----- Response -----------------------------------------------------------
// Error   Present if Result is "Error"
// Result  "Error" or "OK"
unsigned int Tool::OnTerminate(void* aSender, void* aData)
{
    assert(nullptr != aData);

    auto lTransaction = reinterpret_cast<HTTP::Transaction*>(aData);

    auto lData = new DI::Dictionary();

    lTransaction->SetResponseData(lData);

    if (nullptr != mProcess)
    {
        mProcess->Terminate();

        Process_Delete();

        lData->AddConstEntry("Result", &R_OK);
    }
    else
    {
        lData->AddConstEntry("Error" , &E_INVALID_STATE);
        lData->AddConstEntry("Result", &R_ERROR);
    }

    return 0;
}

void Tool::Detach()
{
    mProcess->Detach();

    Process_Delete();
}

void Tool::Exit()
{
    mBrowser.Close();
    mReactApp.mServer.mThread.Stop();

    Process_Delete();
}

void Tool::Process_Delete()
{
    if (nullptr != mProcess)
    {
        delete mProcess;
        mProcess = nullptr;
    }
}
