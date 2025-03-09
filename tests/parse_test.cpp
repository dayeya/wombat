#include <gtest/gtest.h>
#include "test_help.hpp"
#include "parser.hpp"

TEST(ParserEat, EatFromEmptyStream) {
    LazyTokenStream lts{};

    auto p = Parser(lts);

    //! 'cur_tok' should be nullptr since
    //! the parser didnt bump into the first token.
    EXPECT_EQ(p.tok_cur.cur, nullptr);

    //! the 'tok_cur' must be able to advance to be able to eat.
    ASSERT_EQ(p.tok_cur.can_advance(), false);

    //! try to eat.
    p.eat();

    //! 'cur_tok' should stay the same. 
    EXPECT_EQ(p.tok_cur.cur, nullptr);
}


TEST(ParserEat, EatFromNonEmptyStream) {
    LazyTokenStream lts{};

    lts.feed(*build_token("3", TokenKind::LiteralNum));
    lts.feed(*build_token("+", TokenKind::Plus));
    lts.feed(*build_token("1", TokenKind::LiteralNum));
    
    auto p = Parser(lts);

    //! the 'tok_cur' must be able to advance to be able to eat.
    ASSERT_EQ(p.tok_cur.can_advance(), true);

    p.eat();
    EXPECT_EQ(p.tok_cur.prev, nullptr);
    EXPECT_EQ(p.tok_cur.cur->kind, TokenKind::LiteralNum);

    p.eat();
    EXPECT_EQ(p.tok_cur.prev->kind, TokenKind::LiteralNum);
    EXPECT_EQ(p.tok_cur.cur->kind, TokenKind::Plus);

    p.eat();
    EXPECT_EQ(p.tok_cur.prev->kind, TokenKind::Plus);
    EXPECT_EQ(p.tok_cur.cur->kind, TokenKind::LiteralNum);
}