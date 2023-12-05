#include "cue_testfixture"
class FileStructure : public CueTest { };
/* Covers the tests for general file structure.
 * #1001-1019
 *
 */

/*
 * Verifies that a file with only the WebVTT signature will parse correctly.
 * From http://dev.w3.org/html5/webvtt/#webvtt-file-body (12/02/2012):
 *
 * A WebVTT file body consists of the following components, in the following order:
 * 1. An optional U+FEFF BYTE ORDER MARK (BOM) character.
 * 2. The string "WEBVTT".
 * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
 *    character followed by any number of characters that are not U+000A LINE FEED (LF)
 *    or U+000D CARRIAGE RETURN (CR) characters.
 * 4. Two or more WebVTT line terminators.
 * 5. Zero or more WebVTT cues and/or WebVTT comments separated from each other
 *    by two or more WebVTT line terminators.
 * 6. Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, WebVTTNoBOM)
{
  loadVtt( "filestructure/webvtt-no-bom.vtt", 0 );
  ASSERT_EQ( 0, errorCount() ) << "This file should contain no errors.";
}

/*
 * Verifies that a file with a BOM character and the WebVTT signature will parse correctly.
 * From http://dev.w3.org/html5/webvtt/#webvtt-file-body (12/02/2012):
 *
 * A WebVTT file body consists of the following components, in the following order:
 * 1. An optional U+FEFF BYTE ORDER MARK (BOM) character.
 * 2. The string "WEBVTT".
 * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
 *    character followed by any number of characters that are not U+000A LINE FEED (LF)
 *    or U+000D CARRIAGE RETURN (CR) characters.
 * 4. Two or more WebVTT line terminators.
 * 5. Zero or more WebVTT cues and/or WebVTT comments separated from each other
 *    by two or more WebVTT line terminators.
 * 6. Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, WebVTTWithBOM)
{
  loadVtt( "filestructure/webvtt-with-bom.vtt", 0 );
  ASSERT_EQ( 0, errorCount() ) << "This file should contain no errors.";
}

/*
 * Verifies that a file with the WebVTT signature and a space character will parse correctly.
 * From http://dev.w3.org/html5/webvtt/#webvtt-file-body (12/02/2012):
 *
 * A WebVTT file body consists of the following components, in the following order:
 * 1. An optional U+FEFF BYTE ORDER MARK (BOM) character.
 * 2. The string "WEBVTT".
 * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
 *    character followed by any number of characters that are not U+000A LINE FEED (LF)
 *    or U+000D CARRIAGE RETURN (CR) characters.
 * 4. Two or more WebVTT line terminators.
 * 5. Zero or more WebVTT cues and/or WebVTT comments separated from each other by two or more WebVTT line terminators.
 * 6. Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, WebVTTSpace)
{
  loadVtt( "filestructure/webvtt-space.vtt", 0 );
  ASSERT_EQ( 0, errorCount() ) << "This file should contain no errors.";
}

/*
 * Verifies that a file with the WebVTT signature and a tab character will parse correctly.
 * From http://dev.w3.org/html5/webvtt/#webvtt-file-body (12/02/2012):
 *
 * A WebVTT file body consists of the following components, in the following order:
 * 1. An optional U+FEFF BYTE ORDER MARK (BOM) character.
 * 2. The string "WEBVTT".
 * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
 *    character followed by any number of characters that are not U+000A LINE FEED (LF)
 *    or U+000D CARRIAGE RETURN (CR) characters.
 * 4. Two or more WebVTT line terminators.
 * 5. Zero or more WebVTT cues and/or WebVTT comments separated from each other
 *    by two or more WebVTT line terminators.
 * 6. Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, WebVTTTab)
{
  loadVtt( "filestructure/webvtt-tab.vtt", 0 );
  ASSERT_EQ( 0, errorCount() ) << "This file should contain no errors.";
}

/*
 * Verifies that a file with the WebVTT signature, a tab character, and non-line terminating
 * characters will parse correctly.
 * From http://dev.w3.org/html5/webvtt/#webvtt-file-body (12/02/2012):
 *
 * A WebVTT file body consists of the following components, in the following order:
 * 1. An optional U+FEFF BYTE ORDER MARK (BOM) character.
 * 2. The string "WEBVTT".
 * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
 *    character followed by any number of characters that are not U+000A LINE FEED (LF)
 *    or U+000D CARRIAGE RETURN (CR) characters.
 * 4. Two or more WebVTT line terminators.
 * 5. Zero or more WebVTT cues and/or WebVTT comments separated from each other
 *    by two or more WebVTT line terminators.
 * 6. Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, WebVTTTabText)
{
  loadVtt( "filestructure/webvtt-tab-text.vtt", 0 );
  ASSERT_EQ( 0, errorCount() ) << "This file should contain no errors.";
}

/*
 * Verifies that a file with the WebVTT signature, a space, and non-line terminating
 * characters will parse correctly.
 * From http://dev.w3.org/html5/webvtt/#webvtt-file-body (12/02/2012):
 *
 * A WebVTT file body consists of the following components, in the following order:
 * 1. An optional U+FEFF BYTE ORDER MARK (BOM) character.
 * 2. The string "WEBVTT".
 * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
 *    character followed by any number of characters that are not U+000A LINE FEED (LF)
 *    or U+000D CARRIAGE RETURN (CR) characters.
 * 4. Two or more WebVTT line terminators.
 * 5. Zero or more WebVTT cues and/or WebVTT comments separated from each other
 *    by two or more WebVTT line terminators.
 * 6. Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, WebVTTSpaceText)
{
  loadVtt( "filestructure/webvtt-space-text.vtt", 0 );
  ASSERT_EQ( 0, errorCount() ) << "This file should contain no errors.";
}

/*
 * Verifies that a file with the WebVTT signature, a space, non-line terminating
 * characters followed by EOL will parse correctly.
 * From http://dev.w3.org/html5/webvtt/#webvtt-file-body (12/02/2012):
 *
 * A WebVTT file body consists of the following components, in the following order:
 * 1. An optional U+FEFF BYTE ORDER MARK (BOM) character.
 * 2. The string "WEBVTT".
 * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
 *    character followed by any number of characters that are not U+000A LINE FEED (LF)
 *    or U+000D CARRIAGE RETURN (CR) characters.
 * 4. Two or more WebVTT line terminators.
 * 5. Zero or more WebVTT cues and/or WebVTT comments separated from each other
 *    by two or more WebVTT line terminators.
 * 6. Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, WebVTTSpaceTextEOL)
{
  loadVtt( "filestructure/webvtt-space-text-eol.vtt", 0 );
  ASSERT_EQ( 0, errorCount() ) << "This file should contain no errors.";
}

/*
 * Verifies that a file with the WebVTT signature, a space, non-line terminating
 * characters followed by two EOL characters will parse correctly.
 * From http://dev.w3.org/html5/webvtt/#webvtt-file-body (12/02/2012):
 *
 * A WebVTT file body consists of the following components, in the following order:
 * 1. An optional U+FEFF BYTE ORDER MARK (BOM) character.
 * 2. The string "WEBVTT".
 * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
 *    character followed by any number of characters that are not U+000A LINE FEED (LF)
 *    or U+000D CARRIAGE RETURN (CR) characters.
 * 4. Two or more WebVTT line terminators.
 * 5. Zero or more WebVTT cues and/or WebVTT comments separated from each other
 *    by two or more WebVTT line terminators.
 * 6. Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, WebVTTSpaceTextEOLs)
{
  loadVtt( "filestructure/webvtt-space-text-eols.vtt", 0 );
  ASSERT_EQ( 0, errorCount() ) << "This file should contain no errors.";
}

/*
 * Verifies that a file with the WebVTT signature, a space, non-line terminating
 * characters followed by two EOL characters and cue will parse correctly.
 * From http://dev.w3.org/html5/webvtt/#webvtt-file-body (12/02/2012):
 *
 * A WebVTT file body consists of the following components, in the following order:
 * 1. An optional U+FEFF BYTE ORDER MARK (BOM) character.
 * 2. The string "WEBVTT".
 * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
 *    character followed by any number of characters that are not U+000A LINE FEED (LF)
 *    or U+000D CARRIAGE RETURN (CR) characters.
 * 4. Two or more WebVTT line terminators.
 * 5. Zero or more WebVTT cues and/or WebVTT comments separated from each other
 *    by two or more WebVTT line terminators.
 * 6. Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, WebVTTSpaceTextEOLsOneCue)
{
  loadVtt("filestructure/webvtt-space-text-eols-one-cue.vtt", 1);
  ASSERT_EQ(0, errorCount()) << "This file should contain no errors.";
}

/*
 * Verifies that a file with the WebVTT signature, a space, non-line terminating
 * characters followed by single EOL character and cue will report error.
 * From http://dev.w3.org/html5/webvtt/#webvtt-file-body (12/02/2012):
 *
 * A WebVTT file body consists of the following components, in the following
 * order:
 * 1. An optional U+FEFF BYTE ORDER MARK (BOM) character.
 * 2. The string "WEBVTT".
 * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
 *    character followed by any number of characters that are not U+000A LINE FEED (LF)
 *    or U+000D CARRIAGE RETURN (CR) characters.
 * 4. Two or more WebVTT line terminators.
 * 5. Zero or more WebVTT cues and/or WebVTT comments separated from each other
 *    by two or more WebVTT line terminators.
 * 6. Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, WebVTTSpaceTextNoEOLsOneCue) {
  loadVtt("filestructure/webvtt-space-text-eol-cue.vtt", 1);
  ASSERT_EQ(1, errorCount());
  expectEquals(getError(0), WEBVTT_EXPECTED_EOL, 2, 1);
  expectEquals(getCue(0).startTime(), 0, 13, 0);
  expectEquals(getCue(0).endTime(), 0, 16, 0);
}

/*
 * Verifies that a file with text before the WebVTT signature will fail parsing and finish gracefully.
 * From http://dev.w3.org/html5/webvtt/#webvtt-file-body (12/02/2012):
 *
 * A WebVTT file body consists of the following components, in the following order:
 * 1. An optional U+FEFF BYTE ORDER MARK (BOM) character.
 * 2. The string "WEBVTT".
 * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
 *    character followed by any number of characters that are not U+000A LINE FEED (LF)
 *    or U+000D CARRIAGE RETURN (CR) characters.
 * 4. Two or more WebVTT line terminators.
 * 5. Zero or more WebVTT cues and/or WebVTT comments separated from each other
 *    by two or more WebVTT line terminators.
 * 6. Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, TextBeforeHeader)
{
  loadVtt( "filestructure/text-before-header.vtt", false, 0 );
  ASSERT_EQ( 1, errorCount() );
  expectEquals( getError( 0 ), WEBVTT_MALFORMED_TAG, 1, 1 );
}

/*
 * Verifies that a blank file will finish the parsing attempt gracefully.
 * From http://dev.w3.org/html5/webvtt/#webvtt-file-body (12/02/2012):
 *
 * A WebVTT file body consists of the following components, in the following order:
 * 1. An optional U+FEFF BYTE ORDER MARK (BOM) character.
 * 2. The string "WEBVTT".
 * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
 *    character followed by any number of characters that are not U+000A LINE FEED (LF)
 *    or U+000D CARRIAGE RETURN (CR) characters.
 * 4. Two or more WebVTT line terminators.
 * 5. Zero or more WebVTT cues and/or WebVTT comments separated from each other
 *    by two or more WebVTT line terminators.
 * 6. Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, BlankFile)
{
  loadVtt( "filestructure/blank-file.vtt", 0 );
  ASSERT_EQ( 0, errorCount() ) << "This file should contain no errors.";
}

/*
 * Verifies that a file with only a BOM character will finish the parsing attempt gracefully.
 * From http://dev.w3.org/html5/webvtt/#webvtt-file-body (12/02/2012):
 *
 * A WebVTT file body consists of the following components, in the following order:
 * 1. An optional U+FEFF BYTE ORDER MARK (BOM) character.
 * 2. The string "WEBVTT".
 * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
 *    character followed by any number of characters that are not U+000A LINE FEED (LF)
 *    or U+000D CARRIAGE RETURN (CR) characters.
 * 4. Two or more WebVTT line terminators.
 * 5. Zero or more WebVTT cues and/or WebVTT comments separated from each other
 *    by two or more WebVTT line terminators.
 * 6. Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, BlankFileWithBOM)
{
  loadVtt( "filestructure/blank-file-with-bom.vtt", 0 );
  ASSERT_EQ( 0, errorCount() ) << "This file should contain no errors.";
}

/*
 * Verifies that a file with a BOM character, a tab, and the WebVTT signature
 * will fail parsing and finish gracefully.
 * From http://dev.w3.org/html5/webvtt/#webvtt-file-body (12/02/2012):
 *
 * A WebVTT file body consists of the following components, in the following order:
 * 1. An optional U+FEFF BYTE ORDER MARK (BOM) character.
 * 2. The string "WEBVTT".
 * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
 *    character followed by any number of characters that are not U+000A LINE FEED (LF)
 *    or U+000D CARRIAGE RETURN (CR) characters.
 * 4. Two or more WebVTT line terminators.
 * 5. Zero or more WebVTT cues and/or WebVTT comments separated from each other
 *    by two or more WebVTT line terminators.
 * 6. Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, TabAfterBOMBeforeHeader)
{
  loadVtt( "filestructure/tab-after-bom-before-header.vtt", false, 0 );
  ASSERT_LE( 1, errorCount() );
  EXPECT_EQ( 1, errorCount() ) << "This file should contain 1 error.";
  expectEquals( getError( 0 ), WEBVTT_MALFORMED_TAG, 1, 1 );
}

/*
 * Verifies that a file with no line terminator between the WebVTT signature and a cue
 * will parse correctly.
 * From http://dev.w3.org/html5/webvtt/#parsing (12/10/2012):
 *
 * ...
 * 8. If line is more than six characters long but the first six characters do not
 *    exactly equal "WEBVTT", or the seventh character is neither a U+0020 SPACE
 *    character nor a U+0009 CHARACTER TABULATION (tab) character, then abort these steps.
 *    The file does not start with the correct WebVTT file signature and was
 *    therefore not successfully processed.
 * 9. If position is past the end of input, then abort these steps. The file was
 *    successfully processed, but it contains no useful data and so no text track
 *    cues where added to output.
 * 10. The character indicated by position is a U+000A LINE FEED (LF) character.
 *     Advance position to the next character in input.
 * 11. Header: Collect a sequence of characters that are not U+000A LINE FEED (LF)
 *     characters. Let line be those characters, if any.
 * 12. If position is past the end of input, then jump to the step labeled end.
 * 13. The character indicated by position is a U+000A LINE FEED (LF) character.
 *     Advance position to the next character in input.
 * 14. If line contains the three-character substring "-->" (U+002D HYPHEN-MINUS,
 *     U+002D HYPHEN-MINUS, U+003E GREATER-THAN SIGN), then set the already
 *     collected line flag and jump to the step labeled cue loop.
 * 15. If line is not the empty string, then jump back to the step labeled header.
 */
