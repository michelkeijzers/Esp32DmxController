import { useState } from 'react'
import { useParams, useNavigate } from 'react-router-dom'
import './ValueEdit.css'

function ValueEdit({ presets, setPresets }) {
  const { presetId, section, index } = useParams()
  const navigate = useNavigate()
  
  const preset = presets.find(p => p.id === parseInt(presetId))
  const currentValue = preset ? preset[section][parseInt(index)] : 0
  
  const [value, setValue] = useState(currentValue === 0 ? '' : currentValue.toString())

  const handleNumberClick = (num) => {
    const newValue = value + num
    const numValue = parseInt(newValue)
    if (numValue <= 255) {
      setValue(newValue)
    }
  }

  const handleBackspace = () => {
    setValue(value.slice(0, -1) || '0')
  }

  const handleClear = () => {
    setValue('0')
  }

  const handleSave = () => {
    const numValue = parseInt(value) || 0
    const clampedValue = Math.max(0, Math.min(255, numValue))
    setPresets(prevPresets =>
      prevPresets.map(preset => {
        if (preset.id === parseInt(presetId)) {
          return {
            ...preset,
            [section]: preset[section].map((val, i) => i === parseInt(index) ? clampedValue : val)
          }
        }
        return preset
      })
    )
    // Restore scroll position after navigation
    navigate(`/preset/${presetId}`)
    setTimeout(() => {
      window.scrollTo({ top: lastScrollY, behavior: 'auto' });
    }, 0);
  }

  const handleCancel = () => {
    navigate(`/preset/${presetId}`)
  }

  let lastScrollY = 0;

  if (!preset) {
    return (
      <div className="value-edit-error">
        <h2>Preset not found</h2>
        <button onClick={() => navigate('/')}>Back to List</button>
      </div>
    )
  }

  const displayValue = parseInt(value) || 0
  const isValid = displayValue >= 0 && displayValue <= 255

  return (
    <div className="value-edit">
      <div className="value-edit-header-row">
        <span className="scene-title">{preset.name}</span>
        <span className="dmx-address">DMX Address {index}</span>
      </div>

      <div className="value-display">
        <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'flex-start' }}>
          <span className={`value-display-number ${!isValid ? 'invalid' : ''}`}>{value || '0'}</span>
          <span className="value-display-label" style={{ marginLeft: '1rem' }}>
            {isValid ? 'Valid (0-255)' : 'Invalid! Max 255'}
          </span>
        </div>
      </div>

      <div className="number-pad">
        {[1, 2, 3, 4, 5, 6, 7, 8, 9].map(num => (
          <button
            key={num}
            className="number-button"
            onClick={() => handleNumberClick(num.toString())}
          >
            {num}
          </button>
        ))}
        <button className="number-button clear-button" onClick={handleClear}>
          Clear
        </button>
        <button className="number-button" onClick={() => handleNumberClick('0')}>
          0
        </button>
        <button className="number-button backspace-button" onClick={handleBackspace}>
          ⌫
        </button>
      </div>

      <div className="value-edit-actions">
        <button className="cancel-button" onClick={handleCancel}>
          Cancel
        </button>
        <button
          className="save-button"
          onClick={handleSave}
          disabled={!isValid}
        >
          Enter
        </button>
      </div>
    </div>
  )
}

export default ValueEdit
