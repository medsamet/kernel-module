#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include<linux/uaccess.h>
//https://embetronicx.com/tutorials/linux/device-drivers/ioctl-tutorial-in-linux/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Primatec Engineering");
MODULE_DESCRIPTION("A simple example Linux module for TECMP.");
MODULE_VERSION("0.01");
#define DEVICE_NAME "tecmp_primatec"
#define EXAMPLE_MSG "read access TECMP driver !\n"
#define MSG_BUFFER_LEN 1536 /* Prototypes for device functions */
#define TECMP_MAX_FRAME_LEN 1536
#define MAX_FRAME_NUM 1000
#define MAX_FILTER_NUM  10

// IOCTL
#define FIL_CM_ID       _IOW('a','a',int32_t*)
#define FIL_MSG_TYP     _IOW('b','b',int32_t*)
#define FIL_DAT_TYP     _IOW('c','c',int32_t*)
#define FIL_CHAN_ID     _IOW('d','d',int32_t*)
#define FIL_RESET       _IOW('e','e',int32_t*)

#define REVERSE_16(n) ((unsigned short) (((n & 0xFF) << 8) | ((n & 0xFF00) >> 8)))
#define REVERSE_32(n) ((unsigned long) (((n & 0xFF) << 24) | ((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | ((n & 0xFF000000) >> 24)))

typedef enum eFilterType {
    eFilterNone        =  0,
    eFilterCMID         =  1,
    eFilterMsgType      =  2,
    eFilterDataType     =  3,
    eFilterChanId       =  4
}eFilterType_t;

static struct Ioctl_Filter{
    eFilterType_t type;
    int32_t value;
}ioctl_filter_t;

static struct Ioctl_Filter_List{
    struct Ioctl_Filter filter[MAX_FILTER_NUM];
    uint8_t index;
    bool empty;
}Ioctl_Filter_List_t;

struct Ioctl_Filter_List drv_filter;

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static long device_ioctl(struct file *, unsigned int, unsigned long);

static struct TecmpHeader{
    uint16_t CMID;
    uint16_t Counter;
    uint8_t Version;
    uint8_t MessageType;
    uint16_t DataType;
    uint16_t Reserved;
    uint16_t CMFlags;
    uint32_t ChannelId;
}tecmp_hdr;

typedef enum eDataType {
    ONE              = 0x0000,
	RESERVED_0       = 0x0001,
	CAN              = 0x0002,
	CAN_FD           = 0x0003,
	LIN              = 0x0004,
	RESERVED_1       = 0x0007,
	FLEXRAY          = 0x0008,
	GPIO             = 0x000A,
	UART_RS232_ASCII = 0x0010,
	RESERVED_2       = 0x0011,
	UART_RS232_SLA   = 0x0012,
	ANALOG           = 0x0020,
	ANALOG_SLA       = 0x0021,
	ETHERNET_II      = 0x0080
}eDataType_t;


//TECMP internal ring buffer
static struct TecmpFrame {
    uint16_t frameLen;
    char TecmpOneFrame[TECMP_MAX_FRAME_LEN];
} Frame;

static struct circular_buf_t {
	struct TecmpFrame tecmpFrames[MAX_FRAME_NUM];
	size_t head;
	size_t tail;
	size_t size; //of the buffer
	bool full;
    struct mutex cb_mutex;
    bool (*IsFull) (struct circular_buf_t*);
    size_t (*GetSize) (struct circular_buf_t*);
    bool (*IsEmpty) (struct circular_buf_t*);
    void (*ResetCb) (struct circular_buf_t*);
    void (*PushCb) (struct circular_buf_t*, struct TecmpFrame*);
    bool (*PopCb) (struct circular_buf_t*, struct TecmpFrame*);
    void (*InitCb) (struct circular_buf_t*);
}CircBuf;

static bool tecmp_circ_buf_full (struct circular_buf_t* cb);
static bool tecmp_circ_buf_empty (struct circular_buf_t* cb);
static void tecmp_circ_buf_init (struct circular_buf_t* cb);
static size_t tecmp_circ_buf_getsize (struct circular_buf_t* cb);
static void tecmp_circ_buf_reset (struct circular_buf_t* cb);
static void tecmp_circ_buf_push (struct circular_buf_t* cb, struct TecmpFrame* frame);
static bool tecmp_circ_buf_pop (struct circular_buf_t* cb, struct TecmpFrame* frame);

static void initFilterList(struct Ioctl_Filter_List* fltr);
static bool filterListIsFull(struct Ioctl_Filter_List* fltr);
static bool add_filter_to_list(struct Ioctl_Filter_List* fltr, struct Ioctl_Filter new);
static bool shouldKeepPackage(struct Ioctl_Filter_List* fltr, struct TecmpHeader* hdr);

//static int device_ioctl(struct inode *inode, struct file *file, unsigned int ioctl_num, unsigned long ioctl_param);
static int major_num;
static int device_open_count = 0;
static char msg_buffer[MSG_BUFFER_LEN];
struct TecmpFrame frame;

