/** @file
    Generic remotes and sensors using PT2260/PT2262 SC2260/SC2262 EV1527 protocol.

    Copyright (C) 2015 Tommy Vestermark
    Copyright (C) 2015 nebman

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
*/
/**
Generic remotes and sensors using PT2260/PT2262 SC2260/SC2262 EV1527 protocol.

Tested devices:
- SC2260
- EV1527
*/

#include "decoder.h"

static int generic_remote_ev1527_callback(r_device *decoder, bitbuffer_t *bitbuffer)
{
    data_t *data;
    uint8_t *b;
    int id;
    int cmd;
    char const *cmd_str;

    if (bitbuffer->bits_per_row[0] != 25)
        return DECODE_ABORT_LENGTH;
    b = bitbuffer->bb[0];

    // invert bits, short pulse is 0, long pulse is 1
    b[0] = ~b[0];
    b[1] = ~b[1];
    b[2] = ~b[2];

    id  = (b[0] << 12) | (b[1] << 4) | (b[2] >> 4);
    cmd = b[2] & 0x0F;

    /* clang-format off */
    data = data_make(
            "model",    "",             DATA_STRING, "Generic EV1527",
            "id",       "ID (20bit)",   DATA_FORMAT, "0x%x", DATA_INT, id,
            "data",     "Data (4bit)",  DATA_FORMAT, "0x%x", DATA_INT, cmd,
            NULL);
    /* clang-format on */

    decoder_output_data(decoder, data);
    return 1;
}

static char const *const output_fields[] = {
        "model",
        "id",
        "data",
        NULL,
};

r_device const generic_remote_ev1527 = {
        .name        = "Generic Remote EV1527",
        .modulation  = OOK_PULSE_PWM,
        .short_width = 400,
        .long_width  = 1200,
        .reset_limit = 12000,
        .gap_limit   = 1500,
        .sync_width  = 0,   // No sync bit used
        .tolerance   = 200, // us
        .decode_fn   = &generic_remote_ev1527_callback,
        .fields      = output_fields,
};
