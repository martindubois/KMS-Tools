
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/component/LauncherParams.js

import { useState } from 'react'

import Flag   from './Flag'
import Folder from './Folder'

// ----- Properties ---------------------------------------------------------
// OnChange
// Parameters
//  Detach
//  Exit
//  WorkingDirectory
export default function LauncherParams( aProps )
{
    const [ sDetach , SetDetach  ] = useState( aProps.Parameters.Detach )
    const [ sExit   , SetExit    ] = useState( aProps.Parameters.Exit )
    const [ sWorkDir, SetWorkDir ] = useState( aProps.Parameters.sWorkingDirectory )

    const OnDetachChange  = ( aData ) => { SetDetach ( aData ); CallOnChange(); }
    const OnExitChange    = ( aData ) => { SetExit   ( aData ); CallOnChange(); }
    const OnWorkDirChange = ( aData ) => { SetWorkDir( aData ); CallOnChange(); }

    const CallOnChange = () =>
    {
        const lParameters = { 'Detach' : sDetach, 'Exit' : sExit, 'WorkingDirectoy' : sWorkDir }

        aProps.OnChange( lParameters )
    }

    return (
        <>
            <details>
                <summary> <b> Launcher Parameters </b> </summary>
                <table>
                    <tbody>
                        <Flag Label    = "Detach"
                              OnChange = { OnDetachChange }
                              Value    = { sDetach } />
                        <Flag Label    = "Exit"
                              OnChange = { OnExitChange }
                              Value    = { sExit } />
                        <Folder Label    = "WorkingDirectory"
                                OnChange = { OnWorkDirChange }
                                Value    = { sWorkDir } />
                    </tbody>
                </table>
            </details>
            <p />
        </> )
}
