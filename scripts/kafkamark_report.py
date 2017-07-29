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
usage: kafkamark report [options] <dirname>

options:
    -h --help
    -a --all           Print the entire report.
    -s --summary       Print report summary.
    -b --batching      Print the 'batching' section of the report.
    -l --latency       Print the 'latency' section of the report.
'''

def report(args):
    print_default = True

    if args['--summary'] or args['--all']:
        summary(args['<dirname>'])
        print_default = False

    if args['--latency'] or args['--all']:
        latency(args['<dirname>'])
        print_default = False

    if args['--batching'] or args['--all']:
        batching(args['<dirname>'])
        print_default = False

    if print_default:
        summary(args['<dirname>'])

def summary(dirname):
    # TODO(cstlee)
    print("Summary Not Available")

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

def batching(dirname):
    consumerLog = dirname + "/consumer.log"

    cps = None;
    startTime = None
    prevTime = 0
    delta = 0
    batchCount = None
    batchStartTSC = None

    batchDurations = []
    batchSizes = []

    with open(consumerLog, 'r') as logFile:
        for line in logFile:
            row = line.strip().split('|')
            if row[1] == 'CONSUME':
                tsc = int(row[0])
                batchStart = (tsc - prevTime > 2 * delta)
                if batchStart:
                    if batchCount is not None:
                        batchSizes.append(batchCount)
                        batchDurations.append(tsc - batchStartTSC)
                    batchCount = 0
                    batchStartTSC = tsc
                batchCount += 1
                delta = tsc - prevTime
                prevTime = tsc
            elif row[1] == 'CPS':
                cps = float(row[2])

    count = len(batchDurations)
    batchDurations.sort()
    batchDurations = [1000 * dur / cps for dur in batchDurations]
    batchSizes.sort()
    print("# Interval (msec)  Size (msg cnt)  Cum. Fraction\n"
          "#-----------------------------------------------")
    print("%15.2f    %10d      %8.3f" % (0.0, 0, 0.0))
    print("%15.2f    %10d      %8.3f" % (batchDurations[0], batchSizes[0], 1.0/count))
    for i in range(1, 100):
        print("%15.2f    %10d      %8.3f" % (batchDurations[int(count*i/100)], batchSizes[int(count*i/100)], i/100.))
    print("%15.2f    %10d      %8.3f" % (batchDurations[int(count*999/1000)], batchSizes[int(count*999/1000)], .999))
    print("%15.2f    %10d      %9.4f" % (batchDurations[int(count*9999/10000)], batchSizes[int(count*9999/10000)], .9999))
    print("%15.2f    %10d      %8.3f" % (batchDurations[-1], batchSizes[-1], 1.0))
