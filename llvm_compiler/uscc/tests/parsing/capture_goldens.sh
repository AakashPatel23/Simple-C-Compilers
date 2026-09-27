#!/bin/bash
# Temporary, one-time-use script: regenerates expected/*.ast and expected/*.err
# golden files from the current (fixed, verified) compiler's actual output,
# for every test currently active in testParse.py. Delete after running.
set -e
cd "$(dirname "$0")"

capture_ast() {
  local name="$1"
  ../../bin/uscc -a "${name}.usc" > "expected/${name}.ast" 2>&1 || true
}

capture_err() {
  local name="$1"
  ../../bin/uscc -a "${name}.usc" > "expected/${name}.err" 2>&1 || true
}

capture_ast test002
capture_ast test004
capture_ast test005
capture_ast test006
capture_ast test008
capture_ast test009
capture_ast test010
capture_ast test013
capture_ast quicksort
capture_err parse03e
capture_err parse06e
capture_ast test020
capture_ast test021
capture_ast test022
capture_ast test023
capture_ast test024
capture_ast test025
capture_ast test026
capture_ast test027
capture_ast test028
capture_ast test029

echo "Golden files captured. Now run: python2 testParse.py"
