#!/usr/bin/env bash
set -eEuo pipefail

if [[ ! -f tools/v6fs/v6fs ]]; then
  make -C tools/v6fs
fi

cleanup() {
  umount fs
  git restore fs
}
trap cleanup EXIT

git rm -qrf --ignore-unmatch fs
rm -rf fs
mkdir fs

tools/v6fs/v6fs -f -r disk.rk fs &
sleep 1

git -c core.ignorecase=false add -f disk.rk fs
if ! git diff --quiet --staged; then
  git commit -qm "Filesystem snapshot $(date +'%Y-%m-%d %H:%M')"
fi
