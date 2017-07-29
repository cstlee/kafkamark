# ISC License
#
# Copyright (c) 2017, Stanford University
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
# OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

'''
usage: kafkamark format <filename>

options:
    -h, --help
'''

def format(args):
    ttformat(args['<filename>'])

def ttformat(filename):
    cps = None;
    startTime = None
    prevTime = 0.0
    with open(filename, 'r') as logFile:
        for line in logFile:
            row = line.strip().split('|')
            if row[1] == 'CONSUME':
                ns = (1e9 * float(row[0]) / cps) - startTime
                print("%8.1f ns (+%6.1f ns): %s" % (ns, ns - prevTime, row[3]))
                prevTime = ns
            elif row[1] == 'CPS':
                cps = float(row[2])
                startTime = 1e9 * float(row[0]) / cps
