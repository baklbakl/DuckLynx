#include <stdint.h>
#include "register.h"

//Forward declare the functions that are used in the vector table.
void resetInterruptHandler(void);
void nonMaskableInterruptHandler(void);
void faultInterruptHandler(void);
void defaultInterruptHandler(void);
extern int main(void);

//Space in ram for the stack. If weird behavior make this bigger
static uint32_t stack[256];

//The vector table. Contains the stack pointer at the start of it and all the interrupt / fault handlers after
//NOLINTBEGIN(modernize-use-nullptr, performance-no-int-to-ptr)
__attribute__ ((section(".vectorTable")))
void (* const vectorTable[])(void) = {
    (void (*)(void))((uint32_t)stack + sizeof(stack)),   // The initial stack pointer
    resetInterruptHandler,                               // The reset handler
    nonMaskableInterruptHandler,                                  // The NMI handler
    faultInterruptHandler,                               // The hard fault handler
    defaultInterruptHandler,                      // The MPU fault handler
    defaultInterruptHandler,                      // The bus fault handler
    defaultInterruptHandler,                      // The usage fault handler
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    defaultInterruptHandler,                      // SVCall handler
    defaultInterruptHandler,                      // Debug monitor handler
    0,                                      // Reserved
    defaultInterruptHandler,                      // The PendSV handler
    defaultInterruptHandler,                      // The SysTick handler
    defaultInterruptHandler,                      // GPIO Port A
    defaultInterruptHandler,                      // GPIO Port B
    defaultInterruptHandler,                      // GPIO Port C
    defaultInterruptHandler,                      // GPIO Port D
    defaultInterruptHandler,                      // GPIO Port E
    defaultInterruptHandler,                      // UART0 Rx and Tx
    defaultInterruptHandler,                      // UART1 Rx and Tx
    defaultInterruptHandler,                      // SSI0 Rx and Tx
    defaultInterruptHandler,                      // I2C0 Master and Slave
    defaultInterruptHandler,                      // PWM Fault
    defaultInterruptHandler,                      // PWM Generator 0
    defaultInterruptHandler,                      // PWM Generator 1
    defaultInterruptHandler,                      // PWM Generator 2
    defaultInterruptHandler,                      // Quadrature Encoder 0
    defaultInterruptHandler,                      // ADC Sequence 0
    defaultInterruptHandler,                      // ADC Sequence 1
    defaultInterruptHandler,                      // ADC Sequence 2
    defaultInterruptHandler,                      // ADC Sequence 3
    defaultInterruptHandler,                      // Watchdog timer
    defaultInterruptHandler,                      // Timer 0 subtimer A
    defaultInterruptHandler,                      // Timer 0 subtimer B
    defaultInterruptHandler,                      // Timer 1 subtimer A
    defaultInterruptHandler,                      // Timer 1 subtimer B
    defaultInterruptHandler,                      // Timer 2 subtimer A
    defaultInterruptHandler,                      // Timer 2 subtimer B
    defaultInterruptHandler,                      // Analog Comparator 0
    defaultInterruptHandler,                      // Analog Comparator 1
    defaultInterruptHandler,                      // Analog Comparator 2
    defaultInterruptHandler,                      // System Control (PLL, OSC, BO)
    defaultInterruptHandler,                      // FLASH Control
    defaultInterruptHandler,                      // GPIO Port F
    defaultInterruptHandler,                      // GPIO Port G
    defaultInterruptHandler,                      // GPIO Port H
    defaultInterruptHandler,                      // UART2 Rx and Tx
    defaultInterruptHandler,                      // SSI1 Rx and Tx
    defaultInterruptHandler,                      // Timer 3 subtimer A
    defaultInterruptHandler,                      // Timer 3 subtimer B
    defaultInterruptHandler,                      // I2C1 Master and Slave
    defaultInterruptHandler,                      // Quadrature Encoder 1
    defaultInterruptHandler,                      // CAN0
    defaultInterruptHandler,                      // CAN1
    0,                                      // Reserved
    0,                                      // Reserved
    defaultInterruptHandler,                      // Hibernate
    defaultInterruptHandler,                      // USB0
    defaultInterruptHandler,                      // PWM Generator 3
    defaultInterruptHandler,                      // uDMA Software Transfer
    defaultInterruptHandler,                      // uDMA Error
    defaultInterruptHandler,                      // ADC1 Sequence 0
    defaultInterruptHandler,                      // ADC1 Sequence 1
    defaultInterruptHandler,                      // ADC1 Sequence 2
    defaultInterruptHandler,                      // ADC1 Sequence 3
    0,                                      // Reserved
    0,                                      // Reserved
    defaultInterruptHandler,                      // GPIO Port J
    defaultInterruptHandler,                      // GPIO Port K
    defaultInterruptHandler,                      // GPIO Port L
    defaultInterruptHandler,                      // SSI2 Rx and Tx
    defaultInterruptHandler,                      // SSI3 Rx and Tx
    defaultInterruptHandler,                      // UART3 Rx and Tx
    defaultInterruptHandler,                      // UART4 Rx and Tx
    defaultInterruptHandler,                      // UART5 Rx and Tx
    defaultInterruptHandler,                      // UART6 Rx and Tx
    defaultInterruptHandler,                      // UART7 Rx and Tx
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    defaultInterruptHandler,                      // I2C2 Master and Slave
    defaultInterruptHandler,                      // I2C3 Master and Slave
    defaultInterruptHandler,                      // Timer 4 subtimer A
    defaultInterruptHandler,                      // Timer 4 subtimer B
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    defaultInterruptHandler,                      // Timer 5 subtimer A
    defaultInterruptHandler,                      // Timer 5 subtimer B
    defaultInterruptHandler,                      // Wide Timer 0 subtimer A
    defaultInterruptHandler,                      // Wide Timer 0 subtimer B
    defaultInterruptHandler,                      // Wide Timer 1 subtimer A
    defaultInterruptHandler,                      // Wide Timer 1 subtimer B
    defaultInterruptHandler,                      // Wide Timer 2 subtimer A
    defaultInterruptHandler,                      // Wide Timer 2 subtimer B
    defaultInterruptHandler,                      // Wide Timer 3 subtimer A
    defaultInterruptHandler,                      // Wide Timer 3 subtimer B
    defaultInterruptHandler,                      // Wide Timer 4 subtimer A
    defaultInterruptHandler,                      // Wide Timer 4 subtimer B
    defaultInterruptHandler,                      // Wide Timer 5 subtimer A
    defaultInterruptHandler,                      // Wide Timer 5 subtimer B
    defaultInterruptHandler,                      // FPU
    0,                                      // Reserved
    0,                                      // Reserved
    defaultInterruptHandler,                      // I2C4 Master and Slave
    defaultInterruptHandler,                      // I2C5 Master and Slave
    defaultInterruptHandler,                      // GPIO Port M
    defaultInterruptHandler,                      // GPIO Port N
    defaultInterruptHandler,                      // Quadrature Encoder 2
    0,                                      // Reserved
    0,                                      // Reserved
    defaultInterruptHandler,                      // GPIO Port P (Summary or P0)
    defaultInterruptHandler,                      // GPIO Port P1
    defaultInterruptHandler,                      // GPIO Port P2
    defaultInterruptHandler,                      // GPIO Port P3
    defaultInterruptHandler,                      // GPIO Port P4
    defaultInterruptHandler,                      // GPIO Port P5
    defaultInterruptHandler,                      // GPIO Port P6
    defaultInterruptHandler,                      // GPIO Port P7
    defaultInterruptHandler,                      // GPIO Port Q (Summary or Q0)
    defaultInterruptHandler,                      // GPIO Port Q1
    defaultInterruptHandler,                      // GPIO Port Q2
    defaultInterruptHandler,                      // GPIO Port Q3
    defaultInterruptHandler,                      // GPIO Port Q4
    defaultInterruptHandler,                      // GPIO Port Q5
    defaultInterruptHandler,                      // GPIO Port Q6
    defaultInterruptHandler,                      // GPIO Port Q7
    defaultInterruptHandler,                      // GPIO Port R
    defaultInterruptHandler,                      // GPIO Port S
    defaultInterruptHandler,                      // PWM 1 Generator 0
    defaultInterruptHandler,                      // PWM 1 Generator 1
    defaultInterruptHandler,                      // PWM 1 Generator 2
    defaultInterruptHandler,                      // PWM 1 Generator 3
    defaultInterruptHandler                       // PWM 1 Fault
};
//NOLINTEND(modernize-use-nullptr, performance-no-int-to-ptr)

