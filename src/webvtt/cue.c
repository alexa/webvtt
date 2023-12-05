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

#include <string.h>
#include "parser_internal.h"
#include "cue_internal.h"

WEBVTT_EXPORT webvtt_status
webvtt_create_cue( webvtt_cue **pcue )
{
  webvtt_cue *cue;
  if( !pcue ) {
    return WEBVTT_INVALID_PARAM;
  }
  cue = (webvtt_cue *)webvtt_alloc0( sizeof(*cue) );
  if( !cue ) {
    return WEBVTT_OUT_OF_MEMORY;
  }
  /**
   * From http://dev.w3.org/html5/webvtt/#parsing (10/25/2012)
   *
   * Let cue's text track cue snap-to-lines flag be true.
   *
   * Let cue's text track cue line position be auto.
   *
   * Let cue's text track cue text position be 50.
   *
   * Let cue's text track cue size be 100.
   *
   * Let cue's text track cue alignment be middle alignment.
   */
  webvtt_ref( &cue->refs );
  webvtt_init_string( &cue->id );
  webvtt_init_string( &cue->body );
  cue->from = 0xFFFFFFFFFFFFFFFF;
  cue->until = 0xFFFFFFFFFFFFFFFF;
  cue->snap_to_lines = 1;
  cue->settings.position = 50;
  cue->settings.size = 100;
  cue->settings.align = WEBVTT_ALIGN_MIDDLE;
  cue->settings.line = WEBVTT_AUTO;
  cue->settings.vertical = WEBVTT_HORIZONTAL;

  *pcue = cue;
  return WEBVTT_SUCCESS;
}

WEBVTT_EXPORT void
webvtt_ref_cue( webvtt_cue *cue )
{
  if( cue ) {
    webvtt_ref( &cue->refs );
  }
}

WEBVTT_EXPORT void
webvtt_release_cue( webvtt_cue **pcue )
{
  if( pcue && *pcue ) {
    webvtt_cue *cue = *pcue;
    *pcue = 0;
    if( webvtt_deref( &cue->refs ) == 0 ) {
      webvtt_release_string( &cue->id );
      webvtt_release_string( &cue->body );
      webvtt_release_node( &cue->node_head );
      webvtt_free( cue );
    }
  }
}

WEBVTT_EXPORT int
webvtt_validate_cue( webvtt_cue *cue )
{
  if( cue ) {
    /**
     * validate cue-times (Can't do checks against previously parsed cuetimes.
     * That's the applications responsibility
     */
    if( BAD_TIMESTAMP(cue->from) || BAD_TIMESTAMP(cue->until) ) {
      goto error;
    }

    if( cue->until <= cue->from ) {
      goto error;
    }

    /**
     * Don't do any payload validation, because this would involve parsing the
     * payload, which is optional.
     */
    return 1;
  }

error:
  return 0;
}

WEBVTT_INTERN webvtt_bool
cue_is_incomplete( const webvtt_cue *cue ) {
  return !cue || ( cue->flags & CUE_HEADER_MASK ) == CUE_HAVE_ID;
}

WEBVTT_EXPORT webvtt_status
webvtt_cue_set_align( webvtt_cue *cue, const char *value )
{
  unsigned i;
  static const char *values[] = {
    "start",
    "middle",
    "end",
    "left",
    "right",
  };

  if( !cue || !value ) {
    return WEBVTT_INVALID_PARAM;
  }

  for( i=0 ; i < sizeof(values)/sizeof(*values) ; ++i ) {
    if( !strcmp( value, values[i] ) ) {
      cue->settings.align = (webvtt_align_type)i;
      if( cue->flags & CUE_HAVE_ALIGN ) {
        return WEBVTT_ALREADY_ALIGN;
      }
      cue->flags |= CUE_HAVE_ALIGN;
      return WEBVTT_SUCCESS;
    }
  }
  return WEBVTT_BAD_ALIGN;
}

