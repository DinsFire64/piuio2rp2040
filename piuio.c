#include "piuio.h"
#include "debug.h"

#include "tusb.h"

uint8_t buff_to_piuio[PIUIO_BUFF_SIZE];
uint8_t buff_from_piuio[PIUIO_BUFF_SIZE];

uint8_t piuio_dev_addr = 0;

uint8_t counter = 0;

void piuio_init()
{
    // lights are active high.
    memset(buff_to_piuio, 0x00, sizeof(buff_to_piuio));

    // piuio is active low.
    memset(buff_from_piuio, 0xFF, sizeof(buff_to_piuio));
}

void push_buff_to_piuio()
{
    if (piuio_dev_addr == 0)
    {
        return;
    }

    tusb_control_request_t const request =
        {
            .bmRequestType_bit =
                {
                    .recipient = TUSB_REQ_RCPT_INTERFACE,
                    .type = TUSB_REQ_TYPE_VENDOR,
                    .direction = TUSB_DIR_OUT,
                },
            .bRequest = PIUIO_MSG_REQ,
            .wValue = 0,
            .wIndex = 0,
            .wLength = sizeof(buff_to_piuio),
        };

    tuh_xfer_t xfer =
        {
            .daddr = piuio_dev_addr,
            .ep_addr = 0,
            .setup = &request,
            .buffer = &buff_to_piuio[0],
            // calling NULL makes this blocking
            .complete_cb = NULL,
        };

    tuh_control_xfer(&xfer);
}

void pull_buff_from_piuio()
{
    if (piuio_dev_addr == 0)
    {
        return;
    }

    tusb_control_request_t const request =
        {
            .bmRequestType_bit =
                {
                    .recipient = TUSB_REQ_RCPT_INTERFACE,
                    .type = TUSB_REQ_TYPE_VENDOR,
                    .direction = TUSB_DIR_IN,
                },
            .bRequest = PIUIO_MSG_REQ,
            .wValue = 0,
            .wIndex = 0,
            .wLength = sizeof(buff_from_piuio),
        };

    tuh_xfer_t xfer =
        {
            .daddr = piuio_dev_addr,
            .ep_addr = 0,
            .setup = &request,
            .buffer = &buff_from_piuio[0],
            // calling NULL makes this blocking
            .complete_cb = NULL,
        };

    tuh_control_xfer(&xfer);
}

void piuio_task()
{
    push_buff_to_piuio();
    pull_buff_from_piuio();

    // DebugOutputBuffer("PIU:", buff_from_piuio, sizeof(buff_from_piuio));
}