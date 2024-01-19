
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/component/KMSArguments.js

import { useState } from 'react'

// ----- Properties ---------------------------------------------------------
// Arguments
// OnChange
export default function KMSArguments( aProps )
{
    const [ sDisplayConfig, SetDisplayConfig ] = useState( aProps.Arguments.includes( 'DisplayHelp' ) )
    const [ sHelp         , SetHelp          ] = useState( aProps.Arguments.includes( 'Help'        ) )

    const OnDisplayConfigChange = ( aEvent ) => { SetDisplayConfig( aEvent.target.checked ); CallOnChange(); }
    const OnHelpChange          = ( aEvent ) => { SetHelp         ( aEvent.target.checked ); CallOnChange(); }

    const CallOnChange = () =>
    {
        var lArguments = []

        if ( sDisplayConfig ) { lArguments.push( 'DisplayConfig' ) }
        if ( sHelp          ) { lArguments.push( 'Help' ) }

        aProps.OnChange( lArguments )
    }

    return (
        <>
            <details>
                <summary> KMS Arguments </summary>
                <table>
                    <tbody>
                        <tr>
                            <td> </td>
                            <td> ConfigFiles </td>
                            <td> <b> Soon ! </b> </td>
                        </tr>
                        <tr>
                            <td> </td>
                            <td> DisplayConfig </td>
                            <td> <input checked = { sDisplayConfig } onChange = { OnDisplayConfigChange } type = "checkbox" /> </td>
                        </tr>
                        <tr>
                            <td> </td>
                            <td> Help </td>
                            <td> <input checked = { sHelp } onChange = { OnHelpChange } type = "checkbox" /> </td>
                        </tr>
                        <tr>
                            <td> </td>
                            <td> OptionalConfigFiles </td>
                            <td> <b> Soon ! </b> </td>
                        </tr>
                        <tr>
                            <td> </td>
                            <td> SaveConfig </td>
                            <td> <b> Soon ! </b> </td>
                        </tr>
                    </tbody>
                </table>
            </details>
            <p />
        </> )
}
