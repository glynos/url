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


class CodePointIter(object):

    def __init__(self, first, last):
        self.__it = first
        self.__last = last

    def __str__(self):
        return '0x{:04X}'.format(self.__it)

    def __next__(self):
        # if self.__it >= self.__last:
        #     raise StopIteration()
        self.__it += 1
        return self.__it


class CodePointRange(object):

    def __init__(self, range):
        if type(range) == str:
            range = range.split('..') if '..' in range else [range, range]
        if type(range[0]) == str:
            range = [int(range[0], 16), int(range[1], 16)]
        self.range = range
        self.__i = self.range[0]

    @property
    def condition(self):
        if self.range[0] == 0:
            return 'c <= 0x{:04X}'.format(self.range[1])
        elif self.range[0] == self.range[1]:
            return 'c == 0x{:04X}'.format(self.range[1])
        return '(c >= 0x{:04X}) && (c <= 0x{:04X})'.format(self.range[0], self.range[1])

    def __str__(self):
        return '{:04X}..{:04X}'.format(self.range[0], self.range[1])

    def __contains__(self, item):
        return item in range(range[0], self.range[1] + 1)

    def __len__(self):
        return self.range[1] - self.range[0] + 1

    def __iter__(self):
        for i in range(self.range[0], self.range[1]):
            yield CodePointIter(i, self.range[1])


def adjacent(first, last):
    return first.range[1] + 1 == last.range[0]


def squash(first, last):
    assert adjacent(first, last)
    return CodePointRange([first.range[0], last.range[1]])


class MappedCodePoint(object):

    def __init__(self, code_point):
        self.code_point = code_point

    @property
    def mapped(self):
        return '0x{}'.format(self.code_point[2]) if len(self.code_point) > 2 else self.code_point[0]


class CodePointTransform(object):

    def __init__(self, code_point):
        self.range = CodePointRange(code_point[0])
        self.status = code_point[1]
        self.__mapped = MappedCodePoint(code_point)

    @property
    def condition(self):
        return self.range.condition

    @property
    def mapped(self):
        return self.__mapped.mapped

    def __str__(self):
        return '{}'.format(self.range)


def squeeze(code_points):
    code_point_list = code_points.copy()
    for status, points in code_point_list.items():
        new_list = []
        for code_point in points:
            if not new_list:
                new_list.append(code_point)
                continue

            if adjacent(new_list[-1].range, code_point.range):
                r = squash(new_list[-1].range, code_point.range)
                new_list[-1] = CodePointTransform([r.range, status])
            else:
                new_list.append(code_point)
        code_point_list[status] = new_list
    return code_point_list


if __name__ == '__main__':
    input, output = sys.argv[1], sys.argv[2]

    entries = []
    code_points = {}
    with open(input, 'r') as input_file, open(output, 'w+') as output_file:
        for line in input_file.readlines():
            if line.startswith('#') or line == '\n':
                continue

            code_point = parse_line(line)
            entries.append(CodePointRange(code_point[0]))

            status = code_point[1]
            assert status in status_keys, line
            if status not in code_points.keys():
                code_points[status] = []

            code_points[status].append(CodePointTransform(code_point))

        code_point_list = squeeze(code_points)
        print(len(code_point_list))
        print('\n'.join(['{}: {}'.format(status, len(lst)) for status, lst in code_point_list.items()]))

        template_1 = jinja2.Template(
            """// Auto-generated.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "idna_table.hpp"

namespace skyr {
idna_status map_status(char32_t c, bool use_std3_ascii_rules) {
  if (
{% for code_point in statuses.ignored[:-1] %}    ({{ code_point.condition }}) ||
{% endfor %}    ({{ statuses.ignored[-1].condition }})) {
    return idna_status::ignored;
  } else if (
{% for code_point in statuses.mapped[:-1] %}    ({{ code_point.condition }}) ||
{% endfor %}    ({{ statuses.mapped[-1].condition }})) {
    return idna_status::mapped;
  } else if (
{% for code_point in statuses.disallowed[:-1] %}    ({{ code_point.condition }}) ||
{% endfor %}    ({{ statuses.disallowed[-1].condition }})) {
    return idna_status::disallowed;
  } else if (
{% for code_point in statuses.disallowed_STD3_valid[:-1] %}    ({{ code_point.condition }}) ||
{% endfor %}    ({{ statuses.disallowed_STD3_valid[-1].condition }})) {
    return use_std3_ascii_rules? idna_status::disallowed : idna_status::valid;
  } else if (
{% for code_point in statuses.disallowed_STD3_mapped[:-1] %}    ({{ code_point.condition }}) ||
{% endfor %}    ({{ statuses.disallowed_STD3_mapped[-1].condition }})) {
    return use_std3_ascii_rules? idna_status::disallowed : idna_status::mapped;
  } else if (
{% for code_point in statuses.deviation[:-1] %}    ({{ code_point.condition }}) ||
{% endfor %}    ({{ statuses.deviation[-1].condition }})) {
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

        template_2 = jinja2.Template(
            """// Auto-generated.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "idna_table.hpp"

namespace skyr {
namespace {
struct code_point {
  char32_t c;
  idna_status s;
  char32_t m;
};

static const code_point code_points[] = {
{% for code_point_range in entries %}{% for code_point in code_point_range %} { {{ code_point }}, idna_status::valid, {{ code_point }} },
{% endfor %}{% endfor %}
}
}  // namespace

idna_status map_status(char32_t c, bool use_std3_ascii_rules) {
  return idna_status::valid;
}

char32_t map(char32_t c) {
  return c;
}
}  // namespace skyr
""")

        template_1.stream(
            entries=entries,
            statuses=code_point_list,
            mapped=code_points['mapped'],
            disallowed_STD3_mapped=code_points['disallowed_STD3_mapped']).dump(output_file)
