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

#ifndef _PARSER_IL_H_
#define _PARSER_IL_H_

#include "common.h"
#include "cmp_instruction.h"
#include "cmp_rung.h"

/**
 * @brief read an unsigned integer value from string
 * starting @ position start
 * @param line
 * @return number read (positive value) or error (negative value)
 */
int parse_il_extract_number(const char *line);

/**
 * @brief extract bit and byte from X/Y expression starting @ start
 * @param the string
 * @param the byte
 * @param the bit
 * @return OK or error
 */
int parse_il_extract_arguments(const char *buf, uint8_t *byte, uint8_t *bit);

/**
 * @brief read optional descriptor (currently only f) 
 * from line at index
 * and parse grammatically
 * @param line
 * @param current operand
 * @param c index
 * @return IL operand symbol
 */
uint8_t parse_il_read_type(const char *line, uint8_t *operand, unsigned int index);

/**
 * @brief read operand  
 * from line at index
 * and parse grammatically
 * @param line
 * @param c index
 * @return IL operand symbol
 */
uint8_t parse_il_read_operand(const char *line, unsigned int index);

/**
 * @brief read up to ';' or /n
 * @param the line to trunkate comments from
 */
void parse_il_read_line_trunk_comments(char *line);

/**
 * @brief find last ':', truncate up to there, store label.
 * @param buf a place to store labelless lines
 * @param label_buf the extracted label
 */
void parse_il_trunk_label(const char *line, char *buf, char *label_buf);

/**
 * @brief trunkate all whitespaces left and right
 * @param the line to trunkate whitespaces from
 * @return trunkated line
 */
char* parse_il_trunk_whitespace(char *line);

/**
 * @brief find first ' ','!','('. store modifier (0 if not found)
 * @param buf a string
 * @param pos points to where was the modifier found
 * @return the modifier
 */
uint8_t parse_il_read_modifier(const char *buf, char **pos);

/**
 * @brief read operator from beginning to modifier, check if invalid. 
 * @param buf a string
 * @param stop points to end of operator
 * @return error code if invalid, otherwise the operator
 */
uint8_t parse_il_read_operator(const char *buf, const char *stop);

/**
 * @brief check modifier for correctness
 * @param an instruction
 * @return error code if not correct, 0 otherwise
 */
int parse_il_check_modifier(const instruction_t op);

/**
 * @brief check operand for correctness
 * @param an instruction
 * @return error code if not correct, 0 otherwise
 */
int parse_il_check_operand(instruction_t op);

/**
 *@brief find arguments
 * @param a line
 * @param operand
 * @param byte
 * @param bit
 * @return error code on failure
 */
int parse_il_find_arguments(const char *buf, uint8_t *operand, uint8_t *byte, uint8_t *bit);

/**
 * @brief parse IL line and generate microcode
 * line format:[label:]<operator>[<modifier>[%<operand><byte>[/<bit>]]|<label>][;comment]
 * @param a line of IL code
 * @param the rung to generate micorcode in
 * @return OK or error
 */
int parse_il_line(const char *line, rung_t r);

/**
 * @brief
 * @param
 * @param
 * @return
 */
rung_t* parse_il_program(const char *name, const char lines[][MAXSTR]);

#endif //_PARSER-IL_H
