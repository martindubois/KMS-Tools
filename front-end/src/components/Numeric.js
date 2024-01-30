
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/component/Numeric.js

// ----- Properties ---------------------------------------------------------
// Label
// Max
// Min
// OnChange
// Unit      Optional
// Value
export default function Enum( aProps )
{

    const OnChange = ( aEvent ) => { aProps.OnChange( aEvent.target.value ) }

    return (
        <tr>
            <td className = "formlabel" > { aProps.Label } </td>
            <td>
                <input max      = { aProps.Max }
                       min      = { aProps.Min }
                       onChange = { OnChange }
                       type     = 'numeric'
                       value    = { aProps.Value } />
            </td>
            <td> { aProps.Unit } </td>
        </tr> )
}
