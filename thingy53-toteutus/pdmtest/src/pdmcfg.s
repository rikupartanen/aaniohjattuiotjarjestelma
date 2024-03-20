//TODO: make this work in C
.section .text

// Offsets for SECURE application
.equ PDM_BASE,  0x50026000
.equ PDM_RATIO, 0x50026520

.global set_pdm_ratio
.type set_pdm_ratio,%function

.syntax unified

// Set pdm ratio to 64 (0) or 80 (1)
// r0 will contain ratio option
set_pdm_ratio:
    //cmp     r0, #1  // valid options are 0 and 1
    //bhi     .exit   // so just exit if we are given above 1

    ldr     r1, =PDM_RATIO
    str     r0, [r1]

    .exit:
    bx lr

