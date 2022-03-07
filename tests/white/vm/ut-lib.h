#ifndef _UT_LIB_H_ 
#define _UT_LIB_H_ 

#include <stdbool.h>
#include <CUnit/CUnit.h>

#include "common.h"
#include "plclib.h"
#include "rung_vm.h"

extern struct hardware Hw_stub;

void init_mock_plc(plc_t plc) {
    memset(plc, 0, sizeof(struct PLC_regs));
    plc->ni = 8;
    plc->nq = 8;
    plc->nai = 2;
    plc->naq = 2;
    plc->nt = 2;
    plc->ns = 2;
    plc->nm = 8;
    plc->nmr = 8;
    plc->hw = &Hw_stub;
    plc->inputs = (uint8_t*) MEM_MALLOC(plc->ni, "init_mock_plc A");
    plc->outputs = (uint8_t*) MEM_MALLOC(plc->nq, "init_mock_plc B");
    plc->real_in = (uint64_t*) MEM_MALLOC(plc->nai * sizeof(uint64_t), "init_mock_plc C");
    plc->real_out = (uint64_t*) MEM_MALLOC(plc->naq * sizeof(uint64_t), "init_mock_plc D");

    plc->di = (di_t) MEM_MALLOC(BYTESIZE * plc->ni * sizeof(struct digital_input), "init_mock_plc E");
    plc->dq = (do_t) MEM_MALLOC(BYTESIZE * plc->nq * sizeof(struct digital_output), "init_mock_plc F");

    plc->ai = (aio_t) MEM_MALLOC(plc->nai * sizeof(struct analog_io), "init_mock_plc G");
    plc->aq = (aio_t) MEM_MALLOC(plc->naq * sizeof(struct analog_io), "init_mock_plc H");

    plc->t = (dt_t) MEM_MALLOC(plc->nt * sizeof(struct timer), "init_mock_plc I");

    plc->s = (blink_t) MEM_MALLOC(plc->ns * sizeof(struct blink), "init_mock_plc J");
    plc->m = (mvar_t) MEM_MALLOC(plc->nm * sizeof(struct mvar), "init_mock_plc K");
    plc->mr = (mreal_t) MEM_MALLOC(plc->nmr * sizeof(struct mreal), "init_mock_plc L");

    memset(plc->inputs, 0, plc->ni);
    memset(plc->outputs, 0, plc->nq);
    memset(plc->real_in, 0, plc->nai * sizeof(uint64_t));
    memset(plc->real_out, 0, plc->naq * sizeof(uint64_t));
    memset(plc->di, 0, BYTESIZE * plc->ni * sizeof(struct digital_input));
    memset(plc->dq, 0, BYTESIZE * plc->nq * sizeof(struct digital_output));
    memset(plc->ai, 0, plc->nai * sizeof(struct analog_io));
    memset(plc->aq, 0, plc->naq * sizeof(struct analog_io));
    memset(plc->t, 0, plc->nt * sizeof(struct timer));
    memset(plc->s, 0, plc->ns * sizeof(struct blink));
    memset(plc->m, 0, plc->nm * sizeof(struct mvar));
    memset(plc->mr, 0, plc->nmr * sizeof(struct mreal));
}

void ut_codec() {
    struct PLC_regs p;
    init_mock_plc(&p);
    plc_t p_old = NULL;
    p_old = (plc_t) MEM_MALLOC(sizeof(struct PLC_regs), "ut_codec A");
    init_mock_plc(p_old);
    p.old = p_old;

    int i = 0;
    p.inputs[0] = 0xaa;

    //from zero to 0xaa

    p.real_in[0] = UINT64_MAX / 2;
    p.ai[0].min = 0.0l;
    p.ai[0].max = 10.0l;

    uint8_t changed = dec_inp(&p);

    CU_ASSERT(changed == true);
    //everything in buffer should be transferred to inputs
    //any input that changed must have set rising edge
    for (i = 0; i < 8; i++) {
        CU_ASSERT(p.di[i].I == (0xaa >> i) % 2);
        CU_ASSERT(p.di[i].I == p.di[i].RE);
        CU_ASSERT(p.di[i].FE == 0);
    }

    //printf("\n%lf\n", p.ai[0].V);
    //analog should be max / 2 = 5
    CU_ASSERT_DOUBLE_EQUAL(p.ai[0].V, 5.0l, FLOAT_PRECISION);
    //first four outputs are true, next for are set
    for (i = 0; i < 4; i++) {
        p.dq[i].Q = 1;
        p.dq[i + 4].SET = 1;
    }
    //analog 0 has value and is not forced (mask > max)
    p.aq[0].mask = 99.0l;
    p.aq[0].min = -10.0l;
    p.aq[0].max = 10.0l;
    p.aq[0].V = -7.5l;

    changed = enc_out(&p);
    CU_ASSERT(changed == true);

    //printf("\n%x\n", p.outputs[0]);
    //all dout shoud be true
    CU_ASSERT(p.outputs[0] == 0xff);
    //analog 0 should be -2 
    CU_ASSERT(p.real_out[0] == 0x2000000000000000);

    //   printf("%lx\n", p.real_out[0]);
    //0xaa to 0xaa

    p.old->inputs[0] = 0xaa;
    for (i = 0; i < 8; i++) {
        p.old->di[i].I = (0xaa >> i) % 2;
        p.old->di[i].I = p.di[i].RE;
        p.old->di[i].FE = 0;
    }
    p.old->outputs[0] = 0xff;

    //only analog changed
    changed = dec_inp(&p);

    CU_ASSERT(changed == true);

    p.old->aq[0].V = -7.5l;
//same values as old
    changed = enc_out(&p);

    CU_ASSERT(changed == false);

    for (i = 0; i < 8; i++) {
//    printf("input %d value %x re %x fe %x\n",
//                i, p.di[i].I, p.di[i].RE, p.di[i].FE);
        CU_ASSERT(p.di[i].I == (0xaa >> i) % 2);
        CU_ASSERT(p.di[i].FE == p.di[i].RE);
        CU_ASSERT(p.di[i].FE == 0);
    }
    //0xaa to 0xcc        
    p.inputs[0] = 0xcc;
    //RE = 0x44
    //FE = 0x22

    changed = dec_inp(&p);

    CU_ASSERT(changed == true);

    for (i = 0; i < 8; i++) {
        // printf("input %d value %x re %x fe %x\n",
        //        i, p.di[i].I, p.di[i].RE, p.di[i].FE);
        CU_ASSERT(p.di[i].I == (0xcc >> i) % 2);
        CU_ASSERT(p.di[i].RE == (0x44 >> i) % 2);
        CU_ASSERT(p.di[i].FE == (0x22 >> i) % 2);
    }
    //masks
    for (i = 0; i < 8; i++) {
        p.di[i].MASK = (0x33 >> i) % 2;
    }
    for (i = 0; i < 8; i++) {
        p.di[i].N_MASK = (0xee >> i) % 2;
    }
    p.ai[0].mask = 9.0l;
    for (i = 0; i < 8; i++) {
        p.dq[i].MASK = (0x33 >> i) % 2;
        p.dq[i].N_MASK = (0xee >> i) % 2;
    }
    p.aq[0].mask = 2.5l;

    //force 0 has precedence
    //Q:       1111 
    //I:       1100
    //old:     1010
    //force 1: 0011 
    //force 0: 1110
    //result:  0001
    //edge:    1011
    //fe:      1010
    //re:      0001

    changed = dec_inp(&p);

    CU_ASSERT(changed == true);

    CU_ASSERT_DOUBLE_EQUAL(p.ai[0].V, 9.0l, FLOAT_PRECISION);

    for (i = 0; i < 8; i++) {
        //printf("\noutput %d value %x set %x reset %x\n",
        //          i, p.dq[i].Q,
        //        p.dq[i].SET, p.dq[i].RESET);
        CU_ASSERT(p.di[i].I == (0x11 >> i) % 2);
        CU_ASSERT(p.di[i].RE == (0x11 >> i) % 2);
        CU_ASSERT(p.di[i].FE == (0xaa >> i) % 2);
    }

    changed = enc_out(&p);

    CU_ASSERT(changed == true);
    //printf("%x\n", p.outputs[0]);
    CU_ASSERT(p.outputs[0] == 0x11);
    //printf("%lx\n", p.real_out[0]);
    CU_ASSERT(p.real_out[0] == 0xa000000000000000);
}

