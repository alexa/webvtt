#include "payload_testfixture"
class PayloadEscapeCharacter : public PayloadTest {};

/*
 * Verifies that greater than escape characters in the cue text payload are parsed.
 * From http://dev.w3.org/html5/webvtt/#webvtt-cue-gt-escape (11/27/2012)
 */
TEST_F(PayloadEscapeCharacter, GT)
{
  loadVtt( "payload/escape-character/gt-escape-character.vtt", 1 );

  const Node node = getHeadOfCue( 0 )[ 0 ];

  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );  
  assertEquals( ">", node.text() );
}

/*
 * Verifies that less than escape characters in the cue text payload are parsed.
 * From http://dev.w3.org/html5/webvtt/#webvtt-cue-lt-escape (11/27/2012)
 */
TEST_F(PayloadEscapeCharacter, LT)
{
  loadVtt( "payload/escape-character/lt-escape-character.vtt", 1 );

  const Node node = getHeadOfCue( 0 )[ 0 ];

  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "<", node.text() );
}

/*
 * Verifies that ampersand escape characters in the cue text payload are parsed correctly.
 * From http://dev.w3.org/html5/webvtt/#webvtt-cue-amp-escape (11/27/2012)
 */
TEST_F(PayloadEscapeCharacter, Ampersand)
{
  loadVtt( "payload/escape-character/ampersand-escape-character.vtt", 1 );

  const Node node = getHeadOfCue( 0 )[ 0 ];
   
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "&", node.text() );
}

/*
 * Verifies that left to right escape characters are parsed.
 * From http://dev.w3.org/html5/webvtt/#webvtt-cue-lrm-escape (11/27/2012)
 */
TEST_F(PayloadEscapeCharacter, LRM)
{
  loadVtt( "payload/escape-character/lrm-escape-character.vtt", 1 );
   
    const Node node = getHeadOfCue( 0 )[ 0 ];
    
    /* verify that it is a Node */
    ASSERT_EQ( Node::Text, node.kind() );
    
    /* create a UTF8 representation of NBSP and compare with textnode */
    ASSERT_EQ( LRM, node.text().utf16At( 0 ) );
}

/*
 * Verifies that right to left escape characters are parsed.
 * From http://dev.w3.org/html5/webvtt/#webvtt-cue-rlm-escape (11/27/2012)
 */
TEST_F(PayloadEscapeCharacter, RLM)
{
  loadVtt( "payload/escape-character/rlm-escape-character.vtt", 1 );

  const Node node = getHeadOfCue( 0 )[ 0 ];
    
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind()) ;
    
  /* create a UTF8 representation of NBSP and compare with textnode */
  ASSERT_EQ( RLM, node.text().utf16At( 0 ) );
}

/*
 * Verifies that multiple escape characters are parsed.
 * From http://dev.w3.org/html5/webvtt/#webvtt-cue-text (11/27/2012)
 */
TEST_F(PayloadEscapeCharacter, MultipleEscapeCharacter)
{
  loadVtt( "payload/escape-character/multiple-escape-character.vtt", 1 );

  const Node node = getHeadOfCue( 0 )[ 0 ];
    
    /* verify that it is a Node */
    ASSERT_EQ( Node::Text, node.kind() );
    
    /* create a UTF8 representation of NBSP and compare with textnode */
    ASSERT_EQ( NBSP, node.text().utf16At( 0 ) );
}

/*
 * Verifies that multiple escape characters on multiple lines are parsed.
 * From http://dev.w3.org/html5/webvtt/#webvtt-cue-text (11/27/2012)
 *  Cue text text consists of one or more cue text components optionally separated by a single line terminator which can be:
 *    1. CR (U+000D)
 *    2. LF (U+000A)
 *    3. CRLF pair
 */
TEST_F(PayloadEscapeCharacter, MultilineMultipleEscapeCharacter)
{
  loadVtt( "payload/escape-character/multiline-multiple-escape-character.vtt", 1 );

  const Node node = getHeadOfCue( 0 )[ 0 ];

  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
    
  /* create a UTF8 representation of NBSP and compare with textnode */
  ASSERT_EQ( NBSP, node.text().utf16At( 0 ) );
}

/*
 * Verifies that multiple escape characters on multiple lines are parsed.
 * From http://dev.w3.org/html5/webvtt/#webvtt-cue-text (11/27/2012)
 *  Cue text text consists of one or more cue text components optionally separated by a single line terminator which can be:
 *    1. CR (U+000D)
 *    2. LF (U+000A)
 *    3. CRLF pair
 */
