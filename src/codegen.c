#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codegen.h"
#include "error.h"
#include "global.h"

int label_num = 0;

void lval_gen(Node *node);
void if_else_gen(Node *node);
void while_gen(Node *node);
void node_gen(Node *node);

void lval_gen(Node *node)
{
    if (node->kind != ND_LVAR)
        error_at(token->str, "代入の左辺値が変数ではありません");

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

void if_else_gen(Node *node)
{
    label_num++;
    if (node->rhs->kind == ND_ELSE)
    {
        node_gen(node->lhs);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lelse%d\n", label_num);
        node_gen(node->rhs->lhs);
        printf("    jmp .Lend%d\n", label_num);
        printf(".Lelse%d:\n", label_num);
        node_gen(node->rhs->rhs);
        printf(".Lend%d:\n", label_num);
        return;
    }
    else
    {
        node_gen(node->lhs);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lend%d\n", label_num);
        node_gen(node->rhs);
        printf(".Lend%d:\n", label_num);
        return;
    }
}

void while_gen(Node *node)
{
    label_num++;
    printf(".Lbegin%d:\n", label_num);
    node_gen(node->lhs);
    printf("    pop rax\n");
    printf("    cmp rax, 0\n");
    printf("    je .Lend%d\n", label_num);
    node_gen(node->rhs);
    printf("    jmp .Lbegin%d\n", label_num);
    printf(".Lend%d:\n", label_num);
}

void for_gen(Node *node)
{
    label_num++;
    node_gen(node->lhs->lhs);
    printf(".Lbegin%d:\n", label_num);
    node_gen(node->lhs->rhs);
    printf("    pop rax\n");
    printf("    cmp rax, 0\n");
    printf("    je .Lend%d\n", label_num);
    node_gen(node->rhs->rhs);
    node_gen(node->rhs->lhs);
    printf("    jmp .Lbegin%d\n", label_num);
    printf(".Lend%d:\n", label_num);
}

void block_gen(Node *node)
{
    for (int i = 0; i < node->stmts_len; i++)
    {
        node_gen(node->stmts[i]);
        printf("    pop rax\n");
    }
}

void funccall_gen(Node *node)
{
    printf("  call %s\n", node->funcname);
    printf("  push rax\n");
}

void node_gen(Node *node)
{
    switch (node->kind)
    {
    case ND_RETURN:
        node_gen(node->lhs);
        printf("    pop rax\n");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        return;
    case ND_IF:
        if_else_gen(node);
        return;
    case ND_WHILE:
        while_gen(node);
        return;
    case ND_FOR:
        for_gen(node);
        return;
    case ND_BLOCK:
        block_gen(node);
        return;
    case ND_FUNCALL:
        funccall_gen(node);
        return;
    case ND_NUM:
        printf("    push %d\n", node->val);
        return;
    case ND_LVAR:
        lval_gen(node);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;
    case ND_ASSIGN:
        lval_gen(node->lhs);
        node_gen(node->rhs);

        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mov [rax], rdi\n");
        printf("    push rdi\n");
        return;
    }

    node_gen(node->lhs);
    node_gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind)
    {
    case ND_ADD:
        printf("    add rax, rdi\n");
        break;
    case ND_SUB:
        printf("    sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("    imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("    cqo\n");
        printf("    idiv rdi\n");
        break;
    case ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_NE:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LT:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LE:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;
    }

    printf("    push rax\n");
}

void codegen()
{
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // 変数の領域を確保
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    for (int i = 0; code[i]; i++)
    {
        node_gen(code[i]);
        printf("    pop rax\n");
    }

    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}