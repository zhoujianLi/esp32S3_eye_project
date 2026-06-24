# lcd_display Example

This example captures a camera frame on button press and displays it on the LCD.

## Layout

- `main/` contains the firmware entry point and example-specific helper code.
- `tests/` contains host-side unit tests for pure helper logic used by the example.

## Build

```bash
cd examples/lcd_display
idf.py set-target esp32s3
idf.py build
```

## Host Tests

The repository CI can build and run the host tests through the root-level test
cmake entry with `-DBUILD_TESTS=ON`. The example-level test module lives in
`examples/lcd_display/tests`.

## QEMU Tests

The example also includes QEMU-based pytest checks under
`examples/lcd_display/tests/qemu`. Those files are named `pytest_*.py` so the CI
workflow can discover them automatically and run them in the QEMU job.

Future examples should follow the same pattern: keep example-specific tests next
to the example, and promote shared code into `components/` only when it is
truly reusable.