//These are symbols make by the linker so that the startup code knows where to copy .data from and to and where to zero for .bss
extern uint32_t dataLoadAddr;
extern uint32_t data;
extern uint32_t dataEnd;
extern uint32_t bss;
extern uint32_t bssEnd;

//All the registers and constants for setting up the clock
volatile uint32_t * const REGISTER_SYSCTL_RCC = (uint32_t *)(REGISTER_SYSCTL_BASE + 0x60);
const uint32_t REGISTER_SYSCTL_RCC_USESYSDIV = 0b1 << 22;
const uint32_t REGISTER_SYSCTL_RCC_BYPASS = 0b1 << 11;

const uint32_t REGISTER_SYSCTL_RCC_XTAL_SHIFT = 6;
const uint32_t REGISTER_SYSCTL_RCC_XTAL_MASK = 0b11111 << REGISTER_SYSCTL_RCC_XTAL_SHIFT;
const uint32_t REGISTER_SYSCTL_RCC_XTAL_16MHZ = 0x15 << REGISTER_SYSCTL_RCC_XTAL_SHIFT;

const uint32_t REGISTER_SYSCTL_RCC_OSCSRC = 0b11 << 4;
const uint32_t REGISTER_SYSCTL_RCC_MAINOSC_DISABLE = 0b1;

