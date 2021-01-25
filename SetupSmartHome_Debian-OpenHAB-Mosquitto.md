# SETUP START HOME SERVER (Debian + openHAB + Mosquitto)

## INSTALL VBOX GUEST ADDITIONS (solo su VirtualBox)

- Log in to the guest machine as root or sudo user , and install the packages required for building kernel modules:
	- sudo apt update
	- sudo apt install build-essential dkms linux-headers-$(uname -r)
- From the virtual machine menu, click Devices -> “Insert Guest Additions CD Image”
- Open the Debian guest terminal, create a new directory , and mount the ISO file:
	- sudo mkdir -p /mnt/cdrom
	- sudo mount /dev/cdrom /mnt/cdrom
- Navigate to the directory and execute the VBoxLinuxAdditions.run script to install the Guest Additions:
	- cd /mnt/cdrom
	- sudo sh ./VBoxLinuxAdditions.run --nox11
- Reboot the Debian guest for changes to take effect:
	- sudo shutdown -r now

## INSTALL JAVA 11

- To install the OpenJDK version of Java, first update your apt package index:
	- sudo apt update
- Then use the apt install command to install OpenJDK:
	- sudo apt install default-jre
- Verify the installation with:
	- java -version
- You’ll see the following output:
	- openjdk version "11.0.4" 2019-07-16
	  OpenJDK Runtime Environment (build 11.0.4+11-post-Debian-1deb10u1)
	  OpenJDK 64-Bit Server VM (build 11.0.4+11-post-Debian-1deb10u1, mixed mode, sharing)
- You may also need the Java Development Kit (JDK) in order to compile and run some specific Java-based software. To install the JDK, execute the following command:
	- sudo apt install default-jdk
- Verify that the JDK is installed by checking the version of javac, the Java compiler:
	- javac -version
- You’ll see the following output:
	- javac 11.0.4

## INSTALL OPENHAB 3 (Package Repository Installation https://www.openhab.org/docs/installation/linux.html)

- First, add the openHAB Bintray repository key to your package manager and allow Apt to use the HTTPS Protocol:
	- wget -qO - 'https://bintray.com/user/downloadSubjectPublicKey?username=openhab' | sudo apt-key add -
	- sudo apt-get install apt-transport-https
- Add the openHAB Stable Repository to your systems apt sources list:
	- echo 'deb https://dl.bintray.com/openhab/apt-repo2 stable main' | sudo tee /etc/apt/sources.list.d/openhab.list
- Next, resynchronize the package index:
	- sudo apt-get update
- Now install openHAB with the following command:
	- sudo apt-get install openhab
- If everything went well, you can start openHAB and register it to be automatically executed at system startup:
	- sudo systemctl daemon-reload
	- sudo systemctl enable openhab.service
	- sudo systemctl start openhab.service
	- sudo systemctl status openhab.service
- You should be able to reach the openHAB Dashboard at http://'IP address':8080 at this point.

## INSTALL MOSQUITTO

- First, log in using your non-root user and update the package lists using apt update:
	- sudo apt update
- Now, install Mosquitto using apt install:
	- sudo apt install mosquitto mosquitto-clients
- Log in to your server a second time, so you have two terminals side-by-side. In the new terminal, use mosquitto_sub to subscribe to the test topic:
	- mosquitto_sub -h localhost -t test
- Switch back to your other terminal and publish a message:
	- mosquitto_pub -h localhost -t test -m "hello world"