TEST_F(FileStructure, HeaderNoNewLine)
{
  loadVtt( "filestructure/header-no-new-line.vtt", 1 );
  ASSERT_LE( 1, errorCount() );
  EXPECT_EQ( 1, errorCount() );
  expectEquals( getError( 0 ), WEBVTT_EXPECTED_EOL, 2, 1 );
  expectEquals( getCue( 0 ).startTime(), 0, 11, 0 );
  expectEquals( getCue( 0 ).endTime(), 0, 13, 0 );
}

/**
 * Verifies that a file with a cue containing no payload will parse correctly.
 * From http://dev.w3.org/html5/webvtt/#webvtt-file-body (12/02/2012):
 *
 * A WebVTT file body consists of the following components, in the following order:
 * 1. An optional U+FEFF BYTE ORDER MARK (BOM) character.
 * 2. The string "WEBVTT".
 * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER
 *    TABULATION (tab) character followed by any number of characters that are not
 *    U+000A LINE FEED (LF) or U+000D CARRIAGE RETURN (CR) characters.
 * 4. Two or more WebVTT line terminators.
 * 5. Zero or more WebVTT cues and/or WebVTT comments separated from each other
 *    by two or more WebVTT line terminators.
 * 6. Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, NoPayloadText)
{
  loadVtt( "filestructure/no-payload-text.vtt", 1 );
  ASSERT_EQ( 0, errorCount() ) << "This file should contain no errors.";
}

/**
 * Verifies that a file with a malformed cue will parse correctly.
 * From http://dev.w3.org/html5/webvtt/#webvtt-file-body (12/02/2012):
 *
 * A WebVTT file body consists of the following components, in the following order:
 * 1. An optional U+FEFF BYTE ORDER MARK (BOM) character.
 * 2. The string "WEBVTT".
 * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER
 *    TABULATION (tab) character followed by any number of characters that are not
 *    U+000A LINE FEED (LF) or U+000D CARRIAGE RETURN (CR) characters.
 * 4. Two or more WebVTT line terminators.
 * 5. Zero or more WebVTT cues and/or WebVTT comments separated from each other
 *    by two or more WebVTT line terminators.
 * 6. Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, MissingCueTimes)
{
  loadVtt( "filestructure/missing-cue-times.vtt", 0 );
  ASSERT_LE( 1, errorCount() );
  EXPECT_EQ( 1, errorCount() ) << "This file should contain 1 error: WEBVTT_CUE_INCOMPLETE.";
  expectEquals( getError( 0 ), WEBVTT_CUE_INCOMPLETE, 3, 13 );
}

/**
 * Verifies that a file with a BOM character and garbage text in the
 * place of the WEBVTT signature will finish the parsing attempt gracefully.
 * From http://dev.w3.org/html5/webvtt/#parsing (12/10/2012):
 *
 * ...
 * 6. If line is less than six characters long, then abort these steps. The file does
 *    not start with the correct WebVTT file signature and was therefore not successfully processed.
 * 7. If line is exactly six characters long but does not exactly equal "WEBVTT", then
 *    abort these steps. The file does not start with the correct WebVTT file signature and was
 *    therefore not successfully processed.
 * 8. If line is more than six characters long but the first six characters do not exactly equal
 *    "WEBVTT", or the seventh character is neither a U+0020 SPACE character nor a U+0009 CHARACTER
 *    TABULATION (tab) character, then abort these steps. The file does not start with the correct
 *    WebVTT file signature and was therefore not successfully processed.
 */
