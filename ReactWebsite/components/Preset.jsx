import './Preset.css'

function Preset({ preset, onUpdateValue, onSendToESP32 }) {
  const handleValueChange = (section, index, event) => {
    const value = event.target.value
    onUpdateValue(preset.id, section, index, value)
  }

  return (
    <div className="preset-card">
      <div className="preset-header">
        <h2>{preset.name}</h2>
        <button 
          className="send-button"
          onClick={() => onSendToESP32(preset)}
        >
          Send to ESP32
        </button>
      </div>
      <div className="preset-info">
        <span>ID: {preset.id}</span>
        <span>Total values: {preset.values1.length + preset.values2.length}</span>
      </div>
      
      <div className="preset-sections">
        <div className="preset-section">
          <h3>Section 1 (512 values)</h3>
          <div className="values-grid">
            {preset.dmx_values.slice(0, 20).map((value, index) => (
              <span key={index} className="value">
                <span className="index">[{index}]</span>
                <input
                  type="number"
                  className="value-input"
                  value={value}
                  onChange={(e) => handleValueChange('dmx_values', index, e)}
                  min="0"
                  max="255"
                />
              </span>
            ))}
            <span className="more">... and {preset.dmx_values.length - 20} more</span>
          </div>
        </div>
      </div>
      
      <details className="full-data">
        <summary>View All Values</summary>
        <div className="all-values">
          <h4>Section 1</h4>
          <div className="values-grid-full">
            {preset.dmx_values.map((value, index) => (
              <span key={index} className="value-small">
                <span className="index">[{index}]</span>
                <input
                  type="number"
                  className="value-input-small"
                  value={value}
                  onChange={(e) => handleValueChange('dmx_values', index, e)}
                  min="0"
                  max="255"
                />
              </span>
            ))}
          </div>
        </div>
      </details>
    </div>
  )
}

export default Preset
