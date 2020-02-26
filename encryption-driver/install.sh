echo "[@] Verifying kernel linux-headers..."
sudo apt update
sudo apt install linux-headers-$(uname -r)

echo "[@] Building driver..."
make

chmod +x rot47

echo "[@] Creating driver node..."
sudo mknod /dev/encrypt c 62 0
sudo chmod 666 /dev/encrypt

echo "[@] Installing driver..."
sudo rmmod encrypt
sudo insmod encrypt.ko

echo "[@] Complete."