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

namespace Kafkamark {

/**
 * Constructor for KafkaClient::Options.
 *
 * \param vars
 *      Pointer to the variables map which will contain the configured option
 *      variables.
 */
KafkaClient::Options::Options(ProgramOptions::variables_map* vars)
    : vars(vars)
    , generalOptions("Kafka General Options")
    , producerOptions("Kafka Producer Options")
    , consumerOptions("Kafka Consumer Options")
{
    generalOptions.add_options()
        ("brokers,b",
                ProgramOptions::value< std::string >(),
                "Broker address")
        ("topic,t",
                ProgramOptions::value< std::string >(),
                "Topic to fetch / produce")
    ;

    consumerOptions.add_options()
        ("fetch.wait.max.ms",
                ProgramOptions::value< std::string >(),
                "Maximum time the broker may wait to fill the response with "
                "fetch.min.bytes. *Type: integer*")
    ;

    producerOptions.add_options()
        ("queue.buffering.max.ms",
                ProgramOptions::value< std::string >(),
                "Maximum time, in milliseconds, for buffering data on the "
                "producer queue. *Type: integer*")
    ;
}

/**
 * Adds the available Kafka options to the provided OptionsDescription.
 */
void
KafkaClient::Options::addTo(OptionsDescription& options)
{
    options.add(generalOptions);
    options.add(consumerOptions);
    options.add(producerOptions);
}

/**
 * Construct a KafkaClient object with the provided options.
 */
KafkaClient::KafkaClient(KafkaClient::Options& options)
    : conf()
    , tconf()
    , producer()
    , topic()
{
    std::string errstr;
    std::string topic_str;

    // Create kafka configuration
    conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    // General configuration
    if (options.vars->count("brokers")) {
        conf->set("metadata.broker.list",
                options.vars->at("brokers").as<std::string>(),
                errstr);
    } else {
        std::cerr << "Couldn't construct client: No brokers list provided."
                  << std::endl;
        std::cerr << options.generalOptions << std::endl;
        exit(1);
    }

    if (options.vars->count("topic")) {
        topic_str = options.vars->at("topic").as<std::string>();
    } else {
        std::cerr << "Couldn't construct client: No topic provided."
                  << std::endl;
        std::cerr << options.generalOptions << std::endl;
        exit(1);
    }

    // Consumer configuration
    setConfig(options, "fetch.wait.max.ms");

    // Producer configuration
    setConfig(options, "queue.buffering.max.ms");

    // Create producer
    producer = RdKafka::Producer::create(conf, errstr);
    if (!producer) {
         std::cerr << "Failed to create producer: " << errstr << std::endl;
         exit(1);
    }

    // Create topic handle
    topic = RdKafka::Topic::create(producer, topic_str, tconf, errstr);
    if (!topic) {
      std::cerr << "Failed to create topic: " << errstr << std::endl;
      exit(1);
    }
}

/**
 * KafkaClient Destructor
 */
KafkaClient::~KafkaClient()
{
    producer->flush(10*1000);
    delete topic;
    delete producer;
}

/**
 * Produce the provided message to the configured Kafka topic.
 *
 * \parma msg
 *      Message that should be published to the client's configured topic.
 * \param len
 *      Length of the message to be published.
 * \return
 *      True, if the messages produced without error.  False, otherwise.
 */
bool
KafkaClient::produce(char* msg, size_t len)
{
    int partition = 0;
    RdKafka::ErrorCode resp = producer->produce(topic, partition,
                                                RdKafka::Producer::RK_MSG_BLOCK,
                                                msg, len,
                                                NULL, NULL);
    if (resp != RdKafka::ERR_NO_ERROR) {
        std::cerr << "% Produce failed: "
                  << RdKafka::err2str(resp)
                  << std::endl;
        return false;
    }
    return true;
}

/**
 * Helper function to set the client library configuration based on provided
 * option values.
 *
 * \parma options
 *      Contains the option value that should be set.
 * \param optionName
 *      Name of the client library config option that should be set.
 * \return
 *      True, if the option was set. False, otherwise.
 */
bool
KafkaClient::setConfig(Options& options, const char* optionName)
{
    std::string errstr;
    if (options.vars->count(optionName)) {
        conf->set(optionName,
                options.vars->at(optionName).as<std::string>(),
                errstr);
        if (errstr == "") {
            return true;
        } else {
            std::cerr << errstr << std::endl;
        }
    }
    return false;
}

}   // namespace Kafkamark
