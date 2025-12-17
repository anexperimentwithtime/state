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

find . \( -iname '*.cpp' -o -iname '*.cc' -o -iname '*.cxx' \
         -o -iname '*.hpp' -o -iname '*.h' -o -iname '*.ipp' \) \
     -type f -print0 |
{
  failed=0
  while IFS= read -r -d '' file; do
    if ! clang-format --dry-run --Werror "$file"; then
      failed=1
      printf 'clang-format falló en: %s\n' "$file" >&2
    fi
  done

  if [[ "$failed" -ne 0 ]]; then
    exit 1
  fi
}

if [[ -f main.cpp ]] && ! clang-format --dry-run --Werror main.cpp; then
  exit 1
fi