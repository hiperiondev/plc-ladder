/*
 * plc_common.h
 *
 *  Created on: 17 feb. 2022
 *      Author: egonzalez
 */

#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include <poll.h>

#include "data.h"
#include "rung.h"

#define MILLION 1000000
#define THOUSAND 1000

#define MAXRUNG 256

#define FLOAT_PRECISION 0.000001

typedef enum {
    ST_STOPPED, //
    ST_RUNNING  //
} STATUSES;

typedef enum {
    N_ERR = -20,     // ERROR CODES are negative
    ERR_OVFLOW,      //
    ERR_TIMEOUT,     //
    ERR_HARDWARE,    //
    ERR_BADOPERATOR, //
    ERR_BADCOIL,     //
    ERR_BADINDEX,    //
    ERR_BADOPERAND,  //
    ERR_BADFILE,     //
    ERR_BADCHAR,     //
    ERR_BADPROG,     //
} ERRORCODES;

typedef enum {
    IE_PLC,         //
    IE_BADOPERATOR, //
    IE_BADCOIL,     //
    IE_BADINDEX,    //
    IE_BADOPERAND,  //
    IE_BADFILE,     //
    IE_BADCHAR,     //
    N_IE            //
} IL_ERRORCODES;

/**
 * @brief The timer struct.
 * struct which represents  a timer state at a given cycle

typedef struct timer {
    long S; ///scale; S=1000=>increase every 1000 cycles. STEP= 10 msec=> increase every 10 sec
    long sn;    ///internal counter used for scaling
    long V; ///value
    BIT(Q); ///output
    long P; ///Preset value
    BIT(ONDELAY);   ///1=on delay, 0 = off delay
    BIT(START); ///start command: must be on to count
    BIT(RESET); ///down command: sets V = 0
    //BIT(MASK);///true if timer is forced to up or down
    char *nick;    //[NICKLEN];
} *dt_t;
 */
/***********************plc_t*****************************/
/**
 * @brief The digital_input struct

typedef struct digital_input {
    BIT(I);   ///contact value
    BIT(RE);   ///rising edge
    BIT(FE);   ///falling edge
    BIT(EDGE); ///true if value changed
    BIT(MASK); /// true if forced 1
    BIT(N_MASK); /// true if forced 0
    char *nick; //[NICKLEN];///nickname
} *di_t;
 */
/**
 * @brief The digital_output struct

typedef struct digital_output {
    BIT(Q); //contact
    BIT(SET); //set
    BIT(RESET); //reset
    BIT(MASK); /// true if forced true
    BIT(N_MASK); /// true if forced false
    char *nick; //[NICKLEN];//nickname
} *do_t;
 */
/**
 * @brief The analog_io  struct
typedef struct analog_io {
    double V; /// data value
    double min; ///range for conversion to/from raw data
    double max;
    double mask; ///forced value mask
    char *nick; //[NICKLEN];///nickname
} *aio_t;
*/
/**
 * @brief The blink struct
 * struct which represents a blinker

typedef struct blink {
    BIT(Q); ///output
    long S; ///scale; S=1000=>toggle every 1000 cycles. STEP= 10 msec=> toggle every 10 sec
    long sn;    ///internal counter for scaling
    char *nick; //[NICKLEN];
} *blink_t;
*/
/**
 * @brief The mvar struct
 * struct which represents a memory register / counter

typedef struct mvar {
    uint64_t V;     ///TODO: add type
    BIT(RO);    ///1 if read only;
    BIT(DOWN);  ///1: can be used as a down counter
    BIT(PULSE);     ///pulse for up/downcounting
    BIT(EDGE);      ///edge of pulse
    BIT(SET);       ///set pulse
    BIT(RESET);     ///reset pulse
    //BIT(MASK); ///true if pulse is set
    char *nick;    //[NICKLEN];   ///nickname
} *mvar_t;
 */
/**
 * @brief The mreal struct
 * struct which represents a real number memory register

typedef struct mreal {
    double V;     ///TODO: add type
    BIT(RO);    ///1 if read only;
    char *nick; //[NICKLEN];   ///nickname
} *mreal_t;
 */
/**
 * @brief The PLC_regs struct
 * The struct which contains all the software PLC registers

//TODO: should masks and edges be packed inside di_t, dq_t?
typedef struct PLC_regs {
    //hardware_t hw;
    ///hardware interface
    BYTE *inputs;   ///digital input values buffer
    uint64_t *real_in; ///analog raw input values buffer
    BYTE *outputs;  ///digital output values buffer
    uint64_t *real_out; ///analog raw output values buffer

    BYTE command;   ///serial command from plcpipe
    BYTE response;  ///response to named pipe

    BYTE update; ///binary mask of state update
    int status;    ///0 = stopped, 1 = running, negative = error

    BYTE ni; ///number of bytes for digital inputs
    di_t di; ///digital inputs

    BYTE nq; ///number of bytes for digital outputs
    do_t dq; ///the digital outputs

    BYTE nai; ///number of analog input channels
    aio_t ai; ///the analog inputs

    BYTE naq; ///number of analog output channels
    aio_t aq; ///the analog outputs

    BYTE nt; ///number of timers
    dt_t t; ///the timers

    BYTE ns; ///number of blinkers
    blink_t s; ///the blinkers

    BYTE nm; ///number of memory counters
    mvar_t m; ///the memory

    BYTE nmr; ///number of memory registers
    mreal_t mr; ///the memory

    rung_t *rungs;
    BYTE rungno; //256 rungs should suffice

    long step; //cycle time in milliseconds
    char *response_file; //pipe to send response.
    struct pollfd com[1];  //polling on a file descriptor for external
                           //"commands", this will be thrown away and
                           //replaced by usleep
                           //FIXME: throw this feature away
    struct PLC_regs *old; //pointer to previous state
} *plc_t;
 */

#endif /* INCLUDE_COMMON_H_ */
