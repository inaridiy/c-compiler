#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "tokenizer.h"
#include "lvar.h"
#include "func.h"
#include "error.h"
#include "global.h"
#include "node_utils.h"

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

Node *new_node_func_call(Func *func)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_FUNCALL;
    node->funcname = func->name;
    node->funcname_len = func->len;

    return node;
}

Node *new_node_func_def(Func *func, DynamicNodeArray *args, Node *body)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_FUNCDEF;
    node->funcname = func->name;
    node->funcname_len = func->len;
    node->funcdef_args = args;
    node->body = body;

    return node;
}

Node *primary() // 値を返す ex) 1 (1 + 2) a (a + b)なぜか関数の定義もある
{
    Token *indent = consume_indent();
    if (indent)
    {
        if (consume("("))
        {
            Func *func = find_func(indent);
            if (!func)
            {

                func = new_func(indent);
                DynamicNodeArray *args = init_dynamic_node_array(4);

                while (!consume(")"))
                {
                    LVar *lvar = new_lvar(consume_indent());
                    push_node(args, new_node_lvar(lvar));
                    consume(",");
                }

                Node *node = new_node_func_def(func, args, stmt());

                return node;
            }
            else
            {
                Node *node = new_node_func_call(func);
                node->args = init_dynamic_node_array(4);

                while (!consume(")"))
                {
                    Node *arg = expr();
                    push_node(node->args, arg);
                    consume(",");
                }

                return node;
            }
        }

        else
        {
            LVar *lvar = find_lvar(indent);

            if (!lvar)
                lvar = new_lvar(indent);
            return new_node_lvar(lvar);
        }
    }

    if (consume("("))
    {
        Node *node = expr();
        expect(")", "開きカッコに対応する閉じカッコがねぇ！！");
        return node;
    }

    return new_node_num(expect_number());
}

Node *unary() // 単項演算子 + - ex) +1 -1
{
    if (consume("+"))
        return primary();
    if (consume("-"))
        return new_node(ND_SUB, new_node_num(0), primary());
    return primary();
}

Node *mul() // 乗算除算 ex) 1 * 2 1 / 2
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

Node *add() // 加算減算 ex) 1 + 2 1 - 2
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

Node *relational() // 比較演算子 ex) 1 < 2 1 > 2 1 <= 2 1 >= 2
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

Node *equality() // 等価演算子 ex) 1 == 2 1 != 2
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

Node *assign() // 代入演算子 ex) a = 1 a += 1 a -= 1
{
    Node *node = equality();
    if (consume("="))
        node = new_node(ND_ASSIGN, node, assign());
    return node;
}

Node *expr() // 式
{
    return assign();
}

Node *stmt() // 文
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

        if (consume_token(TK_ELSE))
        {
            Node *else_node = calloc(1, sizeof(Node));
            else_node->kind = ND_ELSE;
            else_node->lhs = node->rhs;
            else_node->rhs = stmt();
            node->rhs = else_node;
        }

        return node;
    }
    else if (consume_token(TK_WHILE))
    {
        node = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        expect("(", "whileの後には開きカッコがねぇ！！");
        node->lhs = expr();
        expect(")", "開きカッコに対応する閉じカッコがねぇ！！");
        node->rhs = stmt();
        return node;
    }
    else if (consume_token(TK_FOR))
    {
        node = calloc(1, sizeof(Node));
        node->kind = ND_FOR;
        Node *node_left = calloc(1, sizeof(Node));
        Node *node_right = calloc(1, sizeof(Node));
        node->lhs = node_left;
        node->rhs = node_right;

        expect("(", "forの後には開きカッコがねぇ！！");
        if (!consume(";"))
        {
            node_left->lhs = expr();
            expect(";", "セミコロンがねぇ！！");
        }
        if (!consume(";"))
        {
            node_left->rhs = expr();
            expect(";", "セミコロンがねぇ！！");
        }
        if (!consume(")"))
        {
            node_right->lhs = expr();
            expect(")", "開きカッコに対応する閉じカッコがねぇ！！");
        }
        node_right->rhs = stmt();
        return node;
    }
    else if (consume("{"))
    {

        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;
        node->stmts = init_dynamic_node_array(16);

        while (!consume("}"))
        {
            push_node(node->stmts, stmt());
        }

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