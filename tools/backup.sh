#!/usr/bin/env bash
set -ux

#host="${TELNET_HOST:-localhost}"
#port="${TELNET_PORT:-4030}"
interval="${INTERVAL:-60}"

while true; do
  date +'%Y-%m-%d %H:%M:%S'
  #printf 'root\rsync\004' | telnet "$host" "$port"
  tools/extract_fs.sh
  git push
  sleep "$interval"
done
