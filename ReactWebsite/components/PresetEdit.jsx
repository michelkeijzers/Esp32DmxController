import { useParams, useNavigate } from 'react-router-dom'
import './PresetEdit.css'

function PresetEdit({ presets, onUpdateName }) {
  const { id } = useParams()
  const navigate = useNavigate()
  const preset = presets.find(p => p.id === parseInt(id))

  if (!preset) {
    return (
      <div className="preset-edit-error">
        <h2>Preset not found</h2>
        <button onClick={() => navigate('/')}>Back to List</button>
      </div>
    )
  }

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

  return (
    <div className="preset-edit">
      <div className="preset-edit-header">
        <button className="back-button" onClick={() => navigate('/')}>
          ← Back to List
        </button>
        <div className="preset-title-container">
          <span className="preset-number">Preset {preset.id - 1}</span>
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
          <h3>Universe 1</h3>
          <div className="values-grid-full">
            {preset.values1.map((value, index) => (
              <span 
                key={index} 
                className="value-small"
                onClick={() => navigate(`/preset/${preset.id}/values1/${index}`)}
                style={{ cursor: 'pointer' }}
              >
                <span className="index">[{index}]</span>
                <span className="value-display-inline">{value}</span>
              </span>
            ))}
          </div>
        </div>

        <div className="preset-section">
          <h3>Universe 2</h3>
          <div className="values-grid-full">
            {preset.values2.map((value, index) => (
              <span 
                key={index} 
                className="value-small"
                onClick={() => navigate(`/preset/${preset.id}/values2/${index}`)}
                style={{ cursor: 'pointer' }}
              >
                <span className="index">[{index}]</span>
                <span className="value-display-inline">{value}</span>
              </span>
            ))}
          </div>
        </div>
      </div>
    </div>
  )
}

export default PresetEdit
