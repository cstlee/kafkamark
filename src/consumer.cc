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

#include "Payload.h"
#include "PerfUtils/Cycles.h"
#include "PerfUtils/TimeTrace.h"

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
    exit(1);
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

    // TODO(cstlee): TimeTrace currently requires the output string's lifetime
    //               be longer than any call to print.  Once TimeTrace is fixed,
    //               the timeTraceOutName variable can be moved to a more
    //               reasonable place like in the if statement. 
    std::string timeTraceOutName = logDir;
    if (variables.count("logDir")) {
        timeTraceOutName.append("TimeTrace.log");
        TimeTrace::setOutputFileName(timeTraceOutName.c_str());
    }

    client.configure(variables);

    // Set SIGING handler
    signal(SIGINT, handle_sigint);

    // Run Workload
    while (true) {
        KafkaClient::Message msg;
        uint64_t startTime = Cycles::rdtsc();
        if (!client.consume(&msg, 1000)) {
            // Nothing to do, just wait.
        } else {
            TimeTrace::record(startTime, "Consumer: Get Message");
            TimeTrace::record("Consumer: Message Received");
            uint64_t endTSC = Cycles::rdtsc();
            char* payload = (char*) msg.payload;
            Payload::Header* header = (Payload::Header*) payload;
            TimeTrace::record("Consumer: Print Payload");
            std::cout << Cycles::toMicroseconds(endTSC - header->timestampTSC)
                    << ": "
                    << payload + sizeof(Payload::Header)
                    << header->msgId
                    << std::endl;
            TimeTrace::record("Consumer: Done");
        }
    }

    /* code */
    return 0;
}
