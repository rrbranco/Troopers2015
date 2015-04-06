#include <linux/module.h>
#include <linux/kernel.h>

int index = 0;
module_param(index, int, 0);

#define GET_FULL_ISR(low, high) ( ((uint32_t)(low)) | (((uint32_t)(high)) << 16) )
#define GET_LOW_ISR(addr) ( (uint16_t)(((uint32_t)(addr)) & 0x0000FFFF) )
#define GET_HIGH_ISR(addr) ( (uint16_t)(((uint32_t)(addr)) >> 16) )

uint32_t original_handlers[256];

uint16_t old_gs, old_fs, old_es, old_ds;

typedef struct _idt_gate_desc {
        uint16_t offset_low;
        uint16_t segment_selector;
        uint8_t zero; // zero + reserved
        uint8_t flags;
        uint16_t offset_high;
} idt_gate_desc_t;
idt_gate_desc_t *gates[256];

void handler_implemented(void) {
        printk(KERN_EMERG "IDT Hooked Handler\n");
}

void foo(void) {
        __asm__("push %eax"); // placeholder for original handler

        __asm__("pushw %gs");
        __asm__("pushw %fs");
        __asm__("pushw %es");
        __asm__("pushw %ds");
        __asm__("push %eax");
        __asm__("push %ebp");
        __asm__("push %edi");
        __asm__("push %esi");
        __asm__("push %edx");
        __asm__("push %ecx");
        __asm__("push %ebx");

        __asm__("movw %0, %%ds" : : "m"(old_ds));
        __asm__("movw %0, %%es" : : "m"(old_es));
        __asm__("movw %0, %%fs" : : "m"(old_fs));
        __asm__("movw %0, %%gs" : : "m"(old_gs));

        handler_implemented();

        // place original handler in its placeholder
        __asm__("mov %0, %%eax": : "m"(original_handlers[index]));
		 __asm__("mov %eax, 0x24(%esp)");

        __asm__("pop %ebx");
        __asm__("pop %ecx");
        __asm__("pop %edx");
        __asm__("pop %esi");
        __asm__("pop %edi");
        __asm__("pop %ebp");
        __asm__("pop %eax");
        __asm__("popw %ds");
        __asm__("popw %es");
        __asm__("popw %fs");
        __asm__("popw %gs");

        // ensures that "ret" will be the next instruction for the case
        // compiler adds more instructions in the epilogue
        __asm__("ret");
}

int init_module(void) {
        // IDTR
        unsigned char idtr[6];
        uint16_t idt_limit;
        uint32_t idt_base_addr;

        int i;

        __asm__("mov %%gs, %0": "=m"(old_gs));
        __asm__("mov %%fs, %0": "=m"(old_fs));
        __asm__("mov %%es, %0": "=m"(old_es));
        __asm__("mov %%ds, %0": "=m"(old_ds));

        __asm__("sidt %0": "=m" (idtr));
        idt_limit = *((uint16_t *)idtr);
        idt_base_addr = *((uint32_t *)&idtr[2]);
        printk("IDT Base Address: 0x%x, IDT Limit: 0x%x\n", idt_base_addr, idt_limit);

        gates[0] = (idt_gate_desc_t *)(idt_base_addr);
        for (i = 1; i < 256; i++)
                gates[i] = gates[i - 1] + 1;

        printk("int %d entry addr %x, seg sel %x, flags %x, offset %x\n", index, gates[index], (uint32_t)gates[index]->segment_selector, (uint32_t)gates[index]->flags, GET_FULL_ISR(gates[index]->of
fset_low, gates[index]->offset_high));

        for (i = 0; i < 256; i++)
                original_handlers[i] = GET_FULL_ISR(gates[i]->offset_low, gates[i]->offset_high);

        gates[index]->offset_low = GET_LOW_ISR(&foo);
        gates[index]->offset_high = GET_HIGH_ISR(&foo);

        return 0;
}

void cleanup_module(void) {
        printk("cleanup entry %d\n", index);

        gates[index]->offset_low = GET_LOW_ISR(original_handlers[index]);
        gates[index]->offset_high = GET_HIGH_ISR(original_handlers[index]);
}