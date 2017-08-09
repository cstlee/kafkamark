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

'''
usage: kafkamark <command> [<args>...]

options:
    -h, --help

available commands:
    format      Print a raw log in a human-readable format.
    report      Generate a benchmark report from the benchmark logs.
    run         Run benchmark.
    sweep       Commands benchmarking over a range of configurations.

See 'kafkamark <command> --help' for more information on a specific command.

'''

from docopt import docopt

if __name__ == '__main__':
    args = docopt(__doc__,
                  version='kafkamark 0.1.0',
                  options_first=True)

    argv = [args['<command>']] + args['<args>']

    if args['<command>'] == 'format':
        import kafkamark_format
        args = docopt(kafkamark_format.__doc__, argv=argv)
        kafkamark_format.format(args)
    elif args['<command>'] == 'report':
        import kafkamark_report
        args = docopt(kafkamark_report.__doc__, argv=argv)
        kafkamark_report.report(args)
    elif args['<command>'] == 'run':
        import kafkamark_run
        args = docopt(kafkamark_run.__doc__, argv=argv)
        kafkamark_run.run(args)
    elif args['<command>'] == 'sweep':
        import kafkamark_sweep
        kafkamark_sweep.sweep(argv)
    else:
        exit("%r is not a kafkamark.py command. "
             "See 'kafkamark --help'." % args['<command>'])
