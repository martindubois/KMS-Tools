
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/component/LauncherParameters.js

import { useState } from 'react'

import Flag   from './Flag'
import Folder from './Folder'

// ----- Properties ---------------------------------------------------------
// OnChange
// Parameters
export default function LauncherParameters( aProps )
{
    const [ sDetach          , SetDetach           ] = useState( aProps.Parameters.Detach )
    const [ sExit            , SetExit             ] = useState( aProps.Parameters.Exit )
    const [ sWorkingDirectory, SetWorkingDirectory ] = useState( aProps.Parameters.sWorkingDirectory )

    const OnDetachChange           = ( aEvent ) => { SetDetach          ( aEvent.target.checked ); CallOnChange(); }
    const OnExitChange             = ( aEvent ) => { SetExit            ( aEvent.target.checked ); CallOnChange(); }
    const OnWorkingDirectoryChange = ( aEvent ) => { SetWorkingDirectory( aEvent.target.value   ); CallOnChange(); }

    const CallOnChange = () =>
    {
        const lParameters = { 'Detach' : sDetach, 'Exit' : sExit, 'WorkingDirectoy' : sWorkingDirectory }

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
                        <Folder Label    = "WorkingDirectoy"
                                OnChange = { OnWorkingDirectoryChange }
                                Value    = { sWorkingDirectory } />
                    </tbody>
                </table>
            </details>
            <p />
        </> )
}
