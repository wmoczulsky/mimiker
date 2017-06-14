#define KL_LOG KL_DEV
#include <dev/piixreg.h>
#include <dev/isareg.h>
#include <bus.h>
#include <interrupt.h>
#include <stdc.h>
#include <klog.h>
#include <sync.h>

typedef struct isapnp_state {
  resource_t *io_ports;
} isapnp_state_t;

static int isapnp_attach(device_t *isab) {
  /* isapnp_state_t *isa = isab->state; */

  isab->bus = DEV_BUS_ISA;

  return 0;
}

driver_t isapnp_bus = {
  .desc = "ISA PnP bus driver",
  .size = sizeof(isapnp_state_t),
  .attach = isapnp_attach,
};
