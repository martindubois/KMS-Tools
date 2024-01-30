
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/component/Soon.js

// ----- Properties ---------------------------------------------------------
// Label
// Message  Optional
export default function Soon( aProps )
{
    return (
        <tr>
            <td className = "formlabel" > { aProps.Label } </td>
            <td> <b> Soon ! </b> </td>
            <td> { aProps.Message } </td>
        </tr> )
}
