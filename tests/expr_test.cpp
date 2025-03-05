#include <gtest/gtest.h>
#include "token.hpp"
#include "ast.hpp"
#include "expr.hpp"

TEST(LiteralExprTest, HandlesLiteralExpr) {
    //! Creates a token representing a number.
    auto token = std::make_unique<Token>(
        TokenKind::LiteralNum,
        std::string("8"),
        std::make_pair(1, 0)
    );

    //! Create an expression from that token.
    auto lit_expr = LitExpr(std::move(token));

    EXPECT_EQ(lit_expr.expr_kind, ExprKind::Lit);
    EXPECT_NE(lit_expr.tokens, std::nullopt);
}

TEST(AstNodeTest, CreatesAnAstFromLitExpr) {
    auto token = std::make_unique<Token>(
        TokenKind::LiteralNum,
        std::string("8"),
        std::make_pair(1, 0)
    );

    auto lit_expr = LitExpr(std::move(token));

    auto ast = ExprNode(std::move(lit_expr));

    EXPECT_EQ((bool)ast.inner, true);
}