WEBVTT_EXPORT webvtt_status
webvtt_cue_set_line( webvtt_cue *cue, const char *value )
{
  webvtt_int64 number;
  int digits = 0;
  const char *c;

  if( !cue || !value ) {
    return WEBVTT_INVALID_PARAM;
  }

  /**
   * 1. If value contains any characters other than U+002D HYPHEN-MINUS
   * characters (-), U+0025 PERCENT SIGN characters (%), and ASCII digits,
   * then jump to the step labeled next setting
   */
  for( c = value; *c; ++c ) {
    if( webvtt_isdigit( *c ) ) {
      ++digits;
    } else if( *c == '-' || *c == '%' ) {
    } else {
      return WEBVTT_BAD_LINE;
    }
  }

  /**
   * 2. If value does not contain at least one ASCII digit, then jump to the
   * step labeled next setting.
   */
  if( digits == 0 ) {
    return WEBVTT_BAD_LINE;
  }

  if( strchr( value + 1, '-' ) ) {
    /**
     * 3. If any character in value other than the first character is a U+002D
     * HYPHEN-MINUS character (-), then jump to the step labeled next setting.
     */
    return WEBVTT_BAD_LINE;
  }

  if( (( c = strchr( value, '%' ) ) != NULL) && ( ( c[1] != '\0' ) || *value == '-' ) ) {
    /**
     * 4. If any character in value other than the last character is a U+0025
     * PERCENT SIGN character (%), then jump to the step labeled next setting.
     *
     * 5. If the first character in value is a U+002D HYPHEN-MINUS character (-)
     * and the last character in value is a U+0025 PERCENT SIGN character (%),
     * then jump to the step labeled next setting.
     */
    return WEBVTT_BAD_LINE;
  }

  /**
   * Ignoring the trailing percent sign, if any, interpret value as a
   * (potentially signed) integer and let number be that number.
   */
  c = value;
  number = webvtt_parse_int( &c, &digits );

  if( *c == '%' ) {
    /**
     * 7. If the last character in value is a U+0025 PERCENT SIGN character (%),
     * but number is not in the range 0 < number < 100, then jump to the step
     * labeled next setting.
     */
    if( number > 100 ) {
      return WEBVTT_BAD_LINE;
    }

    /**
     * 9. If the last character in value is a U+0025 PERCENT SIGN character (%),
     * then let cue's text track cue snap-to-lines flag be false.
     */
    cue->snap_to_lines = 0;
  } else {
    /* 9. ... Otherwise, let it be true. */
    cue->snap_to_lines = 1;
  }

  /* 8. Let cue's text track cue line position be number */
  cue->settings.line = (int)number;
  if( cue->flags & CUE_HAVE_LINE ) {
    return WEBVTT_ALREADY_LINE;
  }
  cue->flags |= CUE_HAVE_LINE;
  return WEBVTT_SUCCESS;
}

WEBVTT_EXPORT webvtt_status
webvtt_cue_set_position( webvtt_cue *cue, const char *value )
{
  int digits = 0;
  webvtt_int64 number;
  const char *c;

  if( !cue || !value ) {
    return WEBVTT_INVALID_PARAM;
  }

  /**
   * 1. If value contains any characters other than U+0025 PERCENT SIGN
   * characters (%) and ASCII digits, then jump to the step labeled next
   * setting.
   */
  for( c = value; *c; ++c ) {
    if( webvtt_isdigit( *c ) ) {
      ++digits;
    } else if( *c == '%' ) {
    } else {
      return WEBVTT_BAD_POSITION;
    }
  }

  /**
   * 2. If value does not contain at least one ASCII digit, then jump to the
   * step labeled next setting.
   */
  if( !digits ) {
    return WEBVTT_BAD_POSITION;
  }

  /**
   * 3. If any character in value other than the last character is a U+0025
   * PERCENT SIGN character (%), then jump to the step labeled next setting.
   */
  if( (( c = strchr( value, '%' ) ) != NULL) && ( c[1] != '\0' ) ) {
    return WEBVTT_BAD_POSITION;
  }

  /**
   * 4. If the last character in value is not a U+0025 PERCENT SIGN character
   * (%), then jump to the step labeled next setting.
   */
  if( !c ) {
    return WEBVTT_BAD_POSITION;
  }

  /**
   * 5. Ignoring the trailing percent sign, interpret value as an integer, and
   * let number be that number.
   */
  c = value;
  number = webvtt_parse_int( &c, &digits );

  /**
   * 6. If number is not in the range 0 <= number <= 100, then jump to the step
   * labeled next setting.
   */
  if( number > 100 ) {
    return WEBVTT_BAD_POSITION;
  }

  /* 7. Let cue's text track cue text position be number */
  cue->settings.position = (int)number;
  if( cue->flags & CUE_HAVE_POSITION ) {
    return WEBVTT_ALREADY_POSITION;
  }
  cue->flags |= CUE_HAVE_POSITION;
  return WEBVTT_SUCCESS;
}

