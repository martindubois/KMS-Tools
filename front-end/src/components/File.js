
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/component/File.js

// ----- Properties ---------------------------------------------------------
// Label
// Message   Optional
// OnChange
// Value
export default function File( aProps )
{
    const OnChange = ( aEvent ) => { aProps.OnChange( aEvent.target.value ) }

    let lMandatory = ''

    if ( aProps.Mandatory ) { lMandatory = '*' }

    return (
        <tr>
            <td className = "formlabel" > { aProps.Label } </td>
            <td>
                <input onChange = { OnChange }
                       type     = "text"
                       value    = { aProps.Value } />
                &nbsp; { lMandatory }
            </td>
            <td> { aProps.Message } </td>
        </tr> )
}
