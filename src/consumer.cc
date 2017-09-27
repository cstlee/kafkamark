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
    KafkaClient client(KafkaClient::CONSUMER);

    std::string logDir;

    // Get Command Line Options
    OptionsDescription options("Usage");
    options.add_options()
        ("help",
            "produce help message")
        ("logDir,L",
            ProgramOptions::value< std::string >(&logDir),
            "Destination log directory for log output.")
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
        timeTraceOutName.append("consumer.timetrace.log");
        TimeTrace::setOutputFileName(timeTraceOutName.c_str());

        // TraceLog Config
        std::string traceLogPath = logDir;
        traceLogPath.append("consumer.log");
        TraceLog::setOutputFilePath(traceLogPath.c_str());
    }

    client.configure(variables);

    // Set SIGING handler
    signal(SIGINT, handle_sigint);

    TraceLog::record("CPS|%f", Cycles::perSecond());

    TimeTrace::record("INIT");
    TimeTrace::reset();

    // uint64_t firstNAtsc = 0;
    // int noMsgCnt = 0;

    // Run Workload
    while (run) {
        KafkaClient::Message msg;
        // uint64_t startTime = Cycles::rdtsc();
        if (!client.consume(&msg, 10000)) {
            uint64_t endTSC = Cycles::rdtsc();
            // TimeTrace::record(startTime, "Consumer: Get Message");
            TimeTrace::record(endTSC, "Consumer: No Message Received");
            TraceLog::record(endTSC, "CONSUME|No Message Received");
            // if (noMsgCnt == 0) {
            //     firstNAtsc = endTSC;
            // }
            // ++noMsgCnt;
        } else {
            uint64_t endTSC = Cycles::rdtsc();
            Payload::Header* header = (Payload::Header*) msg.payload;

            // TimeTrace::record(startTime, "Consumer: Get Message");
            TimeTrace::record(endTSC,
                    "Consumer: Message %4d Received in %9lu us",
                    header->msgId,
                    Cycles::toMicroseconds(endTSC - header->timestampTSC));
            TraceLog::record(endTSC,
                    "CONSUME|Message %4d Received in %9lu us",
                    header->msgId,
                    Cycles::toMicroseconds(endTSC - header->timestampTSC));

            // if (noMsgCnt > 0) {
            //     TimeTrace::record(firstNAtsc,
            //             "CONSUME| No Messages Received last %9d tries",
            //             noMsgCnt);
            //     noMsgCnt = 0;
            //     firstNAtsc = 0;
            // }
            //
            // TimeTrace::record(endTSC, "CONSUME| %4d : %6lu us",
            //         header->msgId,
            //         Cycles::toMicroseconds(endTSC - header->timestampTSC));

            // TimeTrace::record("Consumer: Done");
        }
    }

    TimeTrace::print();
    TraceLog::flush();

    return 0;
}