TEST_F(FileStructure, BOMGarbageNoWebVTT)
{
  /* infinite loop within webvtt_parse_chunk:
   * token never appears to change from BADTOKEN
   */
  loadVtt( "filestructure/bom-garbage-no-webvtt.vtt", false, 0 );
  ASSERT_LE( 1, errorCount() );
  EXPECT_EQ( 1, errorCount() ) << "This file should contain 1 error.";
  expectEquals( getError( 0 ), WEBVTT_MALFORMED_TAG, 1, 1 );
}

/*
 * Verifies that a file with a WebVTT signature, 2 newlines, a cue, and multiple
 * newlines (3) will parse successfully.
 * From http://dev.w3.org/html5/webvtt/#webvtt-file-body (12/02/2012):
 *
 * A WebVTT file body consists of the following components, in the following order:
 * 1. An optional U+FEFF BYTE ORDER MARK (BOM) character.
 * 2. The string "WEBVTT".
 * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
 *    character followed by any number of characters that are not U+000A LINE FEED (LF)
 *    or U+000D CARRIAGE RETURN (CR) characters.
 * 4. Two or more WebVTT line terminators.
 * 5. Zero or more WebVTT cues and/or WebVTT comments separated from each other
 *    by two or more WebVTT line terminators.
 * 6. Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, ExtraNewlinesAfterCue)
{
  loadVtt( "filestructure/extra-newlines-after-cue.vtt", 1 );
  ASSERT_EQ( 0, errorCount() ) << "This file should contain no errors.";
}

/*
 * Verifies that a file with multiple cues without a separating line terminator
 * between them will finish the parsing attempt gracefully.
 * From http://dev.w3.org/html5/webvtt/#parsing (12/10/2012)
 */
