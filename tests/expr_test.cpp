#include <gtest/gtest.h>
#include "test_help.hpp"
#include "ast.hpp"
#include "expr.hpp"

TEST(LiteralExprTest, HandlesLiteralExpr) {
    auto tok = build_token("3", TokenKind::LiteralNum);

    auto lit_expr = LitExpr(tok);

    EXPECT_NE(lit_expr.tok, nullptr);
    EXPECT_EQ(lit_expr.expr_kind, ExprKind::Lit);
    EXPECT_EQ(lit_expr.tok->kind, TokenKind::LiteralNum);
    EXPECT_EQ(lit_expr.tok->value, "4");
}