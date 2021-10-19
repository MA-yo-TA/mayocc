#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// トークンの種類
typedef enum {
    TK_RESERVED, // 記号
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;


typedef struct Token Token;


// トークン型
struct Token {
    TokenKind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // kind が TK_NUM の場合， その数値
    char *str;      // トークン文字列
};


// 現在注目しているトークン． global変数
Token *g_current_token;


// 入力プログラム
char *user_input;


// エラーを報告するための関数
// エラー箇所を出力するように改良
void error_at(char *loc, char *format, ...) {
    va_list ap;
    va_start(ap, format);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");  // pos個の空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\n");
    exit(1);
}


// 次のトークンが期待している記号のときには， トークンを一つ読み進めて真を返す．
// それ以外の場合には偽を返す．
bool consume(char op) {
    if (g_current_token->kind != TK_RESERVED || g_current_token->str[0] != op) {
        return false;
    }
    g_current_token = g_current_token->next;
    return true;
}


// 次のトークンが期待している記号のときには， トークンを一つ読み進める.
// それ以外の場合にはエラーを報告する．
void expect(char op) {
    if (g_current_token->kind != TK_RESERVED || g_current_token->str[0] != op) {
        error_at(g_current_token->str, "'%c'ではありません", op);
    }
    g_current_token = g_current_token->next;
}


// 次のトークンが数値の場合， トークンを一つ読み進めてその数値を返す．
// それ以外の場合にはエラーを報告する．
int expect_num() {
    if (g_current_token->kind != TK_NUM) {
        error_at(g_current_token->str, "数ではありません");
    }
    int val = g_current_token->val;
    g_current_token = g_current_token->next;
    return val;
}


bool at_eof() {
    return g_current_token->kind == TK_EOF;
}


// 新しいトークンを作成する
Token *make_new_token(TokenKind kind, char *str) {
    Token *new_token = calloc(1, sizeof(Token));
    new_token->kind = kind;
    new_token->str = str;
    return new_token;
}

//current_tokenに繋げる
void append_new_token(Token *new, Token *cur) {
    cur->next = new;
}


// 入力文字列ｐをトークナイズしてそれを返す
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *current_token = &head;
    Token *new_token = calloc(1, sizeof(Token));

    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            new_token = make_new_token(TK_RESERVED, p++);
            append_new_token(new_token, current_token);
            current_token = new_token;
            continue;
        }

        if (isdigit(*p)) {
            new_token = make_new_token(TK_NUM, p);
            append_new_token(new_token, current_token);
            current_token = new_token;
            current_token->val = strtol(p, &p, 10);
            continue;
        }

        error_at(current_token->str, "トークナイズできません");
    }

    new_token = make_new_token(TK_EOF, p);
    append_new_token(new_token, current_token);
    return head.next;
}


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    user_input = argv[1];

    // トークナイズする
    g_current_token = tokenize(user_input);

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // 最初の式は数でなければならないのでそれをチェックして
    // 最初のmov命令を出力
    printf("    mov rax, %d\n", expect_num());

    // '+ <数>' あるいは '- <数>' というトークンの並びを消費しつつ
    // アセンブリを出力
    while (!at_eof()) {
        if (consume('+')) {
            printf("    add rax, %d\n", expect_num());
            continue;
        }

        if (consume('-')) {
            printf("    sub rax, %d\n", expect_num());
            continue;
        }
    }

    printf("    ret\n");
    return 0;
}