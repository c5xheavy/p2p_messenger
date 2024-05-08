# P2P Messenger

## Install dependencies

```shell
sudo apt update
sudo apt install git
sudo apt install build-essential gdb
sudo apt install cmake
sudo apt install catch2
sudo apt install qtbase5-dev qtcreator qt5-qmake qt5-doc
sudo apt install qt6-base-dev qtcreator
sudo apt-get install libcppunit-dev

# Install OpenDHT dependencies
sudo apt install libncurses5-dev libreadline-dev nettle-dev libgnutls28-dev libargon2-0-dev libmsgpack-dev  libssl-dev libfmt-dev libjsoncpp-dev libhttp-parser-dev libasio-dev

## Install python binding dependencies
#sudo apt-get install cython3 python3-dev python3-setuptools

## Build & install restinio (for proxy server/client):
#mkdir restinio && cd restinio \
#    && wget https://github.com/aberaud/restinio/archive/2224ffedef52cb2b74645d63d871d61dbd0f165e.tar.gz \
#    && ls -l && tar -xzf 2224ffedef52cb2b74645d63d871d61dbd0f165e.tar.gz \
#    && cd restinio-2224ffedef52cb2b74645d63d871d61dbd0f165e/dev \
#    && cmake -DCMAKE_INSTALL_PREFIX=/usr -DRESTINIO_TEST=OFF -DRESTINIO_SAMPLE=OFF \
#             -DRESTINIO_INSTALL_SAMPLES=OFF -DRESTINIO_BENCH=OFF -DRESTINIO_INSTALL_BENCHES=OFF \
#             -DRESTINIO_FIND_DEPS=ON -DRESTINIO_ALLOW_SOBJECTIZER=Off -DRESTINIO_USE_BOOST_ASIO=none . \
#    && make -j8 && make install \
#    && cd ../../ && rm -rf restinio
```

## Build

### Build Boost

```shell
wget https://boostorg.jfrog.io/artifactory/main/release/1.85.0/source/boost_1_85_0.tar.bz2
tar --bzip2 -xf boost_1_85_0.tar.bz2
cd boost_1_85_0
./bootstrap.sh
sudo ./b2 install
```

### Build OpenDHT

```shell
# clone the repo
git clone https://github.com/savoirfairelinux/opendht.git

# build and install
cd opendht
mkdir build && cd build
cmake -DOPENDHT_PYTHON=OFF -DCMAKE_INSTALL_PREFIX=/usr ..
make -j4
sudo make install
```

### Build P2P Messenger

```shell
git clone https://github.com/c5xheavy/p2p_messenger.git
mkdir build && cd build
cmake ..
cmake --build .
```