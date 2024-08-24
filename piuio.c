#include "piuio.h"
#include "debug.h"

#include "tusb.h"

uint8_t current_sensor_mux = 0;

uint8_t buff_from_piuio[NUM_SENSORS][PIUIO_BUFF_SIZE];

uint8_t piuio_dev_addr = 0;

piuio_hid_output_state_t current_hid_light_state;

piuio_output_state_t current_lamp_state;
piuio_input_state_t current_button_state;

uint64_t start, end = 0;

bool piuio_hid_enabled = false;

void piuio_init()
{
    // lights are active high.
    current_lamp_state.raw = 0;

    // piuio is active low.
    memset(buff_from_piuio, 0xFF, sizeof(buff_from_piuio));

    // hid lights are active high.
    memset(current_hid_light_state.raw_buff, 0x00, sizeof(current_hid_light_state.raw_buff));

    // i like to keep track of things with active high.
    current_button_state.raw = 0;

    piuio_hid_enabled = false;
}

piuio_input_state_t make_btn_state_from_buff(uint8_t buff[])
{
    piuio_input_state_t rtn;

    // player 1 [0], player 2 [2], and both cabinets [1] & [3]
    // combined into one word.
    rtn.raw = buff[0] |
              buff[2] << 8 |
              (buff[1] & buff[3]) << 16 |
              0xFF << 24;

    rtn.raw = ~rtn.raw;

    // return active high.
    return rtn;
}

