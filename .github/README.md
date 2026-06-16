# CI Workflows

## GitHub Actions

### build.yml - Build all ESP-IDF examples

Automatically builds every example listed in the `matrix.example` list
against the ESP-IDF versions and SoC targets listed in the matrix.

To add a new example:

```yaml
matrix:
  example:
    - "examples/lcd_display"
    - "examples/your_new_demo"
```

Requires:

- Example must contain a valid `CMakeLists.txt` project root
- Example must contain `idf_component.yml` or valid dependencies
- The `target` must match what the example is set up for (here: esp32s3)
