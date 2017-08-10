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

import os

from docopt import docopt

def sweep(argv):
    args = docopt(__doc__, argv=argv, options_first=True)
    argv = ['sweep', args['<command>']] + args['<args>']
    if args['<command>'] == 'run':
        args = docopt(__run_usage, argv=argv)
        sweep_run(args)
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
