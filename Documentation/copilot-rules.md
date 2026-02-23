- GitHub Copilot Rules and Guidelines:

\*\* Documentation

\*\* Coding Standards

- Use C++, ESP32 IDF, and FreeRTOS APIs for development.
- Follow the ESP32 IDF coding style and best practices.
- Ensure code is modular, maintainable, and well-documented with comments where necessary.
- Use the SOLID principles and describe them when being used.
- Put one class in a file, one file per class.

\*\* Testing

- Always write/adapt unit tests when code changes. This ensures that new code is covered and existing functionality is not broken.
- Each production file (in main) should have a corresponding unit test file (in test), having the same name with a \_test suffix (e.g., rtos_task.cpp and rtos_task_test.cpp) and be in the same folder structure.
- WHen requested to fix test code, do not change the production code, only the test code. If the production code needs to be changed, ask for a separate request to change the production code.
- Use the Google Test framework for unit testing. Follow the standard structure of test cases, including setup and teardown if necessary.
