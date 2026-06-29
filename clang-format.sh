#!/usr/bin/env bash
# format every project c++ source in place using the repo .clang-format file.
# third party code under vendors and the build directory are skipped.

set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
style_file="${root_dir}/.clang-format"

# allow overriding the binary, e.g. CLANG_FORMAT=clang-format-22 to pin a version.
clang_format="${CLANG_FORMAT:-clang-format}"

if ! command -v "${clang_format}" > /dev/null 2>&1; then
    echo "clang-format.sh: ${clang_format} is not installed"
    exit 1
fi

if [ ! -f "${style_file}" ]; then
    echo "clang-format.sh: .clang-format not found at ${root_dir}"
    exit 1
fi

# collect the project sources, skip vendors, build, and .git.
# also skip configure_file templates (*.in.*) like config.in.hh, since clang-format
# mangles the @VAR@ substitution tokens (turns @FOO@ into @FOO @) and breaks cmake.
mapfile -d '' files < <(
    find "${root_dir}" \
        -type d \( -name vendors -o -name build -o -name .git \) -prune -o \
        -type f ! -name '*.in.*' \( -name '*.cc' -o -name '*.hh' -o -name '*.cpp' -o -name '*.hpp' -o -name '*.cxx' -o -name '*.hxx' \) -print0
)

if [ "${#files[@]}" -eq 0 ]; then
    echo "clang-format.sh: no source files found"
    exit 0
fi

echo "clang-format.sh: formatting ${#files[@]} files with ${style_file} using ${clang_format}"

"${clang_format}" -i --style="file:${style_file}" "${files[@]}"

echo "clang-format.sh: done"
