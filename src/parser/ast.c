#include "ast.h"
#include "memory/allocate.h"

const char *
AstTag_ToStr(
    AstTag tag
) {
    switch (tag) {
    case AstTag_StrLit: return "StringLiteral";
    case AstTag_NumLit: return "NumericLiteral";
    case AstTag_BoolLit: return "BooleanLiteral";
    case AstTag_Var: return "Variable";

    case AstTag_LogNotOp: return "LogicalNot";
    case AstTag_LogOrOp:  return "LogicalOr";
    case AstTag_LogAndOp: return "LogicalAnd";

    case AstTag_RelEquOp: return "RelationalEqu";
    case AstTag_RelNeqOp: return "RelationalNeq";
    case AstTag_RelLtOp:  return "RelationalLt";
    case AstTag_RelLteOp: return "RelationalLte";
    case AstTag_RelGtOp:  return "RelationalGt";
    case AstTag_RelGteOp: return "RelationalGte";

    case AstTag_UnaPlusOp: return "UnaryPlus";
    case AstTag_UnaMinusOp: return "UnaryMinus";

    case AstTag_BinMulOp: return "BinaryMultiplication";
    case AstTag_BinDivOp: return "BinaryDivision";
    case AstTag_BinAddOp: return "BinaryAddition";
    case AstTag_BinSubOp: return "BinarySubtraction";
    case AstTag_BinModOp: return "BinaryModulus";
    case AstTag_BinExpOp: return "BinaryExponentiation";

    case AstTag_AsgnStmt: return "Assignment";
    case AstTag_IfStmt: return "If";
    case AstTag_IfElseStmt: return "IfElse";
    case AstTag_BlockStmt: return "Block";

    case AstTag_Prog: return "Program";
    }
}

AstNode *
AstNode_New(void) {
    return (AstNode *)MeMem_Malloc(sizeof(AstNode));
}

AstNode *
AstNode_NewProg(void) {
    AstSeq * seq = AstSeq_New();
    if (seq == NULL) {
        goto Exit;
    }

    AstNode * node = AstNode_New();
    if (node == NULL) {
        goto FreeSeq;
    }

    node->tag = AstTag_Prog;
    node->ext.block.seq = seq;

    return node;

FreeSeq:
    AstSeq_Free(seq);

Exit:
    return NULL;
}

AstNode *
AstNode_NewStrLit(
    FixedBuf * str
) {
    FixedBuf * _str = FixedBuf_Clone(str);
    if (_str == NULL) {
        goto Exit;
    }

    AstNode * node = AstNode_New();
    if (node == NULL) {
        goto FreeStr;
    }

    node->tag = AstTag_StrLit;
    node->ext.str_lit.str = _str;

    return node;

FreeStr:
    FixedBuf_Free(_str);

Exit:
    return NULL;
}

AstNode *
AstNode_NewNumLit(
    ssize num
) {
    AstNode * node = AstNode_New();
    if (node == NULL) {
        goto Exit;
    }

    node->tag = AstTag_NumLit;
    node->ext.num_lit.num = num;

    return node;

Exit:
    return NULL;
}

AstNode *
AstNode_NewBoolLit(
    bool val
) {
    AstNode * node = AstNode_New();
    if (node == NULL) {
        goto Exit;
    }

    node->tag = AstTag_BoolLit;
    node->ext.bool_lit.val = val;

    return node;

Exit:
    return NULL;
}

AstNode *
AstNode_NewVar(
    FixedBuf * str
) {
    FixedBuf * _str = FixedBuf_Clone(str);
    if (_str == NULL) {
        goto Exit;
    }

    AstNode * node = AstNode_New();
    if (node == NULL) {
        goto FreeStr;
    }

    node->tag = AstTag_Var;
    node->ext.var.str = _str;

    return node;

FreeStr:
    FixedBuf_Free(_str);

Exit:
    return NULL;
}

