#include <gtest/gtest.h>
#include "test_help.hpp"
#include "parser.hpp"

TEST(ParserFullTr, EatsSeqOfTokens) {
    LazyTokenStream lts{};

    lts.advance_with_token(build_token("3", TokenKind::LiteralNum));
    lts.advance_with_token(build_token("+", TokenKind::Plus));
    lts.advance_with_token(build_token("1", TokenKind::LiteralNum));

    auto p = Parser(std::move(lts));

    p.eat();
    EXPECT_EQ(p.cur_tok->kind, TokenKind::LiteralNum);

    p.eat();
    EXPECT_EQ(p.cur_tok->kind, TokenKind::LiteralNum);

    p.eat();
    EXPECT_EQ(p.cur_tok->kind, TokenKind::LiteralNum);
}

TEST(ParserFullTr, EatsSeqOfTokens) {
    LazyTokenStream lts{};

    auto p = Parser(std::move(lts));

    p.eat();
    EXPECT_EQ(p.cur_tok->kind, TokenKind::LiteralNum);
}
