# Copyright 2018 Glyn Matthews.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# This script parses the IDNA table from
# https://unicode.org/Public/idna/11.0.0/IdnaMappingTable.txt,
# and converts it to a C++ table.


import sys
import jinja2


def parse_line(line):
    tokens = [token.strip() for token in line.split(';')]
    if tokens and '#' in tokens[-1]:
        tokens[-1] = tokens[-1][0:tokens[-1].find('#')].strip()
        if tokens[-1] == '':
            tokens = tokens[:-1]
    if len(tokens) == 3:
        tokens[2] = tokens[2].split()[0]
    return tokens


status_keys = [
    'valid',
    'mapped',
    'disallowed',
    'disallowed_STD3_valid',
    'disallowed_STD3_mapped',
    'ignored',
    'deviation',
    ]


class CodePointRange(object):

    def __init__(self, range, status, mapped):
        if type(range) == str:
            range = range.split('..') if '..' in range else [range, range]
        if type(range[0]) == str:
            range = [int(range[0], 16), int(range[1], 16)]
        self.range = range
        self.__i = self.range[0]
        self.status = status
        self.mapped = int(mapped, 16) if mapped else None


def squeeze(code_points):
    code_points_copy = [code_points[0]]
    for index in range(1, len(code_points)):
        if code_points_copy[-1].status == code_points[index].status:
            code_points_copy[-1].range[1] = code_points[index].range[1]
        else:
            code_points_copy.append((code_points[index]))
    return code_points_copy


if __name__ == '__main__':
    input, output = sys.argv[1], sys.argv[2]

    code_points = []
    with open(input, 'r') as input_file, open(output, 'w+') as output_file:
        for line in input_file.readlines():
            if line.startswith('#') or line == '\n':
                continue

            code_point = parse_line(line)
            code_points.append(CodePointRange(
                code_point[0], code_point[1], code_point[2] if len(code_point) > 2 else None))

        mapped_code_points = [
            entry for entry in code_points if entry.status in ('mapped', 'disallowed_STD3_mapped')]
        code_points = squeeze(code_points)

        template = jinja2.Template(
            """// Auto-generated.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include "idna_table.hpp"

namespace skyr {
namespace {
struct code_point {
  char32_t first;
  char32_t last;
  idna_status status;
  
  bool operator < (const code_point &other) const {
    return last < other.first;
  }
};

static const code_point code_points[] = {
{% for code_point in entries %}  { 0x{{ '%04x' % code_point.range[0] }}, 0x{{ '%04x' % code_point.range[1] }}, idna_status::{{ code_point.status.lower() }} },
{% endfor %}};
}  // namespace

idna_status map_status(char32_t c) {
  auto first = std::addressof(code_points[0]);
  auto last = first + sizeof(code_points);
  auto it = std::find_if(
    first, last,
    [&c] (const code_point &cp) -> bool {
      return (c >= cp.first) && (c <= cp.last);
    });
  return it->status;
}

namespace {
struct mapped_code_point {
  char32_t code_point;
  char32_t mapped;
  
  bool operator < (const mapped_code_point &other) const {
    return code_point < other.code_point;
  }
};

static const mapped_code_point mapped[] = {
{% for code_point in mapped_entries %}{% if code_point.status in ('mapped', 'disallowed_STD3_mapped') %}  { 0x{{ '%04x' % code_point.range[0] }}, 0x{{ '%04x' % code_point.mapped }} },
{% endif %}{% endfor %}};
}  // namespace

char32_t map(char32_t c) {
  auto first = std::addressof(mapped[0]);
  auto last = first + sizeof(mapped);
  auto it = std::find_if(
    first, last,
    [&c](const mapped_code_point &cp) -> bool {
      return c == cp.code_point;
    });
  if (it != last) {
    return it->mapped;
  }
  return c;
}
}  // namespace skyr
""")

        template.stream(
            entries=code_points,
            mapped_entries=mapped_code_points).dump(output_file)
