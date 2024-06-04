# P2P Messenger

## Install dependencies

```shell
sudo apt update
sudo apt install -y git build-essential gdb libcppunit-dev pkg-config cmake catch2 qtbase5-dev qtcreator qt5-qmake qt5-doc

# Install OpenDHT dependencies
sudo apt install -y libncurses5-dev libreadline-dev nettle-dev libgnutls28-dev libargon2-0-dev libmsgpack-dev  libssl-dev libfmt-dev libjsoncpp-dev libhttp-parser-dev libasio-dev
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
