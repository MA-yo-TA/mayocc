#ifndef MAYOCC_H
#define MAYOCC_H
//-----
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-----
// 型定義
//-----

// トークンの種類
typedef enum {
    TK_RESERVED, // 記号
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;


typedef struct Token Token;


// トークン型
struct Token {
    TokenKind token_kind; // トークンの型
    Token *next;          // 次の入力トークン
    int val;              // token_kind が TK_NUM の場合， その数値
    char *str;            // トークン文字列
    int len;              // トークンの長さ
};

// 抽象構文木のノードの種類
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_EQ,  // ==
    ND_NEQ, // !=
    ND_LEQ, // <=
    ND_L,   // <
    ND_NUM, // 整数
} Nodekind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
    Nodekind node_kind; // ノードの型
    Node *left_child;   // 左辺
    Node *right_child;  // 右辺
    int val;            // node_kindがND_NUMのときだけ使う
};

//-----
// グローバル変数宣言
//-----
// 現在注目しているトークン．
extern Token *g_current_token;

// 入力プログラム
extern char *user_input;

//-----
// 関数宣言
//-----
extern Token *tokenize();
extern Node *expr();
extern void gen(Node *node);
extern void error_at(char *loc, char *format, ...);


//-----
#endif
