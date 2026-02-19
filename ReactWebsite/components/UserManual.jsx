import React from 'react';
import ReactMarkdown from 'react-markdown';
import remarkGfm from 'remark-gfm';

// The manual will be loaded as a prop or via fetch
function UserManual({ manualText }) {
  return (
    <div className="user-manual">
      <ReactMarkdown remarkPlugins={[remarkGfm]}>{manualText}</ReactMarkdown>
    </div>
  );
}

export default UserManual;
