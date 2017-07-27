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

#include "KafkaClient.h"

#include <signal.h>

#include "PerfUtils/Cycles.h"
#include "PerfUtils/TimeTrace.h"

#include "Payload.h"
#include "TraceLog.h"

using namespace Kafkamark;
using PerfUtils::Cycles;
using PerfUtils::TimeTrace;

/**
 * Custom signal handler for SIGINT so that TimeTrace statements are printed
 * before exiting.
 */
void handle_sigint(int s) {
    std::cout << std::endl;
    TimeTrace::print();
    TraceLog::flush();
    exit(1);
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

    // TODO(cstlee): TimeTrace currently requires the output string's lifetime
    //               be longer than any call to print.  Once TimeTrace is fixed,
    //               the timeTraceOutName variable can be moved to a more
    //               reasonable place like in the if statement.
    std::string timeTraceOutName = logDir;
    if (variables.count("logDir")) {
        timeTraceOutName.append("TimeTrace.log");
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

    while (true) {
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

    return 0;
}