void push_buff_to_piuio()
{
    if (piuio_dev_addr == 0)
    {
        return;
    }

    // change the current sensor being asked for p1 and p2.
    // bottom two bits are the multiplexing bits.
    current_lamp_state.p1_lamps.mux_setting = current_sensor_mux;
    current_lamp_state.p2_lamps.mux_setting = current_sensor_mux;

    // DebugOutputBuffer("OUT:", current_lamp_state.raw_buff, sizeof(current_lamp_state.raw_buff));

    tusb_control_request_t const request_to_piuio =
        {
            .bmRequestType_bit =
                {
                    .recipient = TUSB_REQ_RCPT_DEVICE,
                    .type = TUSB_REQ_TYPE_VENDOR,
                    .direction = TUSB_DIR_OUT,
                },
            .bRequest = PIUIO_MSG_REQ,
            .wValue = 0,
            .wIndex = 0,
            .wLength = sizeof(current_lamp_state.raw),
        };

    tuh_xfer_t xfer =
        {
            .daddr = piuio_dev_addr,
            .ep_addr = 0,
            .setup = &request_to_piuio,
            .buffer = &current_lamp_state.raw_buff[0],
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

    tusb_control_request_t const request_from_piuio =
        {
            .bmRequestType_bit =
                {
                    .recipient = TUSB_REQ_RCPT_DEVICE,
                    .type = TUSB_REQ_TYPE_VENDOR,
                    .direction = TUSB_DIR_IN,
                },
            .bRequest = PIUIO_MSG_REQ,
            .wValue = 0,
            .wIndex = 0,
            .wLength = sizeof(buff_from_piuio[current_sensor_mux]),
        };

    tuh_xfer_t xfer =
        {
            .daddr = piuio_dev_addr,
            .ep_addr = 0,
            .setup = &request_from_piuio,
            .buffer = &buff_from_piuio[current_sensor_mux][0],
            // calling NULL makes this blocking
            .complete_cb = NULL,
        };

    tuh_control_xfer(&xfer);

    // DebugOutputBuffer("IN:", buff_from_piuio[current_sensor_mux], sizeof(buff_from_piuio[current_sensor_mux]));

    // first sensor incoming gets the magic.
    current_button_state.raw |= make_btn_state_from_buff(buff_from_piuio[current_sensor_mux]).raw;

    if (current_sensor_mux >= (NUM_SENSORS - 1))
    {
        // if we are gotten all of the sensors, let's check for a release by building the whole message.
        piuio_input_state_t temp_new_state;
        temp_new_state.raw = 0;

        for (int i = 0; i < NUM_SENSORS; i++)
        {
            temp_new_state.raw |= make_btn_state_from_buff(buff_from_piuio[i]).raw;
        }

        current_button_state.raw = temp_new_state.raw;
        current_sensor_mux = 0;
    }
    else
    {
        current_sensor_mux++;
    }
}

void set_reactive_lights()
{
// NOTE: Lights on ITG vs PIU are swapped bytes.
#if REACTIVE_PIU_ORDER
    // this is PIU order
    current_lamp_state.p1_lamps.lamp_ul = current_button_state.p1.btn_UL_U;
    current_lamp_state.p1_lamps.lamp_ur = current_button_state.p1.btn_UR_D;
    current_lamp_state.p1_lamps.lamp_c = current_button_state.p1.btn_C_L;
    current_lamp_state.p1_lamps.lamp_ll = current_button_state.p1.btn_LL_R;
    current_lamp_state.p1_lamps.lamp_lr = current_button_state.p1.btn_LR_START;

    current_lamp_state.p2_lamps.lamp_ul = current_button_state.p2.btn_UL_U;
    current_lamp_state.p2_lamps.lamp_ur = current_button_state.p2.btn_UR_D;
    current_lamp_state.p2_lamps.lamp_c = current_button_state.p2.btn_C_L;
    current_lamp_state.p2_lamps.lamp_ll = current_button_state.p2.btn_LL_R;
    current_lamp_state.p2_lamps.lamp_lr = current_button_state.p2.btn_LR_START;
#else
    // this is ITG order.
    current_lamp_state.p2_lamps.lamp_ul = current_button_state.p1.btn_UL_U;
    current_lamp_state.p2_lamps.lamp_ur = current_button_state.p1.btn_UR_D;
    current_lamp_state.p2_lamps.lamp_c = current_button_state.p1.btn_C_L;
    current_lamp_state.p2_lamps.lamp_ll = current_button_state.p1.btn_LL_R;
    // not used in four panel
    current_lamp_state.p2_lamps.lamp_lr = current_button_state.p1.btn_LR_START;

    current_lamp_state.p1_lamps.lamp_ul = current_button_state.p2.btn_UL_U;
    current_lamp_state.p1_lamps.lamp_ur = current_button_state.p2.btn_UR_D;
    current_lamp_state.p1_lamps.lamp_c = current_button_state.p2.btn_C_L;
    current_lamp_state.p1_lamps.lamp_ll = current_button_state.p2.btn_LL_R;
    // not used in four panel
    current_lamp_state.p1_lamps.lamp_lr = current_button_state.p2.btn_LR_START;
#endif

    // these are for testing the bits.
    current_lamp_state.lamp_neons.lamp_neon = current_button_state.p1.btn_UL_U;

    current_lamp_state.cabinet_lamps.lamp_maq_ll = current_button_state.p2.btn_UL_U;
    current_lamp_state.cabinet_lamps.lamp_maq_lr = current_button_state.p2.btn_UR_D;
    current_lamp_state.cabinet_lamps.lamp_maq_ur = current_button_state.p2.btn_C_L;
    current_lamp_state.p2_lamps.lamp_mar_ul_on_p2 = current_button_state.p2.btn_LL_R;

    current_lamp_state.cabinet_lamps.lamp_coin0_pulse = current_button_state.cab.btn_TEST;
    current_lamp_state.cabinet_lamps.lamp_coin1_usb_enable = current_button_state.cab.btn_TEST;
}

void piuio_task()
{
    if (!piuio_hid_enabled)
    {
        set_reactive_lights();
    }

    start = time_us_64();

    push_buff_to_piuio();
    pull_buff_from_piuio();

    end = time_us_64();

    // DebugPrintf("%llu", (end - start));
}

void piuio_parse_hid()
{
    // stop making reactive lights when we get computer input.
    piuio_hid_enabled = true;

    current_lamp_state.p1_lamps.lamp_ul = current_hid_light_state.hid_lamp_p1_ul > 0;
    current_lamp_state.p1_lamps.lamp_ur = current_hid_light_state.hid_lamp_p1_ur > 0;
    current_lamp_state.p1_lamps.lamp_c = current_hid_light_state.hid_lamp_p1_c > 0;
    current_lamp_state.p1_lamps.lamp_ll = current_hid_light_state.hid_lamp_p1_ll > 0;
    current_lamp_state.p1_lamps.lamp_lr = current_hid_light_state.hid_lamp_p1_lr > 0;

    current_lamp_state.p2_lamps.lamp_ul = current_hid_light_state.hid_lamp_p2_ul > 0;
    current_lamp_state.p2_lamps.lamp_ur = current_hid_light_state.hid_lamp_p2_ur > 0;
    current_lamp_state.p2_lamps.lamp_c = current_hid_light_state.hid_lamp_p2_c > 0;
    current_lamp_state.p2_lamps.lamp_ll = current_hid_light_state.hid_lamp_p2_ll > 0;
    current_lamp_state.p2_lamps.lamp_lr = current_hid_light_state.hid_lamp_p2_lr > 0;

    current_lamp_state.lamp_neons.lamp_neon = current_hid_light_state.hid_lamp_neon > 0;

    current_lamp_state.cabinet_lamps.lamp_maq_ll = current_hid_light_state.hid_lamp_mar_ll > 0;
    current_lamp_state.cabinet_lamps.lamp_maq_lr = current_hid_light_state.hid_lamp_mar_lr > 0;
    current_lamp_state.cabinet_lamps.lamp_maq_ur = current_hid_light_state.hid_lamp_mar_ur > 0;
    current_lamp_state.p2_lamps.lamp_mar_ul_on_p2 = current_hid_light_state.hid_lamp_mar_ul > 0;

    current_lamp_state.cabinet_lamps.lamp_coin0_pulse = current_hid_light_state.hid_lamp_coin0 > 0;
    current_lamp_state.cabinet_lamps.lamp_coin1_usb_enable = current_hid_light_state.hid_lamp_coin1 > 0;
}