static char *read_ptr; /* This structure points to all of the device functions */
static char *write_ptr;

static struct circular_buf_t tecmpProtocol = {
    .IsFull = tecmp_circ_buf_full, //done
    .GetSize = tecmp_circ_buf_getsize,
    .IsEmpty = tecmp_circ_buf_empty, //done
    .ResetCb = tecmp_circ_buf_reset, //done
    .PushCb = tecmp_circ_buf_push,
    .PopCb = tecmp_circ_buf_pop,
    .InitCb = tecmp_circ_buf_init //done
};

static bool tecmp_circ_buf_full (struct circular_buf_t* cb)
{
    return cb->full;
}

static bool tecmp_circ_buf_empty (struct circular_buf_t* cb)
{
    return (!cb->full && (cb->head == cb->tail));
}

static void tecmp_circ_buf_init (struct circular_buf_t* cb)
{
    cb->head = 0;
    cb->tail = 0;
    cb->size = 0;
    cb->full = false;
    mutex_init(&(cb->cb_mutex));
    memset (&(cb->tecmpFrames[0]), 0x0, MAX_FRAME_NUM*sizeof(Frame));
}

static size_t tecmp_circ_buf_getsize (struct circular_buf_t* cb)
{
    size_t size = MAX_FRAME_NUM;

	if(!cb->full)
	{
		if(cb->head >= cb->tail)
		{
			size = (cb->head - cb->tail);
		}
		else
		{
			size = (MAX_FRAME_NUM + cb->head - cb->tail);
		}
	}

	return size;
}

static void tecmp_circ_buf_reset (struct circular_buf_t* cb)
{
    tecmp_circ_buf_init(cb);
}

static void tecmp_circ_buf_push (struct circular_buf_t* cb, struct TecmpFrame* frame)
{
    //lock mutex
    mutex_lock(&(cb->cb_mutex));
    //copy content from frame to RingBuffer
    memcpy(&(cb->tecmpFrames[cb->head]), frame, sizeof(struct TecmpFrame));
    //advance pointer
    if(cb->full)
   	{
		cb->tail = (cb->tail + 1) % MAX_FRAME_NUM;
        printk(KERN_ERR "a TECMP frame has been droped since internal Buffer is Full");
	}

	cb->head = (cb->head + 1) % MAX_FRAME_NUM;
	cb->full = (cb->head == cb->tail);
    //unlock mutex
    mutex_unlock(&(cb->cb_mutex));
}

static bool tecmp_circ_buf_pop (struct circular_buf_t* cb, struct TecmpFrame* frame)
{
    bool retVal = false;
    //lock mutex
    mutex_lock(&(cb->cb_mutex));
    if(!tecmp_circ_buf_empty(cb))
    {
        //we Assume frame structure is already allocated and initialized
        memcpy(frame, &(cb->tecmpFrames[cb->tail]), sizeof(struct TecmpFrame));
        // retreat pointer
        cb->full = false;
	    cb->tail = (cb->tail+ 1) % MAX_FRAME_NUM;
        retVal = true;
    }
    //lock mutex
    mutex_unlock(&(cb->cb_mutex));
    return retVal;
}

static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .unlocked_ioctl = device_ioctl,
    .release = device_release};

static long device_ioctl(struct file *flip, unsigned int cmd, unsigned long arg)
{
    int RetVal = 0; //cmd_value
    struct Ioctl_Filter new;
    copy_from_user(&(new.value) , (int32_t *)arg, sizeof(int32_t));
    printk(KERN_INFO "[%ld]passed cmd = %d - Value = %d",FIL_DAT_TYP, cmd, new.value);
    
    switch (cmd) {
        case FIL_CM_ID:
            new.type = eFilterCMID;
            add_filter_to_list(&drv_filter, new);
            break;
        case FIL_MSG_TYP:
            new.type = eFilterMsgType;
            add_filter_to_list(&drv_filter, new);
            break;
        case FIL_DAT_TYP:
            new.type = eFilterDataType;
            add_filter_to_list(&drv_filter, new);
            break;
        case FIL_CHAN_ID:
            new.type = eFilterChanId;
            add_filter_to_list(&drv_filter, new);
            break;
        case FIL_RESET:
            initFilterList(&drv_filter);
            break;
        default:
            RetVal = -1;
            break;
    }
    return RetVal;
}
/* When a process reads from our device, this gets called. */

static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset)
{
    printk(KERN_INFO "enter device_read()\n");
    int bytes_read = 0;
    // if RingBuffer is empty, just return 0
    if (tecmp_circ_buf_empty(&tecmpProtocol))
        goto end_of_the_story;
    
    if (tecmp_circ_buf_pop(&tecmpProtocol, &frame)) {
        printk(KERN_INFO "pop an item from the ring buffer\n");
        bytes_read = frame.frameLen;
        read_ptr= &(frame.TecmpOneFrame[0]);
    }
    /* If we’re at the end, loop back to the beginning */

    /* Put data in the buffer */
    printk(KERN_INFO "frame length = %d\n", frame.frameLen);
    while (frame.frameLen)
    {
        /* Buffer is in user data, not kernel, so you can’t just reference
         * with a pointer. The function put_user handles this for us */
        put_user(*(read_ptr++), buffer++);
        frame.frameLen--;
        //next instruction seems to be useless
    }
    memset(&frame, 0x0, sizeof(struct TecmpFrame));

end_of_the_story:
    printk(KERN_INFO "leave device_read()\n");
    return bytes_read;
} /* Called when a process tries to write to our device */

