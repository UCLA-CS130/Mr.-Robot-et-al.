# Essential tooling for compilation
sudo apt-get update
sudo apt-get install -y build-essential
sudo apt-get install -y libboost-all-dev
sudo apt-get install -y make
sudo apt-get install -y g++

# This gives us the apt-get-repository command,
# needed for Python 3.6, which has new language features
# used in our integration tests
sudo apt-get install -y python-software-properties
sudo apt-get install -y software-properties-common
sudo add-apt-repository -y ppa:jonathonf/python-3.6
sudo apt-get update
sudo apt-get install -y python3.6
sudo PY_PATH=$(which python3.6)
sudo ln -sf $PY_PATH /usr/bin/python3

# lcov lets us check test coverage
# HTTPie generates HTTP requests for integration tests
sudo apt-get install -y lcov
sudo apt-get install -y httpie

