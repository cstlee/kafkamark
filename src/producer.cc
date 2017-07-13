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

using namespace Kafkamark;

int
main(int argc, char const *argv[])
{
    // Get Command Line Options
    OptionsDescription options("Usage");
    options.add_options()
        ("help",
            "produce help message")
    ;
    ProgramOptions::variables_map variables;

    KafkaClient::Options kafkaClientOptions(&variables);
    kafkaClientOptions.addTo(options);

    // Configure and Init with Options
    ProgramOptions::store(ProgramOptions::parse_command_line(argc,
                                                             argv,
                                                             options),
                          variables);
    ProgramOptions::notify(variables);

    if (variables.count("help")) {
        std::cout << options << std::endl;
        return 0;
    }

    KafkaClient client(kafkaClientOptions);

    char msg[100] = "Hello World";
    // Run Workload
    for (int i = 0; i < 100; ++i) {
        if (!client.produce(msg, 100)) {
            break;
        }
    }

    /* code */
    return 0;
}
