import React, { useState } from 'react'
import { BrowserRouter as Router, Routes, Route, useLocation } from 'react-router-dom'
import './App.css'
import PresetList from './components/PresetList'
import PresetEdit from './components/PresetEdit'
import ValueEdit from './components/ValueEdit'
import Configuration from './components/Configuration'
import ManualButton from './components/ManualButton'
import OtaUpdate from './components/OtaUpdate'
import ManualPage from './components/ManualPage'

// Helper function to generate values between 0 and 255
const generateValues = (count) => {
  return Array.from({ length: count }, () => 0)
}

// Generate presets based on count
const generatePresets = (count) => {
  return Array.from({ length: count }, (_, i) => ({
    id: i + 1,
    name: `Scene ${i + 1}`,
    values1: generateValues(512),
    values2: generateValues(512)
  }))
}

import { useNavigate } from 'react-router-dom'

function HeaderControls({ esp32Ip, sendStatus, presetCount, SendToDmxController, LoadFromDmxController, blackoutActive, setBlackoutActive }) {
  const location = useLocation();
  const navigate = useNavigate();
  const isEditPage = location.pathname.startsWith('/preset/') || location.pathname === '/config';
  const isManualPage = location.pathname === '/manual';
  const isValueEditPage = location.pathname.startsWith('/value-edit/');
  const isOtaPage = location.pathname === '/ota-update';
  if (isEditPage || isManualPage || isValueEditPage || isOtaPage) return null;
  return (
    <>
      <div className="esp32-config">
        <label htmlFor="esp32-ip">ESP32 IP Address:</label>
        <span className="esp32-ip-display">{esp32Ip}</span>
        <button className="config-button" onClick={LoadFromDmxController}>
          Load
        </button>
        <button className="config-button" onClick={SendToDmxController}>
          Save
        </button>
        {sendStatus && (
          <span className={`send-status ${sendStatus.includes('✗') ? 'send-status-error' : ''}`}>
            {sendStatus}
          </span>
        )}
      </div>
      <div className="config-button-container" style={{ display: 'flex', gap: '8px' }}>
        <button
          className={`config-button blackout-button${blackoutActive ? ' blackout-active' : ''}`}
          onClick={() => setBlackoutActive(prev => !prev)}
        >
          <span style={{ color: blackoutActive ? '#fff' : 'black' }}>Blackout</span>
        </button>
        <button className="config-button" onClick={() => navigate('/config')}>
          Configuration
        </button>
        <button className="config-button" onClick={() => navigate('/ota-update')}>
          OTA Update
        </button>
        <ManualButton />
      </div>
      <div className="preset-selector">
        <label htmlFor="preset-count">Number of Presets:</label>
        <span
          className="preset-count-display"
          style={{ color: '#4FC3F7' }} // lighter blue
        >
          {presetCount}
        </span>
      </div>
    </>
  );
}

