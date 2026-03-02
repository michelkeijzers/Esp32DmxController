import React from 'react';
import { render, screen, fireEvent, waitFor } from '@testing-library/react';
import App from '../App';

beforeEach(() => {
  global.fetch = jest.fn(() =>
    Promise.resolve({
      ok: true,
      json: () => Promise.resolve({
        configuration: { /* mock config data */ },
        presets: [
          { id: 1, name: 'Scene 1', dmx_values: []},
          { id: 2, name: 'Scene 2', dmx_values: [] }
        ]
      }),
    })
  );
});

afterEach(() => {
  jest.resetAllMocks();
});

test('Load button fetches all data and updates status', async () => {
  render(<App />);
  const loadButton = screen.getByText(/Load/i);
  fireEvent.click(loadButton);

  await waitFor(() => {
    expect(global.fetch).toHaveBeenCalledWith(
      expect.stringContaining('/all_data'),
      expect.objectContaining({ method: 'GET' })
    );
    expect(screen.getByText(/Loaded 2 presets and configuration successfully/i)).toBeInTheDocument();
  });
});
