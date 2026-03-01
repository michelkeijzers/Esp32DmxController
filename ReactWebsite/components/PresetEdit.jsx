import { useParams, useNavigate } from 'react-router-dom';
import { useEffect, useRef } from 'react';
import './PresetEdit.css'

// Use a global variable to persist last edited index between pages
window.lastEditedIndex = null;

function PresetEdit({ presets, onUpdateName }) {
  const { id } = useParams()
  const navigate = useNavigate()
  const preset = presets.find(p => p.id === parseInt(id))
  const valueGridRef = useRef(null)

  useEffect(() => {
    if (window.lastEditedIndex !== null && valueGridRef.current) {
      const el = valueGridRef.current.querySelector(`[data-index='${window.lastEditedIndex}']`)
      if (el) {
        el.scrollIntoView({ block: 'center', behavior: 'auto' })
        window.lastEditedIndex = null
      }
    }
  }, [presets])

  const handleNameChange = (event) => {
    onUpdateName(preset.id, event.target.value)
  }

  const handlePrevPreset = () => {
    if (preset.id > 1) {
      navigate(`/preset/${preset.id - 1}`)
    }
  }

  const handleNextPreset = () => {
    if (preset.id < presets.length) {
      navigate(`/preset/${preset.id + 1}`)
    }
  }

  const handleIndexClick = (index) => {
    window.lastEditedIndex = index
    navigate(`/value-edit/${preset.id}/values1/${index}`)
  }

  return (
    <div className="preset-edit" style={{ maxWidth: '450px', margin: '0 auto' }}>
      <div className="preset-edit-header">
        <div className="preset-title-container">
          <span className="preset-number">Preset <span className="preset-number-value">{preset.id - 1}</span></span>
          <input
            type="text"
            className="preset-name-edit-input"
            value={preset.name}
            onChange={handleNameChange}
            placeholder="Scene name"
          />
        </div>
        <div className="preset-nav-buttons">
          <button 
            className="preset-nav-button"
            onClick={handlePrevPreset}
            disabled={preset.id === 1}
            title="Previous preset"
          >
            ←
          </button>
          <button 
            className="preset-nav-button"
            onClick={handleNextPreset}
            disabled={preset.id === presets.length}
            title="Next preset"
          >
            →
          </button>
        </div>
      </div>

      <div className="preset-sections">
        <div className="preset-section">
          {/* <h3>Universe 1</h3> */}
          <div className="values-grid-full" ref={valueGridRef}>
            {preset.values1.map((value, index) => (
              <span 
                key={index} 
                className="value-small"
                data-index={index}
                onClick={() => handleIndexClick(index)}
                style={{ cursor: 'pointer' }}
              >
                <span className="index">{index}</span>
                <span className="value-display-inline">{value}</span>
              </span>
            ))}
          </div>
        </div>

        {/* Universe 2 removed */}
      </div>
    </div>
  )
}

export default PresetEdit
