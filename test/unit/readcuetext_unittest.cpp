#include <gtest/gtest.h>
#include <string>
extern "C" {
#include "webvtt/parser_internal.h"
}

class ReadCuetext : public ::testing::Test
{
public:
  ReadCuetext() : self(0), cue(0) {}
  virtual void SetUp() {
    ASSERT_FALSE( WEBVTT_FAILED( webvtt_create_parser( &dummyread, &dummyerr,
                                                       0, &self ) ) )
      << "Failed to create parser";
    ASSERT_FALSE( WEBVTT_FAILED( webvtt_create_cue( &cue ) ) )
      << "Failed to allocate cue";
    self->top->state = T_BODY;
    do_push( self, 0, 0, T_CUE, cue, V_CUE, 1, 1 );
    webvtt_ref_cue( cue );
    cue->from = 1000;
    cue->until = 2000;
  }

  virtual void TearDown() {
    if( uptype() == V_TEXT ) {
      webvtt_release_string( &(self->top+1)->v.text );
      webvtt_release_cue( &self->top->v.cue );
    }
    webvtt_release_cue( &cue );
    webvtt_delete_parser( self );
    self = 0;
  }

  webvtt_status read_cuetext( const std::string &str, webvtt_uint &pos,
                              bool finished = true ) {
    return ::webvtt_read_cuetext( self, str.c_str(), &pos, static_cast<webvtt_uint>(str.size()),
                                  finished );
  }

  std::string cuetext() const {
    return std::string( reinterpret_cast<const char *>( cue->body.d->text ) );
  }

  webvtt_state_value_type uptype() const {
    return (self->top+1)->type;
  }

  std::string uptext() const {
    return std::string( reinterpret_cast<const char *>(
      (self->top+1)->v.text.d->text ) );
  }

private:
  static int WEBVTT_CALLBACK dummyerr( void *userdata, webvtt_uint
                                       line, webvtt_uint col,
                                       webvtt_error error ) {
    return -1;
  }
  static void WEBVTT_CALLBACK dummyread( void *userdata, webvtt_cue *cue ) {
    webvtt_release_cue( &cue );
  }
  webvtt_parser self;
  webvtt_cue *cue;
};

/**
 * Read single-line cuetext terminated by two '\n' characters (an empty line)
 */
TEST_F(ReadCuetext,SingleLineLFLF)
{
  webvtt_uint pos = 0;
  ASSERT_EQ( WEBVTT_SUCCESS, read_cuetext( "CueText\n\n", pos ) );
  EXPECT_EQ( 9, pos );
  EXPECT_EQ( "CueText", cuetext() );
}

/**
 * Read single-line cuetext terminated by two '\r' characters (an empty line)
 */
TEST_F(ReadCuetext,SingleLineCRCR)
{
  webvtt_uint pos = 0;
  ASSERT_EQ( WEBVTT_SUCCESS, read_cuetext( "CueText\r\r", pos ) );
  EXPECT_EQ( 9, pos );
  EXPECT_EQ( "CueText", cuetext() );
}

/**
 * Read single-line cuetext terminated by two '\r' characters (an empty line)
 * with text afterwards
 */
TEST_F(ReadCuetext,SingleLineCRCRXX)
{
  webvtt_uint pos = 0;
  ASSERT_EQ( WEBVTT_SUCCESS, read_cuetext( "CueText\r\rxx", pos ) );
  EXPECT_EQ( 9, pos );
  EXPECT_EQ( "CueText", cuetext() );
}

/**
 * Read single-line cuetext terminated by two '\r\n' sequences (an empty line)
 */
TEST_F(ReadCuetext,SingleLineCRLFCRLF)
{
  webvtt_uint pos = 0;
  ASSERT_EQ( WEBVTT_SUCCESS, read_cuetext( "CueText\r\n\r\n", pos ) );
  EXPECT_EQ( 11, pos );
  EXPECT_EQ( "CueText", cuetext() );
}

/**
 * Read single-line cuetext terminated by EOF marker
 */
TEST_F(ReadCuetext,SingleLineEOF)
{
  webvtt_uint pos = 0;
  ASSERT_EQ( WEBVTT_SUCCESS, read_cuetext( "CueText", pos ) );
  EXPECT_EQ( 7, pos );
  EXPECT_EQ( "CueText", cuetext() );
}

/**
 * Read multi-line cuetext terminated by two '\n' characters (an empty line)
 */
TEST_F(ReadCuetext,MultiLineLFLF)
{
  webvtt_uint pos = 0;
  ASSERT_EQ( WEBVTT_SUCCESS, read_cuetext( "CueText\nLine2\n\n", pos ) );
  EXPECT_EQ( 15, pos );
  EXPECT_EQ( "CueText\nLine2", cuetext() );
}

/**
 * Read multi-line cuetext terminated by two '\r' characters (an empty line)
 */
