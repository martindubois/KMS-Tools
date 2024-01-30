
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/component/KMSArguments.js

import { useState } from 'react'

import Enum   from './Enum'
import Flag   from './Flag'
import File   from './File'
import Folder from './Folder'
import Soon   from './Soon'

// ----- Properties ---------------------------------------------------------
// Arguments
// OnChange
export default function KMSArguments( aProps )
{
    const [ sAddToPath       , SetAddToPath        ] = useState( aProps.Arguments.includes( 'AddToPath' ) )
    const [ sDisplayConfig   , SetDisplayConfig    ] = useState( aProps.Arguments.includes( 'DisplayHelp' ) )
    const [ sHelp            , SetHelp             ] = useState( aProps.Arguments.includes( 'Help' ) )
    const [ sLog_ConsoleLevel, SetLog_ConsoleLevel ] = useState( ExtractValue( aProps.Arguments, "Log_ConsoleLevel=") )
    const [ sLog_ConsoleMode , SetLog_ConsoleMode  ] = useState( ExtractValue( aProps.Arguments, "Log_ConsoleMode=") )
    const [ sLog_FileLevel   , SetLog_FileLevel    ] = useState( ExtractValue( aProps.Arguments, "Log_FileLevel=") )
    const [ sLog_Folder      , SetLog_Folder       ] = useState( ExtractValue( aProps.Arguments, "Log_Folder=" ) )
    const [ sRemoveFromPath  , SetRemoveFromPath   ] = useState( aProps.Arguments.includes( 'RemoveFromPath' ) )
    const [ sSaveConfig      , SetSaveConfig       ] = useState( ExtractValue( aProps.Arguments, "SaveConfig=") )
    const [ sStats_Console   , SetStats_Console    ] = useState( aProps.Arguments.includes( 'Stats_Console' ) )
    const [ sStats_Folder    , SetStats_Folder     ] = useState( ExtractValue( aProps.Arguments, "Stats_Folder=" ) )
    const [ sUninstall       , SetUninstall        ] = useState( aProps.Arguments.includes( 'Uninstall' ) )

    const OnAddToPathChange        = ( aEvent ) => { SetAddToPath       ( aEvent.target.checked ); CallOnChange() }
    const OnDisplayConfigChange    = ( aEvent ) => { SetDisplayConfig   ( aEvent.target.checked ); CallOnChange() }
    const OnHelpChange             = ( aEvent ) => { SetHelp            ( aEvent.target.checked ); CallOnChange() }
    const OnLog_ConsoleLevelChange = ( aEvent ) => { SetLog_ConsoleLevel( aEvent.target.value   ); CallOnChange() }
    const OnLog_ConsoleModeChange  = ( aEvent ) => { SetLog_ConsoleMode ( aEvent.target.value   ); CallOnChange() }
    const OnLog_FileLevelChange    = ( aEvent ) => { SetLog_FileLevel   ( aEvent.target.value   ); CallOnChange() }
    const OnLog_FolderChange       = ( aEvent ) => { SetLog_Folder      ( aEvent.target.value   ); CallOnChange() }
    const OnRemoveFromPathChange   = ( aEvent ) => { SetRemoveFromPath  ( aEvent.target.checked ); CallOnChange() }
    const OnSaveConfigChange       = ( aEvent ) => { SetSaveConfig      ( aEvent.target.value   ); CallOnChange() }
    const OnStats_ConsoleChange    = ( aEvent ) => { SetStats_Console   ( aEvent.target.checked ); CallOnChange() }
    const OnStats_FolderChange     = ( aEvent ) => { SetStats_Folder    ( aEvent.target.value   ); CallOnChange() }
    const OnUninstallChange        = ( aEvent ) => { SetUninstall       ( aEvent.target.checked ); CallOnChange() }

    const CallOnChange = () =>
    {
        var lArguments = []

        if ( sAddToPath        ) { lArguments.push( 'AddToPath' ) }
        if ( sDisplayConfig    ) { lArguments.push( 'DisplayConfig' ) }
        if ( sHelp             ) { lArguments.push( 'Help' ) }
        if ( sLog_ConsoleLevel ) { lArguments.push( 'Log_ConsoleLevel=' + sLog_ConsoleLevel ) }
        if ( sLog_ConsoleMode  ) { lArguments.push( 'Log_ConsoleMode=' + sLog_ConsoleMode ) }
        if ( sLog_FileLevel    ) { lArguments.push( 'Log_FileLevel=' + sLog_FileLevel ) }
        if ( sLog_Folder       ) { lArguments.push( 'Log_Folder=' + sLog_Folder ) }
        if ( sRemoveFromPath   ) { lArguments.push( 'RemoveFromPath' ) }
        if ( sSaveConfig       ) { lArguments.push( 'SaveConfig=' + sSaveConfig ) }
        if ( sStats_Console    ) { lArguments.push( 'Stats_Console' ) }
        if ( sStats_Folder     ) { lArguments.push( 'Stats_Folder=' + sStats_Folder ) }
        if ( sUninstall        ) { lArguments.push( 'Uninstall' ) }

        aProps.OnChange( lArguments )
    }

    return (
        <>
            <details>
                <summary> <b> KMS Arguments </b> </summary>
                <table>
                    <tbody>
                        <Flag Label    = "AddToPath"
                              OnChange = { OnAddToPathChange }
                              Value    = { sAddToPath } />
                        <Soon Label = "ConfigFiles" />
                        <Flag Label    = "DisplayConfig"
                              OnChange = { OnDisplayConfigChange }
                              Value    = { sDisplayConfig } />
                        <Flag Label    = "Help"
                              OnChange = { OnHelpChange }
                              Value    = { sHelp } />
                        <Enum Label    = "Log_ConsoleLevel"
                              Message  = "NOISE | INFO | WARNING | ERROR | NONE"
                              OnChange = { OnLog_ConsoleLevelChange }
                              Options  = { [ 'NOISE', 'INFO', 'WARNING', 'ERROR', 'NONE' ] }
                              Value    = { sLog_ConsoleLevel } />
                        <Enum Label    = "Log_ConsoleMode"
                              Message  = "DEBUG | USER"
                              OnChange = { OnLog_ConsoleModeChange }
                              Options  = { [ 'DEBUG', 'USER' ] }
                              Value    = { sLog_ConsoleMode } />
                        <Enum Label    = "Log_FileLevel"
                              Message  = "NOISE | INFO | WARNING | ERROR | NONE"
                              OnChange = { OnLog_FileLevelChange }
                              Options  = { [ 'NOISE', 'INFO', 'WARNING', 'ERROR', 'NONE' ] }
                              Value    = { sLog_FileLevel } />
                        <Folder Label    = "Log_Folder"
                                OnChange = { OnLog_FolderChange }
                                Value    = { sLog_Folder } />
                        <Soon Label = "OptionalConfigFiles" />
                        <Flag Label    = "RemoveFromPath"
                              OnChange = { OnRemoveFromPathChange }
                              Value    = { sRemoveFromPath } />
                        <File Label    = "SaveConfig"
                              OnChange = { OnSaveConfigChange }
                              Value    = { sSaveConfig } />
                        <Flag Label    = "Stats_Console"
                              OnChange = { OnStats_ConsoleChange }
                              Value    = { sStats_Console } />
                        <Folder Label    = "Stats_Folder"
                                OnChange = { OnStats_FolderChange }
                                Value    = { sStats_Folder } />
                        <Flag Label    = "Uninstall"
                              OnChange = { OnUninstallChange }
                              Value    = { sUninstall } />
                    </tbody>
                </table>
            </details>
            <p />
        </> )
}

// Internal
// //////////////////////////////////////////////////////////////////////////

function ExtractValue( aArguments, aName )
{
    let lResult = ''

    for ( let lArg in aArguments )
    {
        if ( lArg.startsWith( aName ) )
        {
            lResult = lArg.substring( aName.length )
            break
        }
    }

    return lResult
}
