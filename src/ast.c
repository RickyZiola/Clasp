/**
 * Clasp Abstract Syntax Tree Implementation
 * Authored 12/15/2023-present
 * 
 * This program is part of the Clasp Source Libraries
 * 
 * Copyright (c) 2024, Frederick Ziola
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
#include <clasp/variable.h>

ClaspASTNode *new_AST_node(ClaspASTNodeType t, union ASTNodeData *data) {
    ClaspASTNode *node = malloc(sizeof(ClaspASTNode));
    node->type = t;
    node->data = *data;
    node->exprType = NULL;
    return node;
}

ClaspASTNode *new_expr_node(ClaspASTNodeType t, union ASTNodeData *data, struct ClaspType *exprType) {
    ClaspASTNode *node = malloc(sizeof(ClaspASTNode));
    node->type = t;
    node->data = *data;
    node->exprType = exprType;
    return node;
}

ClaspASTNode *binop(ClaspASTNode *left, ClaspASTNode *right, ClaspToken *op) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation error in binop function\n");
        return NULL;
    }

    struct ClaspType *type = malloc(sizeof(struct ClaspType));
    type->type = NULL;
    type->flag = TYPE_IMMUTABLE;
    if (
        (left ->exprType->flag & TYPE_CONST) &&
        (right->exprType->flag & TYPE_CONST)
    ) { type->flag = TYPE_CONST; }

    data->binop.left = left;
    data->binop.op = op;
    data->binop.right = right;

    return new_expr_node(AST_EXPR_BINOP, data, type);
}

ClaspASTNode *unop(ClaspASTNode *right, ClaspToken *op) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation error in unop function\n");
        return NULL;
    }

    struct ClaspType *type = malloc(sizeof(struct ClaspType));
    type->type = NULL;
    type->flag = TYPE_IMMUTABLE;
    if (
        (right->exprType->flag & TYPE_CONST)
    ) { type->flag = TYPE_CONST; }


    data->unop.op = op;
    data->unop.right = right;

    return new_expr_node(AST_EXPR_UNOP, data, type);
}

ClaspASTNode *postfix(ClaspASTNode *left, ClaspToken *op) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation error in postfix function\n");
        return NULL;
    }

    struct ClaspType *type = malloc(sizeof(struct ClaspType));
    type->type = NULL;
    type->flag = TYPE_IMMUTABLE;
    if (
        (left ->exprType->flag & TYPE_CONST)
    ) { type->flag = TYPE_CONST; }


    data->postfix.op = op;
    data->postfix.left = left;

    return new_expr_node(AST_EXPR_POSTFIX, data, type);
}

ClaspASTNode *lit_num(ClaspToken *n) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation error in lit_num function\n");
        return NULL;
    }

    struct ClaspType *type = malloc(sizeof(struct ClaspType));

    union ASTNodeData *typeData = malloc(sizeof(union ASTNodeData));
    typeData->single.name = malloc(sizeof(ClaspToken));
    typeData->single.name->data = "int"; // TODO: floats
    ClaspASTNode *typename = new_AST_node(AST_TYPE_SINGLE, typeData);
    type->type = typename;
    type->flag = TYPE_CONST;

    data->lit_num.value = n;

    return new_expr_node(AST_EXPR_LIT_NUMBER, data, type);
}

ClaspASTNode *var_ref(hashmap_t *vars, ClaspToken *n) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation error in var_ref function\n");
        return NULL;
    }

    struct ClaspType *type = malloc(sizeof(struct ClaspType));
    type->type = NULL;
    type->flag = TYPE_MUTABLE;
    
    ClaspVariable *var = hashmap_get(vars, n->data, strlen(n->data));
    if (var) type->flag = var->type->flag;
    if (var) type->type = var->type->type;

    data->var_ref.varname = n;

    return new_expr_node(AST_EXPR_VAR_REF, data, type);
}

ClaspASTNode *fn_call(ClaspASTNode *ref, cvector(ClaspASTNode *) args) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation error in fn_call function\n");
        return NULL;
    }
    
    struct ClaspType *type = malloc(sizeof(struct ClaspType));
    type->type = NULL;
    type->flag = TYPE_IMMUTABLE;

    data->fn_call.referencer = ref;
    data->fn_call.args = args;

    return new_expr_node(AST_EXPR_FN_CALL, data, type);
}

ClaspASTNode *return_stmt(ClaspASTNode *retval) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        // Handle memory allocation failures
        fprintf(stderr, "Memory allocation error in return_stmt function\n");
        return NULL;
    }
    data->return_stmt.retval = retval;

    return new_AST_node(AST_RETURN_STMT, data);
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
        fprintf(stderr, "Memory allocation error in var_decl function\n");
        return NULL;
    }

    data->var_decl_stmt.name = name;
    data->var_decl_stmt.type = type;
    data->var_decl_stmt.initializer = value;

    return new_AST_node(AST_VAR_DECL_STMT, data);
}

ClaspASTNode *let_decl(ClaspToken *name, ClaspASTNode *type, ClaspASTNode *value) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        fprintf(stderr, "Memory allocation error in let_decl function\n");
        return NULL;
    }

    data->var_decl_stmt.name = name;
    data->var_decl_stmt.type = type;
    data->var_decl_stmt.initializer = value;

    return new_AST_node(AST_LET_DECL_STMT, data);
}

ClaspASTNode *const_decl(ClaspToken *name, ClaspASTNode *type, ClaspASTNode *value) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        fprintf(stderr, "Memory allocation error in const_decl function\n");
        return NULL;
    }

    data->var_decl_stmt.name = name;
    data->var_decl_stmt.type = type;
    data->var_decl_stmt.initializer = value;

    return new_AST_node(AST_CONST_DECL_STMT, data);
}

ClaspASTNode *fn_decl(ClaspToken *name, ClaspASTNode *ret_type, struct ClaspArg **args, ClaspASTNode *body) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        fprintf(stderr, "Memory allocation error in fn_decl function\n");
        return NULL;
    }

    data->fn_decl_stmt.name = name;
    data->fn_decl_stmt.ret_type = ret_type;
    data->fn_decl_stmt.body = body;
    data->fn_decl_stmt.args = args;

    return new_AST_node(AST_FN_DECL_STMT, data);
}

ClaspASTNode *if_stmt(ClaspASTNode *cond, ClaspASTNode *body) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        fprintf(stderr, "Memory allocation error in if_stmt function\n");
        return NULL;
    }

    data->cond_stmt.cond = cond;
    data->cond_stmt.body = body;

    return new_AST_node(AST_IF_STMT, data);
}

ClaspASTNode *while_stmt(ClaspASTNode *cond, ClaspASTNode *body) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        fprintf(stderr, "Memory allocation error in while_stmt function\n");
        return NULL;
    }

    data->cond_stmt.cond = cond;
    data->cond_stmt.body = body;

    return new_AST_node(AST_WHILE_STMT, data);
}

ClaspASTNode *type_single(ClaspToken *name) {
    union ASTNodeData *data = malloc(sizeof(union ASTNodeData));
    if (data == NULL) {
        fprintf(stderr, "Memory allocation error in type_single function\n");
        return NULL;
    }

    data->single.name = name;

    return new_AST_node(AST_TYPE_SINGLE, data);
}

void *visit(ClaspASTNode *node, void *args, ClaspASTVisitor v) {
    if (!node) return NULL;
    if (node->type < 0 || node->type > CLASP_NUM_VISITORS) {
        fprintf(stderr, "Internal error, please report this message: \n\n\"Unknown AST node type: %d\"\n", node->type);
        exit(1);
    }
    return v[node->type](node, args);
}