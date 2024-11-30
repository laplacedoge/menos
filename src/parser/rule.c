#include "rule.h"

static
AstNode *
ParRule_Expr(
    Parser * par
);

static
AstNode *
ParRule_Base(
    Parser * par
) {
    AstNode * base_node = NULL;
    Token * tok;

    if (tok = Parser_Peek(par), tok == NULL) {
        Parser_SetUnexpectedTokenError(par);
        goto Exit;
    }

    switch (tok->tag) {
    case TokTag_Name:
        if (base_node = AstNode_NewVar(tok->ext.name.str),
            base_node == NULL) {

            Parser_SetNoEnoughMemoryError(par);
            goto Exit;
        }

        Parser_Consume(par);

        break;

    case TokTag_StrLit:
        if (base_node = AstNode_NewStrLit(tok->ext.str_lit.str),
            base_node == NULL) {

            Parser_SetNoEnoughMemoryError(par);
            goto Exit;
        }

        Parser_Consume(par);

        break;

    case TokTag_NumLit:
        if (base_node = AstNode_NewNumLit(tok->ext.num_lit.val),
            base_node == NULL) {

            Parser_SetNoEnoughMemoryError(par);
            goto Exit;
        }

        Parser_Consume(par);

        break;

    case TokTag_False:
    case TokTag_True:
        if (base_node = AstNode_NewBoolLit(tok->tag == TokTag_True),
            base_node == NULL) {

            Parser_SetNoEnoughMemoryError(par);
            goto Exit;
        }

        Parser_Consume(par);

        break;

    case TokTag_LeftParen:
        Parser_Consume(par);

        if (base_node = ParRule_Expr(par), Parser_Failed(par)) {
            goto Exit;
        }

        if (Parser_Expect(par, TokTag_RightParen) == NULL) {
            Parser_SetUnexpectedTokenError(par);
            goto FreeBaseNode;
        }

        break;

    default:
        Parser_SetUnexpectedTokenError(par);
        goto Exit;
    }

    goto Exit;

FreeBaseNode:
    AstNode_FreeTree(base_node);

Exit:
    return base_node;
}

static
AstNode *
ParRule_Opd0(
    Parser * par
) {
    return ParRule_Base(par);
}

static
AstNode *
ParRule_Opd1(
    Parser * par
) {
    const TokTag BUF_TAGS[] = { TokTag_Plus, TokTag_Minus, TokTag_Not };
    const usize NUM_TAGS = 3;

    AstNode * res_node = NULL;
    AstNode * opd_node;
    Token * tok;

    if (tok = Parser_ExpectAny(par, BUF_TAGS, NUM_TAGS), tok != NULL) {
        if (opd_node = ParRule_Opd1(par), Parser_Failed(par)) {
            goto Exit;
        }

        if (res_node = AstNode_New(), res_node == NULL) {
            Parser_SetNoEnoughMemoryError(par);
            goto Exit;
        }

        if (tok->tag == TokTag_Plus) {
            res_node->tag = AstTag_UnaPlusOp;
        } else if (tok->tag == TokTag_Minus) {
            res_node->tag = AstTag_UnaMinusOp;
        } else {
            res_node->tag = AstTag_LogNotOp;
        }

        res_node->ext.una_op.opd = opd_node;
    } else {
        res_node = ParRule_Opd0(par);
    }

    goto Exit;

Exit:
    return res_node;
}

