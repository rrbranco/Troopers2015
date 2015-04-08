/*
 * Developed by Gabriel Negreira Barbosa (pirata) and Rodrigo Rubira Branco (BSDaemon)
 *
 * License:  Beerware
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>

unsigned int get_cr3(void);
unsigned int cpuid_mtrr(void);
unsigned int ia32_mtrrcap_msr(void);
unsigned int ia32_mtrr_def_type(void);
unsigned int generic_rdmsr_high(unsigned int msr);
unsigned int generic_rdmsr_low(unsigned int msr);
unsigned int cpuid_phys_addr_size(void);

void print_pae_structure(unsigned int virt_addr) {
	unsigned int cr3_phys, cr3_virt;
	unsigned int *pdpte, *pde, *pte;
	unsigned int i;

	printk(KERN_ALERT "Getting PAE structure for the address 0x%x\n", virt_addr);

	cr3_phys = get_cr3();
	printk(KERN_ALERT "CR3 (physical address): 0x%x\n", cr3_phys);

	cr3_virt = (unsigned int)phys_to_virt(cr3_phys);
	printk(KERN_ALERT "CR3 (virtual address): 0x%x\n", cr3_virt);

	// Get PDPTE
	pdpte = (unsigned int *) cr3_virt;
	i = virt_addr >> 30;
	pdpte += i * 2;
	printk(KERN_ALERT "PDPTE - PDPT Index: %d: 0x%x%x\n", i, *(pdpte+1), *pdpte);

	// Get PDE
	pde = (unsigned int *) (phys_to_virt( (*pdpte & 0xFFFFF000) ) );
	i = virt_addr & 0x3FFFFFFF;
	i = i >> 21;
	pde += i * 2;
	printk(KERN_ALERT "PDE - PD Index: %d: 0x%x%x\n", i, *(pde+1), *pde);

	// Geet PTE
	pte = (unsigned int *) (phys_to_virt( (*pde & 0x7FFFF000) ) );
	i = virt_addr & 0x001FFFFF;
	i = i >> 12;
	pte += i * 2;
	printk(KERN_ALERT "PTE - PT Index: %d: 0x%x%x\n", i, *(pte+1), *pte);
}

void print_mtrr_info(void) {
	unsigned int retval, i, n_var_mtrr;

	retval = cpuid_mtrr();
	printk(KERN_ALERT "CPUID.0x1.EDX: 0x%x\n", retval);
	if ((retval & 0x00001000) != 0)
		printk(KERN_ALERT "    Bit 12 (MTRR) is set\n");

	retval = ia32_mtrrcap_msr();
	printk(KERN_ALERT "IA32_MTRRCAP MSR (low 32-bit): 0x%x\n", retval);
	printk(KERN_ALERT "    Number of variable range registers: %u\n", (n_var_mtrr = retval & 0x000000FF));
	printk(KERN_ALERT "    Fixed range registers supported (boolean): %u\n", ((retval & 0x00000100) >> 8));
	printk(KERN_ALERT "    Write-combining memory type supported (boolean): %u\n", ((retval & 0x00000400) >> 10));
	printk(KERN_ALERT "    SMRR interface supported (boolean): %u\n", ((retval & 0x00000800) >> 11));

	printk(KERN_ALERT "IA32_MTRR_DEF_TYPE MSR: 0x%x\n", ia32_mtrr_def_type());

	printk(KERN_ALERT "IA32_MTRR_FIX64K_00000 MSR: 0x%x%x\n", generic_rdmsr_high(0x250), generic_rdmsr_low(0x250));
	printk(KERN_ALERT "IA32_MTRR_FIX16K_80000 MSR: 0x%x%x\n", generic_rdmsr_high(0x258), generic_rdmsr_low(0x258));
	printk(KERN_ALERT "IA32_MTRR_FIX16K_A0000 MSR: 0x%x%x\n", generic_rdmsr_high(0x259), generic_rdmsr_low(0x259));
	printk(KERN_ALERT "IA32_MTRR_FIX4K_C0000 MSR: 0x%x%x\n", generic_rdmsr_high(0x268), generic_rdmsr_low(0x268));
	printk(KERN_ALERT "IA32_MTRR_FIX4K_C8000 MSR: 0x%x%x\n", generic_rdmsr_high(0x269), generic_rdmsr_low(0x269));
	printk(KERN_ALERT "IA32_MTRR_FIX4K_D0000 MSR: 0x%x%x\n", generic_rdmsr_high(0x26A), generic_rdmsr_low(0x26A));
	printk(KERN_ALERT "IA32_MTRR_FIX4K_D8000 MSR: 0x%x%x\n", generic_rdmsr_high(0x26B), generic_rdmsr_low(0x26B));
	printk(KERN_ALERT "IA32_MTRR_FIX4K_E0000 MSR: 0x%x%x\n", generic_rdmsr_high(0x26C), generic_rdmsr_low(0x26C));
	printk(KERN_ALERT "IA32_MTRR_FIX4K_E8000 MSR: 0x%x%x\n", generic_rdmsr_high(0x26D), generic_rdmsr_low(0x26D));
	printk(KERN_ALERT "IA32_MTRR_FIX4K_F0000 MSR: 0x%x%x\n", generic_rdmsr_high(0x26E), generic_rdmsr_low(0x26E));
	printk(KERN_ALERT "IA32_MTRR_FIX4K_F8000 MSR: 0x%x%x\n", generic_rdmsr_high(0x26F), generic_rdmsr_low(0x26F));

	// CPUID showing the maximum physical address size
	printk(KERN_ALERT "MAXPHYADDR: 0x%x", cpuid_phys_addr_size());

	// Print at most 10 variable MTRR
	if (n_var_mtrr > 10)
		n_var_mtrr = 10;

	for (i = 0; i < n_var_mtrr; i++) {
		printk(KERN_ALERT "IA32_MTRR_PHYSBASE%u MSR: 0x%x%x\n", i, generic_rdmsr_high(0x200 + (i * 2)), generic_rdmsr_low(0x200 + (i * 2)));
		printk(KERN_ALERT "IA32_MTRR_PHYSMASK%u MSR: 0x%x%x\n", i, generic_rdmsr_high(0x201 + (i * 2)), generic_rdmsr_low(0x201 + (i * 2)));
	}
}

int init_module(void) {
	print_pae_structure(0xc000a000);
	printk(KERN_ALERT "\n");
	print_mtrr_info();

	return 0;
}

void cleanup_module(void) {
	printk( "cleanup\n");
}
