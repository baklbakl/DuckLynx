#pragma once

#ifndef REGISTER_H
#define REGISTER_H
//
// register.h naming scheme
//
//Generally register constants should start with 'REGISTER_' and then go from largest to smallest category.

//
// Registers 
//

//For a section of registers. Think GPIO or SYSCTL. Ex. REGISTER_SYSCTL_BASE
//REGISTER_<section name>_BASE

//For a normal register. Think RCC of SYSCTL. Registers should be referenced by their datasheet name.s Ex. REGISTER_SYSCTL_RCC
//REGISTER_<section name>_<register name>

//For registers that get repeated many times. Think GPIO.
//The base of an instance. Sum of instanace offset and section base. Ex. REGISTER_GPIO_A_BASE
//REGISTER_<section name>_<register instance>_BASE
//The offset into a instance that a register can be found. Ex. REGISTER_GPIO_AFSEL_OFFSET
//REGISTER_<section name>_<register name>_OFFSET
//The actual register for specific instance. Sum of instance base and register offset. Ex. REGISTER_GPIO_A_AFSEL.  
//REGISTER_<section name>_<register instance>_<register name>

//
// Register contents for registers that have more than one field
// 

//Single bit fields. The bitmask for the for the 1 bit wide field. Ex. REGISTER_SYSCTL_RCC_BYPASS
//<register from above>_<field name>
//For instance bitmaps in the PERIPHCTL section of SYSCTL they should be named
//REGISTER_SYSCTL_PERIPHCTL_<section>_<instance>_INSTANCEMASK

//Multibit fields
//Bitmask of the field. Ex. REGISTER_SYSCTL_RCC_XTAL_MASK
//<register from above>_<field name>_MASK
//Bitwise shift of a value to line up in the field.  Ex. REGISTER_SYSCTL_RCC_XTAL_SHIFT 
//<register from above>_<field name>_SHIFT
//A specific configuration of a field. Ex. REGISTER_SYSCTL_RCC_XTAL_16MHZ. Should be shifted to the correct location
//<register from above>_<field name>_<configuration name>

#include <stdint.h>

//
// SYSCTL
//

static const uint32_t REGISTER_SYSCTL_BASE = 0x400FE000;

static volatile uint32_t * const REGISTER_SYSCTL_RCC = (uint32_t *)(REGISTER_SYSCTL_BASE + 0x60);

//Periphctl is the combination of peripheral Present, Clock Gating, Reset, and Ready registers
static const uint8_t REGISTER_SYSCTL_PERIPHCTL_GPIO_OFFSET = 0x08;
static const uint8_t REGISTER_SYSCTL_PERIPHCTL_UART_OFFSET = 0x18;
static const uint8_t REGISTER_SYSCTL_PERIPHCTL_WGPTM_OFFSET = 0x5C;

static const uint32_t REGISTER_SYSCTL_PERIPHCTL_GPIO_G_INSTANCEMASK = 0b1 << 6;
static const uint32_t REGISTER_SYSCTL_PERIPHCTL_GPIO_H_INSTANCEMASK = 0b1 << 7;
static const uint32_t REGISTER_SYSCTL_PERIPHCTL_GPIO_M_INSTANCEMASK = 0b1 << 11;

//
// GPIO
//

static const uint32_t REGISTER_GPIO_BASE = 0x40004000;

static const uint8_t REGISTER_GPIO_PIN_0 = 0b00000001;
static const uint8_t REGISTER_GPIO_PIN_1 = 0b00000010;
static const uint8_t REGISTER_GPIO_PIN_2 = 0b00000100;
static const uint8_t REGISTER_GPIO_PIN_3 = 0b00001000;
static const uint8_t REGISTER_GPIO_PIN_4 = 0b00010000;
static const uint8_t REGISTER_GPIO_PIN_5 = 0b00100000;
static const uint8_t REGISTER_GPIO_PIN_6 = 0b01000000;
static const uint8_t REGISTER_GPIO_PIN_7 = 0b10000000;

static const uint32_t REGISTER_GPIO_GPIODR_OFFSET = 0x0;
static const uint32_t REGISTER_GPIO_GPIODIR_OFFSET = 0x400;
static const uint32_t REGISTER_GPIO_AFSEL_OFFSET = 0x420;
static const uint32_t REGISTER_GPIO_GPIODEN_OFFSET = 0x51C;
static const uint32_t REGISTER_GPIO_AMSEL_OFFSET = 0x528;

