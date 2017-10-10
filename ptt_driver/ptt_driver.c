#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/usb.h>
#include <linux/ioctl.h>
#include <linux/termios.h>
 
#define USB_LED_OFF 0x00
#define USB_LED_ON 0x01

#define DEBUG false

static struct usb_device *device;
static struct usb_class_driver class;
 
static int ptt_open(struct inode *i, struct file *f)
{
    if(DEBUG) printk("Function: ptt_open\n");
    return 0;
}

static int ptt_close(struct inode *i, struct file *f)
{
    if(DEBUG) printk("Function: ptt_close\n");
    return 0;
}

static long my_ioctl(struct file *f, unsigned int cmd, unsigned long __user stuff)
{
  int retval = 0;
  char buffer[8];
  int transfer;

  memset(&buffer, 0, sizeof(buffer));
  memset(&transfer, 0, sizeof(transfer));

  switch (cmd)
  {
    case TIOCMGET:
      transfer = 0;
      if (copy_to_user((unsigned long*)stuff,&transfer,4))
      {
        return -EFAULT;
      }
      break;
    case TIOCMSET:
      //These two options are used by Direwolf software to turn on the transitter
      if ((*(unsigned long*)stuff & 0x00000000FFFFFFFF) != 0)
      {
        retval = usb_control_msg(device,usb_sndctrlpipe(device,0),USB_LED_ON,USB_TYPE_VENDOR | USB_RECIP_DEVICE,0,0,&buffer,sizeof(buffer),5000);
      }
      else if((*(unsigned long*)stuff & 0x00000000FFFFFFFF) == 0)
      {
        retval = usb_control_msg(device,usb_sndctrlpipe(device,0),USB_LED_OFF,USB_TYPE_VENDOR | USB_RECIP_DEVICE,0,0,&buffer,sizeof(buffer),5000);
      }
      break;
    default:
      retval = usb_control_msg(device,usb_sndctrlpipe(device,0),USB_LED_OFF,USB_TYPE_VENDOR | USB_RECIP_DEVICE,0,0,&buffer,sizeof(buffer),5000);
  }

  if(retval)
  {
    return retval;
  }
 
  return 0;
}
 
static struct file_operations fops =
{
  .owner = THIS_MODULE,
  .open = ptt_open,
  .release = ptt_close,
  .unlocked_ioctl = my_ioctl,
};
 
static int ptt_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
  int retval;

  if(DEBUG) printk("Function: ptt_probe");
 
  device = interface_to_usbdev(interface);
 
  class.name = "usb/ptt%d";
  class.fops = &fops;
  if ((retval = usb_register_dev(interface, &class)) < 0)
  {
    /* Something prevented us from registering this driver */
    printk("Not able to get a minor for this device.");
  }
  else
  {
    if(DEBUG) printk(KERN_INFO "Minor obtained: %d\n", interface->minor);
  }
 
  return retval;
}
 
static void ptt_disconnect(struct usb_interface *interface)
{
  if(DEBUG) printk("Function: ptt_disconnect\n");
  usb_deregister_dev(interface, &class);
}
 
/* Table of devices that work with this driver */
static struct usb_device_id ptt_table[] =
{
  { USB_DEVICE(0x16c0, 0x05dc) },
  {} /* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, ptt_table);
 
static struct usb_driver ptt_driver =
{
  .name = "ptt_driver",
  .probe = ptt_probe,
  .disconnect = ptt_disconnect,
  .id_table = ptt_table,
};
 
static int __init ptt_init(void)
{
  int result;
  if(DEBUG) printk("Function: ptt_init\n");
 
  /* Register this driver with the USB subsystem */
  if ((result = usb_register(&ptt_driver)))
  {
    printk("usb_register failed. Error number %d", result);
  }
  return result;
}
 
static void __exit ptt_exit(void)
{
  if(DEBUG) printk("Function: ptt_exit\n");
  /* Deregister this driver with the USB subsystem */
  usb_deregister(&ptt_driver);
}
 
module_init(ptt_init);
module_exit(ptt_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dylon Mutz,KK6OTK <dylon.mutz@gmail.com>");
MODULE_DESCRIPTION("USB_PTT Device Driver");
