import React, { Suspense } from 'react';
import { createRoot } from 'react-dom/client';
import App from './components/App';

const rootElement = document.getElementById('root');

const root = createRoot(rootElement);
root.render(
    <Suspense fallback={<div>Loading...</div>}>
      <App />
    </Suspense>,
  );