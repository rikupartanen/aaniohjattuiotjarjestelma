.syntax unified

.global _qspi_configure
.type _qspi_configure,%function

.global _qspi_activate
.type _qspi_activate,%function

.global _qspi_read
.type _qspi_read,%function

.global _qspi_write
.type _qspi_write,%function

.global _qspi_erase
.type _qspi_erase,%function

.global _qspi_status
.type _qspi_status,%function


/* MACROS */
.equ QSPI_BASE_S, 0x5002B000
.equ QSPI_BASE_NS, 0x4002B000

.equ GPIO_BASE_S, 0x50842500
.equ GPIO_BASE_NS, 0x40842500

.equ QSPI_BASE, QSPI_BASE_S
.equ GPIO_BASE, GPIO_BASE_S


/* SECTION .bss */
.section .bss
read_data: .word 0x0    // use for test read
test_data: .word 0x0

/* SECTION .data */
.section .data


/* SECTION .text */
.section .text


/* Configures a pin for peripheral use
 * r0 will contain pin # to be configured 
 * returns # of pin configured
 */
gpio_conf_periph:
    ldr     r1, =GPIO_BASE

    /* Set pin to High drive 0 High Drive 1*/
    /* And MCUSEL to Peripheral */
    ldr     r2, =#0x30000300

    /* Calculate offset for PIN_CNF[r0] */
    mov     r3, #0x4
    mul     r3, r0
    add     r3, #0x200

    str     r2, [r1, r3]

    bx lr

/* Configure pin selection for nRF5340 */
/* ========
 * PIN MAPS
 * ========
 * CSN P0.18 = 18
 * SCK P0.17 = 17
 * IO0 P0.13 = 13
 * IO1 P0.14 = 14
 */
 .equ QSPI_CSN, 18
 .equ QSPI_SCK, 17
 .equ QSPI_IO0, 13
 .equ QSPI_IO1, 14
qspi_pinsel:
    push    {r4, lr}

    ldr     r4, =QSPI_BASE

    /* All pins are contained in port 0
     * so we just have to set the pin number */
    // PSEL.SCK
    ldr     r0, =QSPI_SCK
    bl      gpio_conf_periph
    str     r0, [r4, #0x524]

    // PSEL.CSN
    ldr     r0, =QSPI_CSN
    bl      gpio_conf_periph
    str     r0, [r4, #0x528]

    // PSEL.IO0
    ldr     r0, =QSPI_IO0
    bl      gpio_conf_periph
    str     r0, [r4, #0x530]

  
    // PSEL.IO1
    ldr     r0, =QSPI_IO1
    bl      gpio_conf_periph
    str     r0, [r4, #0x534]


    pop     {r4, pc}



/* Set 192MHz clock to 192MHz 
 * Apparently the peripheral does not work properly
 * if clock divisor is not set to 1 so do this to make sure */ 
hfclk192_config:
    ldr     r0, =#0x50005000
    eor     r1, r1
    str     r1, [r0, #0x5B8]

    bx lr


/* Configure no extended addressing mode 
 * and send WREN */
qspi_addrconf:
    ldr     r0, =QSPI_BASE
    ldr     r1, =#0x8000000
    str     r1, [r0, #0x624]

    bx lr


/* Configures ifconfig0 as 
 * READOC    = READ2IO
 * WRITEOC   = PP
 * ADDRMODE  = 24BIT
 * DPMENABLE = Disable
 * PPSIZE    = 256Bytes
 */
/* Configures ifconfig1 as 
 * SCKDELAY    = 0x80
 * DPMEN       = Exit
 * SPIMODE     = MODE0
 *
 * SCKFREQ is derived from PCLK192M with formula
 * freq = PCLK192M / (2 * (SCKFREQ+1))
 * so set SCKFREQ to 16 with SCKFREQ 3
 */
qspi_ifconfig:
    /* ifconfig0 */
    ldr     r0, =QSPI_BASE
    mov     r1, #0x2
    str     r1, [r0, #0x544]

    /* ifconfig1 */
    ldr     r1, =#0x30000080
    str     r1, [r0, #0x600]
    bx lr   
    
/* Set bit in ENABLE */
qspi_enable:
    ldr     r0, =QSPI_BASE
    mov     r1, #1
    str     r1, [r0, #0x500]
    bx  lr

/* Set bit in TASKS_ACTIVATE */
_qspi_activate:
    ldr     r0, =QSPI_BASE
    mov     r1, #1
    str     r1, [r0]
    bx  lr


/* Set default working configuration for QSPI */
_qspi_configure:
    push    {r3, lr}
    bl      hfclk192_config
    bl      qspi_pinsel
    bl      qspi_ifconfig
    bl      qspi_addrconf
    bl      qspi_enable
    pop     {r3, pc}

/* r0 = start 	- word-aligned address to start erasing from
 * r1 = len 	- how much to erase. 0 = 4KB, 1 = 64KB, 2 = ALL
 */
_qspi_erase:
    cmp     r1, #2
    bhi     .end

    ldr     r3, =QSPI_BASE
    str     r0, [r3, #0x51C]	// ERASE.PTR
    str     r1, [r3, #0x520]	// ERASE.LEN

    mov     r1, #1
    str     r1, [r3, #0x00C]	// TASKS_ERASESTART

    .end:
    bx lr   

/* r0 = *tx_buff - pointer to buffer containing data to be written
 * r1 = tx_len   - length of buffer in bytes, has to be a multiple of 4
 * r2 = dst      - address to write at
 * returns nothing
 */
_qspi_write:
    ldr     r3, =QSPI_BASE

    str     r2, [r3, #0x510]    // WRITE.DST
    str     r0, [r3, #0x514]    // WRITE.SRC
    str     r1, [r3, #0x518]    // WRITE.CNT

    mov     r1, #1
    str     r1, [r3, #0x008]     // TASKS_WRITESTART

    bx  lr


/* r0 = *rx_buff - pointer to buffer to hold read data
 * r1 = rx_len   - length of buffer in bytes
 * r2 = src	 - address to read from
 * returns nothing
 */
_qspi_read:
    ldr     r3, =QSPI_BASE

    str     r2, [r3, #0x504]    // READ.SRC
    str     r0, [r3, #0x508]    // READ.DST
    str     r1, [r3, #0x50C]    // READ.CNT

    mov     r1, #1
    str     r1, [r3, #0x004]    // TASKS_READSTART

    bx lr

/* Returns contents of QSPI status register */
_qspi_status:
    ldr     r1, =QSPI_BASE
    ldr     r0, [r1, #0x604]
    bx lr
