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
     * Flag indicating the
     */
    enum Mode {
        CONSUMER = 1 << 0,
        PRODUCER = 1 << 1,
    };

    /**
     * Encapsulates the command line options available for a KafkaClient.
     */
    class Options {
      public:
        explicit Options(ProgramOptions::variables_map* vars, Mode mode);
        ~Options(){}

        void addTo(OptionsDescription& options);

      private:
        /// Pointer to configured option variables.
        ProgramOptions::variables_map* vars;

        /// Options available to all Kafka clients.
        OptionsDescription generalOptions;

        /// Indicates whether consumer functionality is requested.
        bool isConsumer;

        /// Options available to Kafka consumers.
        OptionsDescription consumerOptions;

        /// Indicates whether producer functionality is requested.
        bool isProducer;

        /// Options available to Kafka producers.
        OptionsDescription producerOptions;

        // Let KafkaClient directly access the variables.
        friend KafkaClient;
    };

    /**
     * Encapsulates a Kafka Message mostly to implement scope based memory
     * management.
     */
    class Message {
      public:
        Message()
            : payload(NULL)
            , len(0)
            , message()
        {}

        ~Message()
        {
            if (message)
                delete message;
        }

        /// Points to the message payload.
        void* payload;

        /// Length of the message payload.
        size_t len;

      private:
        /// Pointer to a message
        RdKafka::Message* message;

        // Let KafkaClient directly access the variables
        friend KafkaClient;
    };

    explicit KafkaClient(Options& options);
    ~KafkaClient();

    void configure(ProgramOptions::variables_map& variables);

    bool consume(Message* msg, int timeout_ms);
    bool produce(char* msg, size_t len);

  private:
    /// Available client configuration options.
    Options options;

    /// Configuration for the client library.
    RdKafka::Conf *conf;

    /// Configuration for the topic.
    RdKafka::Conf *tconf;

    /// Handle to Kafka consumer client
    RdKafka::KafkaConsumer *consumer;

    /// Handle to Kafka producer client.
    RdKafka::Producer *producer;

    /// Handle to Kafka topic.
    RdKafka::Topic *topic;

    bool setConfig(ProgramOptions::variables_map& variables,
            const char* optionName);
};

/**
 * Bitwise or operator for KafkaClient::Mode.
 */
inline KafkaClient::Mode operator|(KafkaClient::Mode a, KafkaClient::Mode b)
{
    return static_cast<KafkaClient::Mode>(static_cast<int>(a) |
                                          static_cast<int>(b));
}

}  // namespace Kafkamark

#endif  // KAFKAMARK_KAFKACLIENT_H
