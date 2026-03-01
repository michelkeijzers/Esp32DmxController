import { useNavigate } from 'react-router-dom'
import './Configuration.css'




function Configuration({ config, onConfigChange }) {
  const navigate = useNavigate();

  const handlePolarityChange = (value) => {
    onConfigChange('footSwitchPolarity', value);
  };

  const handleLongPressTimeChange = (e) => {
    let value = parseInt(e.target.value, 10);
    if (isNaN(value)) value = 700;
    value = Math.max(500, Math.min(2000, value));
    onConfigChange('footSwitchLongPressTime', value);
  };


  // ...existing code...

  return (
    <div className="configuration">
      <div className="configuration-header">
        <button className="manual-button" onClick={() => navigate('/')}> 
          Home
        </button>
        <h2>Configuration</h2>
        <div className="header-spacer"></div>
      </div>


      {/* Presets Section */}
      <div className="config-section">
        <h3>Presets</h3>
        <div className="config-item">
          <label htmlFor="maxPresets">Maximum Number of Presets:</label>
          <input
            id="maxPresets"
            type="number"
            min={20}
            max={50}
            value={config.maxPresets ?? 20}
            onChange={e => {
              let value = parseInt(e.target.value, 10);
              if (isNaN(value)) value = 20;
              value = Math.max(20, Math.min(50, value));
              onConfigChange('maxPresets', value);
            }}
          />
        </div>
        <div className="config-item">
          <label htmlFor="circularNavigation" className="circular-navigation-item">
            <input
              id="circularNavigation"
              type="checkbox"
              checked={!!config.circularNavigation}
              onChange={e => onConfigChange('circularNavigation', e.target.checked)}
            />
            Circular Navigation
          </label>
        </div>
        <div className="config-item">
          {/* Removed Blackout Preset Long Press time (ms) */}
        </div>
      </div>



      {/* Foot Switch Section */}
      <div className="config-section">
        <h3>Foot Switch</h3>
        <div className="config-item">
          <label>Polarity:</label>
          <div className="radio-group">
            <label className="radio-option">
              <input
                type="radio"
                name="polarity"
                value="normallyOpen"
                checked={config.footSwitchPolarity === 'normallyOpen'}
                onChange={(e) => handlePolarityChange(e.target.value)}
              />
              <span>Normally Open</span>
            </label>
            <label className="radio-option">
              <input
                type="radio"
                name="polarity"
                value="normallyClosed"
                checked={config.footSwitchPolarity === 'normallyClosed'}
                onChange={(e) => handlePolarityChange(e.target.value)}
              />
              <span>Normally Closed</span>
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
        <div className="config-item">
          <label style={{ marginTop: '12px', display: 'inline-block', color: '#333' }}>
            <input
              type="checkbox"
              checked={!!config.oscSendFootSwitch}
              disabled
              style={{ accentColor: '#111', filter: 'brightness(0.6)' }}
            />
            <span style={{ color: '#333' }}>Send Foot Switch State Changes through OSC</span>
          </label>
        </div>
      </div>

      {/* OSC Section - now darker */}
      <div className="config-section" style={{ background: '#111', color: '#222', borderRadius: '8px', padding: '18px 16px', marginTop: '12px' }}>
        <h3 style={{ color: '#222' }}>OSC (Open Sound Control)</h3>
        <div className="config-item">
          <label htmlFor="oscAddress" style={{ color: '#222' }}>OSC Address (IP):</label>
          <input
            id="oscAddress"
            type="text"
            value={config.oscAddress ?? ''}
            onChange={e => onConfigChange('oscAddress', e.target.value)}
            placeholder="e.g. 192.168.1.100"
            pattern="^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$"
            title="Enter a valid IP address"
            style={{ background: '#222', color: '#fafafa', border: '1px solid #333' }}
          />
        </div>
        <div className="config-item">
          <label htmlFor="oscReceivePort" style={{ color: '#222' }}>OSC Receive Port:</label>
          <input
            id="oscReceivePort"
            type="number"
            min={1}
            max={65535}
            value={config.oscReceivePort ?? ''}
            onChange={e => onConfigChange('oscReceivePort', parseInt(e.target.value, 10) || '')}
            style={{ background: '#222', color: '#fafafa', border: '1px solid #333' }}
          />
        </div>
        <div className="config-item">
          <label htmlFor="oscSendPort" style={{ color: '#222' }}>OSC Send Port:</label>
          <input
            id="oscSendPort"
            type="number"
            min={1}
            max={65535}
            value={config.oscSendPort ?? ''}
            onChange={e => onConfigChange('oscSendPort', parseInt(e.target.value, 10) || '')}
            style={{ background: '#222', color: '#fafafa', border: '1px solid #333' }}
          />
        </div>
      </div>

      {/* DMX Transmission menu and related code removed as requested */}
    </div>
  );
}

export default Configuration
