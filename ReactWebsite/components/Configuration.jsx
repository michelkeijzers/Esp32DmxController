import { useNavigate } from 'react-router-dom'
import './Configuration.css'



function Configuration({ config, onConfigChange }) {
  const navigate = useNavigate()

  const handlePolarityChange = (value) => {
    onConfigChange('footSwitchPolarity', value)
  }

  const handleLongPressTimeChange = (e) => {
    let value = parseInt(e.target.value, 10)
    if (isNaN(value)) value = 700
    value = Math.max(500, Math.min(2000, value))
    onConfigChange('footSwitchLongPressTime', value)
  }

  return (
    <div className="configuration">
      <div className="configuration-header">
        <button className="back-button" onClick={() => navigate('/')}> 
          ← Back to List
        </button>
        <h2>Configuration</h2>
        <div className="header-spacer"></div>
      </div>

      <div className="config-section">
        <h3>Foot Switch Configuration</h3>
        <div className="config-item">
          <label>Polarity:</label>
          <div className="radio-group">
            <label className="radio-option">
              <input
                type="radio"
                name="polarity"
                value="standard"
                checked={config.footSwitchPolarity === 'standard'}
                onChange={(e) => handlePolarityChange(e.target.value)}
              />
              <span>Standard</span>
            </label>
            <label className="radio-option">
              <input
                type="radio"
                name="polarity"
                value="inverted"
                checked={config.footSwitchPolarity === 'inverted'}
                onChange={(e) => handlePolarityChange(e.target.value)}
              />
              <span>Inverted</span>
            </label>
          </div>
        </div>
        <div className="config-item">
          <label htmlFor="longPressTime">Long Press Time (ms):</label>
          <input
            id="longPressTime"
            type="number"
            min={500}
            max={2000}
            value={config.footSwitchLongPressTime ?? 700}
            onChange={handleLongPressTimeChange}
          />
        </div>
      </div>
    </div>
  )
}

export default Configuration
