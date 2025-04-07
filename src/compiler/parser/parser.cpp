#include "parser.hpp"

using Expr::ExprKind;
using Tokenizer::Token;
using Tokenizer::TokenKind;

Ptr<Node> Parser::expr_to_node(const Ptr<Expr::BaseExpr>& expr) {
    ASSERT(expr != nullptr, "invalid expression ref, got null pointer.");

    switch (expr->kind) {
        case ExprKind::Lit:
        {
            auto* val_expr = dynamic_cast<Expr::Literal*>(expr.get()); 
            ASSERT(val_expr != nullptr, "unexpected behavior: failed to cast to a literal expression.");
            LiteralNode node(*val_expr);
            return mk_ptr<LiteralNode>(std::move(node));
        }
        case ExprKind::Binary:
        {
            auto* bin_expr = dynamic_cast<Expr::BinExpr*>(expr.get());
            ASSERT(bin_expr != nullptr, "unexpected behavior: failed to cast to a binary expression.");
            BinOpNode node(bin_expr->op, expr_to_node(bin_expr->lhs), expr_to_node(bin_expr->rhs));
            return mk_ptr<BinOpNode>(std::move(node));
        }
        case ExprKind::Unary:
        {
            auto* unary = dynamic_cast<Expr::UnaryExpr*>(expr.get());
            ASSERT(unary != nullptr, "unexpected behavior: failed to cast to an unary expression.");
            UnaryOpNode node(unary->op, expr_to_node(unary->expr));
            return mk_ptr<UnaryOpNode>(std::move(node));
        }
        case ExprKind::Group:
        {
            auto* paren_expr = dynamic_cast<Expr::GroupExpr*>(expr.get()); 
            ASSERT(paren_expr != nullptr, "unexpected behavior: failed to cast to a grouped expression.");

            // In that case, we just unwrap the underlying expression.
            return expr_to_node(paren_expr->expr);
        }
        default:
        {
            ASSERT(false, std::format(
                "unreachable expression kind, got '{}'", 
                Expr::meaning_from_expr_kind(expr->kind)
            ));
            return nullptr;
        }
    }
}

Ptr<Node> Parser::stmt_to_node(const Ptr<Statement::Stmt>& stmt) {
    switch (stmt->kind) {
        case StmtKind::Local:
        {
            auto* local = dynamic_cast<Declaration::Var*>(stmt.get());
            ASSERT(local != nullptr, "unexpected behavior: failed to cast to a variable declaration.");

            Ptr<Node> expr_node = nullptr;
            if (local->is_initialized()) {
                expr_node = expr_to_node(local->initializer_expr());
            }

            LocalDeclNode node(mk_ptr<Declaration::Var>(std::move(*local)), std::move(expr_node));
            return mk_ptr<LocalDeclNode>(std::move(node));
        }
        default: {
            ASSERT(false, std::format(
                "unreachable statement kind, got {}", 
                Statement::meaning_from_stmt_kind(stmt->kind)
            ));
            return nullptr;
        }
    }
}

bool Parser::eat_and_expect(bool condition, std::string expect_err) {
    eat();
    if(condition) return true;
    ASSERT(false, expect_err);
}

bool Parser::eat_and_expect_no_err(bool condition) {
    eat();
    return condition;
}

void Parser::parse(AST& ast) {
    align_into_begining();

    Ptr<Statement::Stmt> local = parse_stmt_without_recovery();
    
    Ptr<Node> parent = stmt_to_node(local);
    ast.set_entry_point(parent);
}