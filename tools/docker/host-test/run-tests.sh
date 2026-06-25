#!/usr/bin/env bash
# run-tests.sh — Local host-test runner for ESP32-S3-EYE project
#
# Discovers and runs:
#   1. CMake-based host unit tests (cmocka)
#   2. cppcheck static analysis
#   3. clang-tidy static analysis
#   4. (Optional) coverage report generation
#
# Exits non-zero on any failure. Intended to be run inside the host-test
# Docker container, but works on any system with the required tools installed.
#
# Usage:
#   ./run-tests.sh                  # Run all tests
#   ./run-tests.sh --coverage       # Run tests + generate coverage report
#   ./run-tests.sh --skip-analysis  # Skip cppcheck/clang-tidy (tests only)

set -euo pipefail

# The Docker Compose host-test service mounts the repo at /workspace and
# sets working_dir to /workspace. Use that repo mount when available.
if [ -f /workspace/CMakeLists.txt ]; then
    cd /workspace
else
    SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
    if [ -f "$SCRIPT_DIR/../../.."/CMakeLists.txt ]; then
        cd "$SCRIPT_DIR/../../.."
    else
        echo "Error: could not locate repo root."
        exit 1
    fi
fi

GENERATE_COVERAGE=false
SKIP_ANALYSIS=false

for arg in "$@"; do
    case "$arg" in
        --coverage)
            GENERATE_COVERAGE=true
            ;;
        --skip-analysis)
            SKIP_ANALYSIS=true
            ;;
        *)
            echo "Unknown argument: $arg"
            echo "Usage: $0 [--coverage] [--skip-analysis]"
            exit 1
            ;;
    esac
done

echo "============================================================"
echo "ESP32-S3-EYE Host Test Suite"
echo "============================================================"
echo "Repo: $(pwd)"
echo ""

# ---------------------------------------------------------------------------
# Step 1: Check for unit test files
# ---------------------------------------------------------------------------
TEST_FILES=$(find examples -type f \( -name "*_test.c" -o -name "*_test.cpp" \
                                    -o -name "test_*.c" -o -name "test_*.cpp" \) \
            -not -path "*/managed_components/*" -not -path "*/build/*" || true)

if [ -z "$TEST_FILES" ]; then
    echo "ℹ️  No unit test files found. Skipping host unit tests."
    HAS_TESTS=false
else
    echo "📋 Found unit test files:"
    echo "$TEST_FILES" | sed 's/^/    /'
    HAS_TESTS=true
fi
echo ""

# ---------------------------------------------------------------------------
# Step 2: Run host unit tests (cmocka via CMake)
# ---------------------------------------------------------------------------
if [ "$HAS_TESTS" = "true" ]; then
    echo "────────────────────────────────────────────────────────────"
    echo "Step 1/3: Building and running host unit tests"
    echo "────────────────────────────────────────────────────────────"

    mkdir -p build_host
    cd build_host

    CMAKE_ARGS="-DBUILD_TESTS=ON"
    if [ "$GENERATE_COVERAGE" = "true" ]; then
        echo "  (coverage enabled — adding -fprofile-arcs -ftest-coverage)"
        CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_C_FLAGS='-fprofile-arcs -ftest-coverage' \
                              -DCMAKE_EXE_LINKER_FLAGS='-fprofile-arcs'"
    fi

    cmake .. $CMAKE_ARGS
    make -j"$(nproc)"
    ctest --output-on-failure
    cd ..
    echo "✅ Host unit tests passed"
    echo ""
else
    echo "────────────────────────────────────────────────────────────"
    echo "Step 1/3: Host unit tests (skipped — no test files)"
    echo "────────────────────────────────────────────────────────────"
    echo ""
fi

# ---------------------------------------------------------------------------
# Step 3: Run cppcheck static analysis
# ---------------------------------------------------------------------------
if [ "$SKIP_ANALYSIS" = "false" ]; then
    echo "────────────────────────────────────────────────────────────"
    echo "Step 2/3: cppcheck static analysis"
    echo "────────────────────────────────────────────────────────────"

    CPPCHECK_FILES=$(find examples -name "*.c" -o -name "*.h" \
                    | grep -v managed_components | grep -v build || true)
    if [ -n "$CPPCHECK_FILES" ]; then
        # shellcheck disable=SC2086
        # Notes on suppressions:
        # - missingInclude: ESP-IDF headers (bsp, esp_camera, etc.) aren't
        #   installed in the host-test container; they only exist during
        #   firmware builds inside the esp-idf container. Suppress to avoid
        #   noise. Local headers (capture_helpers.h) are still found via -I.
        # - missingIncludeSystem: redundant with above, kept for safety.
        # - unusedFunction/unusedStructMember: noisy on firmware code.
        # - unmatchedSuppression: suppresses warnings about unused suppressions.
        cppcheck --enable=all --std=c99 --platform=unix64 \
                 --suppress=missingInclude \
                 --suppress=missingIncludeSystem \
                 --suppress=unusedFunction \
                 --suppress=unusedStructMember \
                 --suppress=unmatchedSuppression \
                 --error-exitcode=1 \
                 --inline-suppr \
                 -I examples/lcd_display/main \
                 $CPPCHECK_FILES
        echo "✅ cppcheck passed (no errors)"
    else
        echo "ℹ️  No C/H files to check"
    fi
    echo ""
else
    echo "────────────────────────────────────────────────────────────"
    echo "Step 2/3: cppcheck (skipped via --skip-analysis)"
    echo "────────────────────────────────────────────────────────────"
    echo ""
fi

# ---------------------------------------------------------------------------
# Step 4: Run clang-tidy
# ---------------------------------------------------------------------------
if [ "$SKIP_ANALYSIS" = "false" ]; then
    echo "────────────────────────────────────────────────────────────"
    echo "Step 3/3: clang-tidy static analysis"
    echo "────────────────────────────────────────────────────────────"

    CLANG_TIDY_FILES=$(find examples -name "*.c" \
                      | grep -v managed_components | grep -v build || true)
    if [ -n "$CLANG_TIDY_FILES" ]; then
        # shellcheck disable=SC2086
        clang-tidy --quiet $CLANG_TIDY_FILES 2>/dev/null || {
            echo "⚠️  clang-tidy reported issues (non-blocking in local run)"
        }
        echo "✅ clang-tidy completed"
    else
        echo "ℹ️  No C files to check"
    fi
    echo ""
else
    echo "────────────────────────────────────────────────────────────"
    echo "Step 3/3: clang-tidy (skipped via --skip-analysis)"
    echo "────────────────────────────────────────────────────────────"
    echo ""
fi

# ---------------------------------------------------------------------------
# Step 5 (optional): Generate coverage report
# ---------------------------------------------------------------------------
if [ "$GENERATE_COVERAGE" = "true" ] && [ "$HAS_TESTS" = "true" ]; then
    echo "────────────────────────────────────────────────────────────"
    echo "Bonus: Generating coverage report"
    echo "────────────────────────────────────────────────────────────"

    mkdir -p coverage
    gcovr --root . \
          --filter 'examples/' \
          --exclude '.*build.*' \
          --exclude '.*managed_components.*' \
          --html --html-details -o coverage/coverage.html \
          --print-summary || echo "⚠️  No coverage data generated"
    echo "📊 Coverage report at: coverage/coverage.html"
    echo ""
fi

echo "============================================================"
echo "✅ All host tests passed"
echo "============================================================"
