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


  const handleDmxTransmitChange = (e) => {
    const { name, checked } = e.target;
    // If unchecking and the other is already false, prevent both from being false
    if (!checked) {
      if (name === 'sendViaArtNet' && !config.sendToDmxConnector) return;
      if (name === 'sendToDmxConnector' && !config.sendViaArtNet) return;
    }
    onConfigChange(name, checked);
  };

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
          <label htmlFor="blackoutLongPress">Blackout Preset Long Press time (ms):</label>
          <input
            id="blackoutLongPress"
            type="number"
            min={2000}
            max={5000}
            value={config.blackoutLongPress ?? 2000}
            onChange={e => {
              let value = parseInt(e.target.value, 10);
              if (isNaN(value)) value = 2000;
              value = Math.max(2000, Math.min(5000, value));
              onConfigChange('blackoutLongPress', value);
            }}
          />
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
        <div className="config-item">
          <label style={{ marginTop: '12px', display: 'inline-block' }}>
            <input
              type="checkbox"
              checked={!!config.oscSendFootSwitch}
              onChange={e => onConfigChange('oscSendFootSwitch', e.target.checked)}
            />
            <span>Send Foot Switch State Changes through OSC</span>
          </label>
        </div>
      </div>

      {/* OSC Section */}
      <div className="config-section">
        <h3>OSC (Open Sound Control)</h3>
        <div className="config-item">
          <label htmlFor="oscAddress">OSC Address (IP):</label>
          <input
            id="oscAddress"
            type="text"
            value={config.oscAddress ?? ''}
            onChange={e => onConfigChange('oscAddress', e.target.value)}
            placeholder="e.g. 192.168.1.100"
            pattern="^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$"
            title="Enter a valid IP address"
          />
        </div>
        <div className="config-item">
          <label htmlFor="oscReceivePort">OSC Receive Port:</label>
          <input
            id="oscReceivePort"
            type="number"
            min={1}
            max={65535}
            value={config.oscReceivePort ?? ''}
            onChange={e => onConfigChange('oscReceivePort', parseInt(e.target.value, 10) || '')}
          />
        </div>
        <div className="config-item">
          <label htmlFor="oscSendPort">OSC Send Port:</label>
          <input
            id="oscSendPort"
            type="number"
            min={1}
            max={65535}
            value={config.oscSendPort ?? ''}
            onChange={e => onConfigChange('oscSendPort', parseInt(e.target.value, 10) || '')}
          />
        </div>
      </div>

      {/* DMX Transmission Section (moved after Foot Switch) */}
      <div className="config-section">
        <h3>DMX Transmission</h3>
        <div className="config-item">
          <label>
            <input
              type="checkbox"
              name="sendViaArtNet"
              checked={!!config.sendViaArtNet}
              onChange={handleDmxTransmitChange}
            />
            <span>Send via ArtNET</span>
          </label>
        </div>
        <div className="config-item">
          <label>
            <input
              type="checkbox"
              name="sendToDmxConnector"
              checked={!!config.sendToDmxConnector}
              onChange={handleDmxTransmitChange}
            />
            <span>Send to DMX connector</span>
          </label>
        </div>
        {(!config.sendViaArtNet && !config.sendToDmxConnector) && (
          <div className="config-warning" style={{ color: 'red', marginTop: 8 }}>
            DMX will not be transmitted
          </div>
        )}
      </div>
    </div>
  );
}

export default Configuration
