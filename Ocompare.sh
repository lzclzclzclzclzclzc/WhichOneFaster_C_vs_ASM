#!/bin/bash
SRC="Ocompare.c"
OUTPUT="insert"
OPT_LEVELS=("-O0" "-O1" "-O2" "-O3" "-Ofast")

for OPT in "${OPT_LEVELS[@]}"; do
  echo "Compiling: gcc $OPT $SRC -o ${OUTPUT}_${OPT}"
  gcc $OPT $SRC -o "${OUTPUT}_${OPT}" || exit 1

  echo "Running: ${OUTPUT}_${OPT}"
  OPT_LEVEL="${OPT#-}" ./"${OUTPUT}_${OPT}"
done

echo "All tests completed."

python Ocompare.py
