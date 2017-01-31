sudo add-apt-repository ppa:jonathonf/python-3.6
sudo apt-get update
sudo apt-get install python3.6
PATH=$(which python3.6)
ln -s $PATH /usr/local/bin/python3.6