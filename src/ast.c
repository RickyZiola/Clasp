/**
 * Clasp Abstract Syntax Tree Implementation
 * Authored 12/15/2023-present
 * 
 * This program is part of the Clasp Source Libraries
 * 
 * Copyright (c) 2023, Frederick Ziola
 *                      frederick.ziola@gmail.com
 * 
 * SPDX-License-Identifier: GPL-3.0
 * 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <clasp/ast.h>
#include <stdlib.h>
#include <stdio.h>

ClaspASTNode *new_AST_node(ClaspASTNodeType t, union ASTNodeData *data) {
    ClaspASTNode *node = malloc(sizeof(ClaspASTNode));
    node->type = t;
    node->data = *data;
    return node;
}

ClaspASTNode *binop(ClaspASTNode *left, ClaspASTNode *right, ClaspToken *op) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation error in binop function\n");
        return NULL;
    }

    data->binop.left = left;
    data->binop.op = op;
    data->binop.right = right;

    return new_AST_node(AST_EXPR_BINOP, data);
}

ClaspASTNode *unop(ClaspASTNode *right, ClaspToken *op) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation error in unop function\n");
        return NULL;
    }

    data->unop.op = op;
    data->unop.right = right;

    return new_AST_node(AST_EXPR_UNOP, data);
}

ClaspASTNode *postfix(ClaspASTNode *left, ClaspToken *op) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation error in postfix function\n");
        return NULL;
    }

    data->postfix.op = op;
    data->postfix.left = left;

    return new_AST_node(AST_EXPR_POSTFIX, data);
}

ClaspASTNode *lit_num(ClaspToken *n) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation error in lit_num function\n");
        return NULL;
    }

    data->lit_num.value = n;

    return new_AST_node(AST_EXPR_LIT_NUMBER, data);
}

ClaspASTNode *var_ref(ClaspToken *n) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation error in var_ref function\n");
        return NULL;
    }

    data->var_ref.varname = n;

    return new_AST_node(AST_EXPR_VAR_REF, data);
}

ClaspASTNode *fn_call(ClaspASTNode *ref, cvector(ClaspASTNode *) args) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation error in fn_call function\n");
        return NULL;
    }

    data->fn_call.referencer = ref;
    data->fn_call.args = args;

    return new_AST_node(AST_EXPR_FN_CALL, data);
}

ClaspASTNode *expr_stmt(ClaspASTNode *expr) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation error in expr_stmt function\n");
        return NULL;
    }

    data->expr_stmt.expr = expr;

    return new_AST_node(AST_EXPR_STMT, data);
}

ClaspASTNode *block_stmt(cvector(ClaspASTNode *) block) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        fprintf(stderr, "Memory allocation error in block_stmt function\n");
        return NULL;
    }

    data->block_stmt.body = block;

    return new_AST_node(AST_BLOCK_STMT, data);
}

ClaspASTNode *var_decl(ClaspToken *name, ClaspASTNode *type, ClaspASTNode *value) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        fpritnf(stderr, "Memory allocation error in var_decl function\n");
        return NULL;
    }

    data->var_decl_stmt.name = name;
    data->var_decl_stmt.type = type;
    data->var_decl_stmt.initializer = value;

    data->var_decl_stmt.var_type = VARIABLE_TYPE_VAR;

    return new_AST_node(AST_VAR_DECL_STMT, data);
}

ClaspASTNode *let_decl(ClaspToken *name, ClaspASTNode *type, ClaspASTNode *value) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        fpritnf(stderr, "Memory allocation error in var_decl function\n");
        return NULL;
    }

    data->var_decl_stmt.name = name;
    data->var_decl_stmt.type = type;
    data->var_decl_stmt.initializer = value;

    data->var_decl_stmt.var_type = VARIABLE_TYPE_LET;

    return new_AST_node(AST_VAR_DECL_STMT, data);
}

ClaspASTNode *const_decl(ClaspToken *name, ClaspASTNode *type, ClaspASTNode *value) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        fpritnf(stderr, "Memory allocation error in var_decl function\n");
        return NULL;
    }

    data->var_decl_stmt.name = name;
    data->var_decl_stmt.type = type;
    data->var_decl_stmt.initializer = value;

    data->var_decl_stmt.var_type = VARIABLE_TYPE_CONST;

    return new_AST_node(AST_VAR_DECL_STMT, data);
}

void *visit(ClaspASTNode *node, ClaspASTVisitor v) {
    if (node->type < 0 || node->type > CLASP_NUM_VISITORS) {
        fprintf(stderr, "Internal error, please report this message: \n\n\"Unknown AST node type: %d\"\n", node->type);
        exit(1);
    }
    return v[node->type](node);
}