TEST_F(FileStructure, MultiCueNoNewlineBetweenCues)
{
  /* infinite loop in webvtt_parse_chunk
   * self->state is stuck in T_STARTTIME
   */
  loadVtt( "filestructure/multi-cue-no-newline-between-cues.vtt", true, 2 );
  EXPECT_EQ( 0, errorCount() ) << "This file should contain no errors.";
}

/*
 * Verifies that a file with multiple cues without a payload will finish parsing.
 * From http://dev.w3.org/html5/webvtt/#webvtt-cue-text (12/10/2012):
 *
 * WebVTT cue text consists of zero or more WebVTT cue components, in any order,
 * each optionally separated from the next by a WebVTT line terminator.
 */
TEST_F(FileStructure, MultiCueNoPayload)
{
  loadVtt( "filestructure/multi-cue-no-payload.vtt", 2 );
  ASSERT_EQ( 0, errorCount() ) << "This file should contain no errors.";
}

/*
 * Verifies that a file with a newline before the WebVTT signature will
 * finish the parsing attempt gracefully.
 * From http://dev.w3.org/html5/webvtt/#parsing (12/10/2012):
 *
 * ...
 * 6. If line is less than six characters long, then abort these steps. The file does not start
 * with the correct WebVTT file signature and was therefore not successfully processed.
 */
