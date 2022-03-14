/*
 * Copyright 2022 Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * * Project Site: https://github.com/hiperiondev/plc-ladder *
 *
 * This is based on other projects:
 *    plcemu (https://github.com/kalamara/plcemu)
 *     - Antonis Kalamaras (kalamara AT ceid DOT upatras DOT gr)
 *
 * please contact their authors for more information.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 */

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <CUnit/Basic.h>
#include <CUnit/Console.h>
#include <CUnit/Automated.h>

#include "config.h"
#include "vm_hardware.h"
#include "vm_data.h"
#include "cmp_instruction.h"
#include "cmp_rung.h"
#include "vm_plclib.h"
#include "cmp_parser-tree.h"
#include "cmp_parser-il.h"
#include "cmp_parser-ld.h"
#include "cmp_codegen.h"
#include "mem.h"

#include "ut-conf.h"
#include "ut-cg.h"
#include "ut-data.h"
#include "ut-il.h"
#include "ut-init.h"
#include "ut-io.h"
#include "ut-ld.h"
#include "ut-lib.h"
#include "ut-tree.h"

int init_suite_success(void) {
    return 0;
}
int init_suite_failure(void) {
    return -1;
}
int clean_suite_success(void) {
    return 0;
}
int clean_suite_failure(void) {
    return -1;
}

#define ADD_TEST(suite,name) (NULL == CU_add_test((suite),#name,name))

int vm_tests(void) {
    //CU_pSuite suite_conf = NULL;
    CU_pSuite suite_lib = NULL;
    CU_pSuite suite_io = NULL;
    CU_pSuite suite_il = NULL;
    CU_pSuite suite_ld = NULL;
    CU_pSuite suite_tree = NULL;
    CU_pSuite suite_codegen = NULL;
    CU_pSuite suite_init = NULL;

    // initialize the CUnit test registry
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    // add a suite to the registry
    //suite_conf    = CU_add_suite("vm configurator"                   , init_suite_success, clean_suite_success);
    suite_lib     = CU_add_suite("plclib core vm library"            , init_suite_success, clean_suite_success);
    suite_io      = CU_add_suite("hardware I/O"                      , init_suite_success, clean_suite_success);
    suite_il      = CU_add_suite("instruction list lexer - generator", init_suite_success, clean_suite_success);
    suite_ld      = CU_add_suite("ladder logic expression parser"    , init_suite_success, clean_suite_success);
    suite_tree    = CU_add_suite("syntax tree"                       , init_suite_success, clean_suite_success);
    suite_codegen = CU_add_suite("microcode generator"               , init_suite_success, clean_suite_success);
    suite_init    = CU_add_suite("program initialization"            , init_suite_success, clean_suite_success);

    /* configurator:  TODO: CORRECT!!!
    if (
               ADD_TEST(suite_conf, ut_conf)
          //|| ADD_TEST(suite_conf, ut_save)
            || ADD_TEST(suite_conf, ut_store)
          //|| ADD_TEST(suite_conf, ut_process)
            || ADD_TEST(suite_conf, ut_get)
            || ADD_TEST(suite_conf, ut_set)
            || ADD_TEST(suite_conf, ut_copy)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    */

    if (
               NULL == suite_lib
            || NULL == suite_io
            || NULL == suite_il
            || NULL == suite_ld
            || NULL == suite_tree
            || NULL == suite_codegen
            || NULL == suite_init
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

   // plclib
    if (
               ADD_TEST(suite_lib, ut_codec)
            || ADD_TEST(suite_lib, ut_stack)
            || ADD_TEST(suite_lib, ut_type)
            || ADD_TEST(suite_lib, ut_operate)
            || ADD_TEST(suite_lib, ut_operate_b)
            || ADD_TEST(suite_lib, ut_operate_r)
            || ADD_TEST(suite_lib, ut_jmp)
            || ADD_TEST(suite_lib, ut_rung)
            || ADD_TEST(suite_lib, ut_codeline)
            || ADD_TEST(suite_lib, ut_set_reset)
            || ADD_TEST(suite_lib, ut_st)
            || ADD_TEST(suite_lib, ut_st_discrete)
            || ADD_TEST(suite_lib, ut_st_real)
            || ADD_TEST(suite_lib, ut_ld)
            || ADD_TEST(suite_lib, ut_ld_discrete)
            || ADD_TEST(suite_lib, ut_ld_real)
            || ADD_TEST(suite_lib, ut_stackable)
            || ADD_TEST(suite_lib, ut_instruct_bitwise)
            || ADD_TEST(suite_lib, ut_instruct_scalar)
            || ADD_TEST(suite_lib, ut_instruct_real)
            || ADD_TEST(suite_lib, ut_task_bitwise)
            || ADD_TEST(suite_lib, ut_task_scalar)
            || ADD_TEST(suite_lib, ut_task_real)
            || ADD_TEST(suite_lib, ut_task_timeout)
            || ADD_TEST(suite_lib, ut_force)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // I/O
    if (
            ADD_TEST(suite_io, ut_read)
         || ADD_TEST(suite_io, ut_write)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // IL lexer-codegen
    if (
               ADD_TEST(suite_il, ut_number)
            || ADD_TEST(suite_il, ut_char)
            || ADD_TEST(suite_il, ut_comments)
            || ADD_TEST(suite_il, ut_whitespace)
            || ADD_TEST(suite_il, ut_label)
            || ADD_TEST(suite_il, ut_modifier)
            || ADD_TEST(suite_il, ut_operator)
            || ADD_TEST(suite_il, ut_arguments)
            || ADD_TEST(suite_il, ut_parse)
            || ADD_TEST(suite_il, ut_parse_real)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // LD expression parser
    if (
               ADD_TEST(suite_ld, ut_minmin)
            || ADD_TEST(suite_ld, ut_parse_ld_line)
            || ADD_TEST(suite_ld, ut_find_next_node)
            || ADD_TEST(suite_ld, ut_parse_ld_program)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // syntax tree
    if (
               ADD_TEST(suite_tree, ut_mk_identifier)
            || ADD_TEST(suite_tree, ut_mk_expression)
            || ADD_TEST(suite_tree, ut_mk_assignment)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // code generator
    if (
               ADD_TEST(suite_codegen, ut_gen_expr)
            || ADD_TEST(suite_codegen, ut_gen_ass)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // initialization
    if (
               ADD_TEST(suite_init, ut_config)
            || ADD_TEST(suite_init, ut_construct)
            || ADD_TEST(suite_init, ut_start_stop)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // run all tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    printf("\n");
    CU_basic_show_failures(CU_get_failure_list());
    printf("\n\n");

    // run all tests using the automated interface
    CU_automated_run_tests();
    CU_list_tests_to_file();

    // lean up registry and return
    CU_cleanup_registry();

    return CU_get_error();
}
