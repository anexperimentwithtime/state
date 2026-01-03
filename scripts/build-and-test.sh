# Copyright (C) 2025 Ian Torres <iantorres@outlook.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.

#!/usr/bin/env bash
set -euo pipefail

BUILD_TYPE=${1:-debug}
LINK_TYPE=${2:-static}

mkdir -p build
cd build

if [[ "$LINK_TYPE" == "static" ]]; then
ENABLE_STATIC="ON"
else
ENABLE_STATIC="OFF"
fi

if [[ "$BUILD_TYPE" == "debug" ]]; then
ENABLE_DEBUG="ON"
else
ENABLE_DEBUG="OFF"
fi
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DENABLE_TESTS=ON -DENABLE_CI=ON -DENABLE_DEBUG=$ENABLE_DEBUG -DENABLE_STATIC_LINKING=$ENABLE_STATIC

make -j4
ctest --output-on-failure --verbose

cd ..
if [[ "$BUILD_TYPE" == "debug" ]]; then
  gcovr --filter='src/' --print-summary --sort-percentage --gcov-ignore-parse-errors=negative_hits.warn
  gcovr --filter='src/' --gcov-ignore-parse-errors=negative_hits.warn --sonarqube > coverage.xml
fi