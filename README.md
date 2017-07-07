# kafkamark

*kafkamark* is a simple benchmark used to measure the performance of [Apache
kafka](https://kafka.apache.org).  The benchmark is written in C++ and uses the
[librdkafka](https://github.com/edenhill/librdkafka) client library.

## Dependencies
Required:
* [librdkafka v0.9.5](https://github.com/edenhill/librdkafka)
* [boost 1.55](http://www.boost.org)

Optional:
* [Vagrant](https://www.vagrantup.com)

## Vagrant

A [Vagrant](https://www.vagrantup.com) configuration is provided which defines
a debian jessie VM which includes all necessary build dependencies.  If Vagrant
is installed, use the following commands to build and enter the VM.

```
vagrant up
vagrant ssh
```
