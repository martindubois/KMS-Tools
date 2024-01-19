
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-Tools
// File      front-end/src/App.js

import { BrowserRouter, Route, Routes } from 'react-router-dom'

import './App.css'

import ComTool    from './pages/ComTool'
import Home       from './pages/Home'
import Launcher   from './pages/Launcher'
import Layout     from './pages/Layout'
import ModbusSim  from './pages/ModbusSim'
import ModbusTool from './pages/ModbusTool'
import NoPage     from './pages/NoPage'
import WOPTool    from './pages/WOP-Tool'

export default function App()
{
    return (
        <BrowserRouter>
            <Routes>
                <Route element = { <Layout /> } path = "/">
                    <Route element = { <Home /> } index />
                    <Route element = { <ComTool    /> } path = "ComTool"    />
                    <Route element = { <Launcher   /> } path = "Launcher"   />
                    <Route element = { <ModbusSim  /> } path = "ModbusSim"  />
                    <Route element = { <ModbusTool /> } path = "ModbusTool" />
                    <Route element = { <WOPTool   /> } path = "WOP-Tool"   />
                    <Route element = { <NoPage /> } path = "*" />
                </Route>
            </Routes>
        </BrowserRouter> )
}
