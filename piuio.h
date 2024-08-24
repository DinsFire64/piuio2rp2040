#ifndef _PIUIO_H_
#define _PIUIO_H_

#include <stdint.h>
#include "tusb.h"

#define PIUIO_VID 0x0547
#define PIUIO_PID 0x1002

#define PIUIO_BUFF_SIZE 8
#define PIUIO_MSG_REQ 0xAE

// five player, five player, neon, four marquee, two coin.
#define PIUIO_HID_NUM_LIGHTS (5 + 5 + 1 + 4 + 2)
#define PIUIO_HID_BTN_NUM 32

#define NUM_SENSORS 4

#define REACTIVE_PIU_ORDER true

#pragma pack(push, 1)

/*
 * ------------------------------------------------
 * -------------- BITS FROM PIUIO  ----------------
 * ------------------------------------------------
 */
typedef union
{
    struct
    {
        bool btn_UL_U : 1;
        bool btn_UR_D : 1;
        bool btn_C_L : 1;
        bool btn_LL_R : 1;
        bool btn_LR_START : 1;
        bool btn_SELECT : 1;
        bool btn_MENU_LEFT : 1;
        bool btn_MENU_RIGHT : 1;
    };
    uint8_t raw;
} piuio_player_byte_t;

typedef union
{
    struct
    {
        bool pad0 : 1;
        bool btn_TEST : 1;
        bool btn_COIN : 1;
        bool pad3 : 1;
        bool pad4 : 1;
        bool pad5 : 1;
        bool btn_SERVICE : 1;
        bool pad7 : 1;
        bool pad8 : 1;
    };
    uint8_t raw;
} piuio_cabinet_byte_t;

// this word is entirely made up by me, not related to piuio.
// needed to combine the two cabinet words from the 64b from the piuio
// and it just holds all of the info we need for an itg/piu cab combined.
typedef union
{
    struct
    {
        piuio_player_byte_t p1;
        piuio_player_byte_t p2;
        piuio_cabinet_byte_t cab;
        uint8_t byte3;
    };
    uint32_t raw;
} piuio_input_state_t;

/*
 * ----------------------------------------------
 * -------------- BITS TO PIUIO  ----------------
 * ----------------------------------------------
 */

typedef union
{
    struct
    {
        bool lamp_maq_lr : 1;
        bool lamp_maq_ll : 1;
        bool lamp_maq_ur : 1;
        bool lamp_coin0_pulse : 1;
        bool lamp_coin1_usb_enable : 1;
        bool pad5 : 1;
        bool pad6 : 1;
        bool pad7 : 1;
    };
    uint8_t raw;
} piuio_cabinet_light_byte_t;

typedef union
{
    struct
    {
        bool pad0 : 1;
        bool pad1 : 1;
        bool lamp_neon : 1;
        bool pad3 : 1;
        bool pad4 : 1;
        bool pad5 : 1;
        bool pad6 : 1;
        bool pad7 : 1;
    };
    uint8_t raw;
} piuio_neon_light_byte_t;

typedef union
{
    struct
    {
        uint8_t mux_setting : 2;
        bool lamp_ul : 1;
        bool lamp_ur : 1;
        bool lamp_c : 1;
        bool lamp_ll : 1;
        bool lamp_lr : 1;

        // this is only valid on the "p2" side for some
        // reason andamiro crossed the byte boundry...
        bool lamp_mar_ul_on_p2 : 1;
    };
    uint8_t raw;
} piuio_player_light_byte_t;

typedef union
{
    struct
    {
        piuio_player_light_byte_t p1_lamps;
        piuio_neon_light_byte_t lamp_neons;
        piuio_player_light_byte_t p2_lamps;
        piuio_cabinet_light_byte_t cabinet_lamps;
        uint8_t byte4;
        uint8_t byte5;
        uint8_t byte6;
        uint8_t byte7;
    };
    uint64_t raw;
    uint8_t raw_buff[PIUIO_BUFF_SIZE];
} piuio_output_state_t;

typedef union
{
    struct
    {
        uint8_t hid_lamp_p1_ul;
        uint8_t hid_lamp_p1_ur;
        uint8_t hid_lamp_p1_c;
        uint8_t hid_lamp_p1_ll;
        uint8_t hid_lamp_p1_lr;

        uint8_t hid_lamp_p2_ul;
        uint8_t hid_lamp_p2_ur;
        uint8_t hid_lamp_p2_c;
        uint8_t hid_lamp_p2_ll;
        uint8_t hid_lamp_p2_lr;

        uint8_t hid_lamp_neon;

        uint8_t hid_lamp_mar_ul;
        uint8_t hid_lamp_mar_ur;
        uint8_t hid_lamp_mar_ll;
        uint8_t hid_lamp_mar_lr;

        uint8_t hid_lamp_coin0;
        uint8_t hid_lamp_coin1;
    };
    uint8_t raw_buff[PIUIO_HID_NUM_LIGHTS];
} piuio_hid_output_state_t;

#pragma pack(pop)

extern uint8_t piuio_dev_addr;

extern piuio_hid_output_state_t current_hid_light_state;

extern piuio_output_state_t current_lamp_state;
extern piuio_input_state_t current_button_state;

extern bool piuio_hid_enabled;

void piuio_task();
void piuio_init();

void piuio_parse_hid();

#endif