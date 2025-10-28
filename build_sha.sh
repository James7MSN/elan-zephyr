#!/usr/bin/env bash
set -euo pipefail

# build.sh - wrapper to build elan-zephyr samples with a reproducible env
# Usage: ./build.sh [--sample <path>] [--board <board>] [--keep]
# Defaults:
#   sample: samples/elanspi
#   board: 32f967_dv
#   keep: will pass -p always to west (clean build each time)

REPO_ROOT="$(cd "$(dirname "$0")" && pwd)"
VENV="$HOME/zephyrproject/.venv"
DEFAULT_SAMPLE="samples/elan_sha"
DEFAULT_BOARD="32f967_dv"
KEEP_FLAG="-p always"

SAMPLE="$DEFAULT_SAMPLE"
BOARD="$DEFAULT_BOARD"

# Parse simple args
while [[ $# -gt 0 ]]; do
  case "$1" in
    --sample)
      SAMPLE="$2"; shift 2;;
    --board)
      BOARD="$2"; shift 2;;
    --no-keep)
      KEEP_FLAG=""; shift;;
    --help|-h)
      cat <<EOF
Usage: $0 [--sample <path>] [--board <board>] [--no-keep]
Defaults: sample=${DEFAULT_SAMPLE}, board=${DEFAULT_BOARD}
Example: $0 --sample samples/elanspi --board 32f967_dv
EOF
      exit 0;;
    *)
      echo "Unknown arg: $1" >&2; exit 2;;
  esac
done

# Activate python venv if present
if [[ -r "$VENV/bin/activate" ]]; then
  # shellcheck disable=SC1090
  source "$VENV/bin/activate"
else
  echo "Warning: venv not found at $VENV. Make sure Zephyr python env is active." >&2
fi

# Export module dir so Kconfig rsource resolves
export ZEPHYR_ELAN_ZEPHYR_MODULE_DIR="$REPO_ROOT"
echo "ZEPHYR_ELAN_ZEPHYR_MODULE_DIR=$ZEPHYR_ELAN_ZEPHYR_MODULE_DIR"

cd "$REPO_ROOT"

# Run west build
echo "Running: west build ${KEEP_FLAG} -b ${BOARD} ${SAMPLE}"
west build ${KEEP_FLAG} -b "${BOARD}" "${SAMPLE}"

echo "Build finished."
