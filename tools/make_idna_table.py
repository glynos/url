# Copyright 2018 Glyn Matthews.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# This script arses the IDNA table from
# https://unicode.org/Public/idna/11.0.0/IdnaMappingTable.txt, and convert it to a C++ table.


import sys
import jinja2


def tidy_line(line):
    if line and '#' in line[-1]:
        line[-1] = line[-1][0:line[-1].find('#')].strip()
        if line[-1] == '':
            line = line[:-1]
    if len(line) == 3:
        line[2] = line[2].split()[0]
    return line


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

    def __init__(self, range):
        if type(range) == str:
            self.range = range.split('..') if '..' in range else [range, range]
        elif type(range) == list:
            self.range = range

    @property
    def condition(self):
        if self.range[0] == '0000':
            return 'c <= 0x{}'.format(self.range[1])
        elif self.range[0] == self.range[1]:
            return 'c == 0x{}'.format(self.range[1])
        return '(c >= 0x{}) && (c <= 0x{})'.format(self.range[0], self.range[1])


def adjacent(first, last):
    return int(first.range[1] + 1) == int(last.range[0])


def squash(first, last):
    assert adjacent(first, last)
    return CodePointRange(first.range[0], last.range[1])


class CodePointMapped(object):

    def __init__(self, code_point):
        self.code_point = code_point

    @property
    def mapped(self):
        return '0x{}'.format(self.code_point[2]) if len(self.code_point) > 2 else self.code_point[0]


class CodePointTransform(object):

    def __init__(self, code_point):
        self.__condition = CodePointRange(code_point[0])
        self.status = code_point[1]
        self.__mapped = CodePointMapped(code_point)

    @property
    def condition(self):
        return self.__condition.condition

    @property
    def mapped(self):
        return self.__mapped.mapped


if __name__ == '__main__':
    input, output = sys.argv[1], sys.argv[2]

    code_points = {}
    with open(input, 'r') as input_file, open(output, 'w+') as output_file:
        for line in input_file.readlines():
            if line.startswith('#') or line == '\n':
                continue

            code_point = tidy_line([token.strip() for token in line.split(';')])

            status = code_point[1]
            assert status in status_keys, line
            if status not in code_points.keys():
                code_points[status] = []

            code_points[status].append(CodePointTransform(code_point))

        template = jinja2.Template(
"""// Auto-generated.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "idna_table.hpp"

namespace skyr {
idna_status map_status(char32_t c, bool use_std3_ascii_rules) {
  if (
{% for code_point in ignored[:-1] %}    ({{ code_point.condition }}) ||
{% endfor %}    ({{ ignored[-1].condition }})) {
    return idna_status::ignored;
  } else if (
{% for code_point in mapped[:-1] %}    ({{ code_point.condition }}) ||
{% endfor %}    ({{ mapped[-1].condition }})) {
    return idna_status::mapped;
  } else if (
{% for code_point in disallowed[:-1] %}    ({{ code_point.condition }}) ||
{% endfor %}    ({{ disallowed[-1].condition }})) {
    return idna_status::disallowed;
  } else if (
{% for code_point in disallowed_STD3_valid[:-1] %}    ({{ code_point.condition }}) ||
{% endfor %}    ({{ disallowed_STD3_valid[-1].condition }})) {
    return use_std3_ascii_rules? idna_status::disallowed : idna_status::valid;
  } else if (
{% for code_point in disallowed_STD3_mapped[:-1] %}    ({{ code_point.condition }}) ||
{% endfor %}    ({{ disallowed_STD3_mapped[-1].condition }})) {
    return use_std3_ascii_rules? idna_status::disallowed : idna_status::mapped;
  } else if (
{% for code_point in deviation[:-1] %}    ({{ code_point.condition }}) ||
{% endfor %}    ({{ deviation[-1].condition }})) {
    return idna_status::deviation;
  }
  return idna_status::valid;
}

char32_t map(char32_t c) {
  if ({{ mapped[0].condition }}) {
    return {{ mapped[0].mapped }};
  }{% for code_point in mapped[1:] %} else if ({{ code_point.condition }}) {
    return {{ code_point.mapped }};
  }{% endfor %} else if ({{ disallowed_STD3_mapped[0].condition }}) {
    return {{ disallowed_STD3_mapped[0].mapped }};
  }{% for code_point in disallowed_STD3_mapped[1:] %} else if ({{ code_point.condition }}) {
    return {{ code_point.mapped }};
  }{% endfor %}
  return c;
}
}  // namespace skyr
""")
        template.stream(code_points).dump(output_file)
