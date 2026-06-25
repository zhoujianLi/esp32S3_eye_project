# lcd_display QEMU Tests

This folder contains pytest-based QEMU checks for the `lcd_display` example.

The example root [pytest.ini](../../pytest.ini) limits pytest collection to this
folder and requires the `qemu` marker, so future examples can follow the same
pattern without mixing in unrelated Python tests.

## Files

- `pytest_boot.py` verifies the boot banner and firmware version output.
- `pytest_ready.py` verifies the camera-ready and idle-ready messages.

## CI Discovery

The workflow discovers these tests because they follow the `pytest_*.py` naming
pattern used by `.github/workflows/testing.yml`.