AstNode *
AstNode_NewUnaOp(
    AstTag tag,
    AstNode * opd
) {
    AstNode * node = AstNode_New();
    if (node == NULL) {
        goto Exit;
    }

    node->tag = tag;
    node->ext.una_op.opd = opd;

    return node;

Exit:
    return NULL;
}

AstNode *
AstNode_NewBinOp(
    AstTag tag,
    AstNode * lhs,
    AstNode * rhs
) {
    AstNode * node = AstNode_New();
    if (node == NULL) {
        goto Exit;
    }

    node->tag = tag;
    node->ext.bin_op.lhs = lhs;
    node->ext.bin_op.rhs = rhs;

    return node;

Exit:
    return NULL;
}

AstNode *
AstNode_NewBlock(
    AstTag tag,
    AstSeq * seq
) {
    AstNode * node = AstNode_New();
    if (node == NULL) {
        goto Exit;
    }

    node->tag = tag;
    node->ext.block.seq = seq;

    return node;

Exit:
    return NULL;
}

AstNode *
AstNode_NewAsgnStmt(
    AstNode * lhs,
    AstNode * rhs
) {
    AstNode * node = AstNode_New();
    if (node == NULL) {
        goto Exit;
    }

    node->tag = AstTag_AsgnStmt;
    node->ext.asgn_stmt.lhs = lhs;
    node->ext.asgn_stmt.rhs = rhs;

    return node;

Exit:
    return NULL;
}

AstNode *
AstNode_NewIfStmt(
    AstNode * cond,
    AstNode * then_br
) {
    AstNode * node = AstNode_New();
    if (node == NULL) {
        goto Exit;
    }

    node->tag = AstTag_IfStmt;
    node->ext.if_stmt.cond = cond;
    node->ext.if_stmt.then_br = then_br;

    return node;

Exit:
    return NULL;
}

AstNode *
AstNode_NewIfElseStmt(
    AstNode * cond,
    AstNode * then_br,
    AstNode * else_br
) {
    AstNode * node = AstNode_New();
    if (node == NULL) {
        goto Exit;
    }

    node->tag = AstTag_IfElseStmt;
    node->ext.if_else_stmt.cond = cond;
    node->ext.if_else_stmt.then_br = then_br;
    node->ext.if_else_stmt.else_br = else_br;

    return node;

Exit:
    return NULL;
}

