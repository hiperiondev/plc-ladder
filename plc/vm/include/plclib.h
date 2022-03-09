#ifndef _PLCLIB_H_
#define _PLCLIB_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <inttypes.h>

#include "hardware.h"

#define MILLION  1000000
#define THOUSAND 1000

#define FLOAT_PRECISION 0.000001

typedef enum {
    ST_STOPPED,
    ST_RUNNING
} STATUSES;

typedef enum {
    LANG_LD,
    LANG_IL,
    LANG_ST,
    LANG_PLC // precompiled file
} LANGUAGES;

// boolean function blocks supported
typedef enum {
    BOOL_DI,       // digital input
    BOOL_DQ,       // digital output
    BOOL_COUNTER,  // pulse of counter
    BOOL_TIMER,    // output of timer
    BOOL_BLINKER,  // output of blinker
    N_BOOL
} BOOL_FB;

// what changed since the last cycle
typedef enum {
    CHANGED_I = 0x1,
    CHANGED_O = 0x2,
    CHANGED_M = 0x4,
    CHANGED_T = 0x8,
    CHANGED_S = 0x10,
    CHANGED_STATUS = 0x20
} CHANGE_DELTA;

/***********************plc_t*****************************/

// the digital_input struct
typedef struct digital_input {
    BIT (I);      // contact value
    BIT (RE);     // rising edge
    BIT (FE);     // falling edge
    BIT (EDGE);   // true if value changed
    BIT (MASK);   // true if forced 1
    BIT (N_MASK); // true if forced 0
    char *nick;   // [NICKLEN]; // nickname
} *di_t;

// the digital_output struct
typedef struct digital_output {
    BIT (Q);      // contact
    BIT (SET);    // set
    BIT (RESET);  // reset
    BIT (MASK);   // true if forced true
    BIT (N_MASK); // true if forced false
    char *nick;   // [NICKLEN]; // nickname
} *do_t;

// the analog_io struct
typedef struct analog_io {
    double V;    // data value
    double min;  // range for conversion to/from raw data
    double max;
    double mask; // forced value mask
    char *nick;  // [NICKLEN]; // nickname
} *aio_t;

// the timer struct.
// struct which represents  a timer state at a given cycle
typedef struct timer {
    long S;        // scale; S=1000=>increase every 1000 cycles. STEP= 10 msec=> increase every 10 sec
    long sn;       // internal counter used for scaling
    long V;        // value
    BIT (Q);       // output
    long P;        // Preset value
    BIT (ONDELAY); // 1=on delay, 0 = off delay
    BIT (START);   // start command: must be on to count
    BIT (RESET);   // down command: sets V = 0
    //BIT(MASK);   // true if timer is forced to up or down
    char *nick;    // [NICKLEN];
} *dt_t;

// the blink struct
// struct which represents a blinker
typedef struct blink {
    BIT (Q);    // output
    long S;     // scale; S=1000=>toggle every 1000 cycles. STEP= 10 msec=> toggle every 10 sec
    long sn;    // internal counter for scaling
    char *nick; // [NICKLEN];
} *blink_t;

// the mvar struct
// struct which represents a memory register / counter
typedef struct mvar {
    uint64_t V;  // TODO: add type
    BIT (RO);    // 1 if read only;
    BIT (DOWN);  // 1: can be used as a down counter
    BIT (PULSE); // pulse for up/downcounting
    BIT (EDGE);  // edge of pulse
    BIT (SET);   // set pulse
    BIT (RESET); // reset pulse
    //BIT(MASK); // true if pulse is set
    char *nick;  //[NICKLEN]; // nickname
} *mvar_t;

// the mreal struct
// struct which represents a real number memory register
typedef struct mreal {
    double V;   // TODO: add type
    BIT (RO);   // 1 if read only;
    char *nick; // [NICKLEN]; // nickname
} *mreal_t;

