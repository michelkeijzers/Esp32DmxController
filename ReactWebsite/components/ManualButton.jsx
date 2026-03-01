import React from 'react';
import { useNavigate } from 'react-router-dom';

function ManualButton() {
  const navigate = useNavigate();
  return (
    <button className="config-button" onClick={() => navigate('/manual')}>
      Manual
    </button>
  );
}

export default ManualButton;
