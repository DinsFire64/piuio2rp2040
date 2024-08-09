#include "debug.h"

bool debug_setup = false;

#if ENABLE_DEBUG_UART
char uart_output[DEBUG_UART_OUT_BUFF_SIZE];
#endif

void DebugSetupDebugPin()
{
  gpio_init(PIN_DEBUG);
  gpio_set_dir(PIN_DEBUG, GPIO_OUT);
  gpio_put(PIN_DEBUG, 1);
}

void pulseDebugPin()
{
  gpio_put(PIN_DEBUG, 0);
  sleep_us(2);
  gpio_put(PIN_DEBUG, 1);
}

void DebugSetup()
{
  if (debug_setup)
  {
    return;
  }

  DebugSetupDebugPin();

#if ENABLE_DEBUG_UART
  uart_init(DEBUG_UART_SLOT, DEBUG_UART_BAUD);

  // Set the GPIO pin mux to the UART - 0 is TX, 1 is RX
  gpio_set_function(DEBUG_UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(DEBUG_UART_RX_PIN, GPIO_FUNC_UART);

  uart_puts(DEBUG_UART_SLOT, "start " __DATE__ " " __TIME__ END_LINE);
#endif

  debug_setup = true;
}

#if ENABLE_DEBUG_UART
void DebugPrintf(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vsprintf(uart_output, fmt, args);

  uart_puts(DEBUG_UART_SLOT, uart_output);
  uart_puts(DEBUG_UART_SLOT, END_LINE);

  va_end(args);
}
#endif

#if ENABLE_BUFFER_DUMP
void DebugOutputBuffer(const char *prefix, uint8_t buff[], int len)
{
  uart_puts(DEBUG_UART_SLOT, prefix);
  uart_puts(DEBUG_UART_SLOT, ",");

  sprintf(uart_output, "%03d", len);
  uart_puts(DEBUG_UART_SLOT, uart_output);
  uart_puts(DEBUG_UART_SLOT, ",");

  for (int i = 0; i != len; i++)
  {
    sprintf(uart_output, "%02X ", buff[i]);
    uart_puts(DEBUG_UART_SLOT, uart_output);
  }

  uart_puts(DEBUG_UART_SLOT, END_LINE);
}
#endif

#ifdef ENABLE_DEBUG_UART
int DebugTinyUSBPrintf(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vsprintf(uart_output, fmt, args);

  uart_puts(DEBUG_UART_SLOT, uart_output);

  va_end(args);
}
#endif