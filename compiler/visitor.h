#ifndef VISITOR_H
#define VISITOR_H

#include "lexer.h"

void visitor_literal_ulint(unsigned long int val);

void  visitor_op_unary(TokenTyp operator_type);
void visitor_op_binary(TokenTyp operator_type);

#endif