
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/component/Flag.js

// ----- Properties ---------------------------------------------------------
// Label
// OnChange
// Value
export default function Flag( aProps )
{
    return (
        <tr>
            <td className = "formlabel" > { aProps.Label } </td>
            <td>
                <input checked  = { aProps.Value }
                       onChange = { aProps.OnChange }
                       type     = "checkbox" />
            </td>
        </tr> )
}
