# Keyboard-Driver
Disables the native system's keyboard driver and implements my own keyboard module

Steps:
1. Compile module:
make -C /lib/modules/$(uname -r)/build M=$PWD modules
2. Insert the module:
make load
3. Make the executable:
gcc -o ioctl_test ioctl_test.c
4. Run the file:
./ioctl_test
5. Type away! This interrupt-driven solution supports handling for typing characters, SHIFT
pressing, and backspace
6. Ctrl C to exit
Source Used:
https://www.science.smith.edu/~nhowe/262/oldlabs/keyboard.html
https://cs-people.bu.edu/njavro/classes/CS552/Labs/Lab2/sol/
https://tldp.org/LDP/lkmpg/2.6/html/x1256.html#AEN1320
https://elixir.bootlin.com/linux/v2.6.33.3/C/ident/
