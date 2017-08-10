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
    kafkamark sweep <command> [<args>...]

available commands:
    help        Print usage information.
    run         Run a set of benchmarks with a varying parameter.
    plot        Plot a benchmark sweep.
'''

__run_usage = '''
usage: kafkamark sweep run <output_dir>
                           --param <arg>
                           --param-start <arg>
                           --param-end <arg>
                           [options]
                           [-- <args>...]

options:
    -h, --help              Print usage information.
    --param <arg>           Name of the parameter that should be varied.
    --param-start <arg>     First parameter value in the sweep range.
    --param-end <arg>       Last parameter value in the sweep range.
    --param-step <arg>      Parameter value increment. [default: 1]
'''

__plot_usage = '''
usage: kafkamark sweep plot <input_dirs>...
                            --param <arg>
                            ( --latency
                            | --batch-interval
                            | --batch-size )

options:
    -h, --help              Print usage information.
    --param <arg>           Name of the parameter that should be plotted.
'''

import os
import pickle

from docopt import docopt

from kafkamark_filenames import LATENCY_DATA_FILE
from kafkamark_filenames import BATCH_INTERVAL_FILE
from kafkamark_filenames import BATCH_SIZE_FILE
from kafkamark_filenames import PARAM_FILE

def sweep(argv):
    args = docopt(__doc__, argv=argv, options_first=True)
    argv = ['sweep', args['<command>']] + args['<args>']
    if args['<command>'] == 'run':
        args = docopt(__run_usage, argv=argv)
        sweep_run(args)
    elif args['<command>'] == 'plot':
        args = docopt(__plot_usage, argv=argv)
        sweep_plot(args)
    elif args['<command>'] in ('help', '-h', '--help'):
        print(__doc__.strip("\n"))
    else:
        exit("%r is not a 'kafkamark sweep' command. "
             "See 'kafkamark sweep help'." % args['<command>'])

def sweep_run(args):
    import kafkamark_run
    argv = ['run'] + args['<args>']
    value = int(args['--param-start'])
    while value <= int(args['--param-end']):
        run_argv = list(argv)
        # Add param
        run_argv += ['--' + args['--param'], str(value)]
        # Add logDir
        logDir = args['<output_dir>'].strip('/') + '/' + str(value) + '/'
        if not os.path.exists(logDir):
            os.makedirs(logDir)
        run_argv += ['--logDir', logDir]

        run_args = docopt(kafkamark_run.__doc__, argv=run_argv)

        print("###### {0} = {1} ######".format(args['--param'], value))

        kafkamark_run.run(run_args)

        value += int(args['--param-step'])

def sweep_plot(args):
    import kafkamark_report
    import matplotlib.pyplot as plt
    import numpy as np
    x = []
    y50 = []
    y90 = []
    y99 = []
    for dirname in args['<input_dirs>']:
        dirname = dirname.strip('/') + '/'
        # Get Param value
        with open(dirname + PARAM_FILE, 'rb') as paramFile:
            params = pickle.load(paramFile)
        param_value = params['--' + args['--param']]
        x.append(int(param_value))

        # Generate Reports
        kafkamark_report.report(['report', dirname, '-s'])

        # Get Data
        if args['--latency']:
            numbers = getNumbers(dirname + LATENCY_DATA_FILE)
        elif args['--batch-interval']:
            pass
        elif args['--batch-size']:
            pass

        numbers.sort()
        count = len(numbers)
        y50.append(numbers[int(np.ceil(0.5 * count))])
        y90.append(numbers[int(np.ceil(0.9 * count))])
        y99.append(numbers[int(np.ceil(0.99 * count))])

    index = np.argsort(x)
    x = [x[i] for i in index]
    y50 = [y50[i] for i in index]
    y90 = [y90[i] for i in index]
    y99 = [y99[i] for i in index]

    plt.plot(x, y50)
    plt.plot(x, y90)
    plt.plot(x, y99)
    plt.show()

def getNumbers(filename):
    numbers = []
    with open(filename, 'r') as f:
        for line in f.readlines():
            if line[0] == '#':
                continue
            data = line.split()
            numbers.append(float(data[0]))
    return numbers
