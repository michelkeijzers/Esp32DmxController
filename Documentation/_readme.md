# Setup

- Install ESP-IDF.
- Install VS Code.
- VS Code Extensions to install:
  - ESP-IDF (Espressif)
  - C/C++ (Microsoft)
  - Plant UML (jebbs)

- In Powershell:
  ```
  cd <ESP-IDF installation path\>
  .\install.ps1
  .\export.ps1
  ```

Ask GitHub Copilot to install:

- jest

# Production Build

Build:

```
Ctrl-E + B
```

# Running React website:

In VS Code terminal (rename to React):

```
cd <....\Esp32Test\Esp32DmxController\ReactWebsite\>
npm install
npm run build
npm run dev
```

The link to open the website will be printed in the terminal, usually http://localhost:5173/.

# How to change ESP target:

In an IDF terminal:

```
cd <....\Esp32Test\Esp32DmxController\>
idf.py menuconfig
idf.py build

```

# Unit Tests

Build unit tests:

In VS Code terminal (rename to Unittests):

```
cd <....\Esp32Test\Esp32DmxController\Unittests\main\Debug>
cmake --build . --config Debug
```

Run unit tests:

```
.\Debug\run_tests.exe
```

or

```
.\Debug\run_tests.exe
```

or

```
F5
```

Run Reactor tests:
In VS Code terminal (rename to react tests):

```
cd <....\Esp32Test\Esp32DmxController\ReactWebsite\>
npx jest --config jest.config.js
```

# Git Commands

- git add -A
- git commit -m "message"
- git push

# Third party libraries:

- nlohmann/json