volatile uint32_t * const REGISTER_SYSCTL_RCC2 = (uint32_t *)(REGISTER_SYSCTL_BASE + 0x70);
const uint32_t REGISTER_SYSCTL_RCC2_RCC2ENABLE = 0b1U << 31;
const uint32_t REGISTER_SYSCTL_RCC2_DIV400 = 0b1 << 30;

//This includes the SYSDIV2LSB bit that is enabled by DIV400 being set.
const uint32_t REGISTER_SYSCTL_RCC2_SYSDIV2_SHIFT = 22;
const uint32_t REGISTER_SYSCTL_RCC2_SYSDIV2_MASK = 0b1111111 << REGISTER_SYSCTL_RCC2_SYSDIV2_SHIFT;
const uint32_t REGISTER_SYSCTL_RCC2_SYSDIV2_DIVIDE_5 = 0b100 << REGISTER_SYSCTL_RCC2_SYSDIV2_SHIFT;

const uint32_t REGISTER_SYSCTL_RCC2_PWRDN2 = 0b1 << 13;
const uint32_t REGISTER_SYSCTL_RCC2_BYPASS2 = 0b1 << 11;
const uint32_t REGISTER_SYSCTL_RCC2_OSCSRC = 0b111 << 4;

volatile uint32_t * const REGISTER_SYSCTL_MISC = (uint32_t *)(REGISTER_SYSCTL_BASE + 0x58);
//Main oscillator powerup interrupt status
const uint32_t REGISTER_SYSCTL_MISC_MOSCPUP = 0b1 << 8;
const uint32_t REGISTER_SYSCTL_MISC_PLL = 0b1 << 6;

volatile uint32_t * const REGISTER_SYSCTL_RIS = (uint32_t *)(REGISTER_SYSCTL_BASE + 0x50);
const uint32_t REGISTER_SYSCTL_RIS_MOSCPUP = REGISTER_SYSCTL_MISC_MOSCPUP;
const uint32_t REGISTER_SYSCTL_RIS_PLL = REGISTER_SYSCTL_MISC_PLL;

const uint32_t REGISTER_CORTEX_PERIPHERAL_BASE = 0xE000E000;
volatile uint32_t * const REGISTER_CORTEX_PERIPHERAL_CPAC = (uint32_t *)(REGISTER_CORTEX_PERIPHERAL_BASE + 0xD88);
const uint32_t REGISTER_CORTEX_PERIPHERAL_CPAC_CP10_CP11 = 0b1111 << 20;

const uint32_t REGISTER_SCB_BASE = 0xE000E000;
volatile uint32_t * const REGISTER_SCB_CFGCTRL = (uint32_t *)(REGISTER_SYSCTL_BASE + 0xD14);
const uint32_t REGISTER_SCB_CFGCTRL_UNALIGNED = 0b1 << 3;
const uint32_t REGISTER_SCB_CFGCTRL_DIV0 = 0b1 << 4;

