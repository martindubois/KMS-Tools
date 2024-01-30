
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/component/KMSArguments.js

import { useState } from 'react'

import Enum     from './Enum'
import Flag     from './Flag'
import File     from './File'
import FileList from './FileList'
import Folder   from './Folder'

// ----- Properties ---------------------------------------------------------
// Arguments
// OnChange
export default function KMSArguments( aProps )
{
    const [ sAddToPath     , SetAddToPath      ] = useState( aProps.Arguments.includes( 'AddToPath' ) )
    const [ sConfigFiles   , SetConfigFiles    ] = useState( ExtractValues( aProps.Arguments, "ConfigFiles+=" ) )
    const [ sDisplayConfig , SetDisplayConfig  ] = useState( aProps.Arguments.includes( 'DisplayHelp' ) )
    const [ sHelp          , SetHelp           ] = useState( aProps.Arguments.includes( 'Help' ) )
    const [ sLog_ConLevel  , SetLog_ConLevel   ] = useState( ExtractValue( aProps.Arguments, "Log_ConsoleLevel=") )
    const [ sLog_ConMode   , SetLog_ConMode    ] = useState( ExtractValue( aProps.Arguments, "Log_ConsoleMode=") )
    const [ sLog_FileLevel , SetLog_FileLevel  ] = useState( ExtractValue( aProps.Arguments, "Log_FileLevel=") )
    const [ sLog_Folder    , SetLog_Folder     ] = useState( ExtractValue( aProps.Arguments, "Log_Folder=" ) )
    const [ sOptConfigFiles, SetOptConfigFiles ] = useState( ExtractValues( aProps.Arguments, "OptionalConfigFiles+=" ) )
    const [ sRemoveFromPath, SetRemoveFromPath ] = useState( aProps.Arguments.includes( 'RemoveFromPath' ) )
    const [ sSaveConfig    , SetSaveConfig     ] = useState( ExtractValue( aProps.Arguments, "SaveConfig=") )
    const [ sStats_Con     , SetStats_Con      ] = useState( aProps.Arguments.includes( 'Stats_Console' ) )
    const [ sStats_Folder  , SetStats_Folder   ] = useState( ExtractValue( aProps.Arguments, "Stats_Folder=" ) )
    const [ sUninstall     , SetUninstall      ] = useState( aProps.Arguments.includes( 'Uninstall' ) )

    const OnAddToPathChange      = ( aData ) => { SetAddToPath     ( aData ); CallOnChange() }
    const OnConfigFilesChange    = ( aData ) => { SetConfigFiles   ( aData ); CallOnChange() }
    const OnDisplayConfigChange  = ( aData ) => { SetDisplayConfig ( aData ); CallOnChange() }
    const OnHelpChange           = ( aData ) => { SetHelp          ( aData ); CallOnChange() }
    const OnLog_ConLevelChange   = ( aData ) => { SetLog_ConLevel  ( aData ); CallOnChange() }
    const OnLog_ConModeChange    = ( aData ) => { SetLog_ConMode   ( aData ); CallOnChange() }
    const OnLog_FileLevelChange  = ( aData ) => { SetLog_FileLevel ( aData ); CallOnChange() }
    const OnLog_FolderChange     = ( aData ) => { SetLog_Folder    ( aData ); CallOnChange() }
    const OnOptConfigFilesChange = ( aData ) => { SetOptConfigFiles( aData ); CallOnChange() }
    const OnRemoveFromPathChange = ( aData ) => { SetRemoveFromPath( aData ); CallOnChange() }
    const OnSaveConfigChange     = ( aData ) => { SetSaveConfig    ( aData ); CallOnChange() }
    const OnStats_ConChange      = ( aData ) => { SetStats_Con     ( aData ); CallOnChange() }
    const OnStats_FolderChange   = ( aData ) => { SetStats_Folder  ( aData ); CallOnChange() }
    const OnUninstallChange      = ( aData ) => { SetUninstall     ( aData ); CallOnChange() }

    const CallOnChange = () =>
    {
        var lArguments = []

        if ( sAddToPath      ) { lArguments.push( 'AddToPath' ) }
        if ( sDisplayConfig  ) { lArguments.push( 'DisplayConfig' ) }
        if ( sHelp           ) { lArguments.push( 'Help' ) }
        if ( sLog_ConLevel   ) { lArguments.push( 'Log_ConsoleLevel=' + sLog_ConLevel ) }
        if ( sLog_ConMode    ) { lArguments.push( 'Log_ConsoleMode=' + sLog_ConMode ) }
        if ( sLog_FileLevel  ) { lArguments.push( 'Log_FileLevel=' + sLog_FileLevel ) }
        if ( sLog_Folder     ) { lArguments.push( 'Log_Folder=' + sLog_Folder ) }
        if ( sRemoveFromPath ) { lArguments.push( 'RemoveFromPath' ) }
        if ( sSaveConfig     ) { lArguments.push( 'SaveConfig=' + sSaveConfig ) }
        if ( sStats_Con      ) { lArguments.push( 'Stats_Console' ) }
        if ( sStats_Folder   ) { lArguments.push( 'Stats_Folder=' + sStats_Folder ) }
        if ( sUninstall      ) { lArguments.push( 'Uninstall' ) }

        sConfigFiles.map( ( aFile ) => { lArguments.push( "ConfigFiles+=" + aFile ) } )

        sOptConfigFiles.map( ( aFile ) => { lArguments.push( "OptionalConfigFile+=" + aFile ) } )

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
                        <FileList Label    = "ConfigFiles"
                                  OnChange = { OnConfigFilesChange }
                                  Values   = { sConfigFiles } />
                        <Flag Label    = "DisplayConfig"
                              OnChange = { OnDisplayConfigChange }
                              Value    = { sDisplayConfig } />
                        <Flag Label    = "Help"
                              OnChange = { OnHelpChange }
                              Value    = { sHelp } />
                        <Enum Label    = "Log_ConsoleLevel"
                              Message  = "NOISE | INFO | WARNING | ERROR | NONE"
                              OnChange = { OnLog_ConLevelChange }
                              Options  = { [ 'NOISE', 'INFO', 'WARNING', 'ERROR', 'NONE' ] }
                              Value    = { sLog_ConLevel } />
                        <Enum Label    = "Log_ConsoleMode"
                              Message  = "DEBUG | USER"
                              OnChange = { OnLog_ConModeChange }
                              Options  = { [ 'DEBUG', 'USER' ] }
                              Value    = { sLog_ConMode } />
                        <Enum Label    = "Log_FileLevel"
                              Message  = "NOISE | INFO | WARNING | ERROR | NONE"
                              OnChange = { OnLog_FileLevelChange }
                              Options  = { [ 'NOISE', 'INFO', 'WARNING', 'ERROR', 'NONE' ] }
                              Value    = { sLog_FileLevel } />
                        <Folder Label    = "Log_Folder"
                                OnChange = { OnLog_FolderChange }
                                Value    = { sLog_Folder } />
                        <FileList Label    = "OptionalConfigFiles"
                                  OnChange = { OnOptConfigFilesChange }
                                  Values   = { sOptConfigFiles } />
                        <Flag Label    = "RemoveFromPath"
                              OnChange = { OnRemoveFromPathChange }
                              Value    = { sRemoveFromPath } />
                        <File Label    = "SaveConfig"
                              OnChange = { OnSaveConfigChange }
                              Value    = { sSaveConfig } />
                        <Flag Label    = "Stats_Console"
                              OnChange = { OnStats_ConChange }
                              Value    = { sStats_Con } />
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

function ExtractValues( aArguments, aName )
{
      let lResults = []

      for ( let lArg in aArguments )
      {
          if ( lArg.startsWith( aName ) )
          {
              lResults.push( lArg.substring( aName.length ) )
          }
      }
  
      return lResults 
}