TEST_F(ReadCuetext,MultiLineCRCR)
{
  webvtt_uint pos = 0;
  ASSERT_EQ( WEBVTT_SUCCESS, read_cuetext( "CueText\nLine2\r\r", pos ) );
  EXPECT_EQ( 15, pos );
  EXPECT_EQ( "CueText\nLine2", cuetext() );
}

/**
 * Read multi-line cuetext terminated by two '\r' characters (an empty line)
 * with text afterwards
 */
TEST_F(ReadCuetext,MultiLineCRCRXX)
{
  webvtt_uint pos = 0;
  ASSERT_EQ( WEBVTT_SUCCESS, read_cuetext( "CueText\nLine2\r\rxx", pos ) );
  EXPECT_EQ( 15, pos );
  EXPECT_EQ( "CueText\nLine2", cuetext() );
}

/**
 * Read multi-line cuetext terminated by two '\r\n' sequences (an empty line)
 */
TEST_F(ReadCuetext,MultiLineCRLFCRLF)
{
  webvtt_uint pos = 0;
  ASSERT_EQ( WEBVTT_SUCCESS, read_cuetext( "CueText\nLine2\r\n\r\n", pos ) );
  EXPECT_EQ( 17, pos );
  EXPECT_EQ( "CueText\nLine2", cuetext() );
}

/**
 * Read multi-line cuetext terminated by EOF marker
 */
TEST_F(ReadCuetext,MultiLineEOF)
{
  webvtt_uint pos = 0;
  ASSERT_EQ( WEBVTT_SUCCESS, read_cuetext( "CueText\nLine2", pos ) );
  EXPECT_EQ( 13, pos );
  EXPECT_EQ( "CueText\nLine2", cuetext() );
}

/**
 * Read cuetext in multiple buffers (split on CRLF)
 */
TEST_F(ReadCuetext,MultiBuffersCRLF)
{
  webvtt_uint pos = 0;
  ASSERT_EQ( WEBVTT_UNFINISHED, read_cuetext( "CueText\r", pos, false ) );
  EXPECT_EQ( 8, pos );
  pos = 0;
  ASSERT_EQ( WEBVTT_SUCCESS, read_cuetext( "\nLine2", pos ) );
  EXPECT_EQ( 6, pos );
  EXPECT_EQ( "CueText\nLine2", cuetext() );
}

/**
 * Read cuetext in multiple buffers (single line)
 */
TEST_F(ReadCuetext,MultiBuffersSingleLine)
{
  webvtt_uint pos = 0;
  ASSERT_EQ( WEBVTT_UNFINISHED, read_cuetext( "CueText", pos, false ) );
  EXPECT_EQ( 7, pos );
  pos = 0;
  ASSERT_EQ( WEBVTT_SUCCESS, read_cuetext( " is grrrrrrrrrrrrreat!", pos ) );
  EXPECT_EQ( 22, pos );
  EXPECT_EQ( "CueText is grrrrrrrrrrrrreat!", cuetext() );
}

/**
* Read cuetext in multiple buffers (single line, with newline characters split
* between 2 buffers)
*/
TEST_F(ReadCuetext,MultiBuffersSingleLineSplitEOL)
{
 webvtt_uint pos = 0;
 ASSERT_EQ( WEBVTT_UNFINISHED, read_cuetext( "CueText\n", pos, false ) );
 EXPECT_EQ( 8, pos );
 pos = 0;
 ASSERT_EQ( WEBVTT_SUCCESS, read_cuetext( "\n", pos ) );
 EXPECT_EQ( 1, pos );
 EXPECT_EQ( "CueText", cuetext() );
}

/**
 * Read cuetext in multiple buffers (multi line)
 */
TEST_F(ReadCuetext,MultiBuffersMultiLine)
{
  webvtt_uint pos = 0;
  ASSERT_EQ( WEBVTT_UNFINISHED, read_cuetext( "CueText\n", pos, false ) );
  EXPECT_EQ( 8, pos );
  pos = 0;
  ASSERT_EQ( WEBVTT_SUCCESS, read_cuetext( "is grrrrrrrrrrrrreat!", pos ) );
  EXPECT_EQ( 21, pos );
  EXPECT_EQ( "CueText\nis grrrrrrrrrrrrreat!", cuetext() );
}

/**
 * Test that we stop reading cuetext if we encounter a line containing the
 * cuetimes separator '-->'.
 *
 * ('pos' should still take place at the end of the line containing the
 * arrow, because it's already been read into 'line_buffer')
 */
TEST_F(ReadCuetext,FinishedOnCueTimesSeparator)
{
  webvtt_uint pos = 0;
  ASSERT_EQ( WEBVTT_SUCCESS, read_cuetext( "CueText\n-->", pos ) );
  EXPECT_EQ( 11, pos );
  EXPECT_EQ( "CueText", cuetext() );
  ASSERT_EQ( V_TEXT, uptype() );
  EXPECT_EQ( "-->", uptext() );
}

