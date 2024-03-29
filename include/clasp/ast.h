/**
 * Clasp Abstract Syntax Tree declaration
 * Authored 12/15/2023-present
 * 
 * This program is part of the Clasp Header Libraries
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

#ifndef AST_H
#define AST_H

#include <clasp/lexer.h>
#include <cvector/cvector.h>
#include <sheredom-hashmap/hashmap.h>
#include <stdint.h>

/**
 * Enumeration to store types of AST nodes.
 * CLASP_NUM_VISITORS must remain at the end of the list to accurately store the number of visitor functions needed.
*/
typedef enum {
    AST_EXPR_BINOP,
    AST_EXPR_UNOP,
    AST_EXPR_POSTFIX,
    AST_EXPR_LIT_NUMBER,
    AST_EXPR_VAR_REF,
    AST_EXPR_FN_CALL,

    AST_RETURN_STMT,
    AST_EXPR_STMT,
    AST_BLOCK_STMT,
    AST_VAR_DECL_STMT,
    AST_LET_DECL_STMT,
    AST_CONST_DECL_STMT,
    AST_FN_DECL_STMT,

    AST_IF_STMT,
    AST_WHILE_STMT,

    AST_TYPE_SINGLE,
    AST_TYPE_ARRAY,
    AST_TYPE_FN,
    AST_TYPE_TEMPLATE,
    AST_TYPE_PTR,

    CLASP_NUM_VISITORS
} ClaspASTNodeType;

/**
 * The actual data stored in AST nodes.
*/
typedef struct ClaspASTNode ClaspASTNode;

/**
 * Utility for function arguments.
*/
struct ClaspArg {
    ClaspToken *name;
    ClaspASTNode *type;
};

typedef uint8_t ClaspTypeFlag;

static const ClaspTypeFlag TYPE_CONST     = 0b00000001;
static const ClaspTypeFlag TYPE_MUTABLE   = 0b00000010;
static const ClaspTypeFlag TYPE_IMMUTABLE = 0b00000100;

struct ClaspType {
    ClaspASTNode *type;
    ClaspTypeFlag flag;
};

union ASTNodeData {
    /**
     * Binary operations (5 + 3)
    */
    struct {
        ClaspASTNode *left;
        ClaspASTNode *right;
        ClaspToken *op;
    } binop;
    /**
     * Unary operations (-8)
    */
    struct {
        ClaspASTNode *right;
        ClaspToken *op;
    } unop;
    /**
     * Postfix (x++)
    */
    struct {
        ClaspASTNode *left;
        ClaspToken *op;
    } postfix;
    /**
     * Number literals (22)
    */
    struct {
        ClaspToken *value;
    } lit_num;
    /**
     * Variable references (x, foo)
    */
    struct {
        ClaspToken *varname;
    } var_ref;
    /**
     * Function calls (foo(), mul(a,b))
    */
    struct {
        ClaspASTNode *referencer;
        cvector(ClaspASTNode *) args;
    } fn_call;
    /**
     * Return statements (see syntax.md)
    */
    struct {
        ClaspASTNode *retval;
    } return_stmt;
    /**
     * Expression statements (see syntax.md)
    */
    struct {
        ClaspASTNode *expr;
    } expr_stmt;
    /**
     * Block statment (see syntax.md)
    */
    struct {
        cvector(ClaspASTNode *) body;
    } block_stmt;
    /**
     * Variable declaration statement (see syntax.md)
    */
    struct {
        ClaspToken *name;
        ClaspASTNode *type;
        ClaspASTNode *initializer;
    } var_decl_stmt;

    /**
     * Function declaration (see syntax.md)
    */
    struct {
        ClaspToken *name;
        ClaspASTNode *ret_type;

        ClaspASTNode *body;
        struct ClaspArg **args; // cvector
    } fn_decl_stmt;

    /**
     * If/While statement (see syntax.md)
    */
    struct {
        ClaspASTNode *cond;
        ClaspASTNode *body;
    } cond_stmt;


    // Type node stuff
    /**
     * Single types
    */
    struct {
        ClaspToken *name;
    } single;
    /**
     * Array types
    */
    struct {
        ClaspASTNode *enclosed;
    } array;
    /**
     * Function types
    */
    struct {
        cvector(ClaspASTNode *) args;
        ClaspASTNode *ret;
    } function;
    /**
     * Template types
    */
    struct {
        ClaspToken *typename;
        cvector(ClaspASTNode *) template;
    } template;
    /**
     * Pointer types
    */
    struct {
        ClaspASTNode *pointed;
    } pointer;
};

/**
 * Abstract Syntax Tree node.
 * This is essentially a tagged union that stores its type and all the associated data.
*/
typedef struct ClaspASTNode {
    ClaspASTNodeType type;
    union ASTNodeData data;

    struct ClaspType *exprType;
} ClaspASTNode;

