# pyright: reportMissingImports=false

import pytest

pytestmark = pytest.mark.qemu


def test_camera_ready_banner(dut):
    dut.expect_exact("Camera ready")
    dut.expect_exact("Ready. Press MENU to capture.")