//This does the basic setup that needs to happen before control is passed over to the main function
void resetInterruptHandler(void) {
    // Trap unaligned and divide by zero
    *REGISTER_SCB_CFGCTRL |= REGISTER_SCB_CFGCTRL_UNALIGNED | REGISTER_SCB_CFGCTRL_DIV0;


    //
    // Change the clock over to the external one and set the PLL to output 80 MHz
    // 

    // REV firmware runs the same as we have set up here except PWV div is on and at 101
    // > mrw 0x400fe060
    // 0x15a1540
    // > mrw 0x400fe070
    // 0xc1004000


    //The PLL and clock divider are disabled on reset, so there is no need to change them

    //Write 1 to the interrupt status register to clear it, so we know when the main oscillator has started up
    *REGISTER_SYSCTL_MISC |= REGISTER_SYSCTL_MISC_MOSCPUP;

    //Clear the main oscillator disable bit
    *REGISTER_SYSCTL_RCC &= ~REGISTER_SYSCTL_RCC_MAINOSC_DISABLE;

    //Wait for the main oscillator to settle
    while(!(*REGISTER_SYSCTL_RIS & REGISTER_SYSCTL_RIS_MOSCPUP)) {
        __asm(
            "nop\n"
            "nop\n"
            "nop\n"
        );
    }

    // Enable RCC2 and switch to the main oscillator. Main oscillator is 0b000 in RCC2, so the bits can be cleared. 
    *REGISTER_SYSCTL_RCC2 =  (*REGISTER_SYSCTL_RCC2 | REGISTER_SYSCTL_RCC2_RCC2ENABLE) & ~REGISTER_SYSCTL_RCC2_OSCSRC;

    // //Tell the PLL that the external crystal speed is 16MHz 
    *REGISTER_SYSCTL_RCC = (*REGISTER_SYSCTL_RCC & ~REGISTER_SYSCTL_RCC_XTAL_MASK) | REGISTER_SYSCTL_RCC_XTAL_16MHZ;

    // //Write 1 to the interrupt status register to clear it, so we know when the PLL has locked
    *REGISTER_SYSCTL_MISC |= REGISTER_SYSCTL_MISC_PLL;
    
    *REGISTER_SYSCTL_RCC2 = 
        //Disable the ÷2 after the PLL
        (REGISTER_SYSCTL_RCC2_DIV400 |
        // Divide the 400 MHz PLL by 5 to make 80 MHz for the system clock 
        (*REGISTER_SYSCTL_RCC2 & ~REGISTER_SYSCTL_RCC2_SYSDIV2_MASK) | REGISTER_SYSCTL_RCC2_SYSDIV2_DIVIDE_5)
        // Enable the PLL
        & ~REGISTER_SYSCTL_RCC2_PWRDN2
    ;

    //Wait for the PLL lock
    while(!(*REGISTER_SYSCTL_RIS & REGISTER_SYSCTL_RIS_PLL)) {
        __asm(
            "nop\n"
            "nop\n"
            "nop\n"
        );
    } 

    //Enable the clock divider
    *REGISTER_SYSCTL_RCC |= REGISTER_SYSCTL_RCC_USESYSDIV;
    //Switch to the PLL
    *REGISTER_SYSCTL_RCC2 &= ~REGISTER_SYSCTL_RCC2_BYPASS2; 


    //
    // Copy the data segment initializers from flash to SRAM.
    //
    uint32_t * src = &dataLoadAddr;
    for(uint32_t * dest = &data; dest < &dataEnd;) {
        *dest++ = *src++;
    }

    //
    // Zero fill the bss segment.
    //
    __asm("    ldr     r0, =bss\n"
          "    ldr     r1, =bssEnd\n"
          "    mov     r2, #0\n"
          "    .thumb_func\n"
          "zero_loop:\n"
          "        cmp     r0, r1\n"
          "        it      lt\n"
          "        strlt   r2, [r0], #4\n"
          "        blt     zero_loop");

    //Enable coprocessors 10 and 11 which correspond to the FPU. The bits are set to all ones which allows all code to use the FPU. See page 197 of the datasheet.
    //The FPU is enabled despite the fact that we don't use it because gcc may try to save the FPU registers on the stack when it calls a function and generate a fault.
    //No DMB instructions are needed because the FPU registers are in strongly ordered memory. See page 96-98 of the datasheet.
    *REGISTER_CORTEX_PERIPHERAL_CPAC |= REGISTER_CORTEX_PERIPHERAL_CPAC_CP10_CP11;
    
    //
    // Call the main function
    //
    main();
}

void nonMaskableInterruptHandler(void) {
    //Infinite loop so we can see where this interrupt came from with the debugger
    while(1) {
    }
}

void faultInterruptHandler(void) {
    //Infinite loop so we can see where this interrupt came from with the debugger
    while(1) {
    }
}

//All the other interrupts that don't have their own handlers
void defaultInterruptHandler(void) {
    //Infinite loop so we can see where this interrupt came from with the debugger
    while(1) {
    }
}
