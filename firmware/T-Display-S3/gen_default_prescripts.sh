#!/usr/bin/env bash
# Usage: gen_default_prescripts.sh SOURCE.txt > PrescriptBeeper/default_prescripts.h
set -euo pipefail
src=${1:-prescriptexamples/RienPrescripts.txt}
echo "#ifndef DEFAULT_PRESCRIPTS_H"
echo "#define DEFAULT_PRESCRIPTS_H"
echo ""
echo "// Generated from $(basename "$src") by gen_default_prescripts.sh. Do not edit by hand."
echo "static const char DEFAULT_PRESCRIPTS[] PROGMEM ="
grep -v '^[[:space:]]*#' "$src" | grep -v '^[[:space:]]*$' | sed 's/\r$//; s/\\/\\\\/g; s/"/\\"/g; s/^/  "/; s/$/\\n"/'
echo "  ;"
echo ""
echo "#endif"
