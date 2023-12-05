#include <gtest/gtest.h>
#include <webvttxx/string>

using namespace WebVTT;

/**
 * an-nyung ha-se-yo encoded in UTF8
 */
const char UTF8AnNyungHaSeYo[] =
{
  '\xEC', '\x95', '\x88', /* U+C548 */
  '\xEB', '\x85', '\x95', /* U+B155 */
  '\xED', '\x95', '\x98', /* U+D558 */
  '\xEC', '\x84', '\xB8', /* U+C138 */
  '\xEC', '\x9A', '\x94', /* U+C694 */
  '\x00'                  /* NULL */
};

/**
 * an-nyung ha-se-yo encoded in UTF16
 */
const webvtt_uint16 UTF16AnNyungHaSeYo[] =
{
  0xC548, 0xB155, 0xD558, 0xC138, 0xC694, 0x0000
};

const char UTF8ReplacementChar[] = { '\xEF', '\xBF', '\xBD' };

TEST(String,CreateWithTextStrlen)
{
  webvtt_string str;
  ASSERT_EQ( WEBVTT_SUCCESS, webvtt_create_string_with_text( &str,
                                                             "Hello World",
                                                             -1 ) );
  EXPECT_EQ( 11, webvtt_string_length( &str ) );
  EXPECT_STREQ( "Hello World", webvtt_string_text( &str ) );
  webvtt_release_string( &str );
}

TEST(String,CreateWithText)
{
  webvtt_string str;
  ASSERT_EQ( WEBVTT_SUCCESS, webvtt_create_string_with_text( &str,
                                                             "Hello World",
                                                             11 ) );
  EXPECT_EQ( 11, webvtt_string_length( &str ) );
  EXPECT_STREQ( "Hello World", webvtt_string_text( &str ) );
  webvtt_release_string( &str );
}

/**
 * webvtt_string_getline with automatic length detection
 */
TEST(String,GetLineEOFStrlen)
{
  webvtt_uint pos = 0;
  webvtt_string str;
  webvtt_init_string( &str );
  ASSERT_LT( 0, webvtt_string_getline( &str, "Hello world", &pos,
                                       -1, 0, 1 ) );
  EXPECT_EQ( 11, pos );
  EXPECT_EQ( 11, webvtt_string_length( &str ) );
  EXPECT_STREQ( "Hello world", webvtt_string_text( &str ) );
  webvtt_release_string( &str );
}

TEST(String,GetLineLFStrlen)
{
  webvtt_uint pos = 0;
  webvtt_string str;
  webvtt_init_string( &str );
  ASSERT_LT( 0, webvtt_string_getline( &str, "Hello world\n", &pos,
                                       -1, 0, 1 ) );
  EXPECT_EQ( 11, pos );
  EXPECT_EQ( 11, webvtt_string_length( &str ) );
  EXPECT_STREQ( "Hello world", webvtt_string_text( &str ) );
  webvtt_release_string( &str );
}

TEST(String,GetLineCRStrlen)
{
  webvtt_uint pos = 0;
  webvtt_string str;
  webvtt_init_string( &str );
  ASSERT_LT( 0, webvtt_string_getline( &str, "Hello world\r", &pos,
                                       -1, 0, 1 ) );
  EXPECT_EQ( 11, pos );
  EXPECT_EQ( 11, webvtt_string_length( &str ) );
  EXPECT_STREQ( "Hello world", webvtt_string_text( &str ) );
  webvtt_release_string( &str );
}

/**
 * webvtt_string_getline without automatic length detection
 */
TEST(String,GetLineEOF)
{
  webvtt_uint pos = 0;
  webvtt_string str;
  webvtt_init_string( &str );
  ASSERT_LT( 0, webvtt_string_getline( &str, "Hello world", &pos,
                                       11, 0, 1 ) );
  EXPECT_EQ( 11, pos );
  EXPECT_EQ( 11, webvtt_string_length( &str ) );
  EXPECT_STREQ( "Hello world", webvtt_string_text( &str ) );
  webvtt_release_string( &str );
}

