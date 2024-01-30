
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/component/TextList.js

import { useState } from 'react'

// ----- Properties ---------------------------------------------------------
// Label
// OnChange
// Values
export default function TextList( aProps )
{
    const [ sValue, SetValue ] = useState();

    const OnAdd = ( aEvent ) =>
    {
        var lValues = aProps.Values

        lValues.push( sValue )

        aProps.OnChange( lValues )

        SetValue( '' )
    }

    const OnChange = ( aEvent ) => { SetValue( aEvent.target.value ) }

    const OnClear = ( aEvent ) =>
    {
        aProps.OnChange( [] )

        SetValue( '' )
    }

    const Render = ( aValue ) => { return <p> { aValue } </p> }


    return (
        <tr>
            <td className = "formlabel" > { aProps.Label } </td>
            <td>
                { aProps.Values.map( Render ) }
                <input onChange = { OnChange }
                       type     = "text"
                       value    = { sValue } />
            </td>
            <td>
                <button onClick = { OnAdd   } > Add   </button>
                &nbsp;
                <button onClick = { OnClear } > Clear </button>
            </td>
        </tr> )
}
