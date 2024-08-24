#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"

#include "piuio.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "pio_usb.h"
#include "tusb.h"

typedef struct
{
    uint32_t buttons;
    uint8_t joy0;
    uint8_t joy1;
    uint8_t joy2;
    uint8_t joy3;
} report_t;

report_t hid_report = {0};

#define PIN_5V_EN 18
#define PIN_USB_HOST_DP 16

void setup_hardware()
{
    // rp2040 feather has
    gpio_init(PIN_5V_EN);
    gpio_set_dir(PIN_5V_EN, GPIO_OUT);

    // toggling power like this causes a current spike, so ignore it.
    // gpio_put(PIN_5V_EN, 0);
    // sleep_ms(1000);
    gpio_put(PIN_5V_EN, 1);
}

// core1: handle host events
void core1_main()
{
    sleep_ms(10);

    // Use tuh_configure() to pass pio configuration to the host stack
    // Note: tuh_configure() must be called before
    pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
    pio_cfg.pin_dp = PIN_USB_HOST_DP;
    tuh_configure(1, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, &pio_cfg);

    // To run USB SOF interrupt in core1, init host stack for pio_usb (roothub
    // port1) on core1
    tuh_init(1);

    while (true)
    {
        tuh_task(); // tinyusb host task

        // this is blocking.
        piuio_task();
    }
}

int main(void)
{
    // default 125MHz is not apropriate. Sysclock should be multiple of 12MHz.
    set_sys_clock_khz(120000, true);

    DebugSetup();

    memset(&hid_report, 0x00, sizeof(hid_report));
    piuio_init();

    setup_hardware();

    multicore_reset_core1();
    // all USB host task run in core1
    multicore_launch_core1(core1_main);

    tusb_init();

    while (1)
    {
        // tinyusb device task
        tud_task();

        if (tud_hid_ready())
        {
            hid_report.buttons = current_button_state.raw;
            tud_hid_n_report(0x00, 0x01, &hid_report, sizeof(hid_report));
        }
    }
}

// USB Host

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+

void tuh_mount_cb(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    DebugPrintf("New USB Device %04x:%04x", vid, pid);

    if (vid == PIUIO_VID && pid == PIUIO_PID)
    {
        DebugPrintf("PIUIO Found. %d", dev_addr);

        piuio_dev_addr = dev_addr;
    }
}

void tuh_umount_cb(uint8_t dev_addr)
{
    (void)dev_addr;

    DebugPrintf("device disconnected %d", dev_addr);

    piuio_dev_addr = 0;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
    if (report_id == 0x02 &&
        report_type == HID_REPORT_TYPE_OUTPUT &&
        bufsize == PIUIO_HID_NUM_LIGHTS)
    {
        // only parse changes in lights.
        if (memcmp(buffer, current_hid_light_state.raw_buff, PIUIO_HID_NUM_LIGHTS))
        {
            // DebugOutputBuffer("CHG:", current_hid_light_state.raw_buff, PIUIO_HID_NUM_LIGHTS);

            memcpy(current_hid_light_state.raw_buff, buffer, PIUIO_HID_NUM_LIGHTS);
            piuio_parse_hid();
        }
    }
}