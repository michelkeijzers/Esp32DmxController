import { useState, useRef } from 'react';
import { useNavigate } from 'react-router-dom';
import './OtaUpdate.css';

export default function OtaUpdate() {
  const [fileName, setFileName] = useState('');
  const [selectedFile, setSelectedFile] = useState(null);
  const fileInputRef = useRef();
  const navigate = useNavigate();

  const handleConfirm = () => {
    setFileName('');
    setSelectedFile(null);
  };

  const handleCancel = () => {
    setFileName('');
    setSelectedFile(null);
    navigate('/');
  };

  const handleFileChange = (e) => {
    const file = e.target.files[0];
    if (file) {
      setSelectedFile(file);
      setFileName(file.name);
    }
  };

  const showFileDialog = () => {
    fileInputRef.current.click();
  };

  return (
    <div className="ota-update-page">
      <div className="configuration-header">
        <h2>OTA Update</h2>
      </div>
        <div className="ota-update-input-row" >
          <input
            type="text"
            placeholder="Enter file name..."
            value={fileName}
            onChange={e => setFileName(e.target.value)}
            className="ota-update-file-name"
            readOnly
          />
          <input
            type="file"
            style={{ display: 'none' }}
            ref={fileInputRef}
            onChange={handleFileChange}
          />
          <button className="ota-update-file-dialog" onClick={showFileDialog}>
            Choose File
          </button>
        </div>
        <div className="ota-update-buttons" style={{ marginTop: '1rem' }}>
          <button className="ota-update-cancel" onClick={handleCancel}>Cancel</button>
          <button className="ota-update-confirm" onClick={handleConfirm}>Confirm</button>
        </div>
      </div>
  );
}