void ut_jmp() {
    //degenerates
    unsigned int pc = 0;
    int result = handle_jmp(NULL, &pc);
    CU_ASSERT(result == STATUS_ERR);

    struct rung r;
    memset(&r, 0, sizeof(struct rung));

    result = handle_jmp(&r, &pc);
    CU_ASSERT(result == ERR_BADOPERAND);

    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));

    ins.operation = IL_NOP;
    ins.operand = 5;

    result = rung_append(&ins, &r);
    CU_ASSERT(result == STATUS_OK);

    result = handle_jmp(&r, &pc);
    CU_ASSERT(result == ERR_BADOPERATOR);

    //jump to instruction 5
    pc = 1;
    ins.operation = IL_JMP;
    ins.operand = 5;

    result = rung_append(&ins, &r);
    CU_ASSERT(result == STATUS_OK);

    result = handle_jmp(&r, &pc);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(pc == 5);

    //jump conditional to instruction 5
    pc = 2;
    r.acc.u = false;
    ins.operation = IL_JMP;
    ins.modifier = IL_COND;
    ins.operand = 5;

    result = rung_append(&ins, &r);
    CU_ASSERT(result == STATUS_OK);

    result = handle_jmp(&r, &pc);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(pc == 3);
}

void ut_set_reset() {
    struct PLC_regs p;
    data_t acc;
    acc.u = false;
    init_mock_plc(&p);
    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));
    //degenerates
    int result = handle_set( NULL, acc, false, &p);
    CU_ASSERT(result == STATUS_ERR);

    result = handle_set(&ins, acc, false, &p);
    CU_ASSERT(result == ERR_BADOPERATOR);

    result = handle_reset( NULL, acc, false, &p);
    CU_ASSERT(result == STATUS_ERR);

    result = handle_reset(&ins, acc, false, &p);
    CU_ASSERT(result == ERR_BADOPERATOR);

    ins.operand = -1;

    ins.operation = IL_SET;
    result = handle_set(&ins, acc, false, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.operation = IL_RESET;
    result = handle_reset(&ins, acc, false, &p);
    CU_ASSERT(result == ERR_BADOPERAND);
    memset(&ins, 0, sizeof(struct instruction));

    //CONTACT
    ins.operand = OP_CONTACT;
    ins.modifier = IL_NORM;
    //only boolean
    ins.operation = IL_SET;
    result = handle_set(&ins, acc, false, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.operation = IL_RESET;
    result = handle_reset(&ins, acc, false, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    acc.u = true;
    ins.byte = p.nq;
    ins.bit = 2;
    ins.operation = IL_SET;
    ins.modifier = IL_COND;
    result = handle_set(&ins, acc, true, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.operation = IL_RESET;
    ins.modifier = IL_COND;
    result = handle_reset(&ins, acc, true, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.byte = 1;
    ins.bit = 2;
    ins.operation = IL_SET;
    ins.modifier = IL_COND;
    result = handle_set(&ins, acc, true, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.dq[10].SET == true);
    CU_ASSERT(p.dq[10].RESET == false);

    //conditional
    acc.u = false;
    p.dq[10].SET = false;
    //   p.dq[10].RESET == false;
    ins.operation = IL_RESET;
    ins.modifier = IL_COND;
    result = handle_reset(&ins, acc, true, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.dq[10].SET == false);
    CU_ASSERT(p.dq[10].RESET == false);
    // memset(&ins, 0, sizeof(struct instruction));

    //START
    ins.operand = OP_START;
    ins.modifier = IL_NORM;
    //dont care boolean
    ins.operation = IL_SET;

    ins.byte = p.nt;
    result = handle_set(&ins, acc, true, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.byte = 1;
    result = handle_set(&ins, acc, true, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.t[1].START == true);

    ins.operation = IL_RESET;
    ins.byte = p.nt;
    result = handle_reset(&ins, acc, true, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.byte = 1;
    result = handle_reset(&ins, acc, false, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.t[1].START == false);

    //PULSEIN
    ins.operand = OP_PULSEIN;
    ins.modifier = IL_NORM;
    //dont care boolean
    ins.operation = IL_SET;

    ins.byte = p.nm;
    result = handle_set(&ins, acc, true, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.byte = 1;
    result = handle_set(&ins, acc, true, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.m[1].SET == true);
    CU_ASSERT(p.m[1].RESET == false);

    ins.operation = IL_RESET;
    ins.byte = p.nm;
    result = handle_reset(&ins, acc, true, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.byte = 1;
    result = handle_reset(&ins, acc, false, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.m[1].SET == false);
    CU_ASSERT(p.m[1].RESET == true);
}

void ut_st() {
    struct PLC_regs p;
    init_mock_plc(&p);
    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));
    data_t acc;
    acc.u = -1;

    //degenerates
    int result = handle_st( NULL, acc, &p);
    CU_ASSERT(result == STATUS_ERR);

    result = handle_st(&ins, acc, &p);
    CU_ASSERT(result == ERR_BADOPERATOR);

    ins.operand = -1;

    ins.operation = IL_ST;
    result = handle_st(&ins, acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    //CONTACT
    ins.operand = OP_CONTACT;
    ins.operation = IL_ST;

    ins.modifier = IL_NEG;
    acc.u = 123;
    ins.bit = 8;
    ins.byte = p.nq;
    result = handle_st(&ins, acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.byte = 1;
    result = handle_st(&ins, acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.outputs[1] == 133);

    //bool
    ins.byte = p.nq;
    result = handle_st(&ins, acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.byte = 1;
    ins.bit = 2;
    result = handle_st(&ins, acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.dq[10].Q == true);

    //real
    ins.operand = OP_REAL_CONTACT;
    ins.operation = IL_ST;

    ins.modifier = IL_NEG;
    acc.r = 1.25l;
    p.aq[1].min = -5.0l;
    p.aq[1].max = 5.0l;
    ins.byte = p.nq;
    result = handle_st(&ins, acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.byte = 1;
    result = handle_st(&ins, acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT_DOUBLE_EQUAL(p.aq[1].V, 1.25l, FLOAT_PRECISION);
    //0xa000000000000000);
    //printf("%lx\n", p.aq[1].V);
    init_mock_plc(&p);

    ins.byte = 1;
    ins.bit = 2;
    acc.u = 123;
    //START
    ins.operand = OP_START;
    ins.byte = p.nt;
    result = handle_st(&ins, acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.byte = 1;
    result = handle_st(&ins, acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.t[1].START == true);

    //PULSEIN
    ins.operand = OP_PULSEIN;
    ins.modifier = 0;
    //bool

    ins.byte = p.nm;
    result = handle_st(&ins, acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.byte = 1;
    result = handle_st(&ins, acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.m[1].PULSE == true);
    CU_ASSERT(p.m[1].EDGE == true);

    //byte
    ins.bit = 8;
    result = handle_st(&ins, acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.m[1].V == 123);

    //WRITE
    ins.operand = OP_WRITE;
    result = handle_st(&ins, acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.command == 123);

    ins.operand = OP_REAL_MEMIN;
    ins.modifier = 0;
    //real

    ins.byte = p.nmr;
    result = handle_st(&ins, acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.byte = 1;
    acc.r = -1.25l;

    result = handle_st(&ins, acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT_DOUBLE_EQUAL(p.mr[1].V, -1.25l, FLOAT_PRECISION);
}

void ut_st_discrete() {
    struct PLC_regs p;
    init_mock_plc(&p);
    struct instruction ins;
    int result = 0;
    uint64_t acc;
    memset(&ins, 0, sizeof(struct instruction));
    //CONTACT
    ins.operand = OP_CONTACT;
    ins.operation = IL_ST;

    acc = 0xAABB;

    ins.byte = 7;
    ins.bit = 16;

    //overflow
    result = st_out(&ins, acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.byte = 1;
    //BIG endianness
    result = st_out(&ins, acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.outputs[1] == 0xAA);
    CU_ASSERT(p.outputs[2] == 0xBB);

    acc = 0xAABBCCDD;

    ins.byte = 0;
    ins.bit = 32;

    result = st_out(&ins, acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.outputs[0] == 0xAA);
    CU_ASSERT(p.outputs[1] == 0xBB);

    CU_ASSERT(p.outputs[2] == 0xCC);
    CU_ASSERT(p.outputs[3] == 0xDD);

    acc = 0xAABBCCDDEEFF1122;

    ins.byte = 0;
    ins.bit = 64;

    result = st_out(&ins, acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.outputs[0] == 0xAA);
    CU_ASSERT(p.outputs[1] == 0xBB);

    CU_ASSERT(p.outputs[2] == 0xCC);
    CU_ASSERT(p.outputs[3] == 0xDD);

    CU_ASSERT(p.outputs[4] == 0xEE);
    CU_ASSERT(p.outputs[5] == 0xFF);

    CU_ASSERT(p.outputs[6] == 0x11);
    CU_ASSERT(p.outputs[7] == 0x22);

    //MEMORY
    ins.operand = OP_PULSEIN;
    ins.operation = IL_ST;

    ins.byte = 1;
    ins.bit = 16;

    result = st_mem(&ins, acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.m[1].V == 0x1122);

    ins.bit = 32;

    result = st_mem(&ins, acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.m[1].V == 0xEEFF1122);

    ins.bit = 64;

    result = st_mem(&ins, acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(p.m[1].V == 0xAABBCCDDEEFF1122);
}

void ut_st_real() {
    struct PLC_regs p;
    init_mock_plc(&p);
    struct instruction ins;
    int result = 0;
    double acc = 5.0l;
    memset(&ins, 0, sizeof(struct instruction));
    p.aq[0].min = 0.0l;
    p.aq[0].max = 10.0l;
    //CONTACT
    ins.operand = OP_REAL_CONTACT;
    ins.operation = IL_ST;
    result = st_out_r(&ins, acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT_DOUBLE_EQUAL(p.aq[0].V, 5.0l, FLOAT_PRECISION);
    //0x8000000000000000);

    //MEMORY
    ins.operand = OP_REAL_MEMIN;
    ins.operation = IL_ST;
    result = st_mem_r(&ins, acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT_DOUBLE_EQUAL(p.mr[0].V, 5.0l, FLOAT_PRECISION);
    //printf("%lx\n", p.aq[0].V);
}

void ut_ld() {
    struct PLC_regs p;
    init_mock_plc(&p);
    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));
    data_t acc;
    acc.u = -1;
    //degenerates

    int result = handle_ld( NULL, &acc, &p);
    CU_ASSERT(result == STATUS_ERR);

    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == ERR_BADOPERATOR);

    ins.operand = -1;

    ins.operation = IL_LD;
    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    //OUTPUT
    ins.operand = OP_OUTPUT;
    p.outputs[1] = 123;
    ins.byte = p.nq;

    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);
    ins.bit = 8;
    ins.byte = 1;

    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(acc.u == 123);

    ins.bit = 2;
    p.dq[10].Q = true;
    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(acc.u == 1);
    init_mock_plc(&p);

    //OUTPUT REAL
    ins.operand = OP_REAL_OUTPUT;
    p.aq[1].V = -2.5l;
    // 0x4000000000000000;
    p.aq[1].min = -5.0l;
    p.aq[1].max = 5.0l;
    ins.byte = p.nq;

    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);
    ins.byte = 1;

    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT_DOUBLE_EQUAL(acc.r, -2.5l, FLOAT_PRECISION);
    //printf("%f\n", acc.r);
    init_mock_plc(&p);

    //INPUT
    ins.operand = OP_INPUT;

    ins.byte = p.ni;

    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);
    ins.bit = 8;
    ins.byte = 1;
    p.inputs[1] = 123;
    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(acc.u == 123);

    ins.bit = 2;
    p.di[10].I = true;
    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(acc.u == 1);
    init_mock_plc(&p);

    //INPUT REAL
    ins.operand = OP_REAL_INPUT;

    ins.byte = p.ni;

    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);
    ins.byte = 1;
    p.ai[1].V = 0.0l;
    // 0x8000000000000000;
    p.ai[1].min = -5.0l;
    p.ai[1].max = 5.0l;
    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT_DOUBLE_EQUAL(acc.r, 0.0l, FLOAT_PRECISION);

    init_mock_plc(&p);

    //MEMORY
    ins.operand = OP_MEMORY;
    ins.byte = p.nm;

    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.modifier = IL_NEG;
    ins.byte = 1;
    ins.bit = 8;
    p.m[1].V = 123;
    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(acc.u == 133);

    //printf("%d\n", acc.u);

    ins.bit = 0;
    p.m[1].PULSE = true;
    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(acc.u == false);
    init_mock_plc(&p);

    //MEMORY REAL
    ins.operand = OP_REAL_MEMORY;
    ins.byte = p.nm;

    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.modifier = IL_NEG;
    ins.byte = 1;
    ins.bit = 8;
    p.mr[1].V = 123.4567l;
    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT_DOUBLE_EQUAL(acc.r, -123.4567l, FLOAT_PRECISION);
    //printf("%f\n", acc.r);

    init_mock_plc(&p);

    //TIMEOUT
    ins.operand = OP_TIMEOUT;
    ins.modifier = 0;
    ins.byte = p.nt;

    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.bit = 8;
    ins.byte = 1;
    p.t[1].V = 123;
    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(acc.u == 123);

    ins.bit = 0;
    p.t[1].Q = true;
    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(acc.u == 1);
    init_mock_plc(&p);

    //BLINKOUT
    ins.operand = OP_BLINKOUT;
    ins.byte = p.ns;

    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.byte = 1;
    p.s[1].Q = true;
    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(acc.u == 1);

    //COMMAND
    ins.operand = OP_COMMAND;
    ins.byte = 1;
    p.command = 123;
    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(acc.u == 123);

    //RISING
    ins.operand = OP_RISING;
    ins.byte = p.ni;

    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);

    ins.byte = 1;
    ins.bit = 8;
    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);
    ins.bit = 2;
    p.di[10].RE = true;
    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(acc.u == 1);
    init_mock_plc(&p);

    //FALLING
    ins.operand = OP_FALLING;
    ins.byte = p.ni;

    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);
    ins.bit = 8;
    ins.byte = 1;
    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(result == ERR_BADOPERAND);
    ins.bit = 2;
    p.di[10].FE = true;
    result = handle_ld(&ins, &acc, &p);
    CU_ASSERT(acc.u == 1);
    init_mock_plc(&p);

}

void ut_ld_discrete() {
    int result = STATUS_OK;
    struct PLC_regs p;
    init_mock_plc(&p);
    uint64_t acc = 0;
    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));
    //OUTPUT
    ins.operation = IL_LD;
    ins.operand = OP_OUTPUT;
    ins.modifier = IL_NORM;
    ins.bit = 16;
    ins.byte = 0;
    p.outputs[0] = 0xAA;
    p.outputs[1] = 0xBB;
    p.outputs[2] = 0xCC;
    p.outputs[3] = 0xDD;
    p.outputs[4] = 0xEE;
    p.outputs[5] = 0xFF;
    p.outputs[6] = 0x11;
    p.outputs[7] = 0x22;

    result = ld_out(&ins, &acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(acc == 0xAABB);

    acc = 0;
    ins.bit = 32;
    result = ld_out(&ins, &acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(acc == 0xAABBCCDD);
    //printf("%lx\n", acc); 

    acc = 0;
    ins.bit = 64;
    result = ld_out(&ins, &acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(acc == 0xAABBCCDDEEFF1122);

    //MEMORY
    ins.operand = OP_MEMORY;
    acc = 0;

    ins.byte = 0;
    p.m[0].V = 0xAABBCCDDEEFF1122;

    ins.bit = 16;
    result = ld_mem(&ins, &acc, &p);

    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(acc == 0x1122);

    ins.bit = 32;
    result = ld_mem(&ins, &acc, &p);

    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(acc == 0xEEFF1122);

    ins.bit = 64;
    result = ld_mem(&ins, &acc, &p);

    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(acc == 0xAABBCCDDEEFF1122);

}

void ut_ld_real() {
    int result = STATUS_OK;
    struct PLC_regs p;
    init_mock_plc(&p);
    double acc = 0;
    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));
    //OUTPUT
    ins.operation = IL_LD;
    ins.operand = OP_REAL_OUTPUT;
    ins.modifier = IL_NORM;
    ins.byte = 0;

    p.aq[0].V = 5.0l;

    result = ld_out_r(&ins, &acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT_DOUBLE_EQUAL(acc, 5.0l, FLOAT_PRECISION);
    //INPUT
    ins.operation = IL_LD;
    ins.operand = OP_REAL_INPUT;
    ins.modifier = IL_NORM;
    ins.byte = 0;

    p.ai[0].V = 7.5l;

    acc = 0;
    result = ld_in_r(&ins, &acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT_DOUBLE_EQUAL(acc, 7.5l, FLOAT_PRECISION);

    //MEMORY
    ins.operation = IL_LD;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 0;

    p.mr[0].V = -2.5l;
    acc = 0;
    result = ld_mem_r(&ins, &acc, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT_DOUBLE_EQUAL(acc, -2.5l, FLOAT_PRECISION);
}

void ut_stackable() {
    data_t acc;
    acc.u = -1;

    struct PLC_regs p;
    init_mock_plc(&p);

    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));

    struct rung r;
    memset(&r, 0, sizeof(struct rung));

    //degenerates
    int result = handle_stackable( NULL, NULL, NULL);
    CU_ASSERT(result == STATUS_ERR);

    result = handle_stackable(&ins, &r, &p);
    CU_ASSERT(result == ERR_BADOPERATOR);

    //no modifier should be the same as operate()
    uint8_t op = FIRST_BITWISE;
    ins.operation = op;
    ins.bit = BYTESIZE;
    op += NEGATE;
    data_t a, b, t;
    a.u = 123;
    b.u = 210;
    t = operate(op, T_BYTE, a, b);

    r.acc.u = 123;
    p.m[0].V = 210;
    ins.operand = OP_MEMORY;
    ins.byte = 0;
    ins.bit = BYTESIZE;
    ins.modifier = IL_NEG;

    result = handle_stackable(&ins, &r, &p);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(t.u == r.acc.u);

    ins.modifier = IL_PUSH;
    ins.operand = 0;
    init_mock_plc(&p);
    t.u = 0;

    //5 + (3 x 2) = 11
    /*  
     %M[0] = 3
     %M[1] = 2
     */
    p.m[0].V = 3;
    p.m[1].V = 2;

    /* mock:
     %M[2] = 5
     LD %M[2] => ACC = 5
     */
    r.acc.u = 5;

    /*
     ADD(%M[0] => ACC = 3, STACK = ADD 5
     */

    ins.operation = IL_ADD;
    ins.modifier = IL_PUSH;
    ins.operand = OP_MEMORY;
    ins.byte = 0;

    result = handle_stackable(&ins, &r, &p);

    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(r.acc.u = 3);
    CU_ASSERT(r.stack->depth == 1);

    /*
     MUL %M[1] => ACC = 2 x 3, STACK = ADD 5
     ) => ACC = 2 x 3 + 5
     */

    ins.operation = IL_MUL;
    ins.modifier = 0;
    ins.operand = OP_MEMORY;
    ins.byte = 1;

    result = handle_stackable(&ins, &r, &p);

    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(r.acc.u = 6);
    CU_ASSERT(r.stack->depth == 1);

    acc = pop(r.acc, &(r.stack));

    CU_ASSERT(acc.u == 11);

}

uint8_t gcd(uint8_t a, uint8_t b) {
    uint8_t r = 0;
    while (a != b && a != 0 && b != 0) {
        if (a > b)
            a = a - b;
        else
            b = b - a;
        r = a;
    }
    return r;
}

void ut_instruct_bitwise() {

    struct PLC_regs p;
    init_mock_plc(&p);

    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));

    struct rung r;
    memset(&r, 0, sizeof(struct rung));

    unsigned int pc = 0;
    //degenerates
    int result = instruct(NULL, NULL, &pc);
    CU_ASSERT(result == STATUS_ERR);

    pc = 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(result == STATUS_ERR);

    /* Bitwise:

     Triple majority gate: true when 2 out of 3 are true.
     Y = (A AND B) OR (B AND C) OR (A AND C)

     IL:
     LD  %I0.0   ;A
     AND %I0.1   ;A AND B
     OR (%I0.2   ;C, stack = OR(A AND B)
     AND %I0.1   ;B AND C, stack = OR( A AND B)
     )           ;(B AND C) OR(A AND B)
     OR (%I0.2   ;C, stack = OR ((B AND C) OR (A AND B))
     AND %I0.0   ;(A AND C),  stack = OR ((B AND C) OR (A AND B))
     )
     ST %Q0.0

     LD:

     I0.0-----I0.1-----+--%Q0.0
     |
     I0.1-----I0.2-----+
     |
     I0.2-----I0.0-----+
     */
    /*triple majority gate. C, B true, A false => true*/
    p.di[0].I = false;
    p.di[1].I = true;
    p.di[2].I = true;

    //LD  %I0.0   ;A = true
    ins.operation = IL_LD;
    ins.operand = OP_INPUT;
    ins.byte = 0;
    ins.bit = 0;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == false); //A
    memset(&ins, 0, sizeof(struct instruction));

    //AND %I0.1   
    ins.operation = IL_AND;
    ins.operand = OP_INPUT;
    ins.byte = 0;
    ins.bit = 1;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == false); //A AND B
    memset(&ins, 0, sizeof(struct instruction));

    //OR (%I0.2
    ins.operation = IL_OR;
    ins.operand = OP_INPUT;
    ins.byte = 0;
    ins.bit = 2;
    ins.modifier = IL_PUSH;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == true); //C, stack = OR(A AND B)
    CU_ASSERT(r.stack->depth == 1);
    memset(&ins, 0, sizeof(struct instruction));

    //AND %I0.1   
    ins.operation = IL_AND;
    ins.operand = OP_INPUT;
    ins.byte = 0;
    ins.bit = 1;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == true);
    //;B AND C, stack = OR( A AND B)
    CU_ASSERT(r.stack->depth == 1);
    memset(&ins, 0, sizeof(struct instruction));

    //)           
    ins.operation = IL_POP;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == true);
    CU_ASSERT(r.stack == NULL);
    //;(B AND C) OR(A AND B)
    memset(&ins, 0, sizeof(struct instruction));

    //OR (%I0.2
    ins.operation = IL_OR;
    ins.operand = OP_INPUT;
    ins.byte = 0;
    ins.bit = 2;
    ins.modifier = IL_PUSH;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == true);
    CU_ASSERT(r.stack->depth == 1);
    //C, stack = OR ((B AND C) OR (A AND B))
    memset(&ins, 0, sizeof(struct instruction));

    //AND %I0.0   
    ins.operation = IL_AND;
    ins.operand = OP_INPUT;
    ins.byte = 0;
    ins.bit = 0;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == false);
    //(A AND C),stack = OR((B AND C)OR(A AND B))
    CU_ASSERT(r.stack->depth == 1);
    memset(&ins, 0, sizeof(struct instruction));

    //)           
    ins.operation = IL_POP;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == true);
    CU_ASSERT(r.stack == NULL); //(A AND C) OR (B AND C) OR (A AND B)
    //ST %Q0.0

    ins.operation = IL_ST;
    ins.operand = OP_CONTACT;
    ins.byte = 0;
    ins.bit = 0;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(p.dq[0].Q == true);

    rung_clear(&r);

}

