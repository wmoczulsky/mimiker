#ifndef _SYS_SBRK_H_
#define _SYS_SBRK_H_

typedef struct proc proc_t;

/* The brk segment is located at the first large enough gap after SBRK_START
 * address. */
#define SBRK_START 0x08000000U

void sbrk_attach(proc_t *p);
vm_addr_t sbrk_resize(proc_t *p, intptr_t increment);

#endif /* !_SYS_SBRK_H_ */
