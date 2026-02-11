import { useNavigate } from 'react-router-dom'
import './PresetList.css'

function PresetList({ presets, onDeletePreset, onInsertPreset, onMovePreset, canInsert }) {
  const navigate = useNavigate()

  const handleDelete = (e, presetId) => {
    e.stopPropagation()
    onDeletePreset(presetId)
  }

  const handleInsert = (e, presetId) => {
    e.stopPropagation()
    onInsertPreset(presetId)
  }

  const handleMove = (e, presetId, direction) => {
    e.stopPropagation()
    onMovePreset(presetId, direction)
  }

  return (
    <div className="preset-list">
      <div className="preset-grid">
        {presets.map((preset, index) => (
          <div 
            key={preset.id} 
            className="preset-item"
            onClick={() => navigate(`/preset/${preset.id}`)}
          >
            <h3><span className="preset-number-list">{index}:</span> {preset.name}</h3>
            <div className="preset-actions">
              <button 
                className="move-button"
                onClick={(e) => handleMove(e, preset.id, 'up')}
                disabled={index === 0}
                title="Move preset up"
              >
                ↑
              </button>
              <button 
                className="move-button"
                onClick={(e) => handleMove(e, preset.id, 'down')}
                disabled={index === presets.length - 1}
                title="Move preset down"
              >
                ↓
              </button>
              <button 
                className="insert-button"
                onClick={(e) => handleInsert(e, preset.id)}
                disabled={!canInsert}
                title="Insert preset before this one"
              >
                +
              </button>
              <button 
                className="delete-button"
                onClick={(e) => handleDelete(e, preset.id)}
                title="Delete preset"
              >
                ✕
              </button>
            </div>
          </div>
        ))}
      </div>
    </div>
  )
}

export default PresetList
