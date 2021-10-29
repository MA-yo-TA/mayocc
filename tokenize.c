#include "mayocc.h"

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


// 新しいトークンを作成する
Token *make_new_token(TokenKind kind, char *str, int len) {
    Token *new_token = calloc(1, sizeof(Token));
    new_token->token_kind = kind;
    new_token->str = str;
    new_token->len = len;
    return new_token;
}

//current_tokenに繋げる
void append_new_token(Token *new, Token *cur) {
    cur->next = new;
}

bool startwith(char *p, char *start_str) {
    return memcmp(p, start_str, strlen(start_str)) == 0;
}


// 入力文字列ｐをトークナイズしてそれを返す
Token *tokenize() {
    char *p = user_input;
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

        // 2文字のトークンを先にチェック
        if (startwith(p, "==") || startwith(p, "!=") ||
            startwith(p, "<=") || startwith(p, ">=")) {
            new_token = make_new_token(TK_RESERVED, p, 2);
            append_new_token(new_token, current_token);
            current_token = new_token;
            p += 2;
            continue;
        }

        // 1文字のトークンをチェック
        if (strchr("+-*/()<>", *p)) {
            new_token = make_new_token(TK_RESERVED, p, 1);
            append_new_token(new_token, current_token);
            current_token = new_token;
            p += 1;
            continue;
        }

        if (isdigit(*p)) {
            new_token = make_new_token(TK_NUM, p, 0);
            append_new_token(new_token, current_token);
            current_token = new_token;
            char *q = p;
            current_token->val = strtol(p, &p, 10);
            current_token->len = p - q;
            continue;
        }

        error_at(current_token->str, "トークナイズできません");
    }

    new_token = make_new_token(TK_EOF, p, 0);
    append_new_token(new_token, current_token);
    return head.next;
}