/**
 * Allocate and initialize an AST node.
 * @param type The type of the new node.
 * @param data The data of the new node.
*/
ClaspASTNode *new_AST_node(ClaspASTNodeType type, union ASTNodeData *data);

/**
 * Allocate and initialize an expression node.
 * @param type The type of the new node.
 * @param data The data of the new node.
 * @param exprType The expression type of the new node.
*/
ClaspASTNode *new_expr_node(ClaspASTNodeType type, union ASTNodeData *data, struct ClaspType *exprType);

/**
 * Helper function for creating a binary op node.
 * @param left The left operand.
 * @param right The right operand.
 * @param op The binary operator to use.
*/
ClaspASTNode *binop(ClaspASTNode *left, ClaspASTNode *right, ClaspToken *op);

/**
 * Helper function for creating a unary op node.
 * @param right The right operand.
 * @param op The unary operator to use.
*/
ClaspASTNode *unop(ClaspASTNode *right, ClaspToken *op);

/**
 * Helper function for creating a postfix op node.
 * @param left The left operand.
 * @param op The postix operator to use.
*/
ClaspASTNode *postfix(ClaspASTNode *left, ClaspToken *op);

/**
 * Helper function for creating a number literal node.
 * @param num The number literal token to use.
*/
ClaspASTNode *lit_num(ClaspToken *num);

/**
 * Helper function for creating a variable reference node.
 * @param vars The variable table to use.
 * @param varname The name of the variable to reference.
*/
ClaspASTNode *var_ref(hashmap_t *vars, ClaspToken *varname);

/**
 * Helper function for creating a function call node.
 * @param referencer The object to call. This is usually a variable/fn name but can be any function type.
 * @param args The arguments to pass to the function. TODO: pass args by name instead of order.
*/
ClaspASTNode *fn_call(ClaspASTNode *referencer, cvector(ClaspASTNode *) args);

/**
 * Helper function for creating a return statement node.
 * @param retval The returned value.
*/
ClaspASTNode *return_stmt(ClaspASTNode *retval);

/**
 * Helper function for creating an expression statement node.
 * @param expr The expression.
*/
ClaspASTNode *expr_stmt(ClaspASTNode *expr);

/**
 * Helper function for creating a block statement node.
 * @param block The list of statements to use in the block.
*/
ClaspASTNode *block_stmt(cvector(ClaspASTNode *) block);

/**
 * Helper function for creating a 'var' decl statement node.
 * @param name The name of the variable being declared.
 * @param type The type node representing the type of the variable.
 * @param initializer The expression node representing the variable initializer.
*/
ClaspASTNode *var_decl(ClaspToken *name, ClaspASTNode *type, ClaspASTNode *initializer);

/**
 * Helper function for creating a 'let' decl statement node.
 * @param name The name of the variable being declared.
 * @param type The type node representing the type of the variable.
 * @param initializer The expression node representing the variable initializer.
*/
ClaspASTNode *let_decl(ClaspToken *name, ClaspASTNode *type, ClaspASTNode *initializer);

/**
 * Helper function for creating a 'const' decl statement node.
 * @param name The name of the variable being declared.
 * @param type The type node representing the type of the variable.
 * @param initializer The epxression node representing the variable intializer.
*/
ClaspASTNode *const_decl(ClaspToken *name, ClaspASTNode *type, ClaspASTNode *initializer);

/**
 * Helper function for creating a function declaration statement node.
 * @param name The name of the function being declared.
 * @param ret_type The return type of the function being declared.
 * @param args A cvector, the argument list of the function beind declared.
 * @param body A statement node, the function body.
*/
ClaspASTNode *fn_decl(ClaspToken *name, ClaspASTNode *ret_type, struct ClaspArg **args, ClaspASTNode *body);

/**
 * Helper function for creating an if statement node.
 * @param cond The expression node representing the condition of the if statement
 * @param body The statement representing the body to run if cond is true
*/
ClaspASTNode *if_stmt(ClaspASTNode *cond, ClaspASTNode *body);

/**
 * Helper function for creating a while statement node.
 * @param cond The expression node representing the condition of the while statement
 * @param body The statement representing the body to run while cond is true
*/
ClaspASTNode *while_stmt(ClaspASTNode *cond, ClaspASTNode *body);

/**
 * Helper function for creating a single type node.
 * @param name The typename.
*/
ClaspASTNode *type_single(ClaspToken *name);

// TODO: finish helper functions

/**
 * AST visitor that can return data.
*/
typedef void *(*ClaspVisitorFn) (ClaspASTNode *node, void *args);

/**
 * List of visitors.
*/
typedef ClaspVisitorFn ClaspASTVisitor[CLASP_NUM_VISITORS];

/**
 * Helper function to call the appropriate visitor for the given node.
 * @param node The node to visit
 * @param visitor The visitor table to use.
*/
void *visit(ClaspASTNode *node, void *args, ClaspASTVisitor visitor);

#endif // AST_H