bool
AstNode_PushAsStr_Recur(
    AstNode * node,
    FlexBuf * buf,
    ssize ind,
    usize * dep
) {
    const char * const label = AstTag_ToStr(node->tag);

    usize cur_dep = *dep;

    if (FlexBuf_PushDupByte(buf, ' ', ind * cur_dep) == false) {
        return false;
    }

    switch (node->tag) {
    case AstTag_StrLit: {
        const u8 * const str_buf = FixedBuf_Data(node->ext.str_lit.str);
        const usize str_len = FixedBuf_Size(node->ext.str_lit.str);
        if (FlexBuf_PushFmt(buf, "<%s \"%.*s\">",
            label, str_len, str_buf) == false) {

            return false;
        }

        break;
    }

    case AstTag_NumLit:
        if (FlexBuf_PushFmt(buf, "<%s %zd>",
            label, node->ext.num_lit.num) == false) {

            return false;
        }

        break;

    case AstTag_BoolLit:
        if (FlexBuf_PushFmt(buf, "<%s %s>",
            label, node->ext.bool_lit.val ? "true" : "false") == false) {

            return false;
        }

        break;

    case AstTag_Var: {
        const u8 * const str_buf = FixedBuf_Data(node->ext.var.str);
        const usize str_len = FixedBuf_Size(node->ext.var.str);
        if (FlexBuf_PushFmt(buf, "<%s \"%.*s\">",
            label, str_len, str_buf) == false) {

            return false;
        }

        break;
    }

    default:
        if (FlexBuf_PushFmt(buf, "<%s>", label) == false) {
            return false;
        }

        break;
    }

    if (FlexBuf_PushByte(buf, '\n') == false) {
        return false;
    }

    (*dep)++;

    switch (node->tag) {
    case AstTag_StrLit:
    case AstTag_NumLit:
    case AstTag_BoolLit:

    case AstTag_Var:
        break;

    case AstTag_LogNotOp:

    case AstTag_UnaPlusOp:
    case AstTag_UnaMinusOp:
        AstNode_PushAsStr_Recur(node->ext.una_op.opd, buf, ind, dep);
        break;

    case AstTag_LogOrOp:
    case AstTag_LogAndOp:

    case AstTag_RelEquOp:
    case AstTag_RelNeqOp:
    case AstTag_RelLtOp:
    case AstTag_RelLteOp:
    case AstTag_RelGtOp:
    case AstTag_RelGteOp:

    case AstTag_BinMulOp:
    case AstTag_BinDivOp:
    case AstTag_BinAddOp:
    case AstTag_BinSubOp:
    case AstTag_BinModOp:
    case AstTag_BinExpOp:
        AstNode_PushAsStr_Recur(node->ext.bin_op.lhs, buf, ind, dep);
        AstNode_PushAsStr_Recur(node->ext.bin_op.rhs, buf, ind, dep);
        break;

    case AstTag_AsgnStmt:
        AstNode_PushAsStr_Recur(node->ext.asgn_stmt.lhs, buf, ind, dep);
        AstNode_PushAsStr_Recur(node->ext.asgn_stmt.rhs, buf, ind, dep);
        break;

    case AstTag_IfStmt:
        AstNode_PushAsStr_Recur(node->ext.if_stmt.cond, buf, ind, dep);
        AstNode_PushAsStr_Recur(node->ext.if_stmt.then_br, buf, ind, dep);
        break;

    case AstTag_IfElseStmt:
        AstNode_PushAsStr_Recur(node->ext.if_else_stmt.cond, buf, ind, dep);
        AstNode_PushAsStr_Recur(node->ext.if_else_stmt.then_br, buf, ind, dep);
        AstNode_PushAsStr_Recur(node->ext.if_else_stmt.else_br, buf, ind, dep);
        break;

    case AstTag_BlockStmt:

    case AstTag_Prog: {
        AstNode ** buf_nodes = AstSeq_Data(node->ext.block.seq);
        usize num_nodes = AstSeq_Count(node->ext.block.seq);
        for (usize i = 0; i < num_nodes; i++) {
            AstNode_PushAsStr_Recur(buf_nodes[i], buf, ind, dep);
        }

        break;
    }

    }

    (*dep)--;

    return true;
}

bool
AstNode_PushAsStr(
    AstNode * node,
    FlexBuf * buf,
    ssize ind
) {
    usize dep = 0;
    return AstNode_PushAsStr_Recur(node, buf, ind, &dep);
}

void
AstNode_FreeInner(
    AstNode * node
) {
    switch (node->tag) {
    case AstTag_StrLit: FixedBuf_Free(node->ext.str_lit.str); break;
    case AstTag_Var: FixedBuf_Free(node->ext.var.str); break;
    default: break;
    }
}

void
AstNode_Free(
    AstNode * node
) {
    AstNode_FreeInner(node);
    MeMem_Free(node);
}

