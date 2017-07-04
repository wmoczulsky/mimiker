#define KL_LOG KL_DEV
/* #include <dev/piixreg.h> */
/* #include <dev/isareg.h> */
/* #include <dev/isapnpreg.h> */
#include <dev/pnpreg.h>
#include <bus.h>
#include <interrupt.h>
#include <stdc.h>
#include <pci.h>
#include <klog.h>
#include <sync.h>
#include <sysinit.h>

/* The READ_DATA port that we are using currently */
static int pnp_rd_port;

static resource_t *isa_io_ports; 


typedef struct _pnp_id {
    uint32_t vendor_id;
    uint32_t serial;
    uint8_t checksum;
} pnp_id;



static inline void outb(unsigned port, uint8_t value) {
    bus_space_write_1(isa_io_ports, port, value);
}

static inline uint8_t inb(unsigned port) {
    return bus_space_read_1(isa_io_ports, port);
}

static void pnp_write(int d, uint8_t r)
{
	outb (_PNP_ADDRESS, d);
	outb (_PNP_WRITE_DATA, r);
}


/*
 * Send Initiation LFSR as described in "Plug and Play ISA Specification",
 * Intel May 94.
 */
static void pnp_send_initiation_key(void)
{
	int cur, i;

	/* Reset the LSFR */
	outb(_PNP_ADDRESS, 0);
	outb(_PNP_ADDRESS, 0); /* yes, we do need it twice! */

	cur = 0x6a;
	outb(_PNP_ADDRESS, cur);

	for (i = 1; i < 32; i++) {
		cur = (cur >> 1) | (((cur ^ (cur >> 1)) << 7) & 0xff);
		outb(_PNP_ADDRESS, cur);
	}
}


/*
 * Get the device's serial number.  Returns 1 if the serial is valid.
 */
static int
pnp_get_serial(pnp_id *p)
{
	int i, bit, valid = 0, sum = 0x6a;
	uint8_t *data = (uint8_t *)p;

	bzero(data, sizeof(char) * 9);
	outb(_PNP_ADDRESS, PNP_SERIAL_ISOLATION);
	for (i = 0; i < 72; i++) {
		bit = inb((pnp_rd_port << 2) | 0x3) == 0x55;
		/* DELAY(250); */	/* Delay 250 usec */

		/* Can't Short Circuit the next evaluation, so 'and' is last */
		bit = (inb((pnp_rd_port << 2) | 0x3) == 0xaa) && bit;
		/* DELAY(250); */	/* Delay 250 usec */

		valid = valid || bit;
		if (i < 64)
			sum = (sum >> 1) |
			  (((sum ^ (sum >> 1) ^ bit) << 7) & 0xff);
		data[i / 8] = (data[i / 8] >> 1) | (bit ? 0x80 : 0);
	}

	valid = valid && (data[8] == sum);

	return (valid);
}


/*
 * Run the isolation protocol. Use pnp_rd_port as the READ_DATA port
 * value (caller should try multiple READ_DATA locations before giving
 * up). Upon exiting, all cards are aware that they should use
 * pnp_rd_port as the READ_DATA port.
 *
 * In the first pass, a csn is assigned to each board and pnp_id's
 * are saved to an array, pnp_devices. In the second pass, each
 * card is woken up and the device configuration is called.
 */
static int
pnp_isolation_protocol(void)
{
	int csn;
	pnp_id id;
	int found = 0;
	/* , len; */
	/* /\* u_char *resources = 0; *\/ */
	/* int space = 0; */
	/* int error; */
/* #ifdef PC98 */
/* 	int n, necpnp; */
/* 	u_char buffer[10]; */
/* #endif */

	/*
	 * Put all cards into the Sleep state so that we can clear
	 * their CSNs.
	 */
	pnp_send_initiation_key();

	/*
	 * Clear the CSN for all cards.
	 */
	pnp_write(PNP_CONFIG_CONTROL, PNP_CONFIG_CONTROL_RESET_CSN);

	/*
	 * Move all cards to the Isolation state.
	 */
	pnp_write(PNP_WAKE, 0);

	/*
	 * Tell them where the read point is going to be this time.
	 */
	pnp_write(PNP_SET_RD_DATA, pnp_rd_port);

	for (csn = 1; csn < PNP_MAX_CARDS; csn++) {
		/*
		 * Start the serial isolation protocol.
		 */
		outb(_PNP_ADDRESS, PNP_SERIAL_ISOLATION);
		/* DELAY(1000); */	/* Delay 1 msec */

		if (pnp_get_serial(&id)) {

		  kprintf("FOUND!!\n");
			/*
			 * We have read the id from a card
			 * successfully. The card which won the
			 * isolation protocol will be in Isolation
			 * mode and all others will be in Sleep.
			 * Program the CSN of the isolated card
			 * (taking it to Config state) and read its
			 * resources, creating devices as we find
			 * logical devices on the card.
			 */
/* 			pnp_write(PNP_SET_CSN, csn); */
		} else 
		  break;

/* 		/\* */
/* 		 * Put this card back to the Sleep state and */
/* 		 * simultaneously move all cards which don't have a */
/* 		 * CSN yet to Isolation state. */
/* 		 *\/ */
/* 		pnp_write(PNP_WAKE, 0); */
	}

	/*
	 * Unless we have chosen the wrong read port, all cards will
	 * be in Sleep state. Put them back into WaitForKey for
	 * now. Their resources will be programmed later.
	 */
	pnp_write(PNP_CONFIG_CONTROL, PNP_CONFIG_CONTROL_WAIT_FOR_KEY);

	return (found);
}


/*
 * pnp_identify()
 *
 * autoconfiguration of pnp devices. This routine just runs the
 * isolation protocol over several ports, until one is successful.
 *
 * may be called more than once ?
 *
 */

static void pnp_identify(void) {
	int num_pnp_devs;

	/* Try various READ_DATA ports from 0x203-0x3ff */
	for (pnp_rd_port = 0x80; (pnp_rd_port < 0xff); pnp_rd_port += 0x10) {
			kprintf("pnp_identify: Trying Read_Port at %x\n",
			    (pnp_rd_port << 2) | 0x3);

		num_pnp_devs = pnp_isolation_protocol();
		if (num_pnp_devs)
			break;
	}
		kprintf("PNP Identify complete\n");
}





typedef struct isapnp_state {
  resource_t *io_ports;
} isapnp_state_t;

static int isapnp_attach(device_t *dev) {
  assert(dev->parent->bus == DEV_BUS_PCI);

  pci_bus_state_t *pcib = dev->parent->state;
  isapnp_state_t *isa = dev->state;

  isa->io_ports = pcib->io_space;
  dev->bus = DEV_BUS_ISA;

  isa_io_ports = isa->io_ports;
  
  pnp_identify();

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
