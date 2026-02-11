// Helper function to generate values between 0 and 255
const generateValues = (count) => {
  return Array.from({ length: count }, () => Math.floor(Math.random() * 256))
}

// Sample presets - each with 2 sets of 512 numbers (values 0-255)
export const samplePresets = [
  {
    id: 1,
    name: 'Preset Alpha',
    values1: generateValues(512),
    values2: generateValues(512)
  },
  {
    id: 2,
    name: 'Preset Beta',
    values1: generateValues(512),
    values2: generateValues(512)
  },
  {
    id: 3,
    name: 'Preset Gamma',
    values1: generateValues(512),
    values2: generateValues(512)
  }
]
