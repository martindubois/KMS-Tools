
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/pages/Launcher.js

import { useState } from 'react'

import Enum               from '../components/Enum'
import File               from '../components/File'
import Flag               from '../components/Flag'
import Folder             from '../components/Folder'
import KMSArguments       from '../components/KMSArguments'
import LauncherParameters from '../components/LauncherParameters'
import Soon               from '../components/Soon'
import Text               from '../components/Text'

export default function Launcher()
{
    const [ sErrorMsg, SetErrorMsg ] = useState( '' )
    const [ sKMS     , SetKMS      ] = useState( [] )
    const [ sLauncher, SetLauncher ] = useState( {} )
    const [ sBrowser_Prefered  , SetBrowser_Prefered   ] = useState( '' )
    const [ sExec              , SetExec               ] = useState( '' )
    const [ sFileServer_Root   , SetFileServer_Root    ] = useState( '' )
    const [ sFileServer_Verbose, SetFileServer_Verbose ] = useState( false )
    const [ sPage              , SetPage               ] = useState( '' )
    const [ sSocket_KeepALive  , SetSocket_KeepALive   ] = useState( false )
    const [ sSocket_NoDelay    , SetSocket_NoDelay     ] = useState( false )
    const [ sSocket_ReuseAddr  , SetSocket_ReuseAddr   ] = useState( false )
    const [ sTitle             , SetTitle              ] = useState( '' )

    const OnBrowser_PreferedChange   = ( aEvent ) => { SetBrowser_Prefered  ( aEvent.target.value ) }
    const OnExecChange               = ( aEvent ) => { SetExec              ( aEvent.target.value ) }
    const OnFileServer_RootChange    = ( aEvent ) => { SetFileServer_Root   ( aEvent.target.value ) }
    const OnFileServer_VerboseChange = ( aEvent ) => { SetFileServer_Verbose( aEvent.target.checked ) }
    const OnPageChange               = ( aEvent ) => { SetPage              ( aEvent.target.value ) }
    const OnSocket_KeepALiveChange   = ( aEvent ) => { SetSocket_KeepALive  ( aEvent.target.checked ) }
    const OnSocket_NoDelayChange     = ( aEvent ) => { SetSocket_NoDelay    ( aEvent.target.checked ) }
    const OnSocket_ReuseAddrChange   = ( aEvent ) => { SetSocket_ReuseAddr  ( aEvent.target.checked ) }
    const OnTitleChange              = ( aEvent ) => { SetTitle             ( aEvent.target.value ) }

    const OnError = ( aError ) => { SetErrorMsg( aError.message ) }

    const OnKMSChange = ( aArguments ) => { SetKMS( aArguments ) }

    const OnLaunch = () =>
    {
        SetErrorMsg( '' )

        var lData = {}

        lData.Arguments = sKMS

        lData.Arguments.push( 'Exec=' + sExec )

        if ( sBrowser_Prefered   ) { lData.Arguments.push( 'Browser.Prefered=' + sBrowser_Prefered ) }
        if ( sFileServer_Root    ) { lData.Arguments.push( 'FileServer.Root=' + sFileServer_Root ) }
        if ( sFileServer_Verbose ) { lData.Arguments.push( 'FileServer.Verbose' ) }
        if ( sPage               ) { lData.Arguments.push( 'Page=' + sPage ) }
        if ( sSocket_KeepALive   ) { lData.Arguments.push( 'Socket.KeepALive' ) }
        if ( sSocket_NoDelay     ) { lData.Arguments.push( 'Socket.NoDelay' ) }
        if ( sSocket_ReuseAddr   ) { lData.Arguments.push( 'Socket_ReuseAddr' ) }
        if ( sTitle              ) { lData.Arguments.push( 'Title=' + sTitle ) }

        if ( sLauncher.Detach           ) { lData.Detach           = true }
        if ( sLauncher.Exit             ) { lData.Exit             = true }
        if ( sLauncher.WorkingDirectory ) { lData.WorkingDirectory = sLauncher.WorkingDirecteory }

        fetch( '/back-end/Launch',
            {
                method : 'POST',
                headers : { 'Content-Type' : 'application/json' },
                body : JSON.stringify( lData )
            } )
            .then( ( aResponse ) => { aResponse.json().then( OnResult ) } )
            .catch( OnError )
    }

    const OnLauncherChange = ( aParameters ) => { SetLauncher( aParameters ) }

    const OnResult = ( aData ) => { if ( aData.Error ) { SetErrorMsg( aData.Error ) } }

    return (
        <>
            <h1> Launcher </h1>
            <p className = "error" > { sErrorMsg } </p>
            <table>
                <tbody>
                    <Soon Label = "AllowedRanges" />
                    <Enum Label    = "Browser.Prefered"
                          Message  = "CHROME | DEFAULT | EDGE | FIREFOX | NONE"
                          OnChange = { OnBrowser_PreferedChange }
                          Options  = { [ 'CHROME', 'DEFAULT', 'EDGE', 'FIREFOX', 'NONE'] }
                          Value    = { sBrowser_Prefered } />
                    <File Label    = "Exec"
                          Mandatory
                          OnChange = { OnExecChange }
                          Value    = { sExec } />
                    <Folder Label    = "FileServer.Root"
                            OnChange = { OnFileServer_RootChange }
                            Value    = { sFileServer_Root } />
                    <Flag Label    = "FileServer.Verbose"
                          OnChange = { OnFileServer_VerboseChange }
                          Value    = { sFileServer_Verbose } />
                    <Text Label    = "Page"
                          OnChange = { OnPageChange }
                          Value    = { sPage } />
                    <Soon Label = "Routes" />
                    <Flag Label    = "Socket.KeepALive"
                          OnChange = { OnSocket_KeepALiveChange }
                          Value    = { sSocket_KeepALive } />
                    <Soon Label = "Socket.LocalAddress" />
                    <Flag Label    = "Socket.NoDelay"
                          OnChange = { OnSocket_NoDelayChange }
                          Value    = { sSocket_NoDelay } />
                    <Soon Label = "Socket.ReceiverTimeout" />
                    <Flag Label    = "Socket.ReuseAddr"
                          OnChange = { OnSocket_ReuseAddrChange }
                          Value    = { sSocket_ReuseAddr } />
                    <Soon Label = "Socket.SendTimeout" />
                    <Text Label    = "Title"
                          OnChange = { OnTitleChange }
                          Value    = { sTitle } />
                </tbody>
            </table>
            <p />
            <KMSArguments Arguments = { sKMS } OnChange = { OnKMSChange } />
            <LauncherParameters OnChange = { OnLauncherChange } Parameters = { sLauncher } />
            <button onClick = { OnLaunch } > Launch </button>
        </> )
}
