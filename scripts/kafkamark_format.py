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
usage: kafkamark format [options] <filename>

options:
    -h, --help
    --batching
    --timetrace
'''

def format(args):
    if args['--batching']:
        batching(args['<filename>'])
    if args['--timetrace']:
        ttformat(args['<filename>'])

def batching(filename):
    cps = None;
    startTime = None
    ns = 0.0
    count = 0
    with open(filename, 'r') as logFile:
        for line in logFile:
            row = line.strip().split('|')
            if row[1] == 'CONSUME':
                hasMessage = (row[2].find('No Message') < 0)
                if count < 0 and hasMessage:
                    print("%8.1f ns (+%6.1f ns): "
                          "No Messages Recieved in %d attempts" % (ns, ns - prevTime, -count))
                    count = 0
                if count > 0 and not hasMessage:
                    print("%8.1f ns (+%6.1f ns): "
                          "%d Message Recieved" % (ns, ns - prevTime, count))
                    count = 0
                if count == 0:
                    prevTime = ns
                    ns = (1e9 * float(row[0]) / cps) - startTime
                if hasMessage:
                    count += 1
                else:
                    count -= 1
            elif row[1] == 'CPS':
                cps = float(row[2])
                startTime = 1e9 * float(row[0]) / cps

def ttformat(filename):
    cps = None;
    startTime = None
    prevTime = 0.0
    with open(filename, 'r') as logFile:
        for line in logFile:
            row = line.strip().split('|')
            if row[1] == 'CPS':
                cps = float(row[2])
                startTime = 1e9 * float(row[0]) / cps
            else:
                ns = (1e9 * float(row[0]) / cps) - startTime
                print("%8.1f ns (+%6.1f ns): %s : %s" % (ns, ns - prevTime, row[1], row[2]))
                prevTime = ns
