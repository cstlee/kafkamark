#!/usr/bin/env bash

apt-get -q update

# Install build tools
apt-get -q install -y build-essential

# Install rkt
echo "Downloading rkt"
wget -q https://github.com/rkt/rkt/releases/download/v1.27.0/rkt_1.27.0-1_amd64.deb
sudo dpkg -i rkt_1.27.0-1_amd64.deb
rm rkt_1.27.0-1_amd64.deb
echo "rkt installation complete"

# Install acbuild
echo "Downloading acbuild"
wget -q https://github.com/containers/build/releases/download/v0.4.0/acbuild-v0.4.0.tar.gz
tar -xzf acbuild-v0.4.0.tar.gz -C /usr/local/bin/ --strip-components=1
rm acbuild-v0.4.0.tar.gz
echo "acbuild installation complete"
