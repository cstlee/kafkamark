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
usage:
    kafkamark report [options] <dirname>

options:
    -h, --help
    -f, --force         Force the report to generate from the raw data
    -s, --summary       Print a summaries instead of full reports.
    -q, --quiet         Don't output to standard out.
    -b, --batching      Print the 'batching' section of the report.
    -l, --latency       Print the 'latency' section of the report.
    --clean             Cleanup and remove gnerated ouput files.
'''

import numpy as np
import os

from docopt import docopt

from kafkamark_filenames import LATENCY_DATA_FILE
from kafkamark_filenames import BATCH_INTERVAL_FILE
from kafkamark_filenames import BATCH_SIZE_FILE

def report(argv):
    args = docopt(__doc__, argv=argv)
    if args['--clean']:
        report_clean(args)
    else:
        report_main(args)

def report_main(args):
    if not args['--batching'] and not args['--latency']:
        full_report = True
    else:
        full_report = False

    if args['--latency'] or full_report:
        latency(args['<dirname>'],
                args['--force'],
                args['--summary'],
                args['--quiet'])

    if args['--batching'] or full_report:
        batching(args['<dirname>'],
                 args['--force'],
                 args['--summary'],
                 args['--quiet'])

def report_clean(args):
    dirname = args['<dirname>'].strip('/') + '/'
    files = ( LATENCY_DATA_FILE
            , BATCH_INTERVAL_FILE
            , BATCH_SIZE_FILE)
    for filename in files:
        filepath = dirname + filename
        if os.path.exists(filepath):
        	try:
        		os.remove(filepath)
        	except OSError, e:
        		print("Error: {0} - {1}.".format(e.filename, e.strerror))

def cdf_write(numbers, headers, fileName):
    numbers.sort()
    with open(fileName, 'w') as dataFile:
        dataFile.write(headers)
        count = len(numbers)
        for i in xrange(count):
            dataFile.write("%10.3f    %9.4f\n" % (numbers[i], float(i + 1)/count))

def cat(filename):
    with open(filename, 'r') as dataFile:
        print(dataFile.read())

def printSummary(filename, prefix, unit):
    numbers = []
    with open(filename, 'r') as f:
        for line in f.readlines():
            if line[0] == '#':
                continue
            data = line.split()
            numbers.append(float(data[0]))
    numbers.sort()
    count = len(numbers)
    print("{0:20} {1:>15} {2}".format(prefix + "", numbers[int(np.floor(0.5 * count))], unit))
    print("{0:20} {1:>15} {2}".format(prefix + ".min", numbers[0], unit))
    print("{0:20} {1:>15} {2}".format(prefix + ".9", numbers[int(np.floor(0.9 * count))], unit))
    print("{0:20} {1:>15} {2}".format(prefix + ".99", numbers[int(np.floor(0.99 * count))], unit))
    print("{0:20} {1:>15} {2}".format(prefix + ".999", numbers[int(np.floor(0.999 * count))], unit))

def latency(dirname, force, summary, quiet):
    consumerLog = dirname + "/consumer.log"
    numbers = []

    latencyData = dirname + "/" + LATENCY_DATA_FILE

    if force or not os.path.isfile(latencyData):
        with open(consumerLog, 'r') as logFile:
            for line in logFile:
                row = line.strip().split('|')
                if row[1] == 'CONSUME':
                    numbers.append(float(row[3]) / 1000)

        header = ("# Time (ms)    Cum. Fraction\n"
                 "#---------------------------\n")

        cdf_write(numbers, header, latencyData)

    if not quiet:
        if summary:
            printSummary(latencyData, 'latency', 'ms')
        else:
            cat(latencyData)

def batching(dirname, force, summary, quiet):
    consumerLog = dirname + "/consumer.log"

    cps = None;
    startTime = None
    prevTime = 0
    delta = 0
    batchCount = None
    batchStartTSC = None

    batchDurationsMS = []
    batchSizes = []

    durationData = dirname + "/" + BATCH_INTERVAL_FILE
    sizeData = dirname + "/" + BATCH_SIZE_FILE

    if force or not os.path.isfile(durationData) or not os.path.isfile(sizeData):
        with open(consumerLog, 'r') as logFile:
            for line in logFile:
                row = line.strip().split('|')
                if row[1] == 'CONSUME':
                    tsc = int(row[0])
                    batchStart = (tsc - prevTime > 2 * delta)
                    if batchStart:
                        if batchCount is not None:
                            batchSizes.append(batchCount)
                            batchDurationsMS.append(1000 * (tsc - batchStartTSC) / cps)
                        batchCount = 0
                        batchStartTSC = tsc
                    batchCount += 1
                    delta = tsc - prevTime
                    prevTime = tsc
                elif row[1] == 'CPS':
                    cps = float(row[2])

    if force or not os.path.isfile(durationData):
        header =  ("# Interval (ms)  Cum. Fraction\n"
                 "#---------------------------\n")
        cdf_write(batchDurationsMS, header, durationData)

    if force or not os.path.isfile(sizeData):
        header =  ("# Size (msg cnt)  Cum. Fraction\n"
                 "#---------------------------\n")
        cdf_write(batchSizes, header, sizeData)

    if not quiet:
        if summary:
            printSummary(durationData, 'batch.interval', 'ms')
            printSummary(sizeData, 'batch.size', '')
        else:
            cat(durationData)
            cat(sizeData)
