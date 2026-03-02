# Contract between React/Webserver

**Endpoint:** \_all_data:

Request (POST), Response (GET):

```http
{
    "configuration": {
        "getMaxNumberOfPresets": 20,
        "circularNavigation": true
    },
    "presets": [
        {
            "id": 1,
            "name": "Test Scene",
            "dmx_values": [0, 0, 123, 0, ...] // 512 values
        }
    ]
}
```

- configuration may include other keys (e g. footSwitchPolarity, expressionPedalPolarity).
- presets is an array of objects, each with:
  - id: integer
  - name: string
  - dmx_values: array of 512 integers (0-255)

# Sample JSON Schema for /all_data

```json
{
  "type": "object",
  "properties": {
    "configuration": {
      "type": "object",
      "properties": {
        "getMaxNumberOfPresets": { "type": "integer" },
        "circularNavigation": { "type": "boolean" }
        // Add other config keys as needed
      },
      "required": ["getMaxNumberOfPresets", "circularNavigation"]
    },
    "presets": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "id": { "type": "integer" },
          "name": { "type": "string" },
          "dmx_values": {
            "type": "array",
            "items": { "type": "integer", "minimum": 0, "maximum": 255 },
            "minItems": 512,
            "maxItems": 512
          }
        },
        "required": ["id", "name", "dmx_values"]
      }
    }
  },
  "required": ["configuration", "presets"]
}
```
