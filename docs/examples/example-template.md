# Example Template

Use this as the starting structure for any new example under `examples/`.

## Folder Layout

```text
examples/<example_name>/
├── CMakeLists.txt
├── README.md
├── main/
│   ├── CMakeLists.txt
│   ├── idf_component.yml        # optional, if the example needs managed deps
│   └── <source files>
├── tests/
│   ├── CMakeLists.txt           # host-side cmocka tests
│   └── qemu/
│       ├── README.md            # optional, documents the QEMU suite
│       ├── pytest.ini           # optional, scopes pytest to QEMU tests
│       └── pytest_*.py          # QEMU tests discovered by CI
└── partitions/                  # optional, if the example needs its own table
```

## CMake Pattern

### Example Root

```cmake
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(<example_name>)
```

### Main Component

```cmake
idf_component_register(
    SRCS "main.c" "<helper>.c"
    INCLUDE_DIRS "."
    REQUIRES <board_support_or_other_components>
)
```

### Host Tests

```cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(CMOCKA REQUIRED cmocka)

add_executable(<example_name>_test
    <example_name>_test.c
    ../main/<helper>.c
)

target_include_directories(<example_name>_test PRIVATE
    ${CMOCKA_INCLUDE_DIRS}
    ../main
)

target_compile_options(<example_name>_test PRIVATE ${CMOCKA_CFLAGS_OTHER})
target_link_libraries(<example_name>_test PRIVATE ${CMOCKA_LIBRARIES})
add_test(NAME <example_name>_test COMMAND <example_name>_test)
```

### QEMU Tests

```ini
[pytest]
testpaths = tests/qemu
addopts = -m qemu
markers =
    qemu: QEMU-based pytest checks for this example
```

```python
# pyright: reportMissingImports=false

import pytest

pytestmark = pytest.mark.qemu


def test_boot_logs(dut):
    dut.expect_exact("<boot banner>")
```

## CI Rules

- Add `examples/<name>/tests/` to the root host-test CMake launcher.
- Keep host cmocka tests and QEMU pytest tests in separate subfolders.
- Name QEMU tests `pytest_*.py` so `.github/workflows/testing.yml` can discover them.
- Use the `qemu` marker to keep QEMU tests isolated from other Python tests.

## Copy Checklist

1. Copy the folder layout.
2. Rename the example root project and component targets.
3. Add a host test only for logic that can run on the build machine.
4. Add a QEMU test only for boot-time or serial-log behavior.
5. Update the root test launcher and workflow notes to include the new example.
