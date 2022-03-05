#include "data.h"
#include "instruction.h"
#include "rung.h"

#include "log.h"

#include "config.h"
#include "schema.h"

#include "hardware.h"

int dry_config(const config_t conf) {
    return STATUS_OK;
}

// enable bus communication
int dry_enable() {
    return STATUS_OK;
}

// disable bus communication
int dry_disable() {

    return STATUS_OK;
}

int dry_fetch() {

    return 0;
}

int dry_flush() {
    return 0;
}

void dry_dio_read(unsigned int n, uint8_t *bit) {
    return;
}

void dry_dio_write(const unsigned char *buf, unsigned int n, uint8_t bit) {
    return;
}

void dry_dio_bitfield(const uint8_t *mask, uint8_t *bits) {
    return;
}

void dry_data_read(unsigned int index, uint64_t *value) {
    return;
}

void dry_data_write(unsigned int index, uint64_t value) {

    return;
}

struct hardware Dry = {
        HW_DRY, 0,        // error code
        "no hardware",
        dry_enable,       // enable
        dry_disable,      // disable
        dry_fetch,        // fetch
        dry_flush,        // flush
        dry_dio_read,     // dio_read
        dry_dio_write,    // dio_write
        dry_dio_bitfield, // dio_bitfield
        dry_data_read,    // data_read
        dry_data_write,   // data_write
        dry_config,       // hw_config
        };

