import { useParams, useNavigate } from 'react-router-dom';
import { useEffect, useRef } from 'react';
import './PresetEdit.css'

// Use a global variable to persist last edited index between pages
window.lastEditedIndex = null;

function PresetEdit({ presets, onUpdateName }) {
  const { id } = useParams();
  const navigate = useNavigate();
  let preset = presets.find(p => p.id === parseInt(id));
  // Ensure dmx_values is always an array of 512 values
  if (preset && (!Array.isArray(preset.dmx_values) || preset.dmx_values.length !== 512)) {
    preset = {
      ...preset,
      dmx_values: Array.isArray(preset.dmx_values)
        ? [...preset.dmx_values, ...Array(512 - preset.dmx_values.length).fill(0)].slice(0, 512)
        : Array(512).fill(0)
    };
  }
  if (!preset) {
    return <div>Preset not found.</div>;
  }
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
    navigate(`/value-edit/${preset.id}/dmx_values/${index}`)
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
          <div className="values-grid-full" ref={valueGridRef}>
            {Array.isArray(preset.dmx_values) ? (
              preset.dmx_values.map((value, index) => (
                <span 
                  key={index}
                  className="value-small"
                  data-index={index}
                  onClick={() => handleIndexClick(index)}
                  style={{ cursor: 'pointer' }}
                >
                  <span className="index">{index}</span>
                  <span 
                    className="value-display-inline"
                    style={{
                      border: value !== 0 ? '2px solid #2196f3' : '1px solid #ccc',
                      borderRadius: '4px',
                      background: value !== 0 ? '#2196f3' : 'transparent',
                      color: value !== 0 ? '#fff' : 'inherit',
                      padding: '2px 6px',
                      marginLeft: '4px',
                      display: 'inline-block'
                    }}
                  >
                    {value}
                  </span>
                </span>
              ))
            ) : (
              <span>No DMX values available.</span>
            )}
          </div>
        </div>
      </div>
    </div>
  )
}

export default PresetEdit
