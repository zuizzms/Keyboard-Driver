#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define IOCTL_TEST _IOR(0, 6, struct ioctl_test_t)

int main () {

  /* attribute structures */
  struct ioctl_test_t {
    char field2;
  } ioctl_test;  // "int i"

  int fd = open ("/proc/ioctl_test", O_RDONLY);
  int call;
  while(1) {
    call = ioctl (fd, IOCTL_TEST, &ioctl_test); // infinitely call
    if (call == 0) {
       printf("Character Received: %x\n", ioctl_test.field2);
     }
    else {
      printf("ioctl calls completed.");
      break;
    }
    
  }


  return 0;
}


