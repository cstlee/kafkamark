/* Copyright (c) 2017, Stanford University
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include <signal.h>

#include "PerfUtils/Cycles.h"
#include "PerfUtils/TimeTrace.h"

#include "KafkaClient.h"
#include "Payload.h"
#include "TraceLog.h"

using namespace Kafkamark;
using PerfUtils::Cycles;
using PerfUtils::TimeTrace;

/**
 * Signal whether or not the application should continue to run.
 */
static bool run = true;

/**
 * Custom signal handler for SIGINT to gracefully exit.
 */
void handle_sigint(int s) {
    run = false;
}

int
main(int argc, char const *argv[])
{
    KafkaClient client(KafkaClient::PRODUCER);

    double targetOPS;
    std::string logDir;

    // Get Command Line Options
    OptionsDescription options("Usage");
    options.add_options()
        ("help",
            "produce help message")
        ("logDir,L",
            ProgramOptions::value< std::string >(&logDir),
            "Destination log directory for log output.")
        ("throughput.ops",
            ProgramOptions::value< double >(&targetOPS)->default_value(0),
            "Operations per second the producer should attempt to offer "
            "(0 means there should be no throughput control).")
    ;
    client.addOptionsTo(options);

    // Configure and Init with Options
    ProgramOptions::variables_map variables;
    ProgramOptions::store(ProgramOptions::parse_command_line(argc,
                                                             argv,
                                                             options),
                          variables);
    ProgramOptions::notify(variables);

    if (variables.count("help")) {
        std::cout << options << std::endl;
        return 0;
    }

    if (variables.count("logDir")) {
        if (logDir.back() != '/') {
            logDir.append("/");
        }

        // TimeTrace Config
        std::string timeTraceOutName = logDir;
        timeTraceOutName.append("producer.timetrace.log");
        TimeTrace::setOutputFileName(timeTraceOutName.c_str());

        // TraceLog Config
        std::string traceLogPath = logDir;
        traceLogPath.append("producer.log");
        TraceLog::setOutputFilePath(traceLogPath.c_str());
    }

    client.configure(variables);

    // Set SIGING handler
    signal(SIGINT, handle_sigint);

    TraceLog::record("CPS|%f", Cycles::perSecond());

    // Compute delay
    uint64_t sendDelayTSC = 0;
    if (targetOPS > 0) {
        sendDelayTSC = PerfUtils::Cycles::fromSeconds(1.0 / targetOPS);
    }
    uint64_t nextSendTSC = PerfUtils::Cycles::rdtsc();
    char buf[100];
    Payload::Header* header = (Payload::Header*) buf;
    uint64_t msgId = 0;

    while (run) {
        header->msgId = ++msgId;
        header->timestampTSC = PerfUtils::Cycles::rdtsc();

        if (!client.produce(buf, 100)) {
            break;
        }

        // Log Send
        TraceLog::record(header->timestampTSC, "PRODUCE|%d",
                header->msgId);

        nextSendTSC += sendDelayTSC;
        // Throttle
        while (nextSendTSC > PerfUtils::Cycles::rdtsc());
    }

    TimeTrace::print();
    TraceLog::flush();

    return 0;
}