function App() {
  const [presetCount, setPresetCount] = useState(3)
  const [presets, setPresets] = useState(() => generatePresets(20))
  const [esp32Ip] = useState('192.168.1.254')
  const [sendStatus, setSendStatus] = useState('')
  const [hasUnsavedChanges, setHasUnsavedChanges] = useState(false)
  const [config, setConfig] = useState({
    expressionPedalPolarity: 'normally'
  })

  const updatePresetValue = (presetId, section, index, newValue) => {
    setHasUnsavedChanges(true)
    setPresets(prevPresets => 
      prevPresets.map(preset => {
        if (preset.id === presetId) {
          const clampedValue = Math.max(0, Math.min(255, parseInt(newValue) || 0))
          return {
            ...preset,
            [section]: preset[section].map((val, i) => i === index ? clampedValue : val)
          }
        }
        return preset
      })
    )
  }

  const updatePresetName = (presetId, newName) => {
    const truncatedName = newName.slice(0, 25)
    setHasUnsavedChanges(true)
    setPresets(prevPresets =>
      prevPresets.map(preset => {
        if (preset.id === presetId) {
          return {
            ...preset,
            name: truncatedName
          }
        }
        return preset
      })
    )
  }

  const deletePreset = (presetId) => {
    setHasUnsavedChanges(true)
    setPresets(prevPresets => {
      const newPresets = [...prevPresets]
      const deleteIndex = presetId - 1 // Convert ID to index
      
      // Shift all presets after the deleted one up
      for (let i = deleteIndex; i < newPresets.length - 1; i++) {
        newPresets[i] = {
          ...newPresets[i],
          values1: newPresets[i + 1].values1,
          values2: newPresets[i + 1].values2
        }
      }
      
      // Reset the last preset
      newPresets[newPresets.length - 1] = {
        ...newPresets[newPresets.length - 1],
        values1: generateValues(512),
        values2: generateValues(512)
      }
      
      return newPresets
    })
    
    // Decrease preset count, but keep minimum of 2
    setPresetCount(prev => Math.max(2, prev - 1))
  }

  const insertPreset = (presetId) => {
    if (presetCount >= 20) return
    
    setHasUnsavedChanges(true)
    setPresets(prevPresets => {
      const newPresets = [...prevPresets]
      const insertIndex = presetId // Insert after the selected preset
      
      // Shift all presets from insertIndex down by one
      for (let i = newPresets.length - 1; i > insertIndex; i--) {
        newPresets[i] = {
          ...newPresets[i],
          values1: newPresets[i - 1].values1,
          values2: newPresets[i - 1].values2,
          name: newPresets[i - 1].name
        }
      }
      
      // Insert empty preset at insertIndex with default name
      newPresets[insertIndex] = {
        ...newPresets[insertIndex],
        values1: generateValues(512),
        values2: generateValues(512),
        name: 'New Preset'
      }
      
      return newPresets
    })
    
    // Increase preset count, but keep maximum of 20
    setPresetCount(prev => Math.min(20, prev + 1))
  }

  const movePreset = (presetId, direction) => {
    setHasUnsavedChanges(true)
    setPresets(prevPresets => {
      const newPresets = [...prevPresets]
      const currentIndex = presetId - 1 // Convert ID to index
      const targetIndex = direction === 'up' ? currentIndex - 1 : currentIndex + 1
      
      // Swap the two presets' data including name
      const tempValues1 = newPresets[currentIndex].values1
      const tempValues2 = newPresets[currentIndex].values2
      const tempName = newPresets[currentIndex].name
      
      newPresets[currentIndex] = {
        ...newPresets[currentIndex],
        values1: newPresets[targetIndex].values1,
        values2: newPresets[targetIndex].values2,
        name: newPresets[targetIndex].name
      }
      
      newPresets[targetIndex] = {
        ...newPresets[targetIndex],
        values1: tempValues1,
        values2: tempValues2,
        name: tempName
      }
      
      return newPresets
    })
  }

  const LoadFromDmxController = async () => {
    // Check for unsaved changes
    if (hasUnsavedChanges) {
      const confirmed = window.confirm(
        'You have unsaved changes that will be lost. Do you want to continue loading from the DMX Controller?'
      )
      if (!confirmed) {
        return
      }
    }

    setSendStatus('Loading presets and configuration from DMX Controller...')
    try {
      const controller = new AbortController()
      const timeoutId = setTimeout(() => controller.abort(), 5000)
      const response = await fetch(`http://${esp32Ip}/all_data`, {
        method: 'GET',
        signal: controller.signal
      })
      clearTimeout(timeoutId)
      if (response.ok) {
        const data = await response.json()
        // data should be { configuration, presets }
        if (data.presets && Array.isArray(data.presets) && data.configuration) {
          setPresets(data.presets)
          setPresetCount(data.presets.length)
          setConfig(data.configuration)
          setHasUnsavedChanges(false)
          setSendStatus(`✓ Loaded ${data.presets.length} presets and configuration successfully`)
        } else {
          setSendStatus('✗ Invalid data format from controller')
        }
      } else {
        setSendStatus('✗ Failed to load from controller')
      }
    } catch {
      setSendStatus('✗ Connection failed - controller not reachable')
    }
  }

  const SendToDmxController = async () => {
    setSendStatus('Sending all presets and configuration...')
    try {
      const controller = new AbortController()
      const timeoutId = setTimeout(() => controller.abort(), 5000)
      await fetch(`http://${esp32Ip}/all_data`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ configuration: config, presets: presets.slice(0, presetCount) }),
        signal: controller.signal
      })
      clearTimeout(timeoutId)
      setHasUnsavedChanges(false)
      setSendStatus('✓ All presets and configuration sent successfully')
    } catch {
      setSendStatus('✗ Failed to send all data to controller')
    }
  }

  const handleConfigChange = (key, value) => {
    setConfig(prevConfig => ({
      ...prevConfig,
      [key]: value
    }))
  }

  const [blackoutActive, setBlackoutActive] = useState(false);
  return (
    <Router>
      <div className="app">
        <header>
          <h1
            style={{ cursor: 'pointer' }}
            onClick={() => window.location.pathname = '/'}
            title="Go to Home"
          >
            DMX Controller <span style={{fontSize:'0.5em', color:'#888', marginLeft:'0.5em'}}>v0.1</span>
          </h1>
          <HeaderControls 
            esp32Ip={esp32Ip}
            sendStatus={sendStatus}
            presetCount={presetCount}
            SendToDmxController={SendToDmxController}
            LoadFromDmxController={LoadFromDmxController}
            blackoutActive={blackoutActive}
            setBlackoutActive={setBlackoutActive}
          />
        </header>
        
        <main style={{ marginTop: location.pathname === '/ota-update' ? '0.1rem' : undefined }}>
          <Routes>
            <Route path="/config" element={<Configuration config={config} onConfigChange={handleConfigChange} />} />
            <Route path="/manual" element={<ManualPage />} />
            <Route path="/preset/:id" element={<PresetEdit presets={presets} setPresets={setPresets} />} />
            <Route path="/value-edit/:presetId/:section/:index" element={<ValueEdit presets={presets} setPresets={setPresets} />} />
            <Route path="/ota-update" element={<OtaUpdate />} />
            <Route path="/" element={<PresetList presets={presets} presetCount={presetCount} setPresetCount={setPresetCount} />} />
          </Routes>
        </main>
      </div>
    </Router>
  )
}

export default App
