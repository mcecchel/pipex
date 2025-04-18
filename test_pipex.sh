#!/bin/bash

# Colori per l'output
GREEN="\033[1;32m"
RED="\033[1;31m"
NC="\033[0m"

# Parameters
INFILE="infile"
MY_OUTPUT="my_output"
REF_OUTPUT="ref_output"

# Prende il comando come argomento
CMD1="$1"
CMD2="$2"

echo -e "\n🔧 Testing with:"
echo "  CMD1: $CMD1"
echo "  CMD2: $CMD2"

# Mio pipex
./pipex "$INFILE" "$CMD1" "$CMD2" "$MY_OUTPUT"

# Shell pipe
< "$INFILE" $CMD1 | $CMD2 > "$REF_OUTPUT"

# Comparazione output
if diff "$MY_OUTPUT" "$REF_OUTPUT" > /dev/null; then
    echo -e "${GREEN}✅  Output corretto${NC}"
else
    echo -e "${RED}❌  Output diverso${NC}"
    echo "--- Diff ---"
    diff "$MY_OUTPUT" "$REF_OUTPUT"
    echo "-------------"
fi
