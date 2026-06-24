# pyright: reportMissingImports=false

import pytest
import re

pytestmark = pytest.mark.qemu


def test_boot_logs(dut):
    dut.expect_exact("=== ESP32-S3-EYE: button-capture demo ===")
    dut.expect(re.compile(r"Firmware version: .+"))
    dut.expect_exact("Camera ready")
    dut.expect_exact("Ready. Press MENU to capture.")
