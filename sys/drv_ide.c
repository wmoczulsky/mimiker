#include <common.h>
#include <pci.h>
#include <interrupt.h>
#include <klog.h>
#include <errno.h>
#include <stdc.h>
#include <devfs.h>
#include <sleepq.h>
#include <time.h>
#include <vnode.h>
#include <sysinit.h>


typedef struct ide_state{

} ide_state_t;



static int ide_attach(device_t *dev) {
  assert(dev->parent->bus == DEV_BUS_PCI);

  pci_bus_state_t *pcib = dev->parent->state;
  ide_state_t *state = dev->state;

  (void)state;
  (void)pcib;

  // rtc->regs = pcib->io_space;

  // rtc->intr_handler =
  //   INTR_HANDLER_INIT(rtc_intr, NULL, rtc, "RTC periodic timer", 0);
  // bus_intr_setup(dev, 8, &rtc->intr_handler);

  // /* Configure how the time is presented through registers. */
  // rtc_setb(rtc->regs, MC_REGB, MC_REGB_BINARY | MC_REGB_24HR);

  // /* Set RTC timer so that it triggers interrupt 2 times per second. */
  // rtc_write(rtc->regs, MC_REGA, MC_RATE_2_Hz);
  // rtc_setb(rtc->regs, MC_REGB, MC_REGB_PIE);

  // /* Prepare /dev/rtc interface. */
  // devfs_makedev(NULL, "rtc", &rtc_time_vnodeops, rtc);

  return 0;
}







static driver_t ide_driver = {
  .desc = "IDE driver",
  .size = sizeof(ide_state_t),
  .attach = ide_attach,
};

extern device_t *gt_pci;

static void ide_init(void) {
  // vnodeops_init(&default_vnode_fileops);
  (void)make_device(gt_pci, &ide_driver);
}


SYSINIT_ADD(ide, ide_init, DEPS("rootdev"));