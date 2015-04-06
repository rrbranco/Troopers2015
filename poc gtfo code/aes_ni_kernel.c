#include <linux/module.h>
#include <linux/device.h>
#include <linux/highmem.h>
#include <linux/kallsyms.h>
#include <linux/tty.h>
#include <linux/ptrace.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <asm/io.h>
#include "include/rop.h"
#include <linux/smp.h>

#define _GNU_SOURCE

#define FEATURE_CONFIG_MSR 0x13c

MODULE_LICENSE("GPL");

#define MASK_LOCK_SET           0x00000001
#define MASK_AES_ENABLED        0x00000002
#define MASK_SET_LOCK           0x00000000

void * read_msr_in_c(void * CPUInfo)
{
        int *pointer;
        pointer=(int *) CPUInfo;
        asm volatile("rdmsr" : "=a"(pointer[0]), "=d"(pointer[3]) : "c"(FEATURE_CONFIG_MSR));
        return NULL;
}

int __init
init_module (void)
{
        int CPUInfo[4]={-1};

        printk(KERN_ALERT "AES-NI testing module\n");

        read_msr_in_c(CPUInfo);

        printk(KERN_ALERT "read: %d %d from MSR: 0x%x \n", CPUInfo[0], CPUInfo[3], FEATURE_CONFIG_MSR);

        if (CPUInfo[0] & MASK_LOCK_SET)
		                printk(KERN_ALERT "MSR:  lock bit is set\n");

        if (!(CPUInfo[0] & MASK_AES_ENABLED))
                printk(KERN_ALERT "MSR:  AES_DISABLED bit is NOT set - AES-NI is ENABLED\n");

        return 0;
}

void __exit
cleanup_module (void)
{
        printk(KERN_ALERT "AES-NI MSR unloading \n");
}