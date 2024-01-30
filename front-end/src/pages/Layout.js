
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/pages/Layout.js

import { useState } from 'react'
import { Outlet } from 'react-router-dom'

const VERSION = '0.0.0'

export default function Layout()
{
    const [ sErrorMsg, SetErrorMsg ] = useState( '' )

    const OnError = ( aError ) => { SetErrorMsg( aError.message ) }

    const OnExit = () =>
    {
        SetErrorMsg( '' )

        const lData = {}

        fetch( '/back-end/Exit',
            {
                method : 'POST',
                headers : { 'Content-Type' : 'application/json' },
                body : JSON.stringify( lData )
            } )
            .then( ( aResponse ) => { aResponse.json().then( OnResult ) } )
            .catch( OnError )
    }

    const OnResult = ( aData ) => { if ( aData.Error ) { SetErrorMsg( aData.Error ) } }

    return (
        <>
            <header>
                <h1> KMS-Tools </h1>
                <hr/>
            </header>

            <div>
                <Outlet />
            </div>

            <div>
                <p className = "error" > { sErrorMsg } </p>
                <p> <button onClick = { OnExit }> Exit </button> </p>
            </div>

            <footer>
                <hr/>
                <address>
                    Copyright &copy; 2024 KMS -
                    Version { VERSION } -
                    <span>&nbsp;</span>
                    <a className = "address" href = "mailto:info@kms-quebec.com"> info@kms-quebec.com </a>
                </address>
            </footer>
        </> )
};