WEBVTT_EXPORT webvtt_status
webvtt_cue_set_size( webvtt_cue *cue, const char *value )
{
  int digits = 0;
  webvtt_int64 number;
  const char *c;
  if( !cue || !value ) {
    return WEBVTT_INVALID_PARAM;
  }

  /**
   * 1. If value contains any characters other than U+0025 PERCENT SIGN
   * characters (%) and ASCII digits, then jump to the step labeled next
   * setting.
   */
  for( c = value; *c; ++c ) {
    if( webvtt_isdigit( *c ) ) {
      ++digits;
    } else if( *c == '%' ) {
    } else {
      return WEBVTT_BAD_SIZE;
    }
  }

  /**
   * 2. If value does not contain at least one ASCII digit, then jump to the
   * step labeled next setting.
   */
  if( !digits ) {
    return WEBVTT_BAD_SIZE;
  }

  /**
   * 3. If any character in value other than the last character is a U+0025
   * PERCENT SIGN character (%), then jump to the step labeled next setting.
   */
  if( (( c = strchr( value, '%' ) ) != NULL) && ( c[1] != '\0' ) ) {
    return WEBVTT_BAD_SIZE;
  }

  /**
   * 4. If the last character in value is not a U+0025 PERCENT SIGN character
   * (%), then jump to the step labeled next setting.
   */
  if( !c ) {
    return WEBVTT_BAD_SIZE;
  }

  /**
   * 5. Ignoring the trailing percent sign, interpret value as an integer, and
   * let number be that number.
   */
  c = value;
  number = webvtt_parse_int( &c, &digits );

  /**
   * 6. If number is not in the range 0 <= number <= 100, then jump to the step
   * labeled next setting.
   */
  if( number > 100 ) {
    return WEBVTT_BAD_SIZE;
  }

  /* 7. Let cue's text track cue size be number */
  cue->settings.size = (int)number;
  if( cue->flags & CUE_HAVE_SIZE ) {
    return WEBVTT_ALREADY_SIZE;
  }
  cue->flags |= CUE_HAVE_SIZE;
  return WEBVTT_SUCCESS;
}

WEBVTT_EXPORT webvtt_status
webvtt_cue_set_vertical( webvtt_cue *cue, const char *value )
{
  unsigned i;
  static const char *values[] = {
    "lr",
    "rl",
  };

  if( !cue || !value ) {
    return WEBVTT_INVALID_PARAM;
  }

  for( i=0 ; i < sizeof(values)/sizeof(*values) ; ++i ) {
    if( !strcmp( value, values[i] ) ) {
      cue->settings.vertical = (webvtt_align_type)i+1;
      if( cue->flags & CUE_HAVE_VERTICAL ) {
        return WEBVTT_ALREADY_VERTICAL;
      }
      cue->flags |= CUE_HAVE_VERTICAL;
      return WEBVTT_SUCCESS;
    }
  }
  return WEBVTT_BAD_VERTICAL;
}

static struct
{
  const char *keyword;
  webvtt_status (*setValue)( webvtt_cue *cue, const char *value );
} cuesettings[] = {
  { "align", &webvtt_cue_set_align },
  { "line", &webvtt_cue_set_line },
  { "position", &webvtt_cue_set_position },
  { "size", &webvtt_cue_set_size },
  { "vertical", &webvtt_cue_set_vertical },
};

/**
 * Set a cuesetting from key-value pairs (as C strings)
 */
