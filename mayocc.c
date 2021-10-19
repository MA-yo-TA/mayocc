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
    TokenKind token_kind; // トークンの型
    Token *next;          // 次の入力トークン
    int val;              // token_kind が TK_NUM の場合， その数値
    char *str;            // トークン文字列
};

// 抽象構文木のノードの種類
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
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
    if (g_current_token->token_kind != TK_RESERVED || g_current_token->str[0] != op) {
        return false;
    }
    g_current_token = g_current_token->next;
    return true;
}


// 次のトークンが期待している記号のときには， トークンを一つ読み進める.
// それ以外の場合にはエラーを報告する．
void expect(char op) {
    if (g_current_token->token_kind != TK_RESERVED || g_current_token->str[0] != op) {
        error_at(g_current_token->str, "'%c'ではありません", op);
    }
    g_current_token = g_current_token->next;
}


// 次のトークンが数値の場合， トークンを一つ読み進めてその数値を返す．
// それ以外の場合にはエラーを報告する．
int expect_num() {
    if (g_current_token->token_kind != TK_NUM) {
        error_at(g_current_token->str, "数ではありません");
    }
    int val = g_current_token->val;
    g_current_token = g_current_token->next;
    return val;
}


bool at_eof() {
    return g_current_token->token_kind == TK_EOF;
}


// 新しいトークンを作成する
Token *make_new_token(TokenKind kind, char *str) {
    Token *new_token = calloc(1, sizeof(Token));
    new_token->token_kind = kind;
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

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
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


Node *make_new_operator_node(Nodekind node_kind, Node *left_child, Node *right_child) {
    Node *new_node = calloc(1, sizeof(Node));
    new_node->node_kind = node_kind;
    new_node->left_child = left_child;
    new_node->right_child = right_child;
    return new_node;
}


Node *make_new_num_node(int val) {
    Node *new_node = calloc(1, sizeof(Node));
    new_node->node_kind = ND_NUM;
    new_node->val = val;
    return new_node;
}


// パーサ用の関数。再起的に循環参照しているので最初に宣言だけしておく。
Node *expr();
Node *mul();
Node *primary();

Node *expr() {
    Node *node = mul();

    for (;;) {
        if (consume('+'))
            node = make_new_operator_node(ND_ADD, node, mul());
        else if (consume('-'))
            node = make_new_operator_node(ND_SUB, node, mul());
        else
            return node;
    }
}


Node *mul() {
    Node *node = primary();

    for (;;) {
        if (consume('*'))
            node = make_new_operator_node(ND_MUL, node, primary());
        else if (consume('/'))
            node = make_new_operator_node(ND_DIV, node, primary());
        else
            return node;
    }
}


Node *primary() {
    // 次のトークンが(なら　( expr ) のはず
    if (consume('(')) {
        Node *node = expr();
        expect(')');
        return node;
    }
    // そうでないなら数値
    return make_new_num_node(expect_num());
}

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
    }

    printf("  push rax\n");
}


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    // トークナイズしてパースする
    user_input = argv[1];
    g_current_token = tokenize(user_input);
    Node *tree_top = expr();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // 抽象構文木を下りながらアセンブリ生成
    gen(tree_top);

    // スタックトップに式全体の値が残っているはずなので
    // それをRAXにロードして関数からの返り値とする
    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}