void ut_instruct_scalar() {
    struct PLC_regs p;
    init_mock_plc(&p);

    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));

    struct rung r;
    memset(&r, 0, sizeof(struct rung));

    unsigned int pc = 0;
    //degenerates
    int result = instruct(NULL, NULL, &pc);
    CU_ASSERT(result == STATUS_ERR);
    /* scalar arithmetic:

     euclidean Greatest Common divisor:
     function gcd(a, b)
     while a ≠ b
     if a > b
     a := a − b;
     else
     b := b − a;
     return a;

     IL code:

     LD %i0
     ST %m0
     LD %i1
     ST %m1

     while:LD %m0    ; A
     EQ %m1          ; A == B
     JMP?endwhile    ; while(A != B)
     LD %m0
     LT %m1      ; A < B
     JMP?reverse
     SUB %m1     ; A - B
     ST %m0      ; A = A - B
     JMP while
     reverse:LD %m1  ; B
     SUB %m0     ; B - A
     ST %m1      ; B = B - A
     JMP while
     endwhile:LD %m0 ;
     ST %q0 ; output gcd
     */
    uint8_t a = 0xff;
    uint8_t b = 0x22;
    //printf("GCD (0x%x, 0x%x) = 0x%x\n", a, b, gcd(a,b));
    init_mock_plc(&p);
    memset(&ins, 0, sizeof(struct instruction));
    p.inputs[0] = a;
    p.inputs[1] = b;

    //LD  %I0   ;A = 0xff
    ins.operation = IL_LD;
    ins.operand = OP_INPUT;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 8;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == 0xff); //A

    memset(&ins, 0, sizeof(struct instruction));

    //ST  %m0   ;A = 0xff
    ins.operation = IL_ST;
    ins.operand = OP_PULSEIN;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 8;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(p.m[0].V == 0xff); //A

    memset(&ins, 0, sizeof(struct instruction));

    //LD  %I1   ;B = 0x22
    ins.operation = IL_LD;
    ins.operand = OP_INPUT;
    ins.modifier = IL_NORM;
    ins.byte = 1;
    ins.bit = 8;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == 0x22); //A

    memset(&ins, 0, sizeof(struct instruction));

    //ST  %m1   ;B = 0x22
    ins.operation = IL_ST;
    ins.operand = OP_PULSEIN;
    ins.modifier = IL_NORM;
    ins.byte = 1;
    ins.bit = 8;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(p.m[1].V == 0x22); //A

    memset(&ins, 0, sizeof(struct instruction));

    //while:LD  %m0   ;A = 0xff
    ins.operation = IL_LD;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 8;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    uint8_t _while = pc;
    uint8_t _endwhile = pc + 12;

    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == 0xff); //A

    memset(&ins, 0, sizeof(struct instruction));

    //EQ %m1          ; A == B
    ins.operation = IL_EQ;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 1;
    ins.bit = 8;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == false); //A != B

    memset(&ins, 0, sizeof(struct instruction));

    //JMP?endwhile    ; while(A != B)
    ins.operation = IL_JMP;
    ins.operand = _endwhile;
    ins.modifier = IL_COND;
    ins.byte = 0;
    ins.bit = 0;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(pc == r.insno); //dont jump

    memset(&ins, 0, sizeof(struct instruction));

    //LD  %m0   ;A = 0xff
    ins.operation = IL_LD;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 8;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == 0xff); //A

    memset(&ins, 0, sizeof(struct instruction));

    //LT %m1      ; A < B
    ins.operation = IL_LT;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 1;
    ins.bit = 8;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    uint8_t _reverse = pc + 6;

    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == false); //A > B

    memset(&ins, 0, sizeof(struct instruction));

    //JMP?reverse
    ins.operation = IL_JMP;
    ins.operand = _reverse;
    ins.modifier = IL_COND;
    ins.byte = 0;
    ins.bit = 0;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);

    CU_ASSERT(pc == r.insno); //dont jump

    //LD  %m0   ;A = 0xff
    ins.operation = IL_LD;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 8;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == 0xff); //A

    // SUB %m1     ; A - B
    ins.operation = IL_SUB;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 1;
    ins.bit = 8;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == 0xdd); //A

    //ST  %m0   ;A = 0xdd
    ins.operation = IL_ST;
    ins.operand = OP_PULSEIN;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 8;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(p.m[0].V == 0xdd); //A

    memset(&ins, 0, sizeof(struct instruction));

    //JMP while
    ins.operation = IL_JMP;
    ins.operand = _while;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 0;
    result = rung_append(&ins, &r);

    pc = r.insno - 1;

    result = instruct(&p, &r, &pc);
    CU_ASSERT(pc == _while); //jump!

    while (p.m[0].V != 0x11 && pc < r.insno) {
        result = instruct(&p, &r, &pc);
    }

    CU_ASSERT(r.acc.u == 0x11);
    CU_ASSERT(p.m[0].V == 0x11);
    CU_ASSERT(p.m[1].V == 0x22);
    CU_ASSERT(pc == _reverse - 1);

    //reverse:LD %m1  ; B = 0x22
    ins.operation = IL_LD;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 1;
    ins.bit = 8;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == 0x22);

    memset(&ins, 0, sizeof(struct instruction));

    // SUB %m0     ; B - A
    ins.operation = IL_SUB;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 8;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);

    CU_ASSERT(r.acc.u == 0x11); //B

    memset(&ins, 0, sizeof(struct instruction));

    //ST  %m1   ;A = 0xdd
    ins.operation = IL_ST;
    ins.operand = OP_PULSEIN;
    ins.modifier = IL_NORM;
    ins.byte = 1;
    ins.bit = 8;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT(p.m[0].V == 0x11); //A

    memset(&ins, 0, sizeof(struct instruction));

    //JMP while
    ins.operation = IL_JMP;
    ins.operand = _while;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 0;
    result = rung_append(&ins, &r);

    pc = r.insno - 1;

    result = instruct(&p, &r, &pc);
    CU_ASSERT(pc == _while); //jump!

    //while:LD  %m0   ;A = 0x11
    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == 0x11);

    // EQ %m1          ; A == B
    result = instruct(&p, &r, &pc);
    CU_ASSERT(r.acc.u == true); //A == B

    //JMP?endwhile    ; while(A != B)
    result = instruct(&p, &r, &pc);
    CU_ASSERT(pc == _endwhile); //dont jump

    rung_clear(&r);
}

