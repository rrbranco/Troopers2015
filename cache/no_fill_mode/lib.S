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



	// Enter no-fill mode
	mov eax, cr0
	or eax, 0x40000000
	mov cr0, eax



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



	/*
	 * Commented Code 1
	 *
	 * "Invalidates" the cache with invd and wait some time for it to complete.
	 * Outside of no-fill mode, cache is invalidated. Inside no-fill mode (the case here)
	 * no invalidation will happen - thus, there will be a crash due to write-back performed
	 * after leaving no-fill mode.
	 *
	 * To test this behavior, uncomment the following cpde and make sure
	 * the "Commented Code 2" is commented.
	*/

	/*
	invd // "Invalidates" the cache

	mov ecx, 70000000
	mov edx, 0xc0002000
	teste:
		mov eax, [edx+100]
		add eax, 100
		loop teste
	*/



	// Back to normal cache mode
	mov eax, cr0
	and eax, 0xBFFFFFFF
	mov cr0, eax



	/*
	 * Commented Code 2
	 *
	 * Mark the PDPTEs as "present". There will be a cache
	 * write-back before this code because the no-fill mode ended.
	 *
	 * To test this behavior, uncomment the following code and make sure
	 * the "Commented Code 1" is commented.
	 */

	mov eax,esi // eax has now cr3_virt

	// Mark PDPTE 1 as present
	mov ecx, [eax]
	or ecx, 0x00000001
	mov [eax], ecx

	// Mark PDPTE 2 as present
	mov ecx, [eax + 8]
	or ecx, 0x00000001
	mov [eax + 8], ecx

	// Mark PDPTE 3 as present
	mov ecx, [eax + 16]
	or ecx, 0x00000001
	mov [eax + 16], ecx

	// Mark PDPTE 4 as present
	mov ecx, [eax + 24]
	or ecx, 0x00000001
	mov [eax + 24], ecx



	sti
	popad

	mov eax,0xAABBCCDD // return value just to make sure we reached the end of the code

	ret
