
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/component/Enum.js

// ----- Properties ---------------------------------------------------------
// Label
// Message   Optional
// Options
// OnChange
// Value
export default function Enum( aProps )
{

    const OnChange = ( aEvent ) => { aProps.OnChange( aEvent.target.value ) }

    const Render = ( aOption ) =>
    {
        return <option value = { aOption }
                       key = { aOption } />
    }

    let lMandatory = ''

    if ( aProps.Mandatory ) { lMandatory = '*' }

    return (
        <tr>
            <td className = "formlabel" > { aProps.Label } </td>
            <td>
                <input list     = { aProps.Label }
                       onChange = { OnChange }
                       type     = 'text'
                       value    = { aProps.Value } />
                <datalist id = { aProps.Label } >
                    <option value = '' />
                    { aProps.Options.map( Render ) }
                </datalist>
                &nbsp;
                { lMandatory }
            </td>
            <td> { aProps.Message } </td>
        </tr> )
}