void ut_instruct_real() {
    struct PLC_regs p;
    init_mock_plc(&p);

    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));

    struct rung r;
    memset(&r, 0, sizeof(struct rung));

    unsigned int pc = 0;
    /*

     Knuth's online variance:
     n = 1
     mean = 5.0
     m2 = 0.0
     delta = 0.0

     read x :
     n += 1
     delta = x - mean
     mean += delta/n
     m2 += delta*(x - mean)

     if n < 2:
     return float('nan')
     else:
     return m2 / (n - 1)

     IL code:

     %M0 = 1; n
     %M1 = 5.0; mean
     %M2 = 0; m2
     %M3 = 0; delta
     %M4 = 1; const
     %I0 = 7.5; x: real input %I0

     %Q0 = 0; y: real outpput %Q0


     LD %M0  ; n = n+1
     ADD 1   ;
     ST %M0  ;

     LD %IF0 ; read x
     SUB %M1 ;
     ST %M3  ; delta = x - mean

     LD %M1  ; Acc = mean
     ADD(%M3 ; Acc = delta,              Stack = ADD mean
     DIV %M0 ; Acc = delta/n,            Stack = ADD mean
     )       ; Acc = mean + ( delta/n )
     ST %M1  ; mean = mean + ( delta/n )

     LD %M2  ; Acc = m2
     ADD(%M3 ; Acc = delta,              Stack = ADD m2
     MUL(%I0 ; Acc = x,                  Stack = MUL delta, ADD m2
     SUB %M1 ; Acc = x - mean,           Stack = MUL delta, ADD m2
     )       ; Acc = delta * (x - mean), Stack = ADD m2
     )       ; Acc = m2 + (delta * (x - mean))
     ST %M2  ; m2 = m2 + (delta * (x - mean))

     LD %M2  ; Acc = m2
     DIV(%M0 ; Acc = n,                  Stack = DIV m2
     SUB 1   ; Acc = n-1,                Stack = DIV m2
     )       ; Acc = m2 / n-1,
     ST %Q0  ; y = m2 / (n - 1)
     */
    p.mr[0].V = 1.0l;
    p.mr[1].V = 5.0l;
    p.mr[2].V = 3.0l;
    p.mr[4].V = 1.0l;

