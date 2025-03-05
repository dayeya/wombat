#include <gtest/gtest.h>

#include "test_help.hpp"
#include "ast.hpp"
#include "expr.hpp"

TEST(AstNodeTest, CreatesAnAstFromLitExpr) {
    auto t1 = build_token("3", TokenKind::LiteralNum);
    auto t2 = build_token("4", TokenKind::LiteralNum);

    unique_ptr<ExprNode> left = make_unique<ExprNode>(LitExpr(t1));
    unique_ptr<ExprNode> right = make_unique<ExprNode>(LitExpr(t2));

    auto bin_op = BinOp::Plus;

    auto ast_node = BinOpNode(
        bin_op,
        std::move(left), 
        std::move(right) 
    );

    EXPECT_NE(ast_node.left, nullptr);
    EXPECT_NE(ast_node.right, nullptr);
    EXPECT_EQ(ast_node.operation, BinOp::Plus);
    
    auto casted_left = dynamic_cast<ExprNode*>(ast_node.left.get());
    auto casted_right = dynamic_cast<ExprNode*>(ast_node.right.get());

    ASSERT_NE(casted_left->inner.has_value(), false);
    ASSERT_EQ(casted_left->inner->expr_kind, ExprKind::Lit);

    ASSERT_NE(casted_right->inner.has_value(), false);
    ASSERT_EQ(casted_right->inner->expr_kind, ExprKind::Lit);
}