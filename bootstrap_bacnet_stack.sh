#!/usr/bin/env sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
REVISION=$(tr -d '\r\n[:space:]' < "$ROOT/BACNET_STACK_REVISION")
REPOSITORY=https://github.com/bacnet-stack/bacnet-stack
VENDOR="$ROOT/vendor/bacnet-stack"

case "$REVISION" in
  ''|*[!0-9a-fA-F]*) echo "Invalid BACNET_STACK_REVISION: $REVISION" >&2; exit 1 ;;
esac

if [ -e "$VENDOR" ]; then
  if [ ! -d "$VENDOR/.git" ]; then
    echo "$VENDOR exists but is not a Git checkout; move it aside and retry." >&2
    exit 1
  fi
  REMOTE=$(git -C "$VENDOR" remote get-url origin 2>/dev/null || true)
  if [ "$REMOTE" != "$REPOSITORY" ]; then
    echo "Unexpected upstream remote: $REMOTE" >&2
    exit 1
  fi
else
  mkdir -p "$ROOT/vendor"
  git init "$VENDOR" >/dev/null
  git -C "$VENDOR" remote add origin "$REPOSITORY"
fi

git -C "$VENDOR" fetch --depth 1 origin "$REVISION"
git -C "$VENDOR" -c advice.detachedHead=false checkout --detach "$REVISION"
ACTUAL=$(git -C "$VENDOR" rev-parse HEAD)
if [ "$ACTUAL" != "$REVISION" ]; then
  echo "bacnet-stack revision mismatch: expected $REVISION, got $ACTUAL" >&2
  exit 1
fi
printf 'bacnet-stack pinned at %s\n' "$ACTUAL"
