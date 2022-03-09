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

#ifndef _PARSER_LD_H_
#define _PARSER_LD_H_

#include "common.h"
#include "parser-tree.h"
#include "rung.h"

#define IS_COIL(x)      (x>=LD_COIL && x<=LD_DOWN)
#define IS_VERTICAL(x)  (x>=LD_OR && x<=LD_NODE)

typedef struct ld_line {
            char *buf;
         uint8_t status;
    unsigned int cursor;
          item_t stmt;
} *ld_line_t;

/**
 * @brief horizontal parse
 * parse up to coil or '+' 
 * -> blank or '|' : discard expression for line
 * -> operand: add AND expression
 * -> coil: add assignment statement
 * @param the ld line
 * @return ok or error code
 */
int parse_ld_line(ld_line_t line);

/**
 * @brief for an array arr of integers ,return the smallest of indices i so that arr[i] =  min(arr) >= min
 * @param arr
 * @param min
 * @param max
 * @return the smallest of indices i
 */
int parse_ld_minmin(const int *arr, int min, int max);

/**
 * @brief construct array of ld lines and initialize with text lines
 * @param the pre allocated text lines
 * @param the number of lines
 * @return newly allocated array
 */
ld_line_t* parse_ld_construct_program(const char lines[][MAXSTR], unsigned int length);

/**
 * @brief deallocate memory of ld program
 * @param the program
 * @param the length
 */
void parse_ld_destroy_program(unsigned int length, ld_line_t *program);

/**
 * @brief read ONE character from line at index
 * and parse grammatically
 * @param line
 * @param c index
 * @return LD symbol
 */
uint8_t parse_ld_read_char(const char *line, unsigned int c);

/**
 * @brief parse each program line horizontally up to coil or '+'
 * -> blank or '|' : discard expression for line
 * -> operand: add AND expression
 * -> coil: add assignment statement
 * @param program length (total lines)
 * @param the program (allocated array of lines)
 * @return OK or error
 */
int parse_ld_horizontal_parse(unsigned int length, ld_line_t *program);

/**
 * @brief parse all lines vertically at cursor position
 * -> '+': add push OR
 * -> '|': continue
 * -> default: replace all nodes with OR of all nodes
 * @param line to start at
 * @param program length
 * @param program
 * @return OK or error
 */
int parse_ld_vertical_parse(unsigned int start, unsigned int length, ld_line_t *program);

/** @brief find next valid node for vertical parse.
 * status unresolved,
 * the smallest index of those with the smallest cursor larger than pos
 * @param buffer of ld lines
 * @param current horizontal position
 * @param total number of lines
 * @return index of next node or error
 */
int parse_ld_find_next_node(const ld_line_t *program, unsigned int start, unsigned int lines);

/**
 * @brief parse LD program
 * @param the program as an allocated buffer of allocated strings
 * @param the plc to store the generated microcode to
 * @return plc with updated status
 */
rung_t* parse_ld_program(const char *name, const char lines[][MAXSTR]);

#endif //_PARSER_LD_H
