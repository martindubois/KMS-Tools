
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/pages/ModbusSim.js

import { useState } from 'react'

import LauncherParams from '../components/LauncherParams'
import KMSArguments   from '../components/KMSArguments'

export default function ModbusSim()
{
    const [ sErrorMsg, SetErrorMsg ] = useState( '' )
    const [ sKMS     , SetKMS      ] = useState( [] )
    const [ sLauncher, SetLauncher ] = useState( {} )

    const OnError = ( aError ) => { SetErrorMsg( aError.message ) }

    const OnKMSChange = ( aArguments ) => { SetKMS( aArguments ) }

    const OnLaunch = () =>
    {
        SetErrorMsg( '' )

        const lData = {}

        lData.Arguments = sKMS

        if ( sLauncher.Detach ) { lData.Detach = true }
        if ( sLauncher.Exit   ) { lData.Exit   = true }

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
            <h1> ModbusSim </h1>
            <p className = "error" > { sErrorMsg } </p>
            <KMSArguments Arguments = { sKMS } OnChange = { OnKMSChange } />
            <LauncherParams OnChange = { OnLauncherChange } Parameters = { sLauncher } />
            <button onClick = { OnLaunch } > Launch </button>
        </> )
}