TEST_F(FileStructure, NewlineBeforeWebVTT)
{
  loadVtt( "filestructure/newline-before-webvtt.vtt", false, 0 );
  ASSERT_LE( 1, errorCount() );
  EXPECT_EQ( 1, errorCount() ) << "This file should contain 1 error.";
  expectEquals( getError( 0 ), WEBVTT_MALFORMED_TAG, 1, 1 );
}

/*
 * Verifies that a file with payload text containing a newline in between the
 * text will finish the parsing attempt gracefully.
 *
 * Parser should discover that the cue has ended (2 sequential EOLs)
 * without cue-times on line 7, column 1, and report an incomplete
 * cue.
 *
 * From http://dev.w3.org/html5/webvtt/#parsing (12/10/2012):
 *
 * ...
 * 50. Bad cue: Discard cue.
 * 51. Bad cue loop: If position is past the end of input, then jump to the
 * step labeled end.
 */
TEST_F(FileStructure, NewlineBetweenPayloadText)
{
  loadVtt( "filestructure/newline-between-payload-text.vtt", 1 );
  ASSERT_LE( 1, errorCount() );
  EXPECT_EQ( 1, errorCount() ) << "This file should contain 1 error.";
  expectEquals( getError( 0 ), WEBVTT_CUE_INCOMPLETE, 6, 13 );
}

