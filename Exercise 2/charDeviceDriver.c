/*
 *  chardev.c: Creates a read-only char device that says how many times
 *  you've read from the dev file
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>    /* for put_user */
#include <charDeviceDriver.h>

MODULE_LICENSE("GPL");

/* 
 * This function is called whenever a process tries to do an ioctl on our
 * device file. We get two extra parameters (additional to the inode and file
 * structures, which all device functions get): the number of the ioctl called
 * and the parameter given to the ioctl function.
 *
 * If the ioctl is write or read/write (meaning output is returned to the
 * calling process), the ioctl call returns the output of this function.
 *
 */


DEFINE_MUTEX  (devLock);

static int total_length = 0;
static int length_excl_null = 0;
static char msgs[MAX_LEN];
static int hasRead = 0;

static void insertLatest(char* msg) {
	msg_Ptr = &(msgs[total_length]);
	msg_srt_Ptr = &(msgs[total_length]);
	while (*msg) {
		msgs[total_length] = *(msg);
		msg++;
		total_length++;
		length_excl_null++;
	}
	msgs[total_length] = '\0';
	total_length++;
}

static void removeLatest(void) {
	if (!total_length) {
		return;
	}
	total_length -= 2;
	while (msgs[total_length] != '\0') {
		msgs[total_length] = '\0';
		total_length--;
		if (total_length == -1) {
			break;
		}
	}
	total_length++;

	if (!total_length) {
		msg_Ptr = msgs;
		return;
	}

	msg_srt_Ptr -= 2;
	while (*msg_srt_Ptr) {
		if (msg_srt_Ptr == msgs) {
			return;
		}
		msg_srt_Ptr--;
	}
	msg_srt_Ptr++;
}

/*
 * This function is called when the module is loaded
 * You will not need to modify it for this assignment
 */
int init_module(void)
{
    Major = register_chrdev(0, DEVICE_NAME, &fops);

    if (Major < 0) {
      printk(KERN_ALERT "Registering char device failed with %d\n", Major);
      return Major;
    }

    printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
    printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk(KERN_INFO "the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");

    return SUCCESS;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void)
{
    /*  Unregister the device */
    unregister_chrdev(Major, DEVICE_NAME);
}

/*
 * Methods
 */

/* 
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *file)
{
    
    mutex_lock (&devLock);
    if (Device_Open) {
    mutex_unlock (&devLock);
    	printk(KERN_INFO "Device busy.");
    	return -EBUSY;
    }
    Device_Open++;
    hasRead = 0;
    mutex_unlock (&devLock);
    try_module_get(THIS_MODULE);
   
    return SUCCESS;
}

/* Called when a process closes the device file. */
static int device_release(struct inode *inode, struct file *file)
{
    mutex_lock (&devLock);
    msg_Ptr = msg_srt_Ptr;
    Device_Open--;      /* We're now ready for our next caller */
    mutex_unlock (&devLock);
    /* 
     * Decrement the usage count, or else once you opened the file, you'll
     * never get get rid of the module. 
     */
    module_put(THIS_MODULE);

    return 0;
}

/* 
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 * This is just an example about how to copy a message in the user space
 * You will need to modify this function
 */
static ssize_t device_read(struct file *filp,   /* see include/linux/fs.h   */
               char *buffer,    /* buffer to fill with data */
               size_t length,   /* length of the buffer     */
               loff_t * offset)
{
    /*
     * Number of bytes actually written to the buffer 
     */
    int bytes_read = 0;

    /* result of function calls */
    int result;

    /*
     * If we're at the end of the message, 
     * return 0 signifying end of file 
     */ 

    if (msg_Ptr == NULL) {
    	return -EAGAIN;
    }

    /* The hasRead checks whether something has been read from the driver, and if it hasn't it and the stack is of length 0, it should be
    	caught by the next branch. */
    if (*msg_Ptr == 0 && hasRead) {
        return 0;
    }

    if (total_length == 0){
    	return -EAGAIN;
    }

    hasRead = 1;
    /* 
     * Actually put the data into the buffer 
     */
    while (length && *msg_Ptr) {

        /* 
         * The buffer is in the user data segment, not the kernel 
         * segment so "*" assignment won't work.  We have to use 
         * put_user which copies data from the kernel data segment to
         * the user data segment. 
         */
        result = put_user(*(msg_Ptr++), buffer++);
        if (result != 0) {
                 return -EFAULT;
        }
            
        length--;
        bytes_read++;
    }
    removeLatest();
    /* 
     * Most read functions return the number of bytes put into the buffer
     */
    return bytes_read;
}

/* Called when a process writes to dev file: echo "hi" > /dev/hello  */
static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	char cpy[len+1];
	if (len > BUF_LEN) {
		return -EINVAL;
	}

	if (len + total_length > MAX_LEN) {
		return -EAGAIN;
	}

	copy_from_user(cpy, buff, len);
	/* Append a null character so as not to have garbage characters put into the stack. */
	cpy[len] = '\0';

	insertLatest(cpy);
    return len;
}