//1.    LD %M0  ; n = n+1
    ins.operation = IL_LD;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 0;

    int result = rung_append(&ins, &r);
    CU_ASSERT(result == STATUS_OK);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 1.0l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//2.    ADD 1   ;    
    ins.operation = IL_ADD;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 4;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 2.0l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//3.    ST  %m0   ;
    ins.operation = IL_ST;
    ins.operand = OP_REAL_MEMIN;
    ins.modifier = IL_NORM;
    ins.byte = 0;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(p.mr[0].V, 2.0l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//4.    LD %IF0 ; read x
    ins.operation = IL_LD;
    p.ai[0].min = 5.0l;
    p.ai[0].max = 10.0l;
    p.ai[0].V = 7.5l;
    // 0x8000000000000000;
    ins.operand = OP_REAL_INPUT;
    ins.modifier = IL_NORM;
    ins.byte = 0;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 7.5l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//5.     SUB %M1 ; 
    ins.operation = IL_SUB;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 1;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 2.5l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//6. ST %M3  ; delta = x - mean
    ins.operation = IL_ST;
    ins.operand = OP_REAL_MEMIN;
    ins.modifier = IL_NORM;
    ins.byte = 3;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(p.mr[3].V, 2.5l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//7.  LD %M1  ; Acc = mean
    ins.operation = IL_LD;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 1;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 5.0l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//8.     ADD(%M3 ; Acc = delta,              Stack = ADD mean
    ins.operation = IL_ADD;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_PUSH;
    ins.byte = 3;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 2.5l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//9.    DIV %M0 ; Acc = delta/n,            Stack = ADD mean        
    ins.operation = IL_DIV;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 0;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 1.25l, FLOAT_PRECISION);
//printf("%f\n", r.acc.r);
    memset(&ins, 0, sizeof(struct instruction));
//10. POP ; Acc = mean + ( delta/n )
    ins.operation = IL_POP;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 6.25l, FLOAT_PRECISION);
    CU_ASSERT(r.stack == NULL);
//11.    ST %M1  ; mean = mean + ( delta/n )
    ins.operation = IL_ST;
    ins.operand = OP_REAL_MEMIN;
    ins.modifier = IL_NORM;
    ins.byte = 1;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(p.mr[1].V, 6.25l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//12.    LD %M2  ; Acc = m2
    ins.operation = IL_LD;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 2;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 3.0l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//13.    ADD(%M3 ; Acc = delta,              Stack = ADD m2
    ins.operation = IL_ADD;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_PUSH;
    ins.byte = 3;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 2.5l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//14.    MUL(%I0 ; Acc = x,                  Stack = MUL delta, ADD m2
    ins.operation = IL_MUL;
    ins.operand = OP_REAL_INPUT;
    ins.modifier = IL_PUSH;
    ins.byte = 0;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 7.5l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//15.   SUB %M1 ; Acc = x - mean,           Stack = MUL delta, ADD m2
    ins.operation = IL_SUB;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 1;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 1.25l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//16.   )       ; Acc = delta * (x - mean), Stack = ADD m2        
    ins.operation = IL_POP;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 3.125l, FLOAT_PRECISION);

//printf("%f\n", r.acc.r);
    memset(&ins, 0, sizeof(struct instruction));

//17.    )       ; Acc = m2 + (delta * (x - mean))
    ins.operation = IL_POP;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 6.125l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));

