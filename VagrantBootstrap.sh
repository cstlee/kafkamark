#!/usr/bin/env bash

apt-get -q update

# Install build tools
apt-get -q install -y build-essential

# Install librdkafka v0.9.5
echo "Downloading librdkafka"
pushd /tmp/
wget -q https://github.com/edenhill/librdkafka/archive/v0.9.5.tar.gz
tar -xzf v0.9.5.tar.gz
echo "Installing librdkafka"
pushd librdkafka-0.9.5
./configure
make
make install
popd
rm -rf v0.9.5.tar.gz librdkafka-0.9.5
popd
echo "Finished installing librdkafka"

# Install boost 1.55
apt-get -q install -y libboost1.55-all-dev

# Install rkt
echo "Downloading rkt"
pushd /tmp/
wget -q https://github.com/rkt/rkt/releases/download/v1.27.0/rkt_1.27.0-1_amd64.deb
sudo dpkg -i rkt_1.27.0-1_amd64.deb
rm rkt_1.27.0-1_amd64.deb
popd
echo "Finished installing rkt"

# Install acbuild
echo "Downloading acbuild"
pushd /tmp/
wget -q https://github.com/containers/build/releases/download/v0.4.0/acbuild-v0.4.0.tar.gz
tar -xzf acbuild-v0.4.0.tar.gz -C /usr/local/bin/ --strip-components=1
rm acbuild-v0.4.0.tar.gz
popd
echo "Finished installing acbuild"
