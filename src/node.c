#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "tokenizer.h"
#include "lvar.h"
#include "error.h"
#include "global.h"

Node *node;

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;

    return node;
}

Node *new_node_num(int val)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;

    return node;
}

Node *new_node_lvar(LVar *lvar)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->offset = lvar->offset;

    return node;
}

Node *primary()
{
    Token *tok = consume_indent();
    if (tok)
    {
        LVar *lvar = find_lvar(tok); // Cでこの記法行けるのか？
        if (!lvar)
            lvar = new_lvar(tok);
        return new_node_lvar(lvar);
    }

    if (consume("("))
    {
        Node *node = expr();
        expect(")", "開きカッコに対応する閉じカッコがねぇ！！");
        return node;
    }

    return new_node_num(expect_number());
}

Node *unary()
{
    if (consume("+"))
        return primary();
    if (consume("-"))
        return new_node(ND_SUB, new_node_num(0), primary());
    return primary();
}

Node *mul()
{
    Node *node = unary();

    for (;;)
    {
        if (consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *add()
{
    Node *node = mul();

    for (;;)
    {
        if (consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

Node *relational()
{
    Node *node = add();

    for (;;)
    {
        if (consume("<"))
            node = new_node(ND_LT, node, add());
        else if (consume(">"))
            node = new_node(ND_LT, add(), node);
        else if (consume("<="))
            node = new_node(ND_LE, node, add());
        else if (consume(">="))
            node = new_node(ND_LE, add(), node);
        else
            return node;
    }
}

Node *equality()
{
    Node *node = relational();

    for (;;)
    {
        if (consume("=="))
            return node = new_node(ND_EQ, node, relational());
        else if (consume("!="))
            return node = new_node(ND_NE, node, relational());
        else
            return node;
    }
}

Node *assign()
{
    Node *node = equality();
    if (consume("="))
        node = new_node(ND_ASSIGN, node, assign());
    return node;
}

Node *expr()
{
    return assign();
}

Node *stmt()
{
    Node *node;

    if (consume_token(TK_RETURN))
    {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
        expect(";", "セミコロンがねぇ！！");
        return node;
    }
    else if (consume_token(TK_IF))
    {
        node = calloc(1, sizeof(Node));
        node->kind = ND_IF;
        expect("(", "ifの後には開きカッコがねぇ！！");
        node->lhs = expr();
        expect(")", "開きカッコに対応する閉じカッコがねぇ！！");
        node->rhs = stmt();

                return node;
    }
    else
    {
        node = expr();
        expect(";", "セミコロンがねぇ！！");
        return node;
    }
}

void program()
{
    int i = 0;
    while (!at_eof())
        code[i++] = stmt();
    code[i] = NULL;
}