//18.   ST %M2  ; m2 = m2 + (delta * (x - mean)) 
    ins.operation = IL_ST;
    ins.operand = OP_REAL_MEMIN;
    ins.modifier = IL_NORM;
    ins.byte = 2;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(p.mr[2].V, 6.125l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//19.     LD %M2  ; Acc = m2
    ins.operation = IL_LD;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 2;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 6.125l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//20.     DIV(%M0 ; Acc = n,                  Stack = DIV m2
    ins.operation = IL_DIV;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_PUSH;
    ins.byte = 0;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 2.0l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//21.    SUB 1   ; Acc = n-1,                Stack = DIV m2
    ins.operation = IL_SUB;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 4;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 1.0l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//22.    )       ; Acc = m2 / n-1,           Stack = DIV m2    
    ins.operation = IL_POP;
    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    CU_ASSERT_DOUBLE_EQUAL(r.acc.r, 6.125l, FLOAT_PRECISION);

    memset(&ins, 0, sizeof(struct instruction));
//23.     ST %Q0  ; y = m2 / (n - 1)   
    p.aq[0].min = 5.0l;
    p.aq[0].max = 9.0l; //range 4

    ins.operation = IL_ST;
    ins.operand = OP_REAL_CONTACT;
    ins.modifier = IL_NORM;
    ins.byte = 0;

    result = rung_append(&ins, &r);
    pc = r.insno - 1;
    result = instruct(&p, &r, &pc);
    //6.125 - 5 = 1.125  
    //0x10000 * 1.125 / 4 = 0x4800

    CU_ASSERT_DOUBLE_EQUAL(p.aq[0].V, 6.125l, FLOAT_PRECISION);
    //0x4800000000000000); 
//printf("%lx\n", p.aq[0].V);
    rung_clear(&r);
}

void ut_task_scalar() {
    struct PLC_regs p;
    init_mock_plc(&p);

    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));

    struct rung r;
    memset(&r, 0, sizeof(struct rung));

    //degenerates
    int result = task(0, NULL, NULL);
    CU_ASSERT(result == STATUS_ERR);

    uint8_t _reverse = 18;
    uint8_t _endwhile = 22;
    uint8_t _while = 8;

    //0.LD  %I0   ;A = 0xff
    ins.operation = IL_LD;
    ins.operand = OP_INPUT;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //1.ST  %m0   ;A = 0xff
    ins.operation = IL_ST;
    ins.operand = OP_PULSEIN;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //2.EQ %m3          ; A == 0
    ins.operation = IL_EQ;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 2;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //3.JMP?endwhile    ; while(A != B)
    ins.operation = IL_JMP;
    ins.operand = _endwhile;
    ins.modifier = IL_COND;
    ins.byte = 0;
    ins.bit = 0;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //4.LD  %I1   ;B = 0x22
    ins.operation = IL_LD;
    ins.operand = OP_INPUT;
    ins.modifier = IL_NORM;
    ins.byte = 1;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //5.ST  %m1   ;B = 0x22
    ins.operation = IL_ST;
    ins.operand = OP_PULSEIN;
    ins.modifier = IL_NORM;
    ins.byte = 1;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //6.EQ %m3          ; B == 0
    ins.operation = IL_EQ;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 2;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //7.JMP?endwhile    ; while(A != B)
    ins.operation = IL_JMP;
    ins.operand = _endwhile;
    ins.modifier = IL_COND;
    ins.byte = 0;
    ins.bit = 0;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //8.while:LD  %m0   ;A = 0xff
    ins.operation = IL_LD;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //9.EQ %m1          ; A == B
    ins.operation = IL_EQ;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 1;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //10.JMP?endwhile    ; while(A != B)
    ins.operation = IL_JMP;
    ins.operand = _endwhile;
    ins.modifier = IL_COND;
    ins.byte = 0;
    ins.bit = 0;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //11.LD  %m0   ;A = 0xff
    ins.operation = IL_LD;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //12.LT %m1      ; A < B
    ins.operation = IL_LT;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 1;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    //printf("reverse = %d \n", r.insno + 5);
    memset(&ins, 0, sizeof(struct instruction));

    //13.JMP?reverse
    ins.operation = IL_JMP;
    ins.operand = _reverse;
    ins.modifier = IL_COND;
    ins.byte = 0;
    ins.bit = 0;
    result = rung_append(&ins, &r);

    //14.LD  %m0   ;A = 0xff
    ins.operation = IL_LD;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    //15.SUB %m1     ; A - B
    ins.operation = IL_SUB;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 1;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    //16.ST  %m0   ;A = 0xdd
    ins.operation = IL_ST;
    ins.operand = OP_PULSEIN;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //17.JMP while
    ins.operation = IL_JMP;
    ins.operand = _while;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 0;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //18.reverse:LD %m1  ; B = 0x22
    ins.operation = IL_LD;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 1;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //19. SUB %m0     ; B - A
    ins.operation = IL_SUB;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //20. ST  %m1   ;A = 0xdd
    ins.operation = IL_ST;
    ins.operand = OP_PULSEIN;
    ins.modifier = IL_NORM;
    ins.byte = 1;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //21.JMP while
    ins.operation = IL_JMP;
    ins.operand = _while;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 0;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //22. endwhile:LD %m0 ; 
    ins.operation = IL_LD;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    //23. ST %q0 ; output gcd
    ins.operation = IL_ST;
    ins.operand = OP_CONTACT;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 8;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    p.inputs[0] = 0;
    p.inputs[1] = 0;

    //printf("GCD (0x%x, 0x%x) = 0x%x\n", 0, 0, gcd(0,0));

    result = task(1000, &p, &r);
    CU_ASSERT(p.outputs[0] == 0);
    CU_ASSERT(result == STATUS_OK);

    uint8_t a = 0xff;
    uint8_t b = 0x22;
    // printf("GCD (0x%x, 0x%x) = 0x%x\n", a, b, gcd(a,b));
    uint8_t expected = 0x11;
    p.inputs[0] = a;
    p.inputs[1] = b;

    result = task(1000, &p, &r);
    CU_ASSERT(p.outputs[0] == expected);
    // printf("found 0x%x\n", p.outputs[0]);
    CU_ASSERT(result == STATUS_OK);

    rung_clear(&r);
}

void ut_task_real() {
    struct PLC_regs p;
    init_mock_plc(&p);

    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));

    struct rung r;
    memset(&r, 0, sizeof(struct rung));

    p.mr[0].V = 1.0l;
    p.mr[1].V = 0.0l;
    p.mr[2].V = 0.0l;
    p.mr[4].V = 1.0l;

//1.    LD %M0  ; n = n+1
    ins.operation = IL_LD;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 0;

    int result = rung_append(&ins, &r);
    CU_ASSERT(result == STATUS_OK);
    memset(&ins, 0, sizeof(struct instruction));
//2.    ADD 1   ;    
    ins.operation = IL_ADD;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 4;

    result = rung_append(&ins, &r);
    CU_ASSERT(result == STATUS_OK);
    memset(&ins, 0, sizeof(struct instruction));
//3.    ST  %m0   ;
    ins.operation = IL_ST;
    ins.operand = OP_REAL_MEMIN;
    ins.modifier = IL_NORM;
    ins.byte = 0;

    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//4.    LD %IF0 ; read x
    ins.operation = IL_LD;
    ins.operand = OP_REAL_INPUT;
    ins.modifier = IL_NORM;
    ins.byte = 0;

    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//5.     SUB %M1 ; 
    ins.operation = IL_SUB;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 1;

    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//6. ST %M3  ; delta = x - mean
    ins.operation = IL_ST;
    ins.operand = OP_REAL_MEMIN;
    ins.modifier = IL_NORM;
    ins.byte = 3;

    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//7.  LD %M1  ; Acc = mean
    ins.operation = IL_LD;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 1;

    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//8.     ADD(%M3 ; Acc = delta,              Stack = ADD mean
    ins.operation = IL_ADD;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_PUSH;
    ins.byte = 3;

    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//9.    DIV %M0 ; Acc = delta/n,            Stack = ADD mean        
    ins.operation = IL_DIV;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 0;

    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//10. POP ; Acc = mean + ( delta/n )
    ins.operation = IL_POP;
    result = rung_append(&ins, &r);

//11.    ST %M1  ; mean = mean + ( delta/n )
    ins.operation = IL_ST;
    ins.operand = OP_REAL_MEMIN;
    ins.modifier = IL_NORM;
    ins.byte = 1;

    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//12.    LD %M2  ; Acc = m2
    ins.operation = IL_LD;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 2;

    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//13.    ADD(%M3 ; Acc = delta,              Stack = ADD m2
    ins.operation = IL_ADD;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_PUSH;
    ins.byte = 3;

    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//14.    MUL(%I0 ; Acc = x,                  Stack = MUL delta, ADD m2
    ins.operation = IL_MUL;
    ins.operand = OP_REAL_INPUT;
    ins.modifier = IL_PUSH;
    ins.byte = 0;

    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//15.   SUB %M1 ; Acc = x - mean,           Stack = MUL delta, ADD m2
    ins.operation = IL_SUB;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 1;

    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//16.   )       ; Acc = delta * (x - mean), Stack = ADD m2        
    ins.operation = IL_POP;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