// the PLC_regs struct
// the struct which contains all the software PLC registers
// TODO: should masks and edges be packed inside di_t, dq_t?
typedef struct PLC_regs {
    hardware_t hw;
    // hardware interface
    uint8_t *inputs;       // digital input values buffer
    uint64_t *real_in;     // analog raw input values buffer
    uint8_t *outputs;      // digital output values buffer
    uint64_t *real_out;    // analog raw output values buffer

    uint8_t command;       // serial command from plcpipe
    uint8_t response;      // response to named pipe

    uint8_t update;        // binary mask of state update
    int status;            // 0 = stopped, 1 = running, negative = error

    uint8_t ni;            // number of bytes for digital inputs
    di_t di;               // digital inputs

    uint8_t nq;            // number of bytes for digital outputs
    do_t dq;               // the digital outputs

    uint8_t nai;           // number of analog input channels
    aio_t ai;              // the analog inputs

    uint8_t naq;           // number of analog output channels
    aio_t aq;              // the analog outputs

    uint8_t nt;            // number of timers
    dt_t t;                // the timers

    uint8_t ns;            // number of blinkers
    blink_t s;             // the blinkers

    uint8_t nm;            // number of memory counters
    mvar_t m;              // the memory

    uint8_t nmr;           // number of memory registers
    mreal_t mr;            // the memory

    rung_t *rungs;
    uint8_t rungno;        // 256 rungs should suffice

    long step;             // cycle time in milliseconds
    char *response_file;   // pipe to send response.
    struct pollfd com[1];  // polling on a file descriptor for external
                           // "commands", this will be thrown away and
                           // replaced by usleep
                           // FIXME: throw this feature away
    struct PLC_regs *old;  // pointer to previous state
} *plc_t;

/**
 * @brief start PLC 
 * @param the plc
 * @return plc with updated status
 */
plc_t plc_start(plc_t p);

/**
 * @brief stop PLC 
 * @param the plc
 * @return plc with updated status
 */
plc_t plc_stop(plc_t p);

/**
 * @brief parse IL program
 * @param a unique program identifier
 * @param the program as an allocated buffer of allocated strings
 * @param the plc to store the generated microcode to
 * @return plc with updated status
 */

/**
 * @brief load a PLC program
 * @param the local filename (path relative to config file) 
 * @param the plc
 * @return plc with updated status
 */
plc_t plc_load_program_file(const char *path, plc_t plc);

/**
 * @brief execute JMP instruction
 * @param the rung
 * @param the program counter (index of instruction in the rung)
 * @return OK or error
 */
int handle_jmp(const rung_t r, unsigned int *pc);

/**
 * @brief execute RESET instruction
 * @param the instruction
 * @param current acc value
 * @param true if we are setting a bit from a variable, 
 * false if we are setting the input of a block
 * @param reference to the plc
 * @return OK or error
 */
int handle_reset(const instruction_t op, const data_t acc, uint8_t is_bit, plc_t p);

/**
 * @brief execute SET instruction
 * @param the instruction
 * @param current acc value
 * @param true if we are setting a bit from a variable, 
 * false if we are setting the input of a block
 * @param reference to the plc
 * @return OK or error
 */
int handle_set(const instruction_t op, const data_t acc, uint8_t is_bit, plc_t p);

/**
 * @brief store value to digital outputs
 * @note values are stored in BIG ENDIAN
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int st_out(const instruction_t op, uint64_t val, plc_t p);

/**
 * @brief store value to analog outputs
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int st_out_r(const instruction_t op, double val, plc_t p);

/**
 * @brief store value to memory registers
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int st_mem(const instruction_t op, uint64_t val, plc_t p);

/**
 * @brief store value to floating point memory registers 
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int st_mem_r(const instruction_t op, double val, plc_t p);

/**
 * @brief execute STORE instruction
 * @param the value to be stored
 * @param the instruction
 * @param reference to the plc
 * @return OK or error
 */
int handle_st(const instruction_t op, const data_t val, plc_t p);

/**
 * @brief load value from digital inputs
 * values are loaded in BIG ENDIAN
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int ld_in(const instruction_t op, uint64_t *val, plc_t p);

/**
 * @brief load rising edge from digital inputs
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int ld_re(const instruction_t op, uint8_t *val, plc_t p);

/**
 * @brief load falling edge from digital inputs
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int ld_fe(const instruction_t op, uint8_t *val, plc_t p);

/**
 * @brief load value from analog inputs
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int ld_in_r(const instruction_t op, double *val, plc_t p);

/**
 * @brief load value from digital outputs
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int ld_out(const instruction_t op, uint64_t *val, plc_t p);

/**
 * @brief load value from analog outputs
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int ld_out_r(const instruction_t op, double *val, plc_t p);

/**
 * @brief load value from memory registers
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int ld_mem(const instruction_t op, uint64_t *val, plc_t p);

/**
 * @brief load value from floating point memory 
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int ld_mem_r(const instruction_t op, double *val, plc_t p);

/**
 * @brief load value from timer 
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int ld_timer(const instruction_t op, uint64_t *val, plc_t p);

/**
 * @brief execute LOAD instruction
 * @param the instruction
 * @param where to load the value
 * @param reference to the plc
 * @return OK or error
 */