TEST(String,GetLineLF)
{
  webvtt_uint pos = 0;
  webvtt_string str;
  webvtt_init_string( &str );
  ASSERT_LT( 0, webvtt_string_getline( &str, "Hello world\n", &pos,
                                       12, 0, 1 ) );
  EXPECT_EQ( 11, pos );
  EXPECT_EQ( 11, webvtt_string_length( &str ) );
  EXPECT_STREQ( "Hello world", webvtt_string_text( &str ) );
  webvtt_release_string( &str );
}

TEST(String,GetLineCR)
{
  webvtt_uint pos = 0;
  webvtt_string str;
  webvtt_init_string( &str );
  ASSERT_LT( 0, webvtt_string_getline( &str, "Hello world\r", &pos, 12, 0,
                                       1 ) );
  EXPECT_EQ( 11, pos );
  EXPECT_EQ( 11, webvtt_string_length( &str ) );
  EXPECT_STREQ( "Hello world", webvtt_string_text( &str ) );
  webvtt_release_string( &str );
}

/**
 * string_getline append to existing string with automatic length detection
 */
TEST(String,AppendGetLineStrlen)
{
  WebVTT::uint pos = 0;
  webvtt_string str;
  ASSERT_EQ( WEBVTT_SUCCESS, webvtt_create_string_with_text( &str,
                                                             "Hello Wor",
                                                             9 ) );
  EXPECT_EQ( 9, webvtt_string_length( &str ) );
  ASSERT_LT( 0, webvtt_string_getline( &str, "ld! This is a test! Yup\n",
                                       &pos, -1,  0, 1 ) );
  EXPECT_EQ( 32, webvtt_string_length( &str ) );
  EXPECT_STREQ( "Hello World! This is a test! Yup",
                webvtt_string_text( &str ) );
  webvtt_release_string( &str );
}

/**
 * string_getline append to existing string without automatic length detection
 */
TEST(String,AppendGetLine)
{
  WebVTT::uint pos = 0;
  webvtt_string str;
  ASSERT_EQ( WEBVTT_SUCCESS, webvtt_create_string_with_text( &str, "Hello Wor",
                                                             9 ) );
  EXPECT_EQ( 9, webvtt_string_length( &str ) );
  ASSERT_LT( 0, webvtt_string_getline( &str, "ld! This is a test! Yup\n",
                                       &pos, 24,  0, 1 ) );
  EXPECT_EQ( 32, webvtt_string_length( &str ) );
  EXPECT_STREQ( "Hello World! This is a test! Yup",
                webvtt_string_text( &str ) );
  webvtt_release_string( &str );
}

/**
 * string_append boundary condition with automatic length detection
 */
TEST(String,AppendStrlen)
{
  WebVTT::uint pos = 0;
  webvtt_string str;
  ASSERT_EQ( WEBVTT_SUCCESS, webvtt_create_string_with_text( &str, "Hello Wor",
                                                             9 ) );
  EXPECT_EQ( 9, webvtt_string_length( &str ) );
  ASSERT_EQ( WEBVTT_SUCCESS, webvtt_string_append( &str,
                                                   "ld! This is a test! Yup",
                                                   -1 ) );
  EXPECT_EQ( 32, webvtt_string_length( &str ) );
  EXPECT_STREQ( "Hello World! This is a test! Yup",
                webvtt_string_text( &str ) );
  webvtt_release_string( &str );
}

/**
 * string_append boundary condition without automatic length detection
 */
TEST(String,Append)
{
  WebVTT::uint pos = 0;
  webvtt_string str;
  ASSERT_EQ( WEBVTT_SUCCESS, webvtt_create_string_with_text( &str, "Hello Wor",
                                                             9 ) );
  EXPECT_EQ( 9, webvtt_string_length( &str ) );
  ASSERT_EQ( WEBVTT_SUCCESS, webvtt_string_append( &str,
                                                  "ld! This is a test! Yup",
                                                  23 ) );
  EXPECT_EQ( 32, webvtt_string_length( &str ) );
  EXPECT_STREQ( "Hello World! This is a test! Yup",
                webvtt_string_text( &str ) );
  webvtt_release_string( &str );
}

