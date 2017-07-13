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

#ifndef KAFKAMARK_KAFKACLIENT_H
#define KAFKAMARK_KAFKACLIENT_H

#include <boost/program_options.hpp>
#include <librdkafka/rdkafkacpp.h>

namespace Kafkamark {

/// See boost::program_options, just a synonym for that namespace.
namespace ProgramOptions {
    using namespace boost::program_options; // NOLINT
}
/// See boost::program_options::options_description, just a type synonym.
typedef ProgramOptions::options_description OptionsDescription;

/**
 * The KafkaClient encapsulates a librdkafaka based client to setup, configure,
 * and expose a simple notification interface that can be benchmarked.
 */
class KafkaClient {
  public:
    /**
     * Encapsulates the command line options available for a KafkaClient.
     */
    class Options {
      public:
        explicit Options(ProgramOptions::variables_map* vars);
        ~Options(){}

        void addTo(OptionsDescription& options);

      private:
        /// Pointer to configured option variables.
        ProgramOptions::variables_map* vars;

        /// Options available to all Kafka clients.
        OptionsDescription generalOptions;

        /// Options available to Kafka consumers.
        OptionsDescription consumerOptions;

        /// Options available to Kafka producers.
        OptionsDescription producerOptions;

        // Let KafkaClient directly access the variables.
        friend KafkaClient;
    };

    explicit KafkaClient(Options& options);
    ~KafkaClient();

    bool produce(char* msg, size_t len);
  private:
    /// Configuration for the client library.
    RdKafka::Conf *conf;

    /// Configuration for the topic.
    RdKafka::Conf *tconf;

    /// Handle to Kafka producer client.
    RdKafka::Producer *producer;

    /// Handle to Kafka topic.
    RdKafka::Topic *topic;

    bool setConfig(Options& options, const char* optionName);
};

}  // namespace Kafkamark

#endif  // KAFKAMARK_KAFKACLIENT_H
