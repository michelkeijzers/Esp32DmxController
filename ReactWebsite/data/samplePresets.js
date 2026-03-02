// Helper function to generate values between 0 and 255
const generateValues = (count) => {
  return Array.from({ length: count }, () => Math.floor(Math.random() * 256))
}

// Sample presets - each with 2 sets of 512 numbers (values 0-255)
export const samplePresets = [
  {
    id: 1,
    name: 'Preset Alpha',
    dmx_values: generateValues(512),
  },
  {
    id: 2,
    name: 'Preset Beta',
    dmx_values: generateValues(512)
  },
  {
    id: 3,
    name: 'Preset Gamma',
    dmx_values: generateValues(512)
  }
]
