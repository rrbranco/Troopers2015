/*
 * Developed by Gabriel Negreira Barbosa (pirata) and Rodrigo Rubira Branco (BSDaemon)
 *
 * License:  Beerware
 */

.intel_syntax noprefix

.text

	.global get_cr3
	.global cache_trick

get_cr3:
	mov eax,cr3
	ret

cache_trick:
	pushad
	cli

	and eax, 0xFFFFFFE0 // zero ignored CR3 bits
	mov esi, eax // save cr3_virt addr at esi
	mov edi, edx // save cr3_phys addr at edi



	wbinvd

	// Cache all PDPT entries
	mov eax, esi
	mov ebx, [eax]
	mov ebx, [eax + 4]
	mov ebx, [eax + 8]
	mov ebx, [eax + 12]
	mov ebx, [eax + 16]
	mov ebx, [eax + 20]
	mov ebx, [eax + 24]
	mov ebx, [eax + 28]

	mov eax,esi // eax has now cr3_virt

	// Mark PDPTE 1 as not-present
	mov ecx, [eax]
	and ecx, 0xFFFFFFFE
	mov [eax], ecx

	// Mark PDPTE 2 as not-present
	mov ecx, [eax + 8]
	and ecx, 0xFFFFFFFE
	mov [eax + 8], ecx

	// Mark PDPTE 3 as not-present
	mov ecx, [eax + 16]
	and ecx, 0xFFFFFFFE
	mov [eax + 16], ecx

	// Mark PDPTE 4 as not-present
	mov ecx, [eax + 24]
	and ecx, 0xFFFFFFFE
	mov [eax + 24], ecx

	// Some random memory accesses
	nop
	mov ecx, 0x41414141
	mov edx, 0xc0002000 // 0xc0002000 virt = 0x00002000 phys
	mov [edx], ecx
	mov [edx+4], ecx
	mov [edx+8], ecx
	mov [edx+12], ecx
	mov [edx+16], ecx
	mov [edx+20], ecx
	mov [edx+24], ecx
	mov [edx+28], ecx
	mov [edx+32], ecx
	mov [edx+36], ecx
	mov [edx+40], ecx
	mov [edx+44], ecx
	mov [edx+48], ecx
	mov [edx+52], ecx
	mov ecx, [edx+56]
	mov ecx, [edx+60]
	mov ecx, [edx+64]
	mov ecx, [edx+68]
	mov ecx, [edx+72]
	mov ecx, [edx+76]
	mov ecx, [edx+80]
	mov ecx, [edx+84]
	mov ecx, [edx+88]
	mov ecx, [edx+92]
	nop

	invd // Invalidates the cache

	sti
	popad

	mov eax,0xAABBCCDD // return value just to make sure we reached the end of the code

	ret