TEST_F(PayloadEscapeCharacter, MultilineMultipleEscapeCharacterCR)
{
  loadVtt( "payload/escape-character/multiline-multiple-escape-character-cr.vtt", 1 );

  const Node node = getHeadOfCue( 0 )[ 0 ];

  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
    
  /* create a UTF8 representation of NBSP and compare with textnode */
  ASSERT_EQ( NBSP, node.text().utf16At( 0 ) );
}

/*
 * Verifies that multiple escape characters on multiple lines are parsed.
 * From http://dev.w3.org/html5/webvtt/#webvtt-cue-text (11/27/2012)
 *  Cue text text consists of one or more cue text components optionally separated by a single line terminator which can be:
 *    1. CR (U+000D)
 *    2. LF (U+000A)
 *    3. CRLF pair
 */
TEST_F(PayloadEscapeCharacter, MultilineMultipleEscapeCharacterCRLF)
{
  loadVtt( "payload/escape-character/multiline-multiple-escape-character-crlf.vtt", 1 );

  const Node node = getHeadOfCue( 0 )[ 0 ];

  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
    
  /* create a UTF8 representation of NBSP and compare with textnode */
  ASSERT_EQ( NBSP, node.text().utf16At( 0 ) );
}

/*
 * Verifies that a file with a payload containing a newline in between two escape characters
 * will finish the parsing attempt gracefully.
 * From http://dev.w3.org/html5/webvtt/#parsing (12/10/2012):
 *
 * ...
 * 50. Bad cue: Discard cue.
 * 51. Bad cue loop: If position is past the end of input, then jump to the step labeled end.
 */
TEST_F(PayloadEscapeCharacter, MultilineMultipleEscapeCharacterExtraNewline)
{
  loadVtt( "payload/escape-character/multiline-multiple-escape-character-extra-newline.vtt", 1 );

  const Node node = getHeadOfCue( 0 )[ 0 ];
    
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
    
  /* create a UTF8 representation of NBSP and compare with textnode */
  ASSERT_EQ( NBSP, node.text().utf16At( 0 ) );
}

/*
 * Verifies that a file with a payload containing a newline in between two escape characters
 * will finish the parsing attempt gracefully.
 * From http://dev.w3.org/html5/webvtt/#parsing (12/10/2012):
 *
 * ...
 * 50. Bad cue: Discard cue.
 * 51. Bad cue loop: If position is past the end of input, then jump to the step labeled end.
 */
TEST_F(PayloadEscapeCharacter, MultilineMultipleEscapeCharacterExtraNewlineCR)
{
  loadVtt( "payload/escape-character/multiline-multiple-escape-character-extra-newline-cr.vtt", 1 );

  const Node node = getHeadOfCue( 0 )[ 0 ];
    
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
    
  /* create a UTF8 representation of NBSP and compare with textnode */
  ASSERT_EQ( NBSP, node.text().utf16At( 0 ) );
}

/*
 * Verifies that a file with a payload containing a newline in between two escape characters
 * will finish the parsing attempt gracefully.
 * From http://dev.w3.org/html5/webvtt/#parsing (12/10/2012):
 *
 * ...
 * 50. Bad cue: Discard cue.
 * 51. Bad cue loop: If position is past the end of input, then jump to the step labeled end.
 */
TEST_F(PayloadEscapeCharacter, MultilineMultipleEscapeCharacterExtraNewlineCRLF)
{
  loadVtt( "payload/escape-character/multiline-multiple-escape-character-extra-newline-crlf.vtt", 1 );

  const Node node = getHeadOfCue( 0 )[ 0 ];
    
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
    
  /* create a UTF8 representation of NBSP and compare with textnode */
  ASSERT_EQ( NBSP, node.text().utf16At( 0 ) );
}

/*
 * WebVTT Escape Character Specification being Tested:
 *
 * U+003B SEMICOLON character (;)
 *  First, examine the value of buffer:
 *  If buffer is the string "&amp", then append a U+0026 AMPERSAND character (&) to result.
 *  If buffer is the string "&lt", then append a U+003C LESS-THAN SIGN character (<) to result.
 *  If buffer is the string "&gt", then append a U+003E GREATER-THAN SIGN character (>) to result.
 *  If buffer is the string "&lrm", then append a U+200E LEFT-TO-RIGHT MARK character to result.
 *  If buffer is the string "&rlm", then append a U+200F RIGHT-TO-LEFT MARK character to result.
 *  If buffer is the string "&nbsp", then append a U+00A0 NO-BREAK SPACE character to result.
 *  Otherwise, append buffer followed by a U+003B SEMICOLON character (;) to result.
 *  Then, in any case, set tokenizer state to the WebVTT data state, and jump to the step labeled next.
 *
*/

/* Testing Space Character Escape */


