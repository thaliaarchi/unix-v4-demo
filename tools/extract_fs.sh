#!/usr/bin/env bash
set -eEuo pipefail

date="$(date +'%Y-%m-%d %H:%M:%S')"
message_date="${date%:*}"
git add disk.rk
cp -a disk.rk disk.rk.mnt

if [[ ! -f tools/v6fs/v6fs ]]; then
  make -C tools/v6fs
fi

cleanup() {
  umount mnt
  rmdir mnt
  rm disk.rk.mnt
}
trap cleanup EXIT

git rm -qrf --ignore-unmatch fs
rm -rf fs mnt
mkdir mnt

tools/v6fs/v6fs -f -r --no-same-owner --regular-dev disk.rk.mnt mnt &
sleep 1
cp -a mnt fs

git -c core.ignorecase=false add -f fs logs
if ! git diff --quiet --staged; then
  GIT_AUTHOR_DATE="$date" GIT_COMMITTER_DATE="$date" \
  git commit -qm "Filesystem snapshot $message_date"
fi
