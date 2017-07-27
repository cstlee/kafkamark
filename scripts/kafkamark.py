#!/usr/bin/env python

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

'''Benchmark for Apache Kafka

Usage:
    kafkamark.py latency <dir>
'''

from docopt import docopt

def latency(dirname):
    consumerLog = dirname + "/consumer.log"
    numbers = []

    with open(consumerLog, 'r') as logFile:
        for line in logFile:
            row = line.strip().split('|')
            if row[1] == 'CONSUME':
                numbers.append(float(row[3]) / 1000)

    numbers.sort()
    print("# Time (msec)  Cum. Fraction\n"
          "#---------------------------")
    print("%10.2f    %8.3f" % (0.0, 0.0))
    print("%10.2f    %8.3f" % (numbers[0], 1.0/len(numbers)))
    for i in range(1, 100):
        print("%10.2f    %8.3f" % (numbers[int(len(numbers)*i/100)], i/100.))
    print("%10.2f    %8.3f" % (numbers[int(len(numbers)*999/1000)], .999))
    print("%10.2f    %9.4f" % (numbers[int(len(numbers)*9999/10000)], .9999))
    print("%10.2f    %8.3f" % (numbers[-1], 1.0))

if __name__ == '__main__':
    args = docopt(__doc__, version='cluster.py 0.0.1')

    if args['latency']:
        latency(args['<dir>'])
