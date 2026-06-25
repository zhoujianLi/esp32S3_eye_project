# Host-test Docker image for ESP32-S3-EYE project
#
# This image provides a reproducible environment for:
#   - Host-based unit tests (cmocka)
#   - Static analysis (cppcheck, clang-tidy)
#   - Code coverage (lcov, gcovr)
#   - QEMU-based integration tests (qemu-system-xtensa + pytest-embedded)
#
# It is used by:
#   1. Local development: `docker compose run --rm host-test ./run-tests.sh`
#   2. GitHub Actions CI: declared as `container:` in code-quality.yml and testing.yml
#   3. Future HIL runners: extended with hardware access layers
#
# The image is published to GitHub Container Registry (GHCR) by
# .github/workflows/docker-publish.yml on every change to this Dockerfile.

FROM ubuntu:24.04

LABEL org.opencontainers.image.title="ESP32-S3-EYE Host Test"
LABEL org.opencontainers.image.description="Reproducible test environment for ESP32-S3-EYE project"
LABEL org.opencontainers.image.source="https://github.com/zhoujianLi/esp32S3_eye_project"

# Prevent apt/dpkg interactive prompts during build
ENV DEBIAN_FRONTEND=noninteractive

# Install build tools, test frameworks, static analysis, coverage, and QEMU
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    make \
    git \
    jq \
    curl \
    ca-certificates \
    # C unit testing framework
    libcmocka-dev \
    # Coverage tools
    lcov \
    # Static analysis
    cppcheck \
    clang-tidy \
    clang-format \
    # Python for pytest-embedded and tooling
    python3 \
    python3-pip \
    python3-venv \
    # QEMU for ESP32 emulation (xtensa target)
    qemu-system-xtensa \
    && rm -rf /var/lib/apt/lists/*

# Install Python-based test/coverage tools
# gcovr not in apt for Ubuntu 24.04, install via pip
RUN pip3 install --no-cache-dir --break-system-packages \
    gcovr \
    pytest \
    pytest-embedded \
    pytest-embedded-serial-esp \
    pytest-embedded-idf

# Create a non-root user for running tests (avoids permission issues
# with mounted volumes from Windows host).
# ubuntu:24.04 already has UID 1000 reserved for the 'ubuntu' user, so
# we remove it first to free the UID.
RUN userdel -r ubuntu || true \
    && useradd --create-home --uid 1000 --shell /bin/bash devuser

WORKDIR /workspace

# Copy the test runner script
COPY run-tests.sh /usr/local/bin/run-tests.sh
RUN chmod +x /usr/local/bin/run-tests.sh

# Switch to non-root user
USER devuser

# Default command: run the test suite
CMD ["/usr/local/bin/run-tests.sh"]
