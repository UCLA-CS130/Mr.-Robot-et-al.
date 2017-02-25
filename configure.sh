sudo add-apt-repository -y ppa:jonathonf/python-3.6
sudo apt-get -y update
sudo apt-get install build-essential
sudo apt-get -y install python3.6
PATH=$(which python3.6)
ln -s $PATH /usr/local/bin/python3.6
sudo apt-get install libboost-all-dev
sudo apt-get install lcov
sudo apt-get install httpie