void
AstNode_FreeTree(
    AstNode * node
) {
    switch (node->tag) {
    case AstTag_StrLit:
        FixedBuf_Free(node->ext.str_lit.str);
        break;

    case AstTag_NumLit:
    case AstTag_BoolLit:
        break;

    case AstTag_Var:
        FixedBuf_Free(node->ext.var.str);
        break;

    case AstTag_LogNotOp:

    case AstTag_UnaPlusOp:
    case AstTag_UnaMinusOp:
        AstNode_FreeTree(node->ext.una_op.opd);
        break;

    case AstTag_LogOrOp:
    case AstTag_LogAndOp:

    case AstTag_RelEquOp:
    case AstTag_RelNeqOp:
    case AstTag_RelLtOp:
    case AstTag_RelLteOp:
    case AstTag_RelGtOp:
    case AstTag_RelGteOp:

    case AstTag_BinMulOp:
    case AstTag_BinDivOp:
    case AstTag_BinAddOp:
    case AstTag_BinSubOp:
    case AstTag_BinModOp:
    case AstTag_BinExpOp:
        AstNode_FreeTree(node->ext.bin_op.lhs);
        AstNode_FreeTree(node->ext.bin_op.rhs);
        break;

    case AstTag_AsgnStmt:
        AstNode_FreeTree(node->ext.asgn_stmt.lhs);
        AstNode_FreeTree(node->ext.asgn_stmt.rhs);
        break;

    case AstTag_IfStmt:
        AstNode_FreeTree(node->ext.if_stmt.cond);
        AstNode_FreeTree(node->ext.if_stmt.then_br);
        break;

    case AstTag_IfElseStmt:
        AstNode_FreeTree(node->ext.if_stmt.cond);
        AstNode_FreeTree(node->ext.if_else_stmt.then_br);
        AstNode_FreeTree(node->ext.if_else_stmt.else_br);
        break;

    case AstTag_BlockStmt:

    case AstTag_Prog:
        AstSeq_Free(node->ext.block.seq);
        break;
    }

    MeMem_Free(node);
}

typedef struct _AstSeq {
    FlexBuf * buf_nodes;
    usize num_nodes;
} AstSeq;

AstSeq *
AstSeq_New(void) {
    FlexBuf * buf = FlexBuf_New();
    if (buf == NULL) {
        goto Exit;
    }

    AstSeq * seq = (AstSeq *)MeMem_Malloc(sizeof(AstSeq));
    if (seq == NULL) {
        goto FreeFlexBuf;
    }

    seq->buf_nodes = buf;
    seq->num_nodes = 0;

    return seq;

FreeFlexBuf:
    FlexBuf_Free(buf);

Exit:
    return NULL;
}

bool
AstSeq_Push(
    AstSeq * seq,
    AstNode * node
) {
    if (FlexBuf_PushBuf(seq->buf_nodes, &node, sizeof(AstNode *)) == false) {
        return false;
    }

    seq->num_nodes += 1;

    return true;
}

AstNode **
AstSeq_Data(
    AstSeq * seq
) {
    return (AstNode **)FlexBuf_Data(seq->buf_nodes);
}

usize
AstSeq_Count(
    AstSeq * seq
) {
    return seq->num_nodes;
}

AstNode *
AstSeq_At(
    AstSeq * seq,
    usize idx
) {
    if (idx >= seq->num_nodes) {
        return NULL;
    }

    return ((AstNode **)FlexBuf_Data(seq->buf_nodes))[idx];
}

bool
AstSeq_PushAsStr(
    AstSeq * seq,
    FlexBuf * buf,
    ssize ind
) {
    bool res = false;

    FlexBuf * tmp = FlexBuf_New();
    if (tmp == NULL) {
        goto Exit;
    }

    if (FlexBuf_PushFmt(tmp, "<AstSeq(%zu): [", seq->num_nodes) == false) {
        goto FreeTmp;
    }

    if (FlexBuf_PushStr(tmp, "]>") == false ||
        FlexBuf_Merge(buf, tmp) == false) {
        goto FreeTmp;
    }

    res = true;

FreeTmp:
    FlexBuf_Free(tmp);

Exit:
    return res;
}

static
void
AstSeq_FreeNodes(
    AstSeq * seq
) {
    AstNode ** buf_nodes = (AstNode **)FlexBuf_Data(seq->buf_nodes);
    usize num_nodes = seq->num_nodes;

    for (usize i = 0; i < num_nodes; i++) {
        AstNode_FreeTree(buf_nodes[i]);
    }
}

void
AstSeq_Clear(
    AstSeq * seq
) {
    AstSeq_FreeNodes(seq);
    FlexBuf_Clear(seq->buf_nodes);
    seq->num_nodes = 0;
}

void
AstSeq_Free(
    AstSeq * seq
) {
    AstSeq_FreeNodes(seq);
    FlexBuf_Free(seq->buf_nodes);
    MeMem_Free(seq);
}