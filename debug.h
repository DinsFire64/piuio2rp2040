#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "pico/stdlib.h"
#include <stdio.h>
#include <stdarg.h>

#define ENABLE_DEBUG_UART true
#define ENABLE_BUFFER_DUMP true

#define DEBUG_UART_SLOT uart0
#define DEBUG_UART_BAUD 921600
#define DEBUG_UART_OUT_BUFF_SIZE 512

#define DEBUG_UART_TX_PIN 0
#define DEBUG_UART_RX_PIN 1

#define PIN_DEBUG 4

#define END_LINE "\r\n"

extern bool debug_setup;

void DebugSetup(void);
void DebugPrintf(const char *fmt, ...);
void DebugOutputBuffer(const char *prefix, uint8_t buff[], int len);
int DebugTinyUSBPrintf(const char *fmt, ...);

void DebugSetupDebugPin();
void pulseDebugPin();

#if ENABLE_DEBUG_UART
// nothing
#else
#define DebugPrintf(char, ...) ;
#endif

#if ENABLE_BUFFER_DUMP
// nothing
#else
#define DebugOutputBuffer(char, uint8_t, int) ;
#endif

#endif