int handle_ld(const instruction_t op, data_t *acc, plc_t p);

/**
 * @brief execute any stack operation
 * @param the intruction
 * @param the rung
 * @param reference to the plc
 * @return OK or error
 */
int handle_stackable(const instruction_t op, rung_t r, plc_t p);

/**
 * @brief execute IL instruction
 * @param the plc
 * @param the rung
 * @return OK or error
 */
int instruct(plc_t p, rung_t r, unsigned int *pc);

/**
 * @brief add a new rung to a plc
 * @param a unique identifier
 * @param the plc
 * @return reference to the new rung
 */
rung_t mk_rung(const char *name, plc_t p);

/**
 * @brief get rung reference from plc
 * @param p a plc
 * @param r the rung reference
 * @param idx the index
 * @return reference to rung or NULL
 */
rung_t get_rung(const plc_t p, unsigned int idx);

/**
 * @brief task to execute IL rung
 * @param timeout (usec)
 * @param pointer to PLC registers
 * @param pointer to IL rung
 * @return OK or error
 */
int task(long timeout, plc_t p, rung_t r);

/**
 * @brief task to execute all rungs
 * @param timeout (usec)
 * @param pointer to PLC registers
 * @return OK or error
 */
int all_tasks(long timeout, plc_t p);

/**
 * @brief custom project init code as plugin
 * @return OK or error
 */
int project_init();

/**
 * @brief custom project task code as plugin
 * @param pointer to PLC registers
 * @return OK or error
 */
int project_task(plc_t p);

/**
 * @brief PLC task executed in a loop
 * @param pointer to PLC registers
 * @return OK or error code
 */
int plc_task(plc_t p);

/**
 * @brief Pipe initialization executed once
 * @param pipe to poll
 * @param ref to plc
 * @return OK or error
 */
int open_pipe(const char *pipe, plc_t p);

/**
 * @brief PLC realtime loop
 * Anything in this function normally (ie. when not in error)
 * satisfies the realtime @conditions:
 * 1. No disk I/O
 * 2. No mallocs
 * This way the time it takes to execute is predictable
 * Heavy parts can timeout
 * The timing is based on poll.h
 * which is also realtime when using a preempt scheduler
 * @param the PLC
 * @return PLC with updated state
 */
plc_t plc_func(plc_t p);

/**
 * @brief force operand with value
 * @param the plc
 * @param the operand type
 * @param the operand index
 * @param the value
 * @return new plc state, or NULL in error
 */
plc_t force(plc_t p, int op, uint8_t i, char *val);

/**
 * @brief unforce operand
 * @param the plc
 * @param the operand type
 * @param the operand index
 * @param new plc state, or null in error
 */
plc_t unforce(plc_t p, int op, uint8_t i);

/**
 * @brief is an operand forced
 * @param reference to plc
 * @param operand type
 * @param input index
 * @return true if forced, false if not, error if out of bounds
 */
int is_forced(plc_t p, int op, uint8_t i);

/**
 * @brief decode inputs
 * @param pointer to PLC registers
 * @return true if input changed
 */
unsigned char dec_inp(plc_t p);

/**
 * @brief encode outputs
 * @param pointer to PLC registers
 * @return true if output changed
 */
unsigned char enc_out(plc_t p);

/**
 * @brief write values to memory variables
 * @param pointer to PLC registers
 */
void write_mvars(plc_t p);

/**
 * @brief read_mvars
 * @param pointer to PLC registers
 */
void read_mvars(plc_t p);

/**
 * @brief rising edge of input
 * @param pointer to PLC registers
 * @param type is Digital Input Or Counter
 * @param index
 * @return true if rising edge of operand, false or error otherwise
 */
int re(const plc_t p, int type, int idx);

/**
 * @brief falling edge of input
 * @param pointer to PLC registers
 * @param type is Digital Input Or Counter
 * @param index
 * @return true if falling edge of operand, false or error otherwise
 */
int fe(const plc_t p, int type, int idx);

/**
 * @brief set output
 * @param pointer to PLC registers
 * @param type is Digital Output, Timer or Counter
 * @param index
 * @return OK if success or error code
 */
int set(plc_t p, int type, int idx);

/**
 * @brief reset output
 * @param pointer to PLC registers
 * @param type is Digital Output, Timer or Counter
 * @param index
 * @return OK if success or error code
 */
int reset(plc_t p, int type, int idx);

/**
 * @brief contact value to output
 * @param pointer to PLC registers
 * @param type is Digital Output, Timer or Counter
 * @param index
 * @param value
 * @return OK if success or error code
 */
