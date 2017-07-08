# kafkamark

_kafkamark_ is a simple benchmark used to measure the performance of [Apache
kafka](https://kafka.apache.org).  The benchmark is written in C++ and uses the
[librdkafka](https://github.com/edenhill/librdkafka) client library.

## Dependencies
Required:
* The GNU toolchain
* GNU make
* [librdkafka v0.9.5](https://github.com/edenhill/librdkafka)
* [boost 1.55](http://www.boost.org)

## Vagrant

A [Vagrant](https://www.vagrantup.com) configuration is provided which defines
a debian jessie VM which includes all necessary build dependencies.  If Vagrant
is installed, use the following commands to build and enter the VM.

```
vagrant up
vagrant ssh
```
