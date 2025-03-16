#include <gtest/gtest.h>
#include "test_help.hpp"
#include "ast.hpp"
#include "expr.hpp"

TEST(LiteralExprTest, HandlesLiteralExpr) {
    auto expr = Expr::Value(*build_token("3", TokenKind::LiteralNum));

    EXPECT_NE(expr.lit_kind, LiteralKind::Int);
    EXPECT_EQ(expr.val, "3");
}