static const uint32_t REGISTER_GPIO_GPIOPCTL_UART = 0x1;

static const uint32_t REGISTER_GPIO_G_BASE = REGISTER_GPIO_BASE + 0x5A000;
static const uint32_t REGISTER_GPIO_H_BASE = REGISTER_GPIO_BASE + 0x5B000;
static const uint32_t REGISTER_GPIO_M_BASE = REGISTER_GPIO_BASE + 0x5F000;

//
// UART
//
static const uint32_t REGISTER_UART_BASE = 0x4000C000; 
static const uint32_t REGISTER_UART_0_BASE = REGISTER_UART_BASE + 0x0;

static const uint32_t REGISTER_UART_UARTDR_OFFSET = 0;

//
// GPTM
//
static const uint32_t REGISTER_GPTM_BASE = 0x40030000;

static const uint32_t REGISTER_GPTM_GPTMCFG_OFFSET = 0x0;
//There is a field in GPTMCFG called GPTMCFG
static const uint32_t REGISTER_GPTM_GPTMCFG_GPTMCFG_MASK = 0b111;
static const uint32_t REGISTER_GPTM_GPTMCFG_GPTMCFG_SPLIT = 0x4;

//Timer n mode register
static const uint32_t REGISTER_GPTM_GPTMTAMR_OFFSET = 0x04;
static const uint32_t REGISTER_GPTM_GPTMTnMR_TnMR_OFFSET = 0;
static const uint32_t REGISTER_GPTM_GPTMTnMR_TnMR_MASK = 0b11 << REGISTER_GPTM_GPTMTnMR_TnMR_OFFSET;
static const uint32_t REGISTER_GPTM_GPTMTnMR_TnMR_ONESHOT = 0x01 << REGISTER_GPTM_GPTMTnMR_TnMR_OFFSET;

static const uint32_t REGISTER_GPTM_GPTMCTL_OFFSET = 0x0C;
static const uint32_t REGISTER_GPTM_GPTMCTL_TAEN = 0b1;
static const uint32_t REGISTER_GPTM_GPTMCTL_TAPWML = 0b1 << 6;
static const uint32_t REGISTER_GPTM_GPTMCTL_TBEN = 0b1 << 8;
static const uint32_t REGISTER_GPTM_GPTMCTL_TBPWML = 0b1 << 14;

static const uint32_t REGISTER_GPTM_GPTMTAILR_OFFSET = 0x028;
static const uint32_t REGISTER_GPTM_GPTMTAV_OFFSET = 0x50;

static const uint32_t REGISTER_GPTM_W1_BASE = REGISTER_GPTM_BASE + 0x00007000;
static volatile uint32_t * const REGISTER_GPTM_W1_GPTMTAV = (uint32_t *)(REGISTER_GPTM_W1_BASE + REGISTER_GPTM_GPTMTAV_OFFSET);

//
// DMA
//
static const uint32_t REGISTER_DMA_BASE = 0x400FF000;

static const uint32_t REGISTER_DMA_CHANNEL_8 = 0b1 << 8;
static const uint32_t REGISTER_DMA_CHANNEL_9 = 0b1 << 9;

static volatile uint32_t * const REGISTER_DMA_DMACHIS = (uint32_t *)(REGISTER_DMA_BASE + 0x504);

//
// QEI
//

static const uint32_t REGISTER_QEI_BASE = 0x4002C000;
static const uint32_t REGISTER_QEI_QEIPOS_OFFSET = 0x8;

static const uint32_t REGISTER_QEI_0_BASE = REGISTER_QEI_BASE + 0;
static volatile uint32_t * const REGISTER_QEI_0_QEIPOS = (uint32_t *)(REGISTER_QEI_0_BASE + REGISTER_QEI_QEIPOS_OFFSET);

static const uint32_t REGISTER_QEI_1_BASE = REGISTER_QEI_BASE + 0x1000;
static volatile uint32_t * const REGISTER_QEI_1_QEIPOS = (uint32_t *)(REGISTER_QEI_1_BASE + REGISTER_QEI_QEIPOS_OFFSET);

#endif
