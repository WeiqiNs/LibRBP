#!/bin/sh
status=0
for library in "$@"; do
    size=$(readelf -lW "$library" | awk '$1 == "TLS" {print $6}')
    size=$((${size:-0}))
    if [ "$size" -ge 4096 ]; then
        echo "$library reserves $size bytes of static thread-local storage in every thread"
        status=1
    fi
done
exit $status
