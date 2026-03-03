# Setup

- Install ESP-IDF.
- Install VS Code.
- VS Code Extensions to install:
  - ESP-IDF (Espressif)
  - C/C++ (Microsoft)
  - Plant UML (jebbs)

# VS Code:

MAKE SURE TO OPEN THE FOLDER:

```
<....\Esp32Test\Esp32DmxController\>
```

and not

```
<....\Esp32Test>
```

- In Powershell:
  ```
  cd <ESP-IDF installation path\>
  .\install.ps1
  .\export.ps1
  ```

Ask GitHub Copilot to install:

- jest

Install manually:

- OpenCppCoverage

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
.\Debug\Debug\run_tests.exe
```

or

```
.\Debug\Debug\run_tests.exe
```

or

```
F5
```

Run with code coverage:

```
cmake --build . --config Debug --target coverage
C:\OpenCppCoverage\OpenCppCoverage.exe --export_type html:coverage_report --sources "C:\Users\i4studio\Esp32Test\Esp32DmxController\main\Tasks\dmx_controller.cpp" -- Debug\Debug\run_tests.exe
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
