#include <gtest/gtest.h>
#include <string>
extern "C" {
#include "webvtt/parser_internal.h"
}

class Lexer : public ::testing::Test
{
public:
  Lexer() : self(0) {}
  virtual void SetUp() {
    ASSERT_FALSE( WEBVTT_FAILED( webvtt_create_parser( &dummyread, &dummyerr,
                                                       0, &self ) ) )
      << "Failed to create parser";
  }

  virtual void TearDown() {
    webvtt_delete_parser( self );
    self = 0;
  }

  webvtt_token lex_newline( const std::string &str, webvtt_uint &pos,
                            bool finished = true ) {
    return webvtt_lex_newline( self, str.c_str(), &pos, static_cast<webvtt_uint>(str.size()), finished );
  }

  webvtt_token lex( const std::string &str, webvtt_uint &pos, bool finished = true ) {
    return ::webvtt_lex( self, str.c_str(), &pos, static_cast<webvtt_uint>(str.size()), finished );
  }

  webvtt_lexer_state lexerState() const {
    return self->tstate;
  }

private:
  static int WEBVTT_CALLBACK dummyerr( void *userdata, webvtt_uint
                                       line, webvtt_uint col,
                                       webvtt_error error ) {
    return -1;
  }
  static void WEBVTT_CALLBACK dummyread( void *userdata, webvtt_cue *cue ) {}
  webvtt_parser self;
};

/**
 * Test that we correctly return a NEWLINE token when the final buffer ends with
 * a carriage return.
 */
TEST_F(Lexer,LexNewLineCR)
{
  webvtt_uint pos = 0;
  EXPECT_EQ( NEWLINE, lex_newline( "\r", pos ) );
  EXPECT_EQ( 1, pos );
  EXPECT_EQ( L_START, lexerState() );
}

/**
 * Test that we correctly identify a single line-feed character as a newline
 */
TEST_F(Lexer,LexNewLineLF)
{
  webvtt_uint pos = 0;
  EXPECT_EQ( NEWLINE, lex_newline( "\n", pos ) );
  EXPECT_EQ( 1, pos );
  EXPECT_EQ( L_START, lexerState() );
}

/**
 * Test that we correctly identify a CRLF sequence in the same buffer
 */
TEST_F(Lexer,LexNewLineCRLF)
{
  webvtt_uint pos = 0;
  EXPECT_EQ( NEWLINE, lex_newline( "\r\n", pos ) );
  EXPECT_EQ( 2, pos );
  EXPECT_EQ( L_START, lexerState() );
}

/**
 * Test that we correctly identify a CRLF sequence split between two buffers
 */
TEST_F(Lexer,LexNewLineCRLFSplit)
{
  webvtt_uint pos = 0;
  EXPECT_EQ( UNFINISHED, lex_newline( "\r", pos, false ) );
  EXPECT_EQ( 1, pos );
  EXPECT_EQ( L_NEWLINE0, lexerState() );
  pos = 0;
  EXPECT_EQ( NEWLINE, lex_newline( "\n", pos, true ) );
  EXPECT_EQ( 1, pos );
  EXPECT_EQ( L_START, lexerState() );
}

/**
 * Test that we correctly identify a CR that is not followed by an LF, and
 * backup to the correct position.
 */
TEST_F(Lexer,LexNewLineCRXX)
{
  webvtt_uint pos = 0;
  EXPECT_EQ( NEWLINE, lex_newline( "\rx", pos ) );
  EXPECT_EQ( 1, pos );
  EXPECT_EQ( L_START, lexerState() );
}

/**
 * Test that we correctly identify a LF not that the end of a string.
 */
TEST_F(Lexer,LexNewLineLFXX)
{
  webvtt_uint pos = 0;
  EXPECT_EQ( NEWLINE, lex_newline( "\nx", pos ) );
  EXPECT_EQ( 1, pos );
  EXPECT_EQ( L_START, lexerState() );
}

/**
 * Test that we correctly identify a LF followed by CR.
 */
TEST_F(Lexer,LexNewLineLFCR)
{
  webvtt_uint pos = 0;
  EXPECT_EQ( NEWLINE, lex_newline( "\n\r", pos ) );
  EXPECT_EQ( 1, pos );
  EXPECT_EQ( L_START, lexerState() );
}


/**
 * Test that we return end-of-buffer as a newline.
 */
TEST_F(Lexer,LexNewLineEOL)
{
  webvtt_uint pos = 0;
  EXPECT_EQ( NEWLINE, lex_newline( "", pos ) );
  EXPECT_EQ( 0, pos );
  EXPECT_EQ( L_START, lexerState() );
}

/**
 * Test that we correctly return BADTOKEN and backup appropriately when we don't
 * encounter a CR or LF
 */
TEST_F(Lexer,LexNewLineXX)
{
  webvtt_uint pos = 0;
  EXPECT_EQ( BADTOKEN, lex_newline( "xxx", pos ) );
  EXPECT_EQ( 0, pos );
  EXPECT_EQ( L_START, lexerState() );
}

/**
 * Test that we correctly return WEBVTT when preceded by a BOM character
 */
TEST_F(Lexer,LexWEBVTTWithBOM)
{
  webvtt_uint pos = 0;
  EXPECT_EQ( WEBVTT, lex( "\xEF\xBB\xBFWEBVTT", pos ) );
  EXPECT_EQ( 9, pos );
  EXPECT_EQ( L_START, lexerState() );
}

/**
 * Test that we correctly return WEBVTT when not preceded by a BOM character
 */
TEST_F(Lexer,LexWEBVTT)
{
  webvtt_uint pos = 0;
  EXPECT_EQ( WEBVTT, lex( "WEBVTT", pos ) );
  EXPECT_EQ( 6, pos );
  EXPECT_EQ( L_START, lexerState() );
}