static
AstNode *
ParRule_Opd2(
    Parser * par
) {
    AstNode * res_node = NULL;
    AstNode * lhs_node;
    AstNode * rhs_node;
    Token * tok;

    if (lhs_node = ParRule_Opd1(par), Parser_Failed(par)) {
        goto Exit;
    }

    while (tok = Parser_Expect(par, TokTag_Exponent), tok != NULL) {
        AstNode * op_node;

        if (rhs_node = ParRule_Opd2(par), Parser_Failed(par)) {
            goto FreeLhsNode;
        }

        if (op_node = AstNode_New(), op_node == NULL) {
            Parser_SetNoEnoughMemoryError(par);
            goto FreeRhsNode;
        }

        op_node->tag = AstTag_BinExpOp;
        op_node->ext.bin_op.lhs = lhs_node;
        op_node->ext.bin_op.rhs = rhs_node;

        lhs_node = op_node;
    }

    res_node = lhs_node;

    goto Exit;

FreeRhsNode:
    AstNode_FreeTree(rhs_node);

FreeLhsNode:
    AstNode_Free(lhs_node);

Exit:
    return res_node;
}

static
AstNode *
ParRule_Opd3(
    Parser * par
) {
    const TokTag BUF_TAGS[] = {
        TokTag_Asterisk, TokTag_ForwardSlash, TokTag_Percent };
    const usize NUM_TAGS = 3;

    AstNode * res_node = NULL;
    AstNode * lhs_node;
    AstNode * rhs_node;
    Token * tok;

    if (lhs_node = ParRule_Opd2(par), Parser_Failed(par)) {
        goto Exit;
    }

    while (tok = Parser_ExpectAny(par, BUF_TAGS, NUM_TAGS), tok != NULL) {
        AstNode * op_node;

        if (rhs_node = ParRule_Opd2(par), Parser_Failed(par)) {
            goto FreeLhsNode;
        }

        if (op_node = AstNode_New(), op_node == NULL) {
            Parser_SetNoEnoughMemoryError(par);
            goto FreeRhsNode;
        }

        if (tok->tag == TokTag_Asterisk) {
            op_node->tag = AstTag_BinMulOp;
        } else if (tok->tag == TokTag_ForwardSlash) {
            op_node->tag = AstTag_BinDivOp;
        } else {
            op_node->tag = AstTag_BinModOp;
        }
        op_node->ext.bin_op.lhs = lhs_node;
        op_node->ext.bin_op.rhs = rhs_node;

        lhs_node = op_node;
    }

    res_node = lhs_node;

    goto Exit;

FreeRhsNode:
    AstNode_FreeTree(rhs_node);

FreeLhsNode:
    AstNode_Free(lhs_node);

Exit:
    return res_node;
}

static
AstNode *
ParRule_Opd4(
    Parser * par
) {
    const TokTag BUF_TAGS[] = { TokTag_Plus, TokTag_Minus };
    const usize NUM_TAGS = 2;

    AstNode * res_node = NULL;
    AstNode * lhs_node;
    AstNode * rhs_node;
    Token * tok;

    if (lhs_node = ParRule_Opd3(par), Parser_Failed(par)) {
        goto Exit;
    }

    while (tok = Parser_ExpectAny(par, BUF_TAGS, NUM_TAGS), tok != NULL) {
        AstNode * op_node;

        if (rhs_node = ParRule_Opd3(par), Parser_Failed(par)) {
            goto FreeLhsNode;
        }

        if (op_node = AstNode_New(), op_node == NULL) {
            Parser_SetNoEnoughMemoryError(par);
            goto FreeRhsNode;
        }

        if (tok->tag == TokTag_Plus) {
            op_node->tag = AstTag_BinAddOp;
        } else {
            op_node->tag = AstTag_BinSubOp;
        }
        op_node->ext.bin_op.lhs = lhs_node;
        op_node->ext.bin_op.rhs = rhs_node;

        lhs_node = op_node;
    }

    res_node = lhs_node;

    goto Exit;

FreeRhsNode:
    AstNode_FreeTree(rhs_node);

FreeLhsNode:
    AstNode_Free(lhs_node);

Exit:
    return res_node;
}

