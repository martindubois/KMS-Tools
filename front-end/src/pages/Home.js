
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/pages/Home.js

import { Link } from 'react-router-dom'

export default function Home()
{
    return (
        <>
            <p> <Link to = "ComTool"    > ComTool    </Link> </p>
            <p> <Link to = "Launcher"   > Launcher   </Link> </p>
            <p> <Link to = "ModbusSim"  > ModbusSim  </Link> </p>
            <p> <Link to = "ModbusTool" > ModbusTool </Link> </p>
            <p> <Link to = "WOPTool"    > WOP-Tool   </Link> </p>
        </> )
}
