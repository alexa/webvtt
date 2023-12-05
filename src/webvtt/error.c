/**
 * Copyright (c) 2013 Mozilla Foundation and Contributors
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <webvtt/error.h>

static const char *errstr[] = {
  /* WEBVTT_ALLOCATION_FAILED */ "error allocating object",
  /* WEBVTT_MALFORMED_TAG */ "malformed 'WEBVTT' tag",
  /* WEBVTT_EXPECTED_EOL */ "expected newline",
  /* WEBVTT_EXPECTED_WHITESPACE */ "expected whitespace",
  /* WEBVTT_UNEXPECTED_WHITESPACE */ "unexpected whitespace",
  /* WEBVTT_LONG_COMMENT */ "very long tag-comment",
  /* WEBVTT_ID_TRUNCATED */ "webvtt-cue-id truncated",
  /* WEBVTT_MALFORMED_TIMESTAMP */ "malformed webvtt-timestamp",
  /* WEBVTT_EXPECTED_TIMESTAMP */ "expected webvtt-timestamp",
  /* WEBVTT_MISSING_CUETIME_SEPARATOR */ "missing webvtt-cuetime-separator `-->'",
  /* WEBVTT_EXPECTED_CUETIME_SEPARATOR */ "expected webvtt-cuetime-separator `-->'",
  /* WEBVTT_MISSING_CUESETTING_DELIMITER */ "missing whitespace before webvtt-cuesetting",
  /* WEBVTT_INVALID_CUESETTING_DELIMITER */ "invalid webvtt-cuesetting key:value delimiter. expected `:'",
  /* WEBVTT_INVALID_ENDTIME */ "webvtt-cue end-time must have value greater than start-time",
  /* WEBVTT_INVALID_CUESETTING */ "unrecognized webvtt-cue-setting",
  /* WEBVTT_UNFINISHED_CUETIMES */ "unfinished webvtt cuetimes. expected 'start-timestamp --> end-timestamp'",
  /* WEBVTT_MISSING_CUESETTING_KEYWORD */ "missing setting keyword for value",
  /* WEBVTT_VERTICAL_ALREADY_SET */ "'vertical' cue-setting already used",
  /* WEBVTT_VERTICAL_BAD_VALUE */ "'vertical' setting must have a value of either 'lr' or 'rl'",
  /* WEBVTT_LINE_ALREADY_SET */ "'line' cue-setting already used",
  /* WEBVTT_LINE_BAD_VALUE */ "'line' cue-setting must have a value that is an integer (signed) line number, or percentage (%) from top of video display",
  /* WEBVTT_POSITION_ALREADY_SET */ "'position' cue-setting already used",
  /* WEBVTT_POSITION_BAD_VALUE */ "'position' cue-setting must be a percentage (%) value representing the position in the direction orthogonal to the 'line' setting",
  /* WEBVTT_SIZE_ALREADY_SET */ "'size' cue-setting already used",
  /* WEBVTT_SIZE_BAD_VALUE */ "'size' cue-setting must have percentage (%) value",
  /* WEBVTT_ALIGN_ALREADY_SET */ "'align' cue-setting already used",
  /* WEBVTT_ALIGN_BAD_VALUE */ "'align' cue-setting must have a value of either 'start', 'middle', or 'end'",
  /* WEBVTT_CUE_CONTAINS_SEPARATOR */ "cue-text line contains unescaped timestamp separator '-->'",
  /* WEBVTT_CUE_INCOMPLETE */ "cue contains cue-id, but is missing cuetimes or cue text",
};

/**
 * TODO:
 * Add i18n localized error strings with support for glibc and msvcrt locale
 * identifiers
 * (This might be too much work!)
 */
WEBVTT_EXPORT const char *
webvtt_strerror( webvtt_error err )
{
  if( err >= (sizeof(errstr) / sizeof(*errstr)) || (err < 0)) {
    return "";
  }
  return errstr[ err ];
}

/* Some status codes have error codes as well */
webvtt_bool
webvtt_error_for_status( webvtt_status status, webvtt_error *out )
{
  if( !out ) {
    return 0;
  }
  switch( status ) {
    case WEBVTT_OUT_OF_MEMORY: *out = WEBVTT_ALLOCATION_FAILED; break;
    case WEBVTT_BAD_ALIGN: *out = WEBVTT_ALIGN_BAD_VALUE; break;
    case WEBVTT_BAD_LINE: *out = WEBVTT_LINE_BAD_VALUE; break;
    case WEBVTT_BAD_SIZE: *out = WEBVTT_SIZE_BAD_VALUE; break;
    case WEBVTT_BAD_POSITION: *out = WEBVTT_POSITION_BAD_VALUE; break;
    case WEBVTT_BAD_VERTICAL: *out = WEBVTT_VERTICAL_BAD_VALUE; break;

    case WEBVTT_ALREADY_ALIGN: *out = WEBVTT_ALIGN_ALREADY_SET; break;
    case WEBVTT_ALREADY_LINE: *out = WEBVTT_LINE_ALREADY_SET; break;
    case WEBVTT_ALREADY_SIZE: *out = WEBVTT_SIZE_ALREADY_SET; break;
    case WEBVTT_ALREADY_POSITION: *out = WEBVTT_POSITION_ALREADY_SET; break;
    case WEBVTT_ALREADY_VERTICAL: *out = WEBVTT_VERTICAL_ALREADY_SET; break;

    case WEBVTT_BAD_CUESETTING: *out = WEBVTT_INVALID_CUESETTING; break;

    default: return 0;
  }

  return 1;
}