static
AstNode *
ParRule_Opd5(
    Parser * par
) {
    const TokTag BUF_TAGS[] = {
        TokTag_LessThan, TokTag_Lte, TokTag_GreaterThan, TokTag_Gte };
    const usize NUM_TAGS = 4;

    AstNode * res_node = NULL;
    AstNode * lhs_node;
    AstNode * rhs_node;
    Token * tok;

    if (lhs_node = ParRule_Opd4(par), Parser_Failed(par)) {
        goto Exit;
    }

    while (tok = Parser_ExpectAny(par, BUF_TAGS, NUM_TAGS), tok != NULL) {
        AstNode * op_node;

        if (rhs_node = ParRule_Opd4(par), Parser_Failed(par)) {
            goto FreeLhsNode;
        }

        if (op_node = AstNode_New(), op_node == NULL) {
            Parser_SetNoEnoughMemoryError(par);
            goto FreeRhsNode;
        }

        if (tok->tag == TokTag_LessThan) {
            op_node->tag = AstTag_RelLtOp;
        } else if (tok->tag == TokTag_Lte) {
            op_node->tag = AstTag_RelLteOp;
        } else if (tok->tag == TokTag_GreaterThan) {
            op_node->tag = AstTag_RelGtOp;
        } else {
            op_node->tag = AstTag_RelGteOp;
        }
        op_node->ext.bin_op.lhs = lhs_node;
        op_node->ext.bin_op.rhs = rhs_node;

        lhs_node = op_node;
    }

    res_node = lhs_node;

    goto Exit;

FreeRhsNode:
    AstNode_FreeTree(rhs_node);

FreeLhsNode:
    AstNode_Free(lhs_node);

Exit:
    return res_node;
}

static
AstNode *
ParRule_Opd6(
    Parser * par
) {
    const TokTag BUF_TAGS[] = { TokTag_Equ, TokTag_Neq };
    const usize NUM_TAGS = 2;

    AstNode * res_node = NULL;
    AstNode * lhs_node;
    AstNode * rhs_node;
    Token * tok;

    if (lhs_node = ParRule_Opd5(par), Parser_Failed(par)) {
        goto Exit;
    }

    while (tok = Parser_ExpectAny(par, BUF_TAGS, NUM_TAGS), tok != NULL) {
        AstNode * op_node;

        if (rhs_node = ParRule_Opd5(par), Parser_Failed(par)) {
            goto FreeLhsNode;
        }

        if (op_node = AstNode_New(), op_node == NULL) {
            Parser_SetNoEnoughMemoryError(par);
            goto FreeRhsNode;
        }

        if (tok->tag == TokTag_Equ) {
            op_node->tag = AstTag_RelEquOp;
        } else {
            op_node->tag = AstTag_RelNeqOp;
        }
        op_node->ext.bin_op.lhs = lhs_node;
        op_node->ext.bin_op.rhs = rhs_node;

        lhs_node = op_node;
    }

    res_node = lhs_node;

    goto Exit;

FreeRhsNode:
    AstNode_FreeTree(rhs_node);

FreeLhsNode:
    AstNode_Free(lhs_node);

Exit:
    return res_node;
}

static
AstNode *
ParRule_Opd7(
    Parser * par
) {
    AstNode * res_node = NULL;
    AstNode * lhs_node;
    AstNode * rhs_node;
    Token * tok;

    if (lhs_node = ParRule_Opd6(par), Parser_Failed(par)) {
        goto Exit;
    }

    while (Parser_Expect(par, TokTag_And) != NULL) {
        AstNode * op_node;

        if (rhs_node = ParRule_Opd6(par), Parser_Failed(par)) {
            goto FreeLhsNode;
        }

        if (op_node = AstNode_New(), op_node == NULL) {
            Parser_SetNoEnoughMemoryError(par);
            goto FreeRhsNode;
        }

        op_node->tag = AstTag_LogAndOp;
        op_node->ext.bin_op.lhs = lhs_node;
        op_node->ext.bin_op.rhs = rhs_node;

        lhs_node = op_node;
    }

    res_node = lhs_node;

    goto Exit;

FreeRhsNode:
    AstNode_FreeTree(rhs_node);

FreeLhsNode:
    AstNode_Free(lhs_node);

Exit:
    return res_node;
}

