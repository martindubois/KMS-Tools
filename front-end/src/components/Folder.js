
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/component/Folder.js

// ----- Properties ---------------------------------------------------------
// Label
// Message   Optional
// OnChange
// Value
export default function Flag( aProps )
{
    const OnChange = ( aEvent ) => { aProps.OnChange( aEvent.target.value ) }

    return (
        <tr>
            <td className = "formlabel" > { aProps.Label } </td>
            <td>
                <input onChange = { OnChange }
                       type     = "text"
                       value    = { aProps.Value } />
            </td>
            <td> { aProps.Message } </td>
        </tr> )
}