/**
 * Test the webvtt_utf8_length routine
 */
TEST(String,UTF8CodepointLength)
{
  EXPECT_EQ( 3, webvtt_utf8_length( UTF8AnNyungHaSeYo ) );
}

/**
 * Test the webvtt_utf8_chcount routine
 */
TEST(String,UTF8CodepointCount)
{
  EXPECT_EQ( 5, webvtt_utf8_chcount( UTF8AnNyungHaSeYo, 0 ) )
    << "Incorrectly counted UTF8 codepoints";
}

/**
 * Test the webvtt_utf8_to_utf16 routine
 */
TEST(String,UTF8ToUTF16)
{
  EXPECT_EQ( UTF16AnNyungHaSeYo[0], webvtt_utf8_to_utf16(
    UTF8AnNyungHaSeYo+0, 0, 0 ) );
  EXPECT_EQ( UTF16AnNyungHaSeYo[1], webvtt_utf8_to_utf16(
    UTF8AnNyungHaSeYo+3, 0, 0 ) );
  EXPECT_EQ( UTF16AnNyungHaSeYo[2], webvtt_utf8_to_utf16(
    UTF8AnNyungHaSeYo+6, 0, 0 ) );
  EXPECT_EQ( UTF16AnNyungHaSeYo[3], webvtt_utf8_to_utf16(
    UTF8AnNyungHaSeYo+9, 0, 0 ) );
  EXPECT_EQ( UTF16AnNyungHaSeYo[4], webvtt_utf8_to_utf16(
    UTF8AnNyungHaSeYo+12, 0, 0 ) );
}

/**
 * Test the webvtt_skip_utf8 routine
 */
TEST(String,UTF8Skip)
{
  const char *b = UTF8AnNyungHaSeYo + 0;
  ASSERT_TRUE( webvtt_skip_utf8( &b, 0, 4 ) ) << "Failed to skip"
    << " characters";
  ASSERT_EQ( UTF8AnNyungHaSeYo + 12, b ) << "Didn't skip to correct"
    << " position";
  ASSERT_FALSE( webvtt_skip_utf8( &b, 0, 2 ) ) << "Skipped beyond"
    << " end of string";
}

/**
 * Test the webvtt_skip_utf8 routine starting from non-header
 * character
 */
TEST(String,UTF8SkipTrailing)
{
  const char *b = UTF8AnNyungHaSeYo + 1;
  ASSERT_TRUE( webvtt_skip_utf8( &b, 0, 1 ) ) << "Failed to skip"
    << " character";
  ASSERT_EQ( b, UTF8AnNyungHaSeYo + 3 ) << "Skipped to incorrect"
    << " position";
}

/**
 * Test that we're correctly detecting if a UTF16 code unit is a
 * lead surrogate
 */
TEST(String,UTF16IsLeadSurrogate)
{
  webvtt_uint16 lead = 0xD843;
  ASSERT_TRUE( String::isSurrogateLead( lead ) );
  ASSERT_FALSE( String::isSurrogateTrail( lead ) );
}

/**
 * Test that we're correctly detecting if a UTF16 code unit is a
 * trail surrogate
 */
TEST(String,UTF16IsTrailSurrogate)
{
  webvtt_uint16 trail = 0xDC96;
  ASSERT_TRUE( String::isSurrogateTrail( trail ) );
  ASSERT_FALSE( String::isSurrogateLead( trail ) );
}

/**
 * Test that we can correctly identify codepoints that require
 * multiple UTF16 units.
 */
TEST(String,UTF32RequiresSurrogate)
{
  webvtt_uint32 surrogate = 0xD843DC96; /* U+20C96 */
  webvtt_uint32 single = 0x00000489; /* U+0489 */
  ASSERT_TRUE( String::requiresSurrogate( surrogate ) );
  ASSERT_FALSE( String::requiresSurrogate( single ) );
}