WEBVTT_EXPORT webvtt_status
webvtt_cue_set_setting( webvtt_cue *cue,
                        const char *key, const char *value )
{
  unsigned i;
  if( !key || !value ) {
    return WEBVTT_INVALID_PARAM;
  }
  for( i=0 ; i<sizeof(cuesettings)/sizeof(*cuesettings) ; ++i ) {
    if( !strcmp( key, cuesettings[i].keyword ) ) {
      return cuesettings[i].setValue( cue, value );
    }
  }
  return WEBVTT_BAD_CUESETTING;
}

WEBVTT_EXPORT webvtt_status
webvtt_cue_set_settings( webvtt_cue *cue, const webvtt_string *settings )
{
  /* Share code with the version which validates and emits errors,
     but do not allow it to emit errors. */
  return webvtt_cue_validate_set_settings( 0, cue, settings );
}

/* Separate 'word' into key and value (delimited by ':'), and call
   webvtt_cue_set_setting() with the calculated key/value pair.
   Intern'd due to scariness of the name, and non-usefulness for users. */
WEBVTT_INTERN webvtt_status
webvtt_cue_set_setting_from_string( webvtt_cue *cue, const char *word )
{
  char keyword[32] = "";
  char *value;
  int idx;
  if( !cue || !word ) {
    return WEBVTT_INVALID_PARAM;
  }

  value = strchr( word, ':' );
  if( !value || value == word || *(++value) == '\0' ) {
    return WEBVTT_BAD_CUESETTING;
  }

  idx = (int)(( value - word ) - 1);
  if( idx > 31 ) {
    return WEBVTT_BAD_CUESETTING;
  }

  memcpy( keyword, word, idx );
  keyword[idx] = '\0';

  return webvtt_cue_set_setting( cue, (const char *)keyword, value );
}

WEBVTT_EXPORT webvtt_status
webvtt_cue_validate_set_settings( webvtt_parser self, webvtt_cue *cue,
                                  const webvtt_string *settings )
{
  int line = 1;
  int column = 0;
  int length;
  const char *eol;
  int position = 0;
  webvtt_status s;
  if( !cue || !settings ) {
    return WEBVTT_INVALID_PARAM;
  }
  length = (int)webvtt_string_length( settings );
  if( (( eol = strchr( webvtt_string_text( settings ), '\r' ) ) != NULL)
      || (( eol = strchr( webvtt_string_text( settings ), '\n' ) ) != NULL) ) {
    length = (int)( eol - webvtt_string_text( settings ) );
  }

  if( self ) {
    line = self->line;
    column = self->column;
  }

  /**
   * http://www.w3.org/html/wg/drafts/html/master/single-page.html#split-a-string-on-spaces
   * 4. Skip whitespace
   */
  column += webvtt_string_skip_whitespace( settings, &position );

  while( position < length ) {
    webvtt_string word;
    const char *keyword;
    const char *end;
    int nwhite, ncol;
    /* Collect word (sequence of non-space characters terminated by space) */
    if( WEBVTT_FAILED( webvtt_string_collect_word( settings, &word,
                       &position ) ) ) {
      return WEBVTT_OUT_OF_MEMORY;
    }
    /* skip trailing whitespace */
    nwhite = webvtt_string_skip_whitespace( settings, &position );
    /* Get the word text */
    keyword = webvtt_string_text( &word );
    /* Get pointer to end of the word. (for chcount()) */
    end = keyword + webvtt_string_length( &word );
    /* Get the column count that needs to be skipped. */
    ncol = webvtt_utf8_chcount( keyword, end );
    if( WEBVTT_FAILED( s = webvtt_cue_set_setting_from_string( cue,
                       keyword ) ) ) {
      if( self ) {
        /* Figure out which error to emit */
        webvtt_error error;
        if( webvtt_error_for_status( s, &error ) ) {
          /* There is no non-recoverable cue-setting error.
             Therefore we do not want to abort the loop, regardless
             of the return value from the error handler. */
          WARNING_AT( error, line, column );
        }
      }
    }
    /* Move column pointer beyond word and trailing whitespace */
    column += ncol + nwhite;
    webvtt_release_string( &word );
  }

  if( self ) {
    self->column = column;
  }
  return WEBVTT_SUCCESS;
}

