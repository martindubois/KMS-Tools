
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/component/LauncherParameters.js

import { useState } from 'react'

// ----- Properties ---------------------------------------------------------
// OnChange
// Parameters
export default function LauncherParameters( aProps )
{
    const [ sDetach, SetDetach ] = useState( aProps.Parameters.Detach )
    const [ sExit  , SetExit   ] = useState( aProps.Parameters.Exit   )

    const OnDetachChange = ( aEvent ) => { SetDetach( aEvent.target.checked ); CallOnChange(); }
    const OnExitChange   = ( aEvent ) => { SetExit  ( aEvent.target.checked ); CallOnChange(); }

    const CallOnChange = () =>
    {
        const lParameters = { 'Detach' : sDetach, 'Exit' : sExit }

        aProps.OnChange( lParameters )
    }

    return (
        <>
            <details>
                <summary> Launcher Parameters </summary>
                <table>
                    <tbody>
                        <tr>
                            <td> </td>
                            <td> Detach </td>
                            <td> <input checked = { sDetach } onChange = { OnDetachChange } type = "checkbox" /> </td>
                        </tr>
                        <tr>
                            <td> </td>
                            <td> Exit </td>
                            <td> <input checked = { sExit } onChange = { OnExitChange } type = "checkbox" /> </td>
                        </tr>
                        <tr>
                            <td> </td>
                            <td> WorkingDirectory </td>
                            <td> <b> Soon ! </b> </td>
                        </tr>
                    </tbody>
                </table>
            </details>
            <p />
        </> )
}