int contact(plc_t p, int type, int idx, uint8_t val);

/**
 * @brief resolve an operand value
 * @param pointer to PLC registers
 * @param type of operand
 * @param index
 * @return return value or error code
 */
int resolve(plc_t p, int type, int idx);

/**
 * @brief reset timer
 * @param pointer to PLC registers
 * @param index
 * @return OK if success or error code
 */
int down_timer(plc_t p, int idx);

/**
 * @brief construct a new plc with a configuration
 * @param number of digital inputs
 * @param number of digital outputs
 * @param number of analog inputs 
 * @param number of analog outputs
 * @param number of timers
 * @param number of pulses
 * @param number of integer memory variables
 * @param number of real memory variables
 * @param cycle time in milliseconds
 * @param hardware identifier        

 * @return configured plc
 */
plc_t new_plc(int di, int dq, int ai, int aq, int nt, int ns, int nm, int nr, int step, hardware_t hw);

/**
 * @brief copy constructor
 * @param source plc
 * @return newly allocated copy
 */
plc_t copy_plc(const plc_t plc);

/**
 * @brief dtor
 */
void clear_plc(plc_t plc);

/*configurators*/

/**
 * @brief assign name to a plc register variable
 * @param plc instance   
 * @param the type of variable (IL_OPERANDS enum value)
 * @param variable index
 * @param variable name
 * @see also data.h
 * @return plc instance with saved change or updated error status
 */
plc_t declare_variable(const plc_t p, int var, uint8_t idx, const char *val);

/**
 * @brief assign initial value to a plc register variable
 * @param plc instance   
 * @param the type of variable (IL_OPERANDS enum value)
 * @param variable index
 * @param variable initial value (serialized float or long eg.
 * "5.35" or "12345")
 * @see also data.h
 * @return plc instance with saved change or updated error status
 */
plc_t init_variable(const plc_t p, int var, uint8_t idx, const char *val);

/**
 * @brief configure a plc register variable as readonly
 * @param plc instance   
 * @param the type of variable (IL_OPERANDS enum value)
 * @param variable index
 * @param serialized readonly flag (true if "true", false otherwise)
 * @see also data.h
 * @return plc instance with saved change or updated error status
 */
plc_t configure_variable_readonly(const plc_t p, int var, uint8_t idx, const char *val);

/**
 * @brief assign upper or lower limit to an analog input or output
 * @param plc instance   
 * @param the type of io (IL_OPERANDS enum value)
 * @param variable index
 * @param the limit value (serialized float eg. "5.35")
 * @param upper limit if true, otherwise lower limit
 * @see also data.h
 * @return plc instance with saved change or updated error status
 */
plc_t configure_io_limit(const plc_t p, int io, uint8_t idx, const char *val, uint8_t max);

/**
 * @brief configure a register as up or down counter
 * @param plc instance   
 * @param variable index
 * @param serialized direction flag (true if "DOWN", false otherwise)
 * @return plc instance with saved change or updated error status
 */
plc_t configure_counter_direction(const plc_t p, uint8_t idx, const char *val);
/**
 * @brief configure a timer scale
 * @param plc instance   
 * @param timer index
 * @param serialized long (eg 100000)
 * @see also timer_t
 * @return plc instance with saved change or updated error status
 */
plc_t configure_timer_scale(const plc_t p, uint8_t idx, const char *val);

/**
 * @brief configure a timer preset
 * @param plc instance   
 * @param timer index
 * @param serialized long (eg 100000)
 * @see also timer_t
 * @return plc instance with saved change or updated error status
 */
plc_t configure_timer_preset(const plc_t p, uint8_t idx, const char *val);

/**
 * @brief configure a timer delay mode
 * @param plc instance   
 * @param timer index
 * @param serialized mode flag (true if "ON", false otherwise)
 * @see also timer_t
 * @return plc instance with saved change or updated error status
 */
plc_t configure_timer_delay_mode(const plc_t p, uint8_t idx, const char *val);

/**
 * @brief configure a pulse scale
 * @param plc instance   
 * @param pulse index
 * @param serialized long (eg 100000)
 * @see also blink_t
 * @return plc instance with saved change or updated error status
 */
plc_t configure_pulse_scale(const plc_t p, uint8_t idx, const char *val);

/**
 * @brief
 * @param
 * @return
 */
void read_inputs(plc_t p);

/**
 * @brief
 * @param
 * @return
 */
void write_outputs(plc_t p);

#endif //_PLCLIB_H_
