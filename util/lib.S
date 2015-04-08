/*
 * Developed by Gabriel Negreira Barbosa (pirata) and Rodrigo Rubira Branco (BSDaemon)
 *
 * License:  Beerware
 */

.intel_syntax noprefix

.text

	.global get_cr3
	.global cpuid_mtrr
	.global ia32_mtrrcap_msr
	.global ia32_mtrr_def_type
	.global generic_rdmsr_high
	.global generic_rdmsr_low
	.global cpuid_phys_addr_size

get_cr3:
	mov eax,cr3
	ret

cpuid_mtrr:
	push ebx
	push ecx
	push edx

	mov eax, 0x1
	mov edx, 0x0
	cpuid

	mov eax, edx

	pop edx
	pop ecx
	pop ebx

	ret

ia32_mtrrcap_msr:
	push ecx
	push edx

	mov eax,0
	mov ecx,0xFE
	rdmsr

	pop edx
	pop ecx

	ret

ia32_mtrr_def_type:
	push ecx
	push edx

	mov eax,0
	mov ecx,0x2FF
	rdmsr

	pop edx
	pop ecx

	ret

generic_rdmsr_high:
	push ecx
	push edx

	mov ecx, eax
	mov eax, 0x0
	mov edx, 0x0
	rdmsr

	mov eax, edx

	pop edx
	pop ecx

	ret

generic_rdmsr_low:
	push ecx
	push edx

	mov ecx, eax
	mov eax, 0x0
	mov edx, 0x0
	rdmsr

	pop edx
	pop ecx

	ret

cpuid_phys_addr_size:
	push ebx
	push ecx
	push edx

	mov eax, 0x80000008
	cpuid

	pop edx
	pop ecx
	pop ebx

	ret
