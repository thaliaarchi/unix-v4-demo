#!/usr/bin/env bash
set -eEuo pipefail

date="$(date +'%Y-%m-%d %H:%M:%S')"
message_date="${date%:*}"
git add disk.rk source.rk
cp -a disk.rk disk.rk.mnt
cp -a source.rk source.rk.mnt

if [[ ! -f tools/v6fs/v6fs ]]; then
  make -C tools/v6fs
fi

cleanup() {
  umount source.mnt
  umount disk.mnt
  rmdir disk.mnt source.mnt 
  rm disk.rk.mnt source.rk.mnt 
}
trap cleanup EXIT

git rm -qrf --ignore-unmatch fs
rm -rf fs disk.mnt source.mnt
mkdir disk.mnt source.mnt

tools/v6fs/v6fs -f -r --no-same-owner --regular-dev disk.rk.mnt disk.mnt &
tools/v6fs/v6fs -f -r --no-same-owner --regular-dev source.rk.mnt source.mnt &
sleep 1
cp -a disk.mnt fs
rm -rf fs/usr/source
cp -a source.mnt fs/usr/source

git -c core.ignorecase=false add -f fs logs
if ! git diff --quiet --staged; then
  GIT_AUTHOR_DATE="$date" GIT_COMMITTER_DATE="$date" \
  git commit -qm "Filesystem snapshot $message_date"
fi
