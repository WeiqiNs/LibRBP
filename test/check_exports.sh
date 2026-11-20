#!/bin/sh
status=0
for library in "$@"; do
    label=$(basename "$library" | sed 's/^libRBP_//; s/\.so.*//')
    relic_prefixes="^(${label}_)?(bn|dv|fp[0-9]*|ep[0-9]*|g1|g2|gt|pc|pp|core|rand|md|err|arch|util)_"
    leaked=$(nm -D --defined-only "$library" | awk '{print $3}' | grep -E "$relic_prefixes")
    if [ -n "$leaked" ]; then
        echo "$library exports RELIC symbols:"
        echo "$leaked" | head -20
        status=1
    fi
done
exit $status
