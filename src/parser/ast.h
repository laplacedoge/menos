#ifndef __ME_PARSER_AST_H__
#define __ME_PARSER_AST_H__

#include "menos.h"
#include "util/fixed_buf.h"
#include "util/flex_buf.h"

typedef enum _AstTag {
    AstTag_StrLit,      /* String literal. */
    AstTag_NumLit,      /* Numeric literal. */
    AstTag_BoolLit,     /* Boolean literal. */
    AstTag_Var,         /* Variable. */

    AstTag_LogNotOp,    /* Logical NOT. */
    AstTag_LogOrOp,     /* Logical OR. */
    AstTag_LogAndOp,    /* Logical AND. */

    AstTag_RelEquOp,    /* Relational EQU. */
    AstTag_RelNeqOp,    /* Relational NEQ. */
    AstTag_RelLtOp,     /* Relational LT. */
    AstTag_RelLteOp,    /* Relational LTE. */
    AstTag_RelGtOp,     /* Relational GT. */
    AstTag_RelGteOp,    /* Relational GTE. */

    AstTag_UnaPlusOp,   /* Unary plus. */
    AstTag_UnaMinusOp,  /* Unary minus. */

    AstTag_BinAddOp,    /* Binary addition. */
    AstTag_BinSubOp,    /* Binary subtraction. */
    AstTag_BinMulOp,    /* Binary multiplication. */
    AstTag_BinDivOp,    /* Binary division. */
    AstTag_BinModOp,    /* Binary modulo. */
    AstTag_BinExpOp,    /* Binary exponentiation. */

    AstTag_AsgnStmt,
    AstTag_IfStmt,      /* If statement. */
    AstTag_IfElseStmt,  /* If-else statement. */
    AstTag_BlockStmt,

    AstTag_Prog,
} AstTag;

typedef struct _AstNode AstNode;

typedef struct _AstSeq AstSeq;

AstNode *
AstNode_New(void);

AstNode *
AstNode_NewProg(void);

AstNode *
AstNode_NewStrLit(
    FixedBuf * str
);

AstNode *
AstNode_NewNumLit(
    ssize num
);

AstNode *
AstNode_NewBoolLit(
    bool val
);

AstNode *
AstNode_NewVar(
    FixedBuf * str
);

AstNode *
AstNode_NewUnaOp(
    AstTag tag,
    AstNode * opd
);

AstNode *
AstNode_NewBinOp(
    AstTag tag,
    AstNode * lhs,
    AstNode * rhs
);

AstNode *
AstNode_NewBlock(
    AstTag tag,
    AstSeq * seq
);

AstNode *
AstNode_NewAsgnStmt(
    AstNode * lhs,
    AstNode * rhs
);

AstNode *
AstNode_NewIfStmt(
    AstNode * cond,
    AstNode * then_br
);

AstNode *
AstNode_NewIfElseStmt(
    AstNode * cond,
    AstNode * then_br,
    AstNode * else_br
);

bool
AstNode_PushAsStr(
    AstNode * node,
    FlexBuf * buf,
    ssize ind
);

void
AstNode_Free(
    AstNode * node
);

void
AstNode_FreeTree(
    AstNode * node
);

typedef struct _AstNode {
    AstTag tag;

    union {
        struct {
            FixedBuf * str;
        } str_lit;

        struct {
            ssize num;
        } num_lit;

        struct {
            bool val;
        } bool_lit;

        struct {
            FixedBuf * str;
        } var;

        struct {
            AstNode * opd;
        } una_op;

        struct {
            AstNode * lhs;
            AstNode * rhs;
        } bin_op;

        struct {
            AstNode * cond;
            AstNode * then_br;
        } if_stmt;

        struct {
            AstNode * cond;     /* Condition. */
            AstNode * then_br;  /* then-branch. */
            AstNode * else_br;  /* else-branch. */
        } if_else_stmt;

        struct {
            AstNode * lhs;
            AstNode * rhs;
        } asgn_stmt;

        struct {
            AstSeq * seq;
        } block;
    } ext;
} AstNode;

AstSeq *
AstSeq_New(void);

bool
AstSeq_Push(
    AstSeq * seq,
    AstNode * node
);

AstNode **
AstSeq_Data(
    AstSeq * seq
);

usize
AstSeq_Count(
    AstSeq * seq
);

AstNode *
AstSeq_At(
    AstSeq * seq,
    usize idx
);

bool
AstSeq_PushAsStr(
    AstSeq * seq,
    FlexBuf * buf,
    ssize ind
);

void
AstSeq_Clear(
    AstSeq * seq
);

void
AstSeq_Free(
    AstSeq * seq
);

#endif