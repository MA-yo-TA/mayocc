#include "mayocc.h"

// 構文木をアセンブリへ変換
void gen(Node *node) {
    if (node->node_kind == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->left_child);
    gen(node->right_child);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->node_kind) {
        case ND_ADD:
            printf("  add rax, rdi\n");
            break;
        case ND_SUB:
            printf("  sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("  imul rax, rdi\n");
            break;
        case ND_DIV:
            // x86-64 のidivの仕様のためこういうことになってる
            printf("  cqo\n");
            printf("  idiv rdi\n");
            break;
        case ND_EQ:
            printf("  cmp rax, rdi\n");
            printf("  sete al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_NEQ:
            printf("  cmp rax, rdi\n");
            printf("  setne al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LEQ:
            printf("  cmp rax, rdi\n");
            printf("  setle al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_L:
            printf("  cmp rax, rdi\n");
            printf("  setl al\n");
            printf("  movzb rax, al\n");
            break;
    }

    printf("  push rax\n");
}