static
AstNode *
ParRule_Opd8(
    Parser * par
) {
    AstNode * res_node = NULL;
    AstNode * lhs_node;
    AstNode * rhs_node;
    Token * tok;

    if (lhs_node = ParRule_Opd7(par), Parser_Failed(par)) {
        goto Exit;
    }

    while (Parser_Expect(par, TokTag_Or) != NULL) {
        AstNode * op_node;

        if (rhs_node = ParRule_Opd8(par), Parser_Failed(par)) {
            goto FreeLhsNode;
        }

        if (op_node = AstNode_New(), op_node == NULL) {
            Parser_SetNoEnoughMemoryError(par);
            goto FreeRhsNode;
        }

        op_node->tag = AstTag_LogOrOp;
        op_node->ext.bin_op.lhs = lhs_node;
        op_node->ext.bin_op.rhs = rhs_node;

        lhs_node = op_node;
    }

    res_node = lhs_node;

    goto Exit;

FreeRhsNode:
    AstNode_FreeTree(rhs_node);

FreeLhsNode:
    AstNode_Free(lhs_node);

Exit:
    return res_node;
}

static
AstNode *
ParRule_Expr(
    Parser * par
) {
    return ParRule_Opd8(par);
}

static
AstNode *
ParRule_AsgnStmt(
    Parser * par
) {
    AstNode * stmt_node = NULL;
    AstNode * lhs_node;
    AstNode * rhs_node;
    Token * tok;

    if (tok = Parser_Expect(par, TokTag_Name), tok == NULL) {
        Parser_SetUnexpectedTokenError(par);
        goto Exit;
    }

    if (lhs_node = AstNode_NewVar(tok->ext.name.str), lhs_node == NULL) {
        Parser_SetNoEnoughMemoryError(par);
        goto Exit;
    }

    if (Parser_Expect(par, TokTag_Assign) == NULL) {
        Parser_SetUnexpectedTokenError(par);
        goto FreeLhsNode;
    }

    if (rhs_node = ParRule_Expr(par), Parser_Failed(par)) {
        goto FreeLhsNode;
    }

    if (Parser_Expect(par, TokTag_Semicolon) == NULL) {
        Parser_SetUnexpectedTokenError(par);
        goto FreeRhsNode;
    }

    if (stmt_node = AstNode_NewAsgnStmt(lhs_node, rhs_node),
        stmt_node == NULL) {

        Parser_SetNoEnoughMemoryError(par);
        goto FreeStmtNode;
    }

    goto Exit;

FreeStmtNode:
    AstNode_FreeTree(stmt_node);

FreeRhsNode:
    AstNode_FreeTree(rhs_node);

FreeLhsNode:
    AstNode_FreeTree(lhs_node);

Exit:
    return stmt_node;
}

static
AstNode *
ParRule_Stmt(
    Parser * par
);

static
AstSeq *
ParRule_Block(
    Parser * par
) {
    AstSeq * seq = NULL;
    AstNode * stmt_node;

    if (seq = AstSeq_New(), seq == NULL) {
        Parser_SetNoEnoughMemoryError(par);
        goto Exit;
    }

    if (Parser_Expect(par, TokTag_LeftBrace) == NULL) {
        Parser_SetUnexpectedTokenError(par);
        goto FreeSeq;
    }

    while (Parser_Check(par, TokTag_RightBrace) == false) {
        if (stmt_node = ParRule_Stmt(par), Parser_Failed(par)) {
            goto FreeSeq;
        }

        if (AstSeq_Push(seq, stmt_node) == false) {
            Parser_SetNoEnoughMemoryError(par);
            goto FreeStmtNode;
        }
    }

    Parser_Consume(par);

    goto Exit;

FreeStmtNode:
    AstNode_Free(stmt_node);

FreeSeq:
    AstSeq_Free(seq);

Exit:
    return seq;
}

