import React, { useEffect, useState } from 'react';

import UserManual from './UserManual';
import { useNavigate } from 'react-router-dom';

function ManualPage() {
  const [manualText, setManualText] = useState('');

  useEffect(() => {
    fetch('/UserManual.md')
      .then((res) => {
        if (!res.ok) {
          setManualText('Manual not found. (HTTP ' + res.status + ')');
          throw new Error('Manual not found');
        }
        return res.text();
      })
      .then(setManualText)
      .catch((err) => {
        setManualText('Manual not found.');
        console.error('Manual fetch error:', err);
      });
  }, []);

  const navigate = useNavigate();
  return (
    <div className="manual-page" style={{ maxWidth: '450px', margin: '0 auto' }}>
      <UserManual manualText={manualText} />
    </div>
  );
}

export default ManualPage;