static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset)
{
    printk(KERN_INFO "enter device_write()\n");
    struct TecmpHeader hdr;
    int bytes_write = 0;

    for (bytes_write = 0; bytes_write < len && bytes_write < MSG_BUFFER_LEN; bytes_write++)
		get_user(msg_buffer[bytes_write], buffer + bytes_write);

    //this instruction  should be removed
    memcpy(&hdr, &msg_buffer[0], sizeof(struct TecmpHeader));
    hdr.CMID = REVERSE_16(hdr.CMID);
    hdr.DataType = REVERSE_16(hdr.DataType);
    hdr.ChannelId = REVERSE_32(hdr.ChannelId);
    printk(KERN_INFO "data_type = %d\n", hdr.DataType);

    if (shouldKeepPackage(&drv_filter, &hdr)) {
        frame.frameLen = len;
        memcpy(&(frame.TecmpOneFrame[0]), msg_buffer, len);
        tecmp_circ_buf_push(&tecmpProtocol, &frame);
        printk(KERN_INFO "add an item to the ring buffer\n");
        memset(&frame, 0x0, sizeof (struct TecmpFrame));
    }
	/* 
	 * Again, return the number of input characters used 
	 */
    printk(KERN_INFO "leave device_write()\n");
    return bytes_write;
} /* Called when a process opens our device */

static int device_open(struct inode *inode, struct file *file)
{
    /* If device is open, return busy */
    if (device_open_count)
    {
        return -EBUSY;
    }
    device_open_count++;
    try_module_get(THIS_MODULE);
    return 0;
} /* Called when a process closes our device */

static int device_release(struct inode *inode, struct file *file)
{
    /* Decrement the open counter and usage count. Without this, the module would not unload. */
    device_open_count--;
    module_put(THIS_MODULE);
    return 0;
}

static void initFilterList(struct Ioctl_Filter_List* fltr)
{
    memset(fltr, 0x0, sizeof(struct Ioctl_Filter_List));
}
static bool filterListIsFull(struct Ioctl_Filter_List* fltr)
{
    return (fltr->index == MAX_FILTER_NUM-1);
}
static bool add_filter_to_list(struct Ioctl_Filter_List* fltr, struct Ioctl_Filter new)
{
    bool ret = false;
    printk(KERN_INFO "enter add_filter_to_list() \n");
    if (!filterListIsFull(fltr)) {
        printk(KERN_INFO "add new filter with value %d\n", new.value);
        memcpy(&(fltr->filter[fltr->index]), &new, sizeof(struct Ioctl_Filter));
        fltr->index++;
        ret = true;
    }
    printk(KERN_INFO "leave add_filter_to_list() \n");
    return ret;
}
static bool shouldKeepPackage(struct Ioctl_Filter_List* fltr, struct TecmpHeader* hdr)
{
    bool retVal = false;
    int i;
    if (! fltr->empty) {
        for (i = 0; i <= fltr->index; i++) {
            if (retVal)
                break;
            switch (fltr->filter[i].type) {
                case eFilterDataType:/* constant-expression */
                    retVal = (fltr->filter[i].value == hdr->DataType);
                    break;
                case eFilterMsgType:
                    retVal = (fltr->filter[i].value == hdr->MessageType);
                    break;
                case eFilterChanId:
                    retVal = (fltr->filter[i].value == hdr->ChannelId);
                    break;
                case eFilterCMID:
                    retVal = (fltr->filter[i].value == hdr->CMID);
                    break;
                default:
                    break;
            }
        } 
    }
    return retVal;
}
static int __init tecmp_driver_init(void)
{
    /* Fill buffer with our message */
    strncpy(msg_buffer, EXAMPLE_MSG, 28);
    /* Set the msg_ptr to the buffer */
    read_ptr = msg_buffer;
    write_ptr = msg_buffer;
    tecmp_circ_buf_init(&tecmpProtocol);
    filterListIsFull(&drv_filter);

    /* Try to register character device */
    major_num = register_chrdev(0, DEVICE_NAME, &file_ops);
    if (major_num < 0)
    {
        printk(KERN_ALERT "Could not register device: % d\n", major_num);
        return major_num;
    }
    else
    {
        printk(KERN_INFO "TECMP module loaded with device major number % d\n", major_num);
        return 0;
    }
}

static void __exit tecmp_driver_exit(void)
{
    /* Remember — we have to clean up after ourselves. Unregister the character device. */
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_INFO "Goodbye, Primatec !\n");
} /* Register module functions */

module_init(tecmp_driver_init);
module_exit(tecmp_driver_exit);