/**
 * Test that we correctly report that a string is empty.
 */
TEST(String,IsEmpty)
{
  const char ne[] = "Not empty!";
  webvtt_string str = { 0 };
  webvtt_init_string( &str );
  ASSERT_TRUE( webvtt_string_is_empty( &str ) );
  webvtt_release_string( &str );
  webvtt_create_string( 0, &str );
  ASSERT_TRUE( webvtt_string_is_empty( &str ) );
  webvtt_string_append( &str, ne, sizeof( ne ) );
  ASSERT_FALSE( webvtt_string_is_empty( &str ) );
  webvtt_release_string( &str );
}

/**
 * Test that we correctly report that a string is empty using the C++
 * bindings.
 */
TEST(String,IsEmptyCXX)
{
  String str;
  ASSERT_TRUE( str.isEmpty() );
  str = String( "" );
  ASSERT_TRUE( str.isEmpty() );
  str.append( "Not Empty!" );
  ASSERT_FALSE( str.isEmpty() );
}

/**
 * Test that replace behaves correctly
 */
TEST(String,Replace)
{
  char conststr[3] = {0,0,0};
  char expectedOutput[] = { '\xEF', '\xBF', '\xBD', 0, 0, 0 };
  webvtt_string str;
  ASSERT_EQ( WEBVTT_SUCCESS, webvtt_create_string_with_text( &str, conststr,
                                                             3 ) );
  EXPECT_EQ( 3, webvtt_string_length( &str ) );
  EXPECT_EQ( 1, webvtt_string_replace( &str, "\0", 1, UTF8ReplacementChar,
                                       3 ) );
  EXPECT_STREQ( expectedOutput, webvtt_string_text( &str ) );
  webvtt_release_string( &str );
}

/**
 * Test that replace behaves correctly (with strlen)
 */
TEST(String,ReplaceStrlen)
{
  char conststr[] = "potato";
  char expectedOutput[] = "poTato";
  webvtt_string str;
  ASSERT_EQ( WEBVTT_SUCCESS, webvtt_create_string_with_text( &str, conststr,
                                                             6 ) );
  EXPECT_EQ( 6, webvtt_string_length( &str ) );
  EXPECT_EQ( 1, webvtt_string_replace( &str, "t", 1, "T", 1 ) );
  EXPECT_STREQ( expectedOutput, webvtt_string_text( &str ) );
  webvtt_release_string( &str );
}


/**
 * Test that replace_all behaves correctly
 */
TEST(String,ReplaceAll)
{
  char conststr[] = "\0a\0b";
  char expectedOutput[] = { '\xEF', '\xBF', '\xBD', 'a', '\xEF', '\xBF', '\xBD', 'b', 0 };
  webvtt_string str;
  ASSERT_EQ( WEBVTT_SUCCESS, webvtt_create_string_with_text( &str, conststr,
                                                             4 ) );
  EXPECT_EQ( 4, webvtt_string_length( &str ) );
  EXPECT_EQ( WEBVTT_SUCCESS, webvtt_string_replace_all( &str, "\0", 1,
                                                        UTF8ReplacementChar,
                                                        3 ) );
  EXPECT_STREQ( expectedOutput, webvtt_string_text( &str ) );
  webvtt_release_string( &str );
}

/**
 * Test that replace_all behaves correctly (with strlen)
 */
TEST(String,ReplaceAllStrlen)
{
  char conststr[] = "potato";
  char expectedOutput[] = "poTaTo";
  webvtt_string str;
  ASSERT_EQ( WEBVTT_SUCCESS, webvtt_create_string_with_text( &str, conststr,
                                                             6 ) );
  EXPECT_EQ( 6, webvtt_string_length( &str ) );
  EXPECT_EQ( WEBVTT_SUCCESS, webvtt_string_replace_all( &str, "t", -1,
                                                        "T", -1 ) );
  EXPECT_STREQ( expectedOutput, webvtt_string_text( &str ) );
  webvtt_release_string( &str );
}