//17.    )       ; Acc = m2 + (delta * (x - mean))
    ins.operation = IL_POP;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//18.   ST %M2  ; m2 = m2 + (delta * (x - mean)) 
    ins.operation = IL_ST;
    ins.operand = OP_REAL_MEMIN;
    ins.modifier = IL_NORM;
    ins.byte = 2;

    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//19.     LD %M2  ; Acc = m2
    ins.operation = IL_LD;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 2;

    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//20.     DIV(%M0 ; Acc = n,                  Stack = DIV m2
    ins.operation = IL_DIV;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_PUSH;
    ins.byte = 0;

    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//21.    SUB 1   ; Acc = n-1,                Stack = DIV m2
    ins.operation = IL_SUB;
    ins.operand = OP_REAL_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 4;

    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//22.    )       ; Acc = m2 / n-1,           Stack = DIV m2    
    ins.operation = IL_POP;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));
//23.     ST %Q0  ; y = m2 / (n - 1)   

    ins.operation = IL_ST;
    ins.operand = OP_REAL_CONTACT;
    ins.modifier = IL_NORM;
    ins.byte = 0;

    result = rung_append(&ins, &r);
    //6.125 - 5 = 1.125  
    //0x10000 * 1.125 / 4 = 0x4800

    p.ai[0].min = 0.0l;
    p.ai[0].max = 24.0l;

    p.aq[0].min = 0.0l;
    p.aq[0].max = 100.0l;    //range 4

    int i = 0;
    for (; i < 16; i++) {
        p.ai[0].V = (double) i;
        result = task(1000, &p, &r);

        //printf("Input: %lg\n", p.ai[0].V);
        //printf("Mean: %lg\n", p.m[1].V.r);
        //printf("Variance: %lg\n", r.acc.r);
    }
    //CU_ASSERT(p.aq[0].V == 0x4800000000000000); 

    rung_clear(&r);
}

void ut_task_bitwise() {
    struct PLC_regs p;
    init_mock_plc(&p);

    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));

    struct rung r;
    memset(&r, 0, sizeof(struct rung));

    //degenerates
    int result = task(1000, NULL, NULL);
    CU_ASSERT(result == STATUS_ERR);

    //LD  %I0.0   ;A = true
    ins.operation = IL_LD;
    ins.operand = OP_INPUT;
    ins.byte = 0;
    ins.bit = 0;
    result = rung_append(&ins, &r);
    memset(&ins, 0, sizeof(struct instruction));

    //AND %I0.1   
    ins.operation = IL_AND;
    ins.operand = OP_INPUT;
    ins.byte = 0;
    ins.bit = 1;
    result = rung_append(&ins, &r);
    memset(&ins, 0, sizeof(struct instruction));

    //OR (%I0.2
    ins.operation = IL_OR;
    ins.operand = OP_INPUT;
    ins.byte = 0;
    ins.bit = 2;
    ins.modifier = IL_PUSH;
    result = rung_append(&ins, &r);
    memset(&ins, 0, sizeof(struct instruction));

    //AND %I0.1   
    ins.operation = IL_AND;
    ins.operand = OP_INPUT;
    ins.byte = 0;
    ins.bit = 1;
    result = rung_append(&ins, &r);
    memset(&ins, 0, sizeof(struct instruction));

    //)           
    ins.operation = IL_POP;
    result = rung_append(&ins, &r);
    memset(&ins, 0, sizeof(struct instruction));

    //OR (%I0.2
    ins.operation = IL_OR;
    ins.operand = OP_INPUT;
    ins.byte = 0;
    ins.bit = 2;
    ins.modifier = IL_PUSH;
    result = rung_append(&ins, &r);
    memset(&ins, 0, sizeof(struct instruction));

    //AND %I0.0   
    ins.operation = IL_AND;
    ins.operand = OP_INPUT;
    ins.byte = 0;
    ins.bit = 0;
    result = rung_append(&ins, &r);
    memset(&ins, 0, sizeof(struct instruction));

    //)           
    ins.operation = IL_POP;
    result = rung_append(&ins, &r);
    memset(&ins, 0, sizeof(struct instruction));

    //ST %Q0.0
    ins.operation = IL_ST;
    ins.operand = OP_CONTACT;
    ins.byte = 0;
    ins.bit = 0;
    result = rung_append(&ins, &r);
    memset(&ins, 0, sizeof(struct instruction));

    /*triple majority gate*/
    p.di[0].I = false;
    p.di[1].I = false;
    p.di[2].I = false;
    result = task(1000, &p, &r);
    CU_ASSERT(p.dq[0].Q == false);
    CU_ASSERT(result == STATUS_OK);

    p.di[0].I = true;
    p.di[1].I = false;
    p.di[2].I = false;
    result = task(1000, &p, &r);
    CU_ASSERT(p.dq[0].Q == false);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(r.acc.u == false);
    CU_ASSERT(r.stack == NULL);

    p.di[0].I = false;
    p.di[1].I = true;
    p.di[2].I = false;
    result = task(1000, &p, &r);
    CU_ASSERT(p.dq[0].Q == false);
    CU_ASSERT(result == STATUS_OK);

    p.di[0].I = false;
    p.di[1].I = false;
    p.di[2].I = true;
    result = task(1000, &p, &r);
    CU_ASSERT(p.dq[0].Q == false);
    CU_ASSERT(result == STATUS_OK);

    p.di[0].I = true;
    p.di[1].I = true;
    p.di[2].I = false;
    result = task(1000, &p, &r);
    CU_ASSERT(p.dq[0].Q == true);
    CU_ASSERT(result == STATUS_OK);

    p.di[0].I = true;
    p.di[1].I = false;
    p.di[2].I = true;
    result = task(1000, &p, &r);
    CU_ASSERT(p.dq[0].Q == true);
    CU_ASSERT(result == STATUS_OK);

    p.di[0].I = false;
    p.di[1].I = true;
    p.di[2].I = true;
    result = task(1000, &p, &r);
    CU_ASSERT(p.dq[0].Q == true);
    CU_ASSERT(result == STATUS_OK);

    p.di[0].I = true;
    p.di[1].I = true;
    p.di[2].I = true;
    result = task(1000, &p, &r);
    CU_ASSERT(p.dq[0].Q == true);
    CU_ASSERT(result == STATUS_OK);

    rung_clear(&r);
}

void ut_task_timeout() {

    struct PLC_regs p;
    init_mock_plc(&p);

    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));

    struct rung r;
    memset(&r, 0, sizeof(struct rung));

    //while:LD  %m0   ;
    ins.operation = IL_LD;
    ins.operand = OP_MEMORY;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 8;
    int result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    long timeout = 0;

    result = task(timeout, &p, &r);
    CU_ASSERT(result == ERR_TIMEOUT);

    //JMP while
    ins.operation = IL_JMP;
    ins.operand = 0;
    ins.modifier = IL_NORM;
    ins.byte = 0;
    ins.bit = 0;
    result = rung_append(&ins, &r);

    memset(&ins, 0, sizeof(struct instruction));

    timeout = 10000;

    result = task(timeout, &p, &r);
    CU_ASSERT(result == ERR_TIMEOUT);

}

void ut_rung() {
//degenerates
    instruction_t pi = NULL;
    int result = get(NULL, -1, NULL);
    CU_ASSERT(result == STATUS_ERR);

    struct instruction i;
    memset(&i, 0, sizeof(struct instruction));

    result = get(NULL, -1, &pi);
    CU_ASSERT(result == STATUS_ERR);

    struct rung r;
    memset(&r, 0, sizeof(struct rung));

    result = get(&r, 0, &pi);
    CU_ASSERT(result == STATUS_ERR);

    result = rung_append(NULL, NULL);
    CU_ASSERT(result == STATUS_ERR);

    result = rung_append(NULL, &r);
    CU_ASSERT(r.insno == 0);
    CU_ASSERT(result == STATUS_OK);

//should rung_append
    result = rung_append(&i, &r);

    struct instruction pop;
    memset(&pop, 0, sizeof(struct instruction));
    pop.operation = IL_POP;
    strcpy(pop.label, "pop!");

    result = rung_append(&pop, &r);
    CU_ASSERT(r.insno == 2);
    CU_ASSERT(result == STATUS_OK);

    result = get(&r, 0, &pi);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(pi->operation == i.operation);

    result = get(&r, 1, &pi);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(memcmp(pi, &pop, sizeof(struct instruction)) == 0);

//should lookup
    result = rung_lookup(NULL, NULL);
    CU_ASSERT(result == STATUS_ERR);

    result = rung_lookup("pop!", &r);
    CU_ASSERT(result == 1);

    result = rung_lookup("flop!", &r);
    CU_ASSERT(result == STATUS_ERR);

//should intern
    result = rung_intern(NULL);
    CU_ASSERT(result == STATUS_ERR);

    result = rung_append(&pop, &r);        //don't allow duplicate labels
    CU_ASSERT(result == STATUS_ERR);
    CU_ASSERT(r.insno == 2);

    struct instruction jmp;
    memset(&jmp, 0, sizeof(struct instruction));
    jmp.operation = IL_JMP;

    memset(jmp.lookup, 0, MAXBUF);
    strcpy(jmp.lookup, "pop!");

    result = rung_append(&jmp, &r);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(r.insno == 3);

    result = rung_intern(&r);        //find all labels
    CU_ASSERT(result == STATUS_OK);

    result = get(&r, 2, &pi);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(pi->operand == 1); //they are interned

    strcpy(jmp.lookup, "flop!");
    result = rung_append(&jmp, &r);
    CU_ASSERT(result == STATUS_OK);
    CU_ASSERT(r.insno == 4);

    result = rung_intern(&r); //fail to find label
    CU_ASSERT(result == STATUS_ERR);

//should cleanup    
    rung_clear(&r);
    CU_ASSERT(r.insno == 0);
    CU_ASSERT(r.instructions == NULL);
}

