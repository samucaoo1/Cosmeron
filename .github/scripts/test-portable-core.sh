#!/bin/sh
# Portable C11 regression suite: cc + POSIX shell only. No GNU make required.
# Deliberately uses the real module tests, not fabricated compile-only probes.
set -eu
CC=${CC:-cc}
export CC
mkdir -p .build-portable
printf '[portable] %s %s | compiler: %s\n' "$(uname -s)" "$(uname -m)" "$CC"

run_suite() {
  module=$1
  shift
  for test in "$@"; do
    echo "[portable] $module/$test"
    (
      cd "Codespace/Tests/$module"
      "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
        -D_POSIX_C_SOURCE=200809L "$test.c" \
        -o "../../../.build-portable/${module}_${test}"
      "../../../.build-portable/${module}_${test}"
    )
  done
}

run_suite Core Namespace Cast Preprocessor Detect Error Memory MemoryOperations MemoryUtilities Algorithm IncludeAll
run_suite Bit Bit Widths IncludeAll
run_suite Struct Struct Variants AllTypes IncludeAll
run_suite Type Type TypeNoFunctionTable AllFunctions IncludeAll FundamentalSmall
run_suite Math Math Arithmetic Value Equations BSDQuadraticRegression AllTypes IncludeAll
run_suite Random Random Engines Distribution IncludeAll
run_suite Chronometry Chronometry Duration Instant Calendar DateTime EpochTimezoneFormat ClockTimer IncludeAll
run_suite Container Functional LinearExtra LinearAllTypes TreeAllFunctions Hash Graph \
  FunctionTableOff TreeFunctionTableOff CustomType IncludeAll IncludeAllOff
echo "[portable] Container/MultiTU"
(
  cd Codespace/Tests/Container
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L \
    MultiTU_A.c MultiTU_B.c MultiTU_Main.c -o ../../../.build-portable/Container_MultiTU
  ../../../.build-portable/Container_MultiTU
)
run_suite Text IncludeAll Text Namespace Unicode AllFunctions

run_suite GUI Surface Interaction Editor Render Containers Failure InputText Window WindowNative Advanced IncludeAll Namespace FunctionTableOff
(
  cd Codespace/Tests/GUI
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L \
    MultiTU_A.c MultiTU_B.c MultiTU_Main.c -o ../../../.build-portable/GUI_MultiTU
  ../../../.build-portable/GUI_MultiTU
)
"$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L \
  Codespace/Examples/GUI/Lab.c -o .build-portable/GUI_Lab
.build-portable/GUI_Lab --ppm .build-portable/GUI_Lab.ppm

echo "[portable] Sort"
for test in Sort SortCatalog; do
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror -I Codespace \
    "Codespace/Tests/$test.c" -o ".build-portable/$test"
  ".build-portable/$test"
done
echo "[portable] all selected C11 suites passed"
