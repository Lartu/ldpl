#!/bin/bash
command=`find src -regex ".*\.\(cpp\|h\)" -exec clang-format -Werror -style=Google -n {} \; 2> /tmp/ldpl-find`
count=`cat /tmp/ldpl-find | wc -c`
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color
if [[ $count  -eq 0  ]]
then
	printf "${GREEN}Its OK${NC}\n"
else
	printf "${RED}Need formatter${NC}\n"
	printf "RUN ./apply_formatter.sh\n"
fi
