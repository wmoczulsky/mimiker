#define KL_LOG KL_DEV
#include <dev/piixreg.h>
#include <dev/isareg.h>
#include <bus.h>
#include <interrupt.h>
#include <stdc.h>
#include <pci.h>
#include <klog.h>
#include <sync.h>
#include <sysinit.h>

typedef struct isapnp_state {
  resource_t *io_ports;
} isapnp_state_t;

static int isapnp_attach(device_t *dev) {
  assert(dev->parent->bus == DEV_BUS_PCI);

  pci_bus_state_t *pcib = dev->parent->state;
  isapnp_state_t *isa = dev->state;

  isa->io_ports = pcib->io_space;
  dev->bus = DEV_BUS_ISA;

  return 0;
}

driver_t isapnp_bus = {
  .desc = "ISA PnP bus driver",
  .size = sizeof(isapnp_state_t),
  .attach = isapnp_attach,
};

extern device_t *gt_pci;

static void isapnp_init(void) {
  (void)make_device(gt_pci, &isapnp_bus);
}

SYSINIT_ADD(isapnp, isapnp_init, DEPS("rootdev"));