/*
 Test expecting parser to succeed if a bunch of new lines at the bottom of file exists.

 From http://dev.w3.org/html5/webvtt/#the-webvtt-file-format
 A WebVTT file must consist of a WebVTT file body encoded as UTF-8 and labeled
 with the MIME type text/vtt. [RFC3629]

 A WebVTT file body consists of the following components, in the following order:

 1.  An optional U+FEFF BYTE ORDER MARK (BOM) character.
 2.  The string "WEBVTT".
 3.  Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
     character followed by any number of characters that are not U+000A LINE FEED (LF)
     or U+000D CARRIAGE RETURN (CR) characters.
 4.  Two or more WebVTT line terminators.
 5.  Zero or more WebVTT cues and/or WebVTT comments separated from each other
     by two or more WebVTT line terminators.
 6.  Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, NewLinesAtTheEnd)
{
  loadVtt( "filestructure/new_lines_at_end.vtt");
  ASSERT_EQ(0, errorCount()) << "This file should contain no errors.";
}


/*
 This test checks for a bom charecter followed by garbage data to make sure that
 a WEBVTT header follows the bom character and nothing else (garbage data).

 From http://dev.w3.org/html5/webvtt/#the-webvtt-file-format
 A WebVTT file must consist of a WebVTT file body encoded as UTF-8 and labeled
 with the MIME type text/vtt. [RFC3629]

 A WebVTT file body consists of the following components, in the following order:

 1.  An optional U+FEFF BYTE ORDER MARK (BOM) character.
 2.  The string "WEBVTT".
 3.  Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
     character followed by any number of characters that are not U+000A LINE FEED (LF)
     or U+000D CARRIAGE RETURN (CR) characters.
 4.  Two or more WebVTT line terminators.
 5.  Zero or more WebVTT cues and/or WebVTT comments separated from each other
     by two or more WebVTT line terminators.
 6.  Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, BOMGarbageData)
{
  loadVtt( "filestructure/bom_garbage_data.vtt", false, 0 );
  ASSERT_LE( 1, errorCount() );
  EXPECT_EQ( 1, errorCount() ) << "This file should fail the test because it has garbage data";
  expectEquals( getError( 0 ), WEBVTT_MALFORMED_TAG, 1, 1 );
}

/*
 This test checks for a tab after the bom charecter to make sure that trailing
 spaces after bom do  not make it pass.

 From http://dev.w3.org/html5/webvtt/#the-webvtt-file-format
 A WebVTT file must consist of a WebVTT file body encoded as UTF-8 and labeled
 with the MIME type text/vtt. [RFC3629]

 A WebVTT file body consists of the following components, in the following order:

 1.  An optional U+FEFF BYTE ORDER MARK (BOM) character.
 2.  The string "WEBVTT".
 3.  Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER TABULATION (tab)
     character followed by any number of characters that are not U+000A LINE FEED (LF)
     or U+000D CARRIAGE RETURN (CR) characters.
 4.  Two or more WebVTT line terminators.
 5.  Zero or more WebVTT cues and/or WebVTT comments separated from each other by
     two or more WebVTT line terminators.
 6.  Zero or more WebVTT line terminators.
 */
TEST_F(FileStructure, BOMTabWebvtt)
{
  loadVtt( "filestructure/bom_tab_webvtt.vtt", false, 0 );
  ASSERT_LE( 1, errorCount() );
  EXPECT_EQ( 1, errorCount() ) << "This file should fail the test because it" <<
    " contains a tab before WEBVTT";
  expectEquals( getError( 0 ), WEBVTT_MALFORMED_TAG, 1, 1 );
}

