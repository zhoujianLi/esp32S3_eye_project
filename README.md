# ESP32-S3-EYE Project

This repository contains firmware examples and a complete DevOps pipeline for the ESP32-S3-EYE development board.

## Hardware

The **ESP32-S3-EYE** is a development board featuring:
- ESP32-S3 WROOM-1 module with Wi-Fi and Bluetooth 5 (LE)
- 2-inch LCD display (ST7789 controller, 320x240 pixels)
- OV2640 camera module (2 megapixel)
- MEMS microphone
- SD card slot
- USB Type-C for power and programming
- User buttons and LEDs

## Project Structure

```
esp32_s3_eye/
├── examples/               # Firmware examples
│   └── lcd_display/       # LCD display example
├── components/             # Reusable components
├── docs/                   # Documentation
│   ├── hardware/           # Hardware schematics and datasheets
├── tools/                  # Helper scripts
└── .github/
    └── workflows/          # GitHub Actions CI/CD pipelines
        ├── main.yml        # Main orchestrator workflow
        ├── build.yml       # Build matrix with multiple IDF versions
        ├── code-quality.yml  # Static analysis and linting
        ├── testing.yml     # Unit tests and coverage
        ├── security.yml    # Security scanning
        ├── docs.yml        # Documentation build
        └── release.yml     # Release management with OTA
```

## CI/CD Pipeline Features

### 🛠️ Build Pipeline (`build.yml`)
- **Matrix builds** across multiple ESP-IDF versions (v5.5.4, v5.4.2, v5.3.3)
- **Automatic example discovery**
- **Cache optimization** for faster builds
- **Firmware size analysis** (size, size-components, size-files)
- **Combined firmware image** generation
- **Build manifest** with version metadata
- Artifact retention for 30 days

### 🔍 Code Quality (`code-quality.yml`)
- **Cppcheck** - Static C/C++ code analysis
- **Clang-Tidy** - C++ linter and static analyzer
- **Clang-Format** - Code formatting enforcement
- **CMake Lint** - CMake file linting
- **Compiler warnings** tracking

### 🧪 Testing (`testing.yml`)
- Host-based unit tests (CMocka)
- QEMU-emulated tests (pytest-embedded)
- Code coverage reporting (gcovr + lcov)
- SonarCloud integration

### 🔒 Security (`security.yml`)
- **CodeQL Analysis** - Code scanning for vulnerabilities
- **Dependency Review** - Check for vulnerable dependencies
- **Secret Scanning** (Gitleaks) - Detect accidental secrets
- **Trivy** - Filesystem vulnerability scanning
- **OpenSSF Scorecard** - Open source health metrics

### 📚 Documentation (`docs.yml`)
- Markdown linting
- Broken link checking
- Spell checking
- MkDocs documentation site build
- Automatic GitHub Pages deployment

### 🚀 Release Management (`release.yml`)
- Automated release on tag push (`v*` pattern)
- **OTA firmware generation** for in-field updates
- Full factory flash image generation
- Automatic release notes from git history
- Manifest file with version metadata
- GitHub Release with all assets

## Getting Started

### Prerequisites

- **ESP-IDF v5.x** - Follow the [ESP-IDF Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html)
- Python 3.7+
- CMake and build tools

### Building Locally

```bash
# Set up ESP-IDF environment
. $HOME/esp/esp-idf/export.sh

# Navigate to an example
cd examples/lcd_display

# Set target
idf.py set-target esp32s3

# Build
idf.py build

# Flash to device
idf.py flash

# Monitor serial output
idf.py monitor
```

## Pipeline Usage

### Triggering Builds

Pipelines run automatically on:
- Push to `master` or `main` branches
- Pull requests targeting `master` or `main`
- Tag pushes matching `v*` (triggers release)

### Manual Triggers

All workflows can be manually triggered from the **Actions** tab:
- `workflow_dispatch` triggers with optional parameters
- Specify custom ESP-IDF versions
- Enable verbose build output
- Control draft/pre-release status

## OTA Updates

The release pipeline generates OTA-ready firmware images. Use them with your OTA server:

```python
# Example OTA manifest entry
{
  "version": "v1.0.0",
  "url": "https://github.com/your-username/esp32_s3_eye/releases/download/v1.0.0/lcd_display_v1.0.0_ota.bin",
  "md5": "..."
}
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Ensure all CI checks pass
5. Submit a pull request

## License

This project follows the licensing of the included ESP-IDF components. See individual files for details.

## Support

For issues:
1. Check the [GitHub Issues](../../issues)
2. Review ESP-IDF documentation
3. See hardware schematics in `docs/hardware/`
