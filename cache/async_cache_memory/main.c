/*
 * Developed by Gabriel Negreira Barbosa (pirata) and Rodrigo Rubira Branco (BSDaemon)
 *
 * License:  Beerware
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>

unsigned int get_cr3(void);
unsigned int cache_trick(unsigned int addr_virt, unsigned int addr_phys);

int init_module(void) {
	unsigned int cr3_phys, cr3_virt;
	unsigned int retval, i;
	unsigned int *ptr;

	cr3_phys = get_cr3();
	printk(KERN_ALERT "cr3 phys = 0x%x\n", cr3_phys);

	cr3_virt = (unsigned int)phys_to_virt(cr3_phys);
	printk(KERN_ALERT "cr3 virt = 0x%x\n", cr3_virt);

	retval = cache_trick(cr3_virt, cr3_phys);

	// Just a loop to access some random addresses
	ptr = (unsigned int *)0xc0004000; // virtual address of 0x4000
	for (i = 0; i < 1000; i++) {
		*ptr = i;
		printk(KERN_ALERT "%u\n", *ptr);
		ptr++;
	}

	// If 0xaabbccdd is printed here, then only_nem() executed until the end
	printk(KERN_ALERT "retval = %x\n", retval);

	return 0;
}

void cleanup_module(void) {
	printk( "cleanup\n");
}
