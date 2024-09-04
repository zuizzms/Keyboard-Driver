/*
 *  ioctl test module --
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h> /* error codes */
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/tty.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");

/* attribute structures */
struct ioctl_test_t {
  char field2;
};

// ioctl command that prints a message to the kernel log
// _IOW is used to create a custom ioctl command (changed to IOR to read)
#define IOCTL_TEST _IOR(0, 6, struct ioctl_test_t)

static int pseduo_device_read(struct file *, char __user *, size_t, loff_t *);

static int pseudo_device_ioctl(struct inode *inode, struct file *file,
			       unsigned int cmd, unsigned long arg);

static struct file_operations pseudo_dev_proc_operations;

static struct proc_dir_entry *proc_entry;

// initializes file operations and sets up the '/proc' entry
static int __init initialization_routine(void) {
  printk("<1> Loading module\n");

  pseudo_dev_proc_operations.ioctl = pseudo_device_ioctl;

  /* Start create proc entry */
  proc_entry = create_proc_entry("ioctl_test", 0444, NULL);
  if(!proc_entry)
  {
    printk("<1> Error creating /proc entry.\n");
    return 1;
  }

  //proc_entry->owner = THIS_MODULE; <-- This is now deprecated
  proc_entry->proc_fops = &pseudo_dev_proc_operations;

  return 0;
}

/* 'printk' version that prints to active tty (terminal). */
void my_printk(char *string)
{
  struct tty_struct *my_tty;

  my_tty = current->signal->tty;

  if (my_tty != NULL) {
    (*my_tty->driver->ops->write)(my_tty, string, strlen(string));
    (*my_tty->driver->ops->write)(my_tty, "\015\012", 2);
  }
} 

// removes the '/proc' entry when module is unloaded
static void __exit cleanup_routine(void) {

  printk("<1> Dumping module\n");
  remove_proc_entry("ioctl_test", NULL);

  return;
}

// inb function definition
static inline unsigned char inb( unsigned short usPort ) {
    unsigned char uch;
    asm volatile( "inb %1,%0" : "=a" (uch) : "Nd" (usPort) );
    return uch;
}

// outb function definition
static inline void outb( unsigned char uch, unsigned short usPort ) {
    asm volatile( "outb %0,%1" : : "a" (uch), "Nd" (usPort) );
}

char my_getchar ( void ) {
  char c;
  static char scancode[128] = "\0\e1234567890-=\177\tqwertyuiop[]\n\0asdfghjkl;'`\0\\zxcvbnm,./\0*\0 \0\0\0\0\0\0\0\0\0\0\0\0\000789-456+1230.\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
  /* Poll keyboard status register at port 0x64 checking bit 0 to see if
   * output buffer is full. We continue to poll if the msb of port 0x60
   * (data port) is set, as this indicates out-of-band data or a release
   * keystroke
   */
  // below while loop causes crash
  // while( !(inb( 0x64 ) & 0x1) || ( ( c = inb( 0x60 ) ) & 0x80 ) );

/*
my get char
check if output buffer is full (if status register's right most bit being 1)
two scan codes: one when you press, one when u release (we need press)
 --> release indicated by left most bit of data register being 1 (0x60)
 --> only then you read value of 0x60 (scan code)
      -> convert scan code to key (using array)
while loop doesn't work --> use if/else statement

*/
  if (!(inb( 0x64 ) & 0x1) || ( ( c = inb( 0x60 ) ) & 0x80 )) {
     // do nothing
      printk("Doing nothing\n");
      
  } else {
     printk("Scancode from my_getchar: %c", c);
     printk("Character converted from scancode: %c", scancode[(int)c]);
     return scancode[ (int)c ];
  }
}


/***
 * ioctl() entry point...
 */
static int pseudo_device_ioctl(struct inode *inode, struct file *file,
				unsigned int cmd, unsigned long arg)
{
  struct ioctl_test_t data;
  char ch  = NULL;
  switch (cmd){

  case IOCTL_TEST:
    ch = my_getchar(); // call my_getchar to get inputted key
    data.field2 = ch;
    if (ch == '\0') { // error
      return 1;
    }
    else {
      copy_to_user((struct ioctl_test_t *)arg, &data, sizeof(struct ioctl_test_t));
    }
    break;
  default:
    return -EINVAL;
    break;
  }
  return 0;
}

module_init(initialization_routine); // module is loaded into kernel
module_exit(cleanup_routine); // module removed from kernel