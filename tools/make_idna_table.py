# Copyright 2018 Glyn Matthews.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

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


class CodePoint(object):

    def __init__(self, code_point):
        self.code_point = code_point

    @property
    def condition(self):
        code_point = self.code_point[0]
        if '..' in code_point:
            f, t = code_point.split('..')
            return '(c >= 0x{}) && (c <= 0x{})'.format(f, t) if f != '0000' else 'c <= 0x{}'.format(t)
        return 'c == 0x{}'.format(code_point)

    @property
    def mapped(self):
        return '0x{}'.format(self.code_point[1])


if __name__ == '__main__':
    input, output = sys.argv[1], sys.argv[2]

    code_points = {}
    with open(input, 'r') as input_file, open(output, 'w+') as output_file:
        counter = 0
        for line in input_file.readlines():
            if line.startswith('#') or line == '\n':
                continue

            line_data = tidy_line([token.strip() for token in line.split(';')])
            counter += 1

            status = line_data[1]
            assert status in status_keys, line
            if status not in code_points.keys():
                code_points[status] = []
            del line_data[1]
            code_points[status].append(CodePoint(line_data))

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
