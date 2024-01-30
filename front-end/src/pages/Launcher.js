
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/pages/Launcher.js

import { useState } from 'react'

import Enum           from '../components/Enum'
import File           from '../components/File'
import Flag           from '../components/Flag'
import Folder         from '../components/Folder'
import KMSArguments   from '../components/KMSArguments'
import LauncherParams from '../components/LauncherParams'
import Numeric        from '../components/Numeric'
import Text           from '../components/Text'
import TextList       from '../components/TextList'

export default function Launcher()
{
    const [ sErrorMsg, SetErrorMsg ] = useState( '' )
    const [ sKMS     , SetKMS      ] = useState( [] )
    const [ sLauncher, SetLauncher ] = useState( {} )
    const [ sAllowedRanges   , SetAllowedRanges ] = useState( [] )
    const [ sB_Prefered      , SetB_Prefered    ] = useState( '' )
    const [ sExec            , SetExec          ] = useState( '' )
    const [ sFS_Root         , SetFS_Root       ] = useState( '' )
    const [ sFS_Verbose      , SetFS_Verbose    ] = useState( false )
    const [ sPage            , SetPage          ] = useState( '' )
    const [ sRoutes          , SetRoutes        ] = useState( [] )
    const [ sS_KeepALive     , SetS_KeepALive   ] = useState( false )
    const [ sS_LocalAddr     , SetS_LocalAddr   ] = useState( '' )
    const [ sS_NoDelay       , SetS_NoDelay     ] = useState( false )
    const [ sS_RecvTimeout_ms, SetS_RecvTimeout ] = useState( 0 )
    const [ sS_ReuseAddr     , SetS_ReuseAddr   ] = useState( false )
    const [ sS_SendTimeout_ms, SetS_SendTimeout ] = useState( 0 )
    const [ sTitle           , SetTitle         ] = useState( '' )

    const OnAllowedRangesChange = ( aData ) => { SetAllowedRanges( aData ) }
    const OnB_PreferedChange    = ( aData ) => { SetB_Prefered   ( aData ) }
    const OnExecChange          = ( aData ) => { SetExec         ( aData ) }
    const OnFS_RootChange       = ( aData ) => { SetFS_Root      ( aData ) }
    const OnFS_VerboseChange    = ( aData ) => { SetFS_Verbose   ( aData ) }
    const OnPageChange          = ( aData ) => { SetPage         ( aData ) }
    const OnRoutesChange        = ( aData ) => { SetRoutes       ( aData ) }
    const OnS_KeepALiveChange   = ( aData ) => { SetS_KeepALive  ( aData ) }
    const OnS_LocalAddr         = ( aData ) => { SetS_LocalAddr  ( aData ) }
    const OnS_NoDelayChange     = ( aData ) => { SetS_NoDelay    ( aData ) }
    const OnS_RecvTimeoutChange = ( aData ) => { SetS_RecvTimeout( aData ) }
    const OnS_ReuseAddrChange   = ( aData ) => { SetS_ReuseAddr  ( aData ) }
    const OnS_SendTimeoutChange = ( aData ) => { SetS_SendTimeout( aData ) }
    const OnTitleChange         = ( aData ) => { SetTitle        ( aData ) }

    const OnError = ( aError ) => { SetErrorMsg( aError.message ) }

    const OnKMSChange = ( aArguments ) => { SetKMS( aArguments ) }

    const OnLaunch = () =>
    {
        SetErrorMsg( '' )

        var lData = {}

        lData.Arguments = sKMS

        if ( sB_Prefered       ) { lData.Arguments.push( 'Browser.Prefered=' + sB_Prefered ) }
        if ( sExec             ) { lData.Arguments.push( 'Exec=' + sExec ) }
        if ( sFS_Root          ) { lData.Arguments.push( 'FileServer.Root=' + sFS_Root ) }
        if ( sFS_Verbose       ) { lData.Arguments.push( 'FileServer.Verbose' ) }
        if ( sPage             ) { lData.Arguments.push( 'Page=' + sPage ) }
        if ( sS_KeepALive      ) { lData.Arguments.push( 'Socket.KeepALive' ) }
        if ( sS_LocalAddr      ) { lData.Arguments.push( 'Socket.LocalAddress=' + sS_LocalAddr ) }
        if ( sS_NoDelay        ) { lData.Arguments.push( 'Socket.NoDelay' ) }
        if ( sS_RecvTimeout_ms ) { lData.Arguments.push( 'Socket.ReceiverTimeout=' + sS_RecvTimeout_ms ) }
        if ( sS_ReuseAddr      ) { lData.Arguments.push( 'Socket_ReuseAddr' ) }
        if ( sS_SendTimeout_ms ) { lData.Arguments.push( 'Socket.SendTimeout=' + sS_SendTimeout_ms ) }
        if ( sTitle            ) { lData.Arguments.push( 'Title=' + sTitle ) }

        for ( let lAllowedRange in sAllowedRanges )
        {
            lData.Arguments.push( 'AllowerRanges+=' + lAllowedRange )
        }

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
                    <TextList Label = "AllowedRanges"
                              OnChange = { OnAllowedRangesChange }
                              Values   = { sAllowedRanges } />
                    <Enum Label    = "Browser.Prefered"
                          Message  = "CHROME | DEFAULT | EDGE | FIREFOX | NONE"
                          OnChange = { OnB_PreferedChange }
                          Options  = { [ 'CHROME', 'DEFAULT', 'EDGE', 'FIREFOX', 'NONE'] }
                          Value    = { sB_Prefered } />
                    <File Label    = "Exec"
                          OnChange = { OnExecChange }
                          Value    = { sExec } />
                    <Folder Label    = "FileServer.Root"
                            OnChange = { OnFS_RootChange }
                            Value    = { sFS_Root } />
                    <Flag Label    = "FileServer.Verbose"
                          OnChange = { OnFS_VerboseChange }
                          Value    = { sFS_Verbose } />
                    <Text Label    = "Page"
                          OnChange = { OnPageChange }
                          Value    = { sPage } />
                    <TextList Label    = "Routes"
                              OnChange = { OnRoutesChange }
                              Values   = { sRoutes } />
                    <Flag Label    = "Socket.KeepALive"
                          OnChange = { OnS_KeepALiveChange }
                          Value    = { sS_KeepALive } />
                    <Text Label    = "Socket.LocalAddress"
                          OnChange = { OnS_LocalAddr }
                          Value    = { sS_LocalAddr } />
                    <Flag Label    = "Socket.NoDelay"
                          OnChange = { OnS_NoDelayChange }
                          Value    = { sS_NoDelay } />
                    <Numeric Label    = "Socket.ReceiverTimeout"
                             Max      = "60000"
                             Min      = "0"
                             OnChange = { OnS_RecvTimeoutChange }
                             Unit     = "ms"
                             Value    = { sS_RecvTimeout_ms } />
                    <Flag Label    = "Socket.ReuseAddr"
                          OnChange = { OnS_ReuseAddrChange }
                          Value    = { sS_ReuseAddr } />
                    <Numeric Label    = "Socket.SendTimeout"
                             Max      = "60000"
                             Min      = "0"
                             OnChange = { OnS_SendTimeoutChange }
                             Unit     = "ms"
                             Value    = { sS_SendTimeout_ms } />
                    <Text Label    = "Title"
                          OnChange = { OnTitleChange }
                          Value    = { sTitle } />
                </tbody>
            </table>
            <p />
            <KMSArguments Arguments = { sKMS } OnChange = { OnKMSChange } />
            <LauncherParams OnChange = { OnLauncherChange } Parameters = { sLauncher } />
            <button onClick = { OnLaunch } > Launch </button>
        </> )
}
