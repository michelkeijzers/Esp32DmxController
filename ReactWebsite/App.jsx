import { useState } from 'react'
import { BrowserRouter as Router, Routes, Route, useLocation } from 'react-router-dom'
import './App.css'
import PresetList from './components/PresetList'
import PresetEdit from './components/PresetEdit'
import ValueEdit from './components/ValueEdit'
import Configuration from './components/Configuration'

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

function HeaderControls({ esp32Ip, sendStatus, presetCount, SendToDmxController, LoadFromDmxController }) {
  const location = useLocation()
  const navigate = useNavigate()
  const isEditPage = location.pathname.startsWith('/preset/') || location.pathname === '/config'
  
  if (isEditPage) return null
  
  return (
    <>
      <div className="esp32-config">
        <label htmlFor="esp32-ip">ESP32 IP Address:</label>
        <span className="esp32-ip-display">{esp32Ip}</span>
        <button className="load-all-button" onClick={LoadFromDmxController}>
          Load
        </button>
        <button className="send-all-button" onClick={SendToDmxController}>
          Save
        </button>
        {sendStatus && (
          <span className={`send-status ${sendStatus.includes('✗') ? 'send-status-error' : ''}`}>
            {sendStatus}
          </span>
        )}
      </div>
      
      <div className="config-button-container">
        <button className="config-button" onClick={() => navigate('/config')}>
          Configuration
        </button>
      </div>
      
      <div className="preset-selector">
        <label htmlFor="preset-count">Number of Presets:</label>
        <span className="preset-count-display">{presetCount}</span>
      </div>
    </>
  )
}

function App() {
  const [presetCount, setPresetCount] = useState(3)
  const [presets, setPresets] = useState(() => generatePresets(20))
  const [esp32Ip] = useState('192.168.1.100')
  const [sendStatus, setSendStatus] = useState('')
  const [hasUnsavedChanges, setHasUnsavedChanges] = useState(false)
  const [config, setConfig] = useState({
    expressionPedalPolarity: 'standard'
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

    setSendStatus('Loading presets from DMX Controller...')
    
    try {
      const controller = new AbortController()
      const timeoutId = setTimeout(() => controller.abort(), 5000)
      
      const response = await fetch(`http://${esp32Ip}/api/presets`, {
        method: 'GET',
        signal: controller.signal
      })
      
      clearTimeout(timeoutId)
      
      if (response.ok) {
        const data = await response.json()
        // data should be { count: number, presets: array }
        if (data.presets && Array.isArray(data.presets)) {
          setPresets(data.presets)
          setPresetCount(data.count || data.presets.length)
          setHasUnsavedChanges(false)
          setSendStatus(`✓ Loaded ${data.presets.length} presets successfully`)
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
    
    // First send configuration
    try {
      const configController = new AbortController()
      const configTimeoutId = setTimeout(() => configController.abort(), 5000)
      
      await fetch(`http://${esp32Ip}/api/config`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(config),
        signal: configController.signal
      })
      
      clearTimeout(configTimeoutId)
    } catch {
      // Continue even if config fails
    }
    
    // Then send presets
    const sendPreset = async (preset) => {
      try {
        const controller = new AbortController()
        const timeoutId = setTimeout(() => controller.abort(), 5000)
        
        const response = await fetch(`http://${esp32Ip}/api/preset`, {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify({
            id: preset.id,
            name: preset.name,
            values1: preset.values1,
            values2: preset.values2
          }),
          signal: controller.signal
        })
        
        clearTimeout(timeoutId)
        return response.ok
      } catch {
        return false
      }
    }
    
    const activePresets = presets.slice(0, presetCount)
    const results = await Promise.all(activePresets.map(preset => sendPreset(preset)))
    const successCount = results.filter(success => success).length
    const failCount = results.filter(success => !success).length
    
    if (failCount === 0) {
      setHasUnsavedChanges(false)
      setSendStatus(`✓ All ${successCount} presets sent successfully`)
    } else {
      setSendStatus(`✓ ${successCount} sent, ✗ ${failCount} failed`)
    }
  }

  const handleConfigChange = (key, value) => {
    setConfig(prevConfig => ({
      ...prevConfig,
      [key]: value
    }))
  }

  return (
    <Router>
      <div className="app">
        <header>
          <h1>DMX Controller</h1>
          <HeaderControls 
            esp32Ip={esp32Ip}
            sendStatus={sendStatus}
            presetCount={presetCount}
            SendToDmxController={SendToDmxController}
            LoadFromDmxController={LoadFromDmxController}
          />
        </header>
        
        <main>
          <Routes>
            <Route 
              path="/" 
              element={
                <PresetList 
                  presets={presets.slice(0, presetCount)} 
                  onDeletePreset={deletePreset}
                  onInsertPreset={insertPreset}
                  onMovePreset={movePreset}
                  canInsert={presetCount < 20}
                />} 
            />
            <Route 
              path="/preset/:id" 
              element={
                <PresetEdit 
                  presets={presets}
                  onUpdateValue={updatePresetValue}
                  onUpdateName={updatePresetName}
                />
              } 
            />
            <Route 
              path="/preset/:presetId/:section/:index" 
              element={
                <ValueEdit 
                  presets={presets}
                  onUpdateValue={updatePresetValue}
                />
              } 
            />
            <Route 
              path="/config" 
              element={
                <Configuration 
                  config={config}
                  onConfigChange={handleConfigChange}
                />
              } 
            />
          </Routes>
        </main>
      </div>
    </Router>
  )
}

export default App
