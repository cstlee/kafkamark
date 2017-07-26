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

#include "Payload.h"
#include "PerfUtils/Cycles.h"

using namespace Kafkamark;
using PerfUtils::Cycles;

int
main(int argc, char const *argv[])
{
    KafkaClient client(KafkaClient::PRODUCER);

    double targetOPS;

    // Get Command Line Options
    OptionsDescription options("Usage");
    options.add_options()
        ("help",
            "produce help message")
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

    client.configure(variables);

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
        snprintf(buf + sizeof(Payload::Header), 100, "Message ID #");

        header->timestampTSC = PerfUtils::Cycles::rdtsc();

        if (!client.produce(buf, 100)) {
            break;
        }

        nextSendTSC += sendDelayTSC;
        // Throttle
        while (nextSendTSC > PerfUtils::Cycles::rdtsc());
    }

    return 0;
}
