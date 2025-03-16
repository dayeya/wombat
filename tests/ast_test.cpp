#include <gtest/gtest.h>

#include "test_help.hpp"
#include "ast.hpp"
#include "expr.hpp"

TEST(AstNodeTest, CreatesAnAstFromLitExpr) {
    auto t1 = build_token("3", TokenKind::LiteralNum);
    auto t2 = build_token("4", TokenKind::LiteralNum);

    Ptr<ValueNode> left = std::make_unique<ValueNode>(Expr::Value(*t1));
    Ptr<ValueNode> right = std::make_unique<ValueNode>(Expr::Value(*t2));

    auto bin_op = BinaryOperator::Add;

    auto ast_node = BinOpNode(
        bin_op,
        std::move(left), 
        std::move(right) 
    );

    EXPECT_NE(ast_node.left, nullptr);
    EXPECT_NE(ast_node.right, nullptr);
    EXPECT_EQ(ast_node.match(BinOp::Plus), true);
    
    auto casted_left = dynamic_cast<ValueNode*>(ast_node.left.get());
    auto casted_right = dynamic_cast<ValueNode*>(ast_node.right.get());

    ASSERT_NE(casted_left->expr_exists(), false);
    ASSERT_EQ(casted_left->match(ExprKind::Lit), true);

    ASSERT_NE(casted_right->expr_exists(), false);
    ASSERT_EQ(casted_right->match(ExprKind::Lit), true);
}

TEST(TraversalTest, TraversesAnAst) {
    auto t1 = build_token("3", TokenKind::LiteralNum);
    auto t2 = build_token("4", TokenKind::LiteralNum);

    Ptr<ValueNode> left = std::make_unique<ValueNode>(Expr::Value(*t1));
    Ptr<ValueNode> right = std::make_unique<ValueNode>(Expr::Value(*t2));

    auto bin_op = BinOp::Plus;

    auto ast_node = BinOpNode(
        bin_op,
        std::move(left), 
        std::move(right) 
    );

    AST ast(std::make_unique<AstNode>(std::move(ast_node)));
    // ast.visit();
}