/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
  Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Space Character Escape : '&nsp;' instead of '&nbsp;'
 Expected Output: '&nsp;'
*/
TEST_F(PayloadEscapeCharacter, NSP)
{
  loadVtt( "payload/escape-character/space-character-escape-nsp.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Space Character escape: &nsp;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates To:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 The Parser currently allows incorrect escapes to be outputted in the cue text string
 Expected Output: '&nbp'
*/
TEST_F(PayloadEscapeCharacter, NBP)
{
  loadVtt( "payload/escape-character/space-character-escape-nbp.vtt", 1 );

  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Space Character escape: &nbp", node.text() );

}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
  Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Space Character Escape : '&bsp;' instead of '&nbsp;'
 Expected Output: '&bsp;'
*/
TEST_F(PayloadEscapeCharacter, BSP)
{
  loadVtt( "payload/escape-character/space-character-escape-bsp.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Space Character escape: &bsp;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Space Character Escape : '&bp;' instead of '&nbsp;'
 Expected Output: '&bp;'
*/
TEST_F(PayloadEscapeCharacter, BP)
{
  loadVtt( "payload/escape-character/space-character-escape-bp.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Space Character escape: &bp;", node.text() );

}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Space Character Escape : '&b;' instead of '&nbsp;'
 Expected Output: '&b;'
*/
TEST_F(PayloadEscapeCharacter, B)
{
  loadVtt( "payload/escape-character/space-character-escape-b.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Space Character escape: &b;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Space Character Escape : '&s;' instead of '&nbsp;'
 Expected Output: '&s;'
*/
TEST_F(PayloadEscapeCharacter, S)
{
  loadVtt( "payload/escape-character/space-character-escape-bsp.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Space Character escape: &s;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Space Character Escape : '&ns;' instead of '&nbsp;'
 Expected Output: '&ns;'
*/
TEST_F(PayloadEscapeCharacter, NS)
{
  loadVtt( "payload/escape-character/space-character-escape-ns.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Space Character escape: &ns;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Space Character Escape : '&np;' instead of '&nbsp;'
 Expected Output: '&np;'
*/
TEST_F(PayloadEscapeCharacter, NP)
{
  loadVtt( "payload/escape-character/space-character-escape-np.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Space Character escape: &np", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Space Character Escape : '&sp;' instead of '&nbsp;'
 Expected Output: '&sp;'
*/
TEST_F(PayloadEscapeCharacter, SP)
{
  loadVtt( "payload/escape-character/space-character-escape-sp.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Space Character escape: &sp;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Space Character Escape : '&p;' instead of '&nbsp;'
 Expected Output: '&p;'
*/
TEST_F(PayloadEscapeCharacter, P)
{
  loadVtt( "payload/escape-character/space-character-escape-p.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Space Character escape: &p;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Space Character Escape : '&bs;' instead of '&nbsp;'
 Expected Output: '&bs;'
*/
TEST_F(PayloadEscapeCharacter, BS)
{
  loadVtt( "payload/escape-character/space-character-escape-bs.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Space Character escape: &bs;", node.text() );
}

/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Space Character Escape : '&nbs;' instead of '&nbsp;'
 Expected Output: '&nbs;'
*/
TEST_F(PayloadEscapeCharacter, NBS)
{
  loadVtt( "payload/escape-character/space-character-escape-nbs.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Space Character escape: &nbs;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Space Character Escape : '&nb;' instead of '&nbsp;'
 Expected Output: '&nb;'
*/
TEST_F(PayloadEscapeCharacter, NB)
{
  loadVtt( "payload/escape-character/space-character-escape-nb.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Space Character escape: &nb;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Space Character Escape : '&n;' instead of '&nbsp;'
 Expected Output: '&n;'
*/
TEST_F(PayloadEscapeCharacter, N)
{
  loadVtt( "payload/escape-character/space-character-escape-n.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Space Character escape: &n;", node.text() );
}

/* Testing Left-To-Right Character Escape */

/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Left to Right Character Escape : '&lr;' instead of '&lrm;'
 Expected Output: '&lr;'
*/
TEST_F(PayloadEscapeCharacter, LeftToRight)
{
  loadVtt( "payload/escape-character/left-to-right-character-escape-lr.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Left to Right Character escape: &lr;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Left to Right Character Escape : '&l;' instead of '&lrm;'
 Expected Output: '&l;'
*/
TEST_F(PayloadEscapeCharacter, L)
{
  loadVtt( "payload/escape-character/left-to-right-character-escape-l.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Left to Right Character escape: &l;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Left to Right Character Escape : '&lm;' instead of '&lrm;'
 Expected Output: '&lm;'
*/
TEST_F(PayloadEscapeCharacter, LM)
{
  loadVtt( "payload/escape-character/left-to-right-character-escape-lm.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Left to Right Character escape: &lm;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Left to Right Character Escape : '&rm;' instead of '&lrm;'
 Expected Output: '&rm;'
*/
TEST_F(PayloadEscapeCharacter, RM)
{
  loadVtt( "payload/escape-character/left-to-right-character-escape-rm.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Left to Right Character escape: &rm;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Left to Right Character Escape : '&m;' instead of '&lrm;'
 Expected Output: '&m;'
*/
TEST_F(PayloadEscapeCharacter, M)
{
  loadVtt( "payload/escape-character/left-to-right-character-escape-m.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Left to Right Character escape: &m;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Left to Right Character Escape : '&r;' instead of '&lrm;'
 Expected Output: '&r;'
*/
TEST_F(PayloadEscapeCharacter, R)
{
  loadVtt( "payload/escape-character/left-to-right-character-escape-r.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Left to Right Character escape: &r;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Left to Right Character Escape : '&lm;' instead of '&lrm;'
 Expected Output: '&lm;'
*/
TEST_F(PayloadEscapeCharacter, LM_LeftToRight)
{
  loadVtt( "payload/escape-character/left-to-right-character-escape-lm.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Left to Right Character escape: &lm;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Left to Right Character Escape : '&rl;' instead of '&lrm;'
 Expected Output: '&rl;'
*/
TEST_F(PayloadEscapeCharacter, RL)
{
  loadVtt( "payload/escape-character/left-to-right-character-escape-rl.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Left to Right Character escape: &rl;", node.text() );
}

/* Test Ampersand Character Escape */

/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Expected Output: '&'
*/
TEST_F(PayloadEscapeCharacter, AmpersandCharacter)
{
  loadVtt( "payload/escape-character/ampersand-character-escape.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Ampersand escape: &", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Ampersand Escape : '&a;' instead of '&amp;'
 Expected Output: '&a;'
*/
TEST_F(PayloadEscapeCharacter, A)
{
  loadVtt( "payload/escape-character/ampersand-character-escape-a.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Ampersand escape: &a;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Ampersand Escape : '&am;' instead of '&amp;'
 Expected Output: '&am;'
*/
TEST_F(PayloadEscapeCharacter, AM)
{
  loadVtt( "payload/escape-character/ampersand-character-escape-a.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Ampersand escape: &am;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Ampersand Escape : '&mp;' instead of '&amp;'
 Expected Output: '&mp;'
*/
TEST_F(PayloadEscapeCharacter, MP)
{
  loadVtt( "payload/escape-character/ampersand-character-escape-mp.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Ampersand escape: &mp;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Ampersand Escape : '&p;' instead of '&amp;'
 Expected Output: '&p;'
*/
TEST_F(PayloadEscapeCharacter, P_Ampersand)
{
  loadVtt( "payload/escape-character/ampersand-character-escape-p.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Ampersand escape: &p;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Ampersand Escape : '&ap;' instead of '&amp;'
 Expected Output: '&ap;'
*/
TEST_F(PayloadEscapeCharacter, AP)
{
  loadVtt( "payload/escape-character/ampersand-character-escape-ap.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Ampersand escape: &ap;", node.text() );
}

/* Test Less Than Character Escape */

/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Less Than Escape : '&l;' instead of '&lt;'
 Expected Output: '&l;'
*/
TEST_F(PayloadEscapeCharacter, LessThan)
{
  loadVtt( "payload/escape-character/less-than-character-escape-l.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Less Than escape: &l;", node.text() );
}
/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Less Than Escape : '&t;' instead of '&lt;'
 Expected Output: '&t;'
*/
TEST_F(PayloadEscapeCharacter, T)
{
  loadVtt( "payload/escape-character/less-than-character-escape-t.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Less Than escape: &t;", node.text() );
}

/* Test Greater Than Escape Character */

/*
 WebVTT Specification Version:
 WebVTT - Living Standard Last Updated 2 October 2012

 Relates to:
 Cue Text Escape Characters:
 http://dev.w3.org/html5/webvtt/#webvtt-cue-text-parsing-rules

 Description:
 Incorrect Greater Than Escape : '&g;' instead of '&gt;'
 Expected Output: '&g;'
*/
TEST_F(PayloadEscapeCharacter, GreaterThan)
{
  loadVtt( "payload/escape-character/greater-than-character-escape-g.vtt", 1 );
  const Node node = getHeadOfCue( 0 )[ 0 ];
  
  /* verify that it is a Node */
  ASSERT_EQ( Node::Text, node.kind() );
  assertEquals( "Test Greater Than escape: &g;", node.text() );
}
