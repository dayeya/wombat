#include <gtest/gtest.h>
#include "token.hpp"
#include "ast.hpp"
#include "expr.hpp"

unique_ptr<Token> build_token(std::string v, TokenKind k) {
    unique_ptr<Token> tok = std::make_unique<Token>();
    tok->fill_with_no_pos(v, k);
    return tok;
}

TEST(LiteralExprTest, HandlesLiteralExpr) {
    auto tok = build_token("3", TokenKind::LiteralNum);

    auto lit_expr = LitExpr(tok);

    EXPECT_NE(lit_expr.tok, nullptr);
    EXPECT_EQ(lit_expr.expr_kind, ExprKind::Lit);
    EXPECT_EQ(lit_expr.tok->kind, TokenKind::LiteralNum);
    EXPECT_EQ(lit_expr.tok->value, "4");
}