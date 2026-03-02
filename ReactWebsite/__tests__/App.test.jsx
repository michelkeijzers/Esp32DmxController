import React from 'react';
import { render, screen, fireEvent, waitFor } from '@testing-library/react';
import App from '../App';
import { MemoryRouter } from 'react-router-dom';

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

describe('React Load button system test', () => {
  beforeEach(() => {
    jest.spyOn(global, 'fetch').mockImplementation((url) => {
      if (url.includes('/all_data')) {
        return Promise.resolve({
          ok: true,
          json: () => Promise.resolve({
            configuration: { getMaxNumberOfPresets: 20, circularNavigation: true },
            presets: [
              { id: 1, name: 'Test Scene', dmx_values: Array(512).fill(0).map((v, i) => i === 5 ? 123 : 0) },
              { id: 2, name: 'Another Scene', dmx_values: Array(512).fill(0) }
            ]
          })
        });
      }
      return Promise.reject(new Error('Unknown endpoint'));
    });
  });

  afterEach(() => {
    global.fetch.mockRestore();
  });

  it('loads and displays data from the webserver when Load button is clicked', async () => {
    render(<App />);
    const loadButton = screen.getByText(/Load/i);
    fireEvent.click(loadButton);
    await waitFor(() => expect(screen.getByText(/✓ Loaded/)).toBeInTheDocument());
    expect(screen.getByText('Test Scene')).toBeInTheDocument();
    const presetCountSpan = screen.getByText('2');
    expect(presetCountSpan).toBeInTheDocument();

    // Simulate navigation to preset edit page by clicking preset item
    fireEvent.click(screen.getByText('Test Scene'));
    // Wait for DMX value '123' to appear in the edit view using a custom matcher
    await waitFor(() => {
      const valueSpans = screen.queryAllByText((content, element) => {
        return element.tagName.toLowerCase() === 'span' && content === '123';
      });
      expect(valueSpans.length).toBeGreaterThan(0);
    });
  });
});
