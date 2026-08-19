#!/usr/bin/env bash
set -eEuo pipefail

date="$(date +'%Y-%m-%d %H:%M:%S')"
message_date="${date%:*}"
cp -a disk.rk disk.rk.mnt

if [[ ! -f tools/v6fs/v6fs ]]; then
  make -C tools/v6fs
fi

cleanup() {
  umount mnt
  rmdir mnt
  rm disk.rk.mnt
  git restore fs
}
trap cleanup EXIT

git rm -qrf --ignore-unmatch fs
rm -rf fs mnt
mkdir mnt

tools/v6fs/v6fs -f -r disk.rk.mnt mnt &
sleep 1

mkdir fs
find mnt -mindepth 1 -print0 |
while IFS= read -r -d '' src; do
  dest="fs/${src#mnt/}"
  if [[ -d "$src" ]]; then
    mkdir -p "$dest"
  elif [[ -f "$src" || -L "$src" ]]; then
    mkdir -p "${dest%/*}"
    cp -Pp "$src" "$dest"
  else # devices
    mkdir -p "${dest%/*}"
    touch -r "$src" "$dest"
  fi
done

git -c core.ignorecase=false add -f disk.rk fs logs
if ! git diff --quiet --staged; then
  GIT_AUTHOR_DATE="$date" GIT_COMMITTER_DATE="$date" \
  git commit -qm "Filesystem snapshot $message_date"
fi
