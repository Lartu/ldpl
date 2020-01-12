#!/usr/bin/env bash
$(awk -f src/lib-to-string.awk src/ldpl_lib.cpp > src/ldpl_included_lib.cpp)
exit 0