static
AstNode *
ParRule_BlockStmt(
    Parser * par
) {
    AstNode * block_node = NULL;
    AstSeq * seq;

    if (seq = ParRule_Block(par), Parser_Failed(par)) {
        goto Exit;
    }

    if (block_node = AstNode_NewBlock(AstTag_BlockStmt, seq),
        block_node == NULL) {

        Parser_SetNoEnoughMemoryError(par);
        goto Exit;
    }

    goto Exit;

FreeSeq:
    AstSeq_Free(seq);

Exit:
    return block_node;
}

static
AstNode *
ParRule_IfOrIfElseStmt(
    Parser * par
) {
    AstNode * stmt_node = NULL;
    AstNode * cond_node;
    AstNode * then_br_node;
    AstNode * else_br_node;

    if (Parser_Expect(par, TokTag_If) == NULL) {
        Parser_SetUnexpectedTokenError(par);
        goto Exit;
    }

    if (cond_node = ParRule_Expr(par), Parser_Failed(par)) {
        goto Exit;
    }

    if (then_br_node = ParRule_BlockStmt(par), Parser_Failed(par)) {
        goto FreeCondNode;
    }

    if (Parser_Check(par, TokTag_Else)) {
        Parser_Consume(par);

        if (else_br_node = ParRule_BlockStmt(par), Parser_Failed(par)) {
            goto FreeThenBrNode;
        }

        if (stmt_node = AstNode_NewIfElseStmt(
            cond_node, then_br_node, else_br_node),
            stmt_node == NULL) {

            Parser_SetNoEnoughMemoryError(par);
            goto FreeElseBrNode;
        }
    } else {
        if (stmt_node = AstNode_NewIfStmt(cond_node, then_br_node),
            stmt_node == NULL) {

            Parser_SetNoEnoughMemoryError(par);
            goto FreeThenBrNode;
        }
    }

    goto Exit;

FreeElseBrNode:
    AstNode_FreeTree(else_br_node);

FreeThenBrNode:
    AstNode_FreeTree(then_br_node);

FreeCondNode:
    AstNode_FreeTree(cond_node);

Exit:
    return stmt_node;
}

static
AstNode *
ParRule_Stmt(
    Parser * par
) {
    AstNode * stmt_node = NULL;
    Token * tok;

    tok = Parser_Peek(par);
    switch (tok->tag) {
    case TokTag_Name:
        if (stmt_node = ParRule_AsgnStmt(par), Parser_Failed(par)) {
            goto Exit;
        }

        break;

    case TokTag_If:
        if (stmt_node = ParRule_IfOrIfElseStmt(par), Parser_Failed(par)) {
            goto Exit;
        }

        break;

    case TokTag_LeftBrace:
        if (stmt_node = ParRule_BlockStmt(par), Parser_Failed(par)) {
            goto Exit;
        }

        break;

    default:
        Parser_SetUnexpectedTokenError(par);
        goto Exit;
    }

    goto Exit;

Exit:
    return stmt_node;
}

AstNode *
ParRule_Prog(
    Parser * par
) {
    AstNode * prog_node = NULL;
    AstNode * stmt_node;
    AstSeq * seq;

    prog_node = AstNode_NewProg();
    if (prog_node == NULL) {
        Parser_SetNoEnoughMemoryError(par);
        goto Exit;
    }

    seq = prog_node->ext.block.seq;

    while (Parser_Check(par, TokTag_Eof) == false) {
        if (stmt_node = ParRule_Stmt(par), Parser_Failed(par)) {
            goto FreeProgNode;
        }

        if (AstSeq_Push(seq, stmt_node) == false) {
            Parser_SetNoEnoughMemoryError(par);
            goto FreeStmtNode;
        }
    }

    Parser_Expect(par, TokTag_Eof);

    goto Exit;

FreeStmtNode:
    AstNode_FreeTree(stmt_node);

FreeProgNode:
    AstNode_FreeTree(prog_node);
    prog_node = NULL;

Exit:
    return prog_node;
}