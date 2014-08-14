/*
 * fortune - Fortune Cookie Linux Kernel Module
 *
 * Copied from online article by M. Tim Jones <mtj@mtjones.com>
 * (http://www.ibm.com/developerworks/linux/library/l-proc/index.html)
 *
 *   docs/l-proc-pdf.pdf 
 *
 * CHANGELOG:
 *  Updated for kernel v3.10 - Jade Tucker <jadeatucker@gmail.com>
 *
 * Write fortunes to /proc/fortune
 *
 *   echo "Success is an individual proposition. Thomas Watson" > /proc/fortune
 *
 * Read back fortunes from /proc/fortune
 *
 *   cat /proc/fortune
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Fortune Cookie Kernel Module");
MODULE_AUTHOR("M. Tim Jones");

#define MAX_COOKIE_LENGTH PAGE_SIZE

ssize_t fortune_write(struct file *, const char __user *, size_t, loff_t *);
ssize_t fortune_read(struct file *filep, char __user *buf, size_t count, loff_t *offp);

static struct proc_dir_entry *proc_entry;
static struct file_operations proc_fops = {
  read: fortune_read
  write: fortune_write
  owner: THIS_MODULE
};

static char *cookie_pot;
static int cookie_index;
static int next_fortune;

int init_fortune_module(void) {

  cookie_pot = (char *)vmalloc(MAX_COOKIE_LENGTH);
  if(!cookie_pot) {
    return -ENOMEM;
  }

  proc_entry = proc_create("fortune", 0644, NULL, &proc_fops);
  if(proc_entry == NULL) {
    vfree(cookie_pot);
    printk(KERN_INFO "fortune: Couldn't create proc entry.\n");
    return -ENOMEM;
  } else {
    cookie_index = 0;
    next_fortune = 0;
    printk(KERN_INFO "fortune: Module loaded.\n");
  }

  return 0;
}

void cleanup_fortune_module(void) {
  remove_proc_entry("fortune", NULL);
  vfree(cookie_pot);
  printk(KERN_INFO "fortune: Module unloaded.\n");
}

module_init(init_fortune_module);
module_exit(cleanup_fortune_module);

ssize_t fortune_write(struct file *filep, const char __user *buff,
    size_t len, loff_t *data) {

  int avail = (MAX_COOKIE_LENGTH - cookie_index) + 1;

  if(len > avail) {
    return -ENOSPC;
  }

  if(copy_from_user(&cookie_pot[cookie_index], buff, len)) {
    return -EFAULT;
  }

  cookie_index += len;
  cookie_pot[cookie_index-1] = 0;

  return len;
}

ssize_t fortune_read(struct file *filep, char __user *buf,
    size_t count, loff_t *offp) {

  int len;

  if(next_fortune >= cookie_index) {
    next_fortune = 0;
  }

  len = sprintf(buf, "%s\n", &cookie_pot[next_fortune]);

  next_fortune += len;

  return len;
}