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
usage: kafkamark run [options] <bindir>

options:
    -h, --help
    -r, --run-time <arg>        Duration of the experiment in seconds.
                                [default: 0]
    --pre-run <arg>             Command run before the benchmark runs.
    --post-run <arg>            Command run after the benchmark runs.
    -n, --dry-run               Show what will be run without actual exicution.
    --re-run <arg>              Re-run a previous execution using the options
                                from the provide param file.

general client options:
    -L, --logDir <arg>          Destination log directory for log output.
    -b, --brokers <arg>         Broker address
                                *Type: string*
    -t, --topic <arg>           Topic to fetch / produce
                                *Type: string*
    -g, --group.id <arg>        Client group id string. All clients sharing the
                                same group.id belong to the same group.
                                *Type: string*

consumer client options:
    --fetch.wait.max.ms <arg>           Maximum time the broker may wait to fill
                                        the response with fetch.min.bytes.
                                        *Type: integer*

producer client options:
    --throughput.ops <arg>              Operations per second the producer
                                        should attempt to offer.
                                        *Type: float*
    --queue.buffering.max.ms <arg>      Maximum time, in milliseconds, for
                                        buffering data on the producer queue.
                                        *Type: integer*
'''

import atexit
import pickle
import signal
import subprocess
import time

from kafkamark_filenames import PARAM_FILE

execute = True
consumer = None
producer = None
post_run_cmd = None

def run(args):
    global execute
    global consumer
    global producer
    global post_run_cmd

    bindir = args['<bindir>'].strip('/')

    if args['--dry-run']:
        execute = False

    if args['--re-run'] is not None:
        args = pickle.load(open(args['--re-run'], 'rb'))

    atexit.register(cleanup)

    consumer_cmd = "{0}/consumer".format(bindir)
    producer_cmd = "{0}/producer".format(bindir)

    # Add options
    generalOptions = getGeneralOptions(args)
    consumer_cmd += generalOptions
    producer_cmd += generalOptions
    consumer_cmd += getConsumerOptions(args)
    producer_cmd += getProducerOptions(args)

    post_run_cmd = args['--post-run']

    if args['--pre-run'] is not None:
        print_log("running pre-run command")
        print_log(args['--pre-run'])
        if execute:
            subprocess.call(args['--pre-run'], shell=True)

    if args['--logDir'] is not None and execute:
        filePath = "{0}/{1}".format(args['--logDir'].strip('/'), PARAM_FILE)
        pickle.dump(args, open(filePath, 'wb'))

    print_log("starting consumer...")
    print_log(consumer_cmd)
    if execute:
        consumer = subprocess.Popen(consumer_cmd.split())

    if execute:
        time.sleep(1)

    print_log("starting producer...")
    print_log(producer_cmd)
    if execute:
        producer = subprocess.Popen(producer_cmd.split())

    print_log("run timer start")

    if execute and producer.poll() is None and consumer.poll() is None:
        time.sleep(int(args['--run-time']))

    print_log("run complete")

    if execute and producer.poll() is None:
        producer.send_signal(signal.SIGINT)
    if execute and consumer.poll() is None:
        consumer.send_signal(signal.SIGINT)

    print_log("wait for output flush")
    if execute:
        time.sleep(1)

    cleanup()
    post_run_cmd = None

def getGeneralOptions(args):
    options = ''
    options += getOption(args, '--logDir')
    options += getOption(args, '--brokers')
    options += getOption(args, '--topic')
    options += getOption(args, '--group.id')
    return options

def getConsumerOptions(args):
    options = ''
    options += getOption(args, '--fetch.wait.max.ms')
    return options

def getProducerOptions(args):
    options = ''
    options += getOption(args, '--throughput.ops')
    options += getOption(args, '--queue.buffering.max.ms')
    return options

def getOption(args, optionName):
    option = ''
    if args[optionName] is not None:
        option += ' {0} {1}'.format(optionName, args[optionName])
    return option

def print_log(msg):
    print("[ {0} ] {1}".format(
            time.strftime("%d %b %Y %H:%M:%S", time.localtime()),
            msg))

def cleanup():
    global execute
    global consumer
    global producer
    global post_run_cmd

    if producer is not None:
        if producer.poll() is None:
            producer.kill()
            print_log("producer ({0}) killed".format(producer.pid))

    if consumer is not None:
        if consumer.poll() is None:
            consumer.kill()
            print_log("consumer ({0}) killed".format(consumer.pid))

    if post_run_cmd is not None:
        print_log("running post-run command")
        print_log(post_run_cmd)
        if execute:
            subprocess.call(post_run_cmd, shell=True)