void ut_codeline() {

    //rung_append null to null should have no effect
    codeline_t res = rung_append_line(NULL, NULL);
    CU_ASSERT_PTR_NULL(res);

    char *line = "lol";
    //rung_append to null should create a new list
    res = rung_append_line(line, NULL);
    CU_ASSERT_STRING_EQUAL(res->line, "lol");

    //rung_append null to list should have no effect
    res = rung_append_line(NULL, res);
    CU_ASSERT_STRING_EQUAL(res->line, "lol");
    CU_ASSERT_PTR_NULL(res->next);

    //rung_append to list should increase list
    res = rung_append_line("lala", res);
    CU_ASSERT_STRING_EQUAL(res->line, "lol");
    CU_ASSERT_STRING_EQUAL(res->next->line, "lala");

    CU_ASSERT_PTR_NULL(res->next->next);

}

void ut_stack() {

    struct rung r;
    memset(&r, 0, sizeof(struct rung));
    //opcode_t stack = NULL;
    //pop with empty stack
    data_t val;
    val.u = 5;
    data_t res = pop(val, &(r.stack));
    CU_ASSERT(res.u == val.u);

    //push any one, pop one
    uint8_t op = FIRST_BITWISE;
    data_t a;
    a.u = 1;
    data_t b;
    b.u = 2;
    data_t c;
    c.u = 3;
    int result;

    for (; op <= N_IL_INSN; op++) {
        data_t t = operate(op, T_BYTE, a, b);
        //push one pop one
        push(op, T_BYTE, a, &r);
        //&stack);
        CU_ASSERT(r.stack->depth == 1);
        res = pop(b, &(r.stack));
        CU_ASSERT(res.u == t.u);
        CU_ASSERT(r.stack == NULL);
        //push one pop two
        res = pop(b, &(r.stack));
        CU_ASSERT(res.u == b.u);
        CU_ASSERT(r.stack == NULL);
        //push two pop one
        push(op, T_BYTE, a, &r);
        push(op, T_BYTE, c, &r);
        t = operate(op, T_BYTE, c, b);
        CU_ASSERT(r.stack->depth == 2);
        res = pop(b, &(r.stack));
        CU_ASSERT(res.u == t.u);
        CU_ASSERT(r.stack->depth == 1);
        //push two pop two
        t = operate(op, T_BYTE, a, b);
        res = pop(b, &(r.stack));
        CU_ASSERT(res.u == t.u);
        CU_ASSERT(r.stack == NULL);
    }
    //stack overflow
    int i = 0;

    for (; i < MAXBUF - 1; i++)
        result = push(op, T_BYTE, a, &r);

    CU_ASSERT(result == STATUS_OK);

    result = push(op, T_BYTE, a, &r);
    CU_ASSERT(result == STATUS_ERR);
}

void ut_type() {
    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));
    int rv = instruction_get_type(NULL);
    CU_ASSERT(rv == STATUS_ERR);
    //degenerates
    rv = instruction_get_type(&ins);
    CU_ASSERT(rv == STATUS_ERR);

    //discrete
    ins.operand = OP_INPUT;
    ins.bit = 57;

    rv = instruction_get_type(&ins);
    CU_ASSERT(rv == STATUS_ERR);

    ins.bit = 5;

    rv = instruction_get_type(&ins);
    CU_ASSERT(rv == T_BOOL);

    ins.bit = 8;

    rv = instruction_get_type(&ins);
    CU_ASSERT(rv == T_BYTE);

    ins.bit = 16;

    rv = instruction_get_type(&ins);
    CU_ASSERT(rv == T_WORD);

    ins.bit = 32;

    rv = instruction_get_type(&ins);
    CU_ASSERT(rv == T_DWORD);

    ins.bit = 64;

    rv = instruction_get_type(&ins);
    CU_ASSERT(rv == T_LWORD);

    //real
    ins.operand = OP_REAL_INPUT;

    rv = instruction_get_type(&ins);
    CU_ASSERT(rv == T_REAL);
}

void ut_force() {
    struct PLC_regs plc;
    //degenerates
    plc_t r = force(NULL, -1, -1, NULL);
    CU_ASSERT(is_forced(NULL, -1, -1) == STATUS_ERR);
    CU_ASSERT_PTR_NULL(unforce(NULL, -1, -1));
    CU_ASSERT_PTR_NULL(r);
    //unsupported returns NULL
    r = force(&plc, N_OPERANDS, -1, NULL);
    CU_ASSERT(is_forced(&plc, -1, -1) == STATUS_ERR);
    CU_ASSERT_PTR_NULL(unforce(&plc, -1, -1));
    CU_ASSERT_PTR_NULL(r);

    r = force(&plc, OP_INPUT, -1, NULL);

    CU_ASSERT(is_forced(&plc, OP_INPUT, -1) == STATUS_ERR);
    CU_ASSERT_PTR_NULL(unforce(&plc, OP_INPUT, -1));
    CU_ASSERT_PTR_NULL(r);

    r = force(&plc, OP_INPUT, 1, NULL);
    CU_ASSERT_PTR_NULL(r);

    r = force(&plc, OP_INPUT, -1, "1");
    CU_ASSERT_PTR_NULL(r);
//regular behavior        
    init_mock_plc(&plc);
    plc.ai[1].min = 0.0;
    plc.ai[1].max = 2.0;
    plc.aq[1].min = 0.0;
    plc.aq[1].max = 2.0;
    r = force(&plc, OP_INPUT, 1, "1");

    CU_ASSERT_PTR_NOT_NULL(r);
    CU_ASSERT(r->di[1].MASK == 1);
    r = force(&plc, OP_INPUT, 1, "0");
    CU_ASSERT(r->di[1].N_MASK == 1);
    CU_ASSERT(is_forced(r, OP_INPUT, 1) == 1);
    r = unforce(&plc, OP_INPUT, 1);

    CU_ASSERT(r->di[1].MASK == 0);
    CU_ASSERT(r->di[1].N_MASK == 0);
    CU_ASSERT(is_forced(r, OP_INPUT, 1) == 0);

    r = force(&plc, OP_OUTPUT, 1, "1");

    CU_ASSERT_PTR_NOT_NULL(r);
    CU_ASSERT(r->dq[1].MASK == 1);
    r = force(&plc, OP_OUTPUT, 1, "0");
    CU_ASSERT(r->dq[1].N_MASK == 1);
    CU_ASSERT(is_forced(r, OP_OUTPUT, 1) == 1);
    r = unforce(&plc, OP_OUTPUT, 1);

    CU_ASSERT(r->dq[1].MASK == 0);
    CU_ASSERT(r->dq[1].N_MASK == 0);
    CU_ASSERT(is_forced(r, OP_OUTPUT, 1) == 0);

    r = force(&plc, OP_REAL_INPUT, 1, "-1.5");
    //value less than min should not apply force
    CU_ASSERT(is_forced(r, OP_REAL_INPUT, 1) == 0);

    r = force(&plc, OP_REAL_INPUT, 1, "1.5");

    CU_ASSERT_PTR_NOT_NULL(r);
    CU_ASSERT_DOUBLE_EQUAL(r->ai[1].mask, 1.5, FLOAT_PRECISION);

    CU_ASSERT(is_forced(r, OP_REAL_INPUT, 1) == 1);
    r = unforce(&plc, OP_REAL_INPUT, 1);

    CU_ASSERT(r->ai[1].mask <= r->ai[1].min);
    CU_ASSERT(is_forced(r, OP_REAL_INPUT, 1) == 0);

    r = force(&plc, OP_REAL_OUTPUT, 1, "-1.5");
    //value less than min should not apply force
    CU_ASSERT(is_forced(r, OP_REAL_OUTPUT, 1) == 0);

    r = force(&plc, OP_REAL_OUTPUT, 1, "1.3");

    CU_ASSERT_PTR_NOT_NULL(r);
    CU_ASSERT_DOUBLE_EQUAL(r->aq[1].mask, 1.3, FLOAT_PRECISION);

    CU_ASSERT(is_forced(r, OP_REAL_OUTPUT, 1) == 1);
    r = unforce(&plc, OP_REAL_OUTPUT, 1);

    CU_ASSERT(is_forced(r, OP_OUTPUT, 1) == 0);
    CU_ASSERT(r->aq[1].mask <= r->aq[1].min);
}

#endif //_UT_LIB_H_

