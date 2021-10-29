#include "mayocc.h"


// 次のトークンが期待している記号のときには， トークンを一つ読み進めて真を返す．
// それ以外の場合には偽を返す．
bool consume(char *op) {
    if (g_current_token->token_kind != TK_RESERVED ||
        strlen(op) != g_current_token->len ||
        memcmp(g_current_token->str, op, g_current_token->len)) {
        return false;
    }
    g_current_token = g_current_token->next;
    return true;
}


// 次のトークンが期待している記号のときには， トークンを一つ読み進める.
// それ以外の場合にはエラーを報告する．
void expect(char *op) {
    if (g_current_token->token_kind != TK_RESERVED ||
        strlen(op) != g_current_token->len ||
        memcmp(g_current_token->str, op, g_current_token->len)) {
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


// パーサ用の関数。循環参照しているので最初に宣言だけしておく。
Node *expr();

Node *equality();

Node *relational();

Node *add();

Node *mul();

Node *unary();

Node *primary();


Node *expr() {
    return equality();
}


Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume("=="))
            node = make_new_operator_node(ND_EQ, node, relational());
        else if (consume("!="))
            node = make_new_operator_node(ND_NEQ, node, relational());
        else
            return node;
    }
}


Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume("<="))
            node = make_new_operator_node(ND_LEQ, node, add());
        else if (consume(">="))
            node = make_new_operator_node(ND_LEQ, add(), node);
        else if (consume("<"))
            node = make_new_operator_node(ND_L, node, add());
        else if (consume(">"))
            node = make_new_operator_node(ND_L, add(), node);
        else
            return node;
    }
}


Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume("+"))
            node = make_new_operator_node(ND_ADD, node, mul());
        else if (consume("-"))
            node = make_new_operator_node(ND_SUB, node, mul());
        else
            return node;
    }
}


Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*"))
            node = make_new_operator_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = make_new_operator_node(ND_DIV, node, unary());
        else
            return node;
    }
}


Node *unary() {
    if (consume("+"))
        return primary();
    if (consume("-"))
        return make_new_operator_node(ND_SUB, make_new_num_node(0), primary());
    return primary();
}


Node *primary() {
    // 次のトークンが(なら　( expr ) のはず
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }
    // そうでないなら数値
    return make_new_num_node(expect_num());
}
