#include "env.hpp"
#include "parser.hpp"

using Expr::ExprKind;
using Tokenizer::Token;
using Tokenizer::TokenKind;

Ptr<ExprNode> Parser::expr_to_node(const Ptr<Expr::BaseExpr>& expr) {
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
        case ExprKind::Local:
        {
            auto* local = dynamic_cast<Expr::Local*>(expr.get());
            ASSERT(local != nullptr, "unexpected behavior: failed to cast to a local resource.");

            VarTerminalNode node(std::move(local->ident));
            return mk_ptr<VarTerminalNode>(std::move(node));
        }
        case ExprKind::FnCall:
        {
            auto* fn_call = dynamic_cast<Expr::FnCall*>(expr.get());
            ASSERT(fn_call != nullptr, "unexpected behavior: failed to cast to a function call expression.");

            std::vector<Ptr<ExprNode>> args{};
            for(auto& arg : fn_call->args) {
                args.push_back(expr_to_node(arg));
            }

            FnCallNode node(std::move(fn_call->ident), std::move(args));
            return mk_ptr<FnCallNode>(std::move(node));
        }
        case ExprKind::ArraySubscription:
        {
            auto* array_sub = dynamic_cast<Expr::ArraySubscription*>(expr.get());
            ASSERT(array_sub != nullptr, "unexpected behavior: failed to cast to an array subscription expression.");

            ArraySubscriptionNode node(
                std::move(array_sub->arr), 
                expr_to_node(array_sub->index)
            );
            return mk_ptr<ArraySubscriptionNode>(std::move(node));
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

Ptr<StmtNode> Parser::stmt_to_node(const Ptr<Statement::Stmt>& stmt) {
    switch (stmt->kind) {
        case StmtKind::Local:
        {
            auto* local = dynamic_cast<Var*>(stmt.get());
            ASSERT(local != nullptr, "unexpected behavior: failed to cast to a variable declaration.");

            Option<AssignOp> op = std::nullopt;
            Ptr<ExprNode> expr_node = nullptr;
            if (local->is_initialized()) {
                op = std::move(local->init.value().assignment);
                expr_node = expr_to_node(local->initializer_expr());
            }

            VarDeclarationNode node(
                std::move(local->info), 
                std::move(op), 
                std::move(expr_node)
            );
            return mk_ptr<VarDeclarationNode>(std::move(node));
        }
        case StmtKind::FnDefinition:
        {
            auto* fn = static_cast<Fn*>(stmt.get());
            ASSERT(fn != nullptr, "unexpected behavior: failed to cast to a function declaration.");
            
            std::vector<Ptr<StmtNode>> cur_ctx_stmt{};
            for(auto& stmt : fn->body.as_list())
            {
                cur_ctx_stmt.push_back(stmt_to_node(stmt));
            }

            BlockNode body(std::move(cur_ctx_stmt));
            FnHeaderNode header(std::move(fn->header));

            FnNode node(
                std::make_unique<FnHeaderNode>(std::move(header)), 
                std::make_unique<BlockNode>(std::move(body))
            );
            return mk_ptr<FnNode>(std::move(node));
        }
        case StmtKind::Expr:
        {
            auto* fn_call = dynamic_cast<FnCall*>(stmt.get());
            ASSERT(fn_call != nullptr, "unexpected behavior: failed to cast to a function call statement.");

            std::vector<Ptr<ExprNode>> args{};
            for(auto& arg : fn_call->inner_expr->args) {
                args.push_back(expr_to_node(arg));
            }

            FnCallNode node(std::move(fn_call->inner_expr->ident), std::move(args));
            return mk_ptr<FnCallNode>(std::move(node));
        }
        case StmtKind::FnReturn:
        {
            auto* ret_stmt = dynamic_cast<Statement::Return*>(stmt.get());
            ASSERT(ret_stmt != nullptr, "unexpected behavior: failed to cast to a return statement.");

            ReturnNode node(expr_to_node(ret_stmt->expr));
            return mk_ptr<ReturnNode>(std::move(node));
        }
        case StmtKind::Assignment:
        {
            auto* assign_stmt = dynamic_cast<Assignment*>(stmt.get());
            ASSERT(assign_stmt != nullptr, "unexpected behavior: failed to cast to an assignment statement.");

            AssignmentNode node(assign_stmt->init.assignment, assign_stmt->ident, expr_to_node(assign_stmt->init.expr));
            return mk_ptr<AssignmentNode>(std::move(node));
        }
        case StmtKind::Import:
        {
            auto* import_stmt = dynamic_cast<Statement::Import*>(stmt.get());
            ASSERT(import_stmt != nullptr, "unexpected behavior: failed to cast to an import statement.");

            ImportNode node(std::move(import_stmt->ident));
            return mk_ptr<ImportNode>(std::move(node));
        }
        default: 
        {
            ASSERT(false, std::format(
                "unreachable statement kind, got {}", 
                Statement::meaning_from_stmt_kind(stmt->kind)
            ));
            return nullptr;
        }
    }
}

Ptr<FnNode> Parser::parse_function_to_node() {
    Fn fn = parse_fn_decl();
    Ptr<StmtNode> stmt = stmt_to_node(mk_ptr<Fn>(std::move(fn)));
    
    FnNode* fn_node = dynamic_cast<FnNode*>(stmt.get());
    ASSERT(fn_node != nullptr, "unexpected behavior: failed to cast to a function node.");

    return mk_ptr<FnNode>(std::move(*fn_node));
}

void Parser::parse(AST& ast) {
    align_into_begining();

    while(tok_cur.can_advance()) {
        ast.push_function(std::move(parse_function_to_node()));
    }
}