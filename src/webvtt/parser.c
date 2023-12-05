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

#include "parser_internal.h"
#include "cuetext_internal.h"
#include "cue_internal.h"
#include <string.h>

#define _ERROR(X) do { if( skip_error == 0 ) { ERROR(X); } } while(0)

static const char separator[] = {
  '-', '-', '>'
};
/* UTF8 encoding of U+FFFD REPLACEMENT CHAR */
static const char replacement[] = { 0xEF, 0xBF, 0xBD };

#define MSECS_PER_HOUR (3600000)
#define MSECS_PER_MINUTE (60000)
#define MSECS_PER_SECOND (1000)
#define BUFFER (self->buffer + self->position)
#define MALFORMED_TIME ((webvtt_timestamp_t)-1.0)

static webvtt_status find_bytes( const char *buffer, webvtt_uint len,
                                 const char *sbytes, webvtt_uint slen );

WEBVTT_EXPORT webvtt_status
webvtt_create_parser( webvtt_cue_fn on_read,
                      webvtt_error_fn on_error, void *
                      userdata,
                      webvtt_parser *ppout )
{
  webvtt_parser p;
  if( !on_read || !on_error || !ppout ) {
    return WEBVTT_INVALID_PARAM;
  }

  if( ( p = ( webvtt_parser )webvtt_alloc0( sizeof * p ) ) == NULL ) {
    return WEBVTT_OUT_OF_MEMORY;
  }

  memset( p->astack, 0, sizeof( p->astack ) );
  p->stack = p->astack;
  p->top = p->stack;
  p->top->state = T_INITIAL;
  p->stack_alloc = sizeof( p->astack ) / sizeof( p->astack[0] );

  p->read = on_read;
  p->error = on_error;
  p->column = p->line = 1;
  p->userdata = userdata;
  p->finished = 0;
  *ppout = p;

  return WEBVTT_SUCCESS;
}

/**
 * Helper to validate a cue and, if valid, notify the application that a cue has
 * been read.
 * If it fails to validate, silently delete the cue.
 *
 * ( This might not be the best way to go about this, and additionally,
 * webvtt_validate_cue has no means to report errors with the cue, and we do
 * nothing with its return value )
 */
static void
finish_cue( webvtt_parser self, webvtt_cue **pcue )
{
  if( pcue ) {
    webvtt_cue *cue = *pcue;
    if( cue ) {
      if( webvtt_validate_cue( cue ) ) {
        self->read( self->userdata, cue );
      } else {
        webvtt_release_cue( &cue );
      }
      *pcue = 0;
    }
  }
}

/**
 * This routine tries to clean up the stack
 * for us, to prevent leaks.
 *
 * It should also help find errors in stack management.
 */
WEBVTT_INTERN void
cleanup_stack( webvtt_parser self )
{
  webvtt_state *st = self->top;
  while( st >= self->stack ) {
    switch( st->type ) {
      case V_CUE:
        webvtt_release_cue( &st->v.cue );
        break;
      case V_TEXT:
        webvtt_release_string( &st->v.text );
        break;
        /**
         * TODO: Clean up cuetext nodes as well.
         * Eventually the cuetext parser will probably be making use
         * of this stack, and will need to manage it well also.
         */
      default:
        break;
    }
    st->type = V_NONE;
    st->line = st->column = st->token = 0;
    st->v.cue = NULL;
    if( st > self->stack ) {
      --self->top;
    }
    --st;
  }
  if( self->stack != self->astack ) {
    /**
     * If the stack is dynamically allocated (probably not),
     * then point it to the statically allocated one (and zeromem it),
     * then finally delete the old dynamically allocated stack
     */
    webvtt_state *pst = self->stack;
    memset( self->astack, 0, sizeof( self->astack ) );
    self->stack = self->astack;
    self->stack_alloc = sizeof( self->astack ) / sizeof( *( self->astack ) );
    webvtt_free( pst );
  }
}

/**
 *
 */
WEBVTT_EXPORT webvtt_status
webvtt_finish_parsing( webvtt_parser self )
{
  webvtt_status status = WEBVTT_SUCCESS;
  const char buffer[] = "\0";
  const webvtt_uint len = 0;
  webvtt_uint pos = 0;

  if( !self->finished ) {
    self->finished = 1;

retry:
    switch( self->mode ) {
      /**
       * We've left off parsing cue settings and are not in the empty state,
       * return WEBVTT_CUE_INCOMPLETE.
       */
      case M_WEBVTT:
        if( self->top->state == T_CUEREAD ) {
          SAFE_ASSERT( self->top != self->stack );
          --self->top;
          self->popped = 1;
        }

        if( self->top->state == T_CUE ) {
          webvtt_string text;
          webvtt_cue *cue;
          if( self->top->type == V_NONE ) {
            webvtt_create_cue( &self->top->v.cue );
            self->top->type = V_CUE;
          }
          cue = self->top->v.cue;
          SAFE_ASSERT( self->popped && (self->top+1)->state == T_CUEREAD );
          SAFE_ASSERT( cue != 0 );
          text.d = (self->top+1)->v.text.d;
          (self->top+1)->v.text.d = 0;
          (self->top+1)->type = V_NONE;
          (self->top+1)->state = 0;
          self->column = 1;
          status = webvtt_proc_cueline( self, cue, &text );
          if( cue_is_incomplete( cue ) ) {
            ERROR( WEBVTT_CUE_INCOMPLETE );
          }
          ++self->line;
          self->column = 1;
          if( self->mode == M_CUETEXT ) {
            goto retry;
          }
        }
        break;
      /**
       * We've left off on trying to read in a cue text.
       * Parse the partial cue text read and pass the cue back to the
       * application if possible.
       */
      case M_CUETEXT:
        status = webvtt_proc_cuetext( self, buffer, &pos, len, self->finished );
        break;
      case M_SKIP_CUE:
        /* Nothing to do here. */
        break;
    }
    cleanup_stack( self );
  }

  return status;
}

WEBVTT_EXPORT void
webvtt_delete_parser( webvtt_parser self )
{
  if( self ) {
    cleanup_stack( self );

    webvtt_release_string( &self->line_buffer );
    webvtt_free( self );
  }
}

#define BEGIN_STATE(State) case State: {
#define END_STATE } break;
#define IF_TOKEN(Token,Actions) case Token: { Actions } break;
#define BEGIN_DFA switch(top->state) {
#define END_DFA }
#define BEGIN_TOKEN switch(token) {
#define END_TOKEN }
#define IF_TRANSITION(Token,State) if( token == Token ) { self->state = State;
#define ELIF_TRANSITION(Token,State) } else IF_TRANSITION(Token,State)
#define ENDIF }
#define ELSE } else {

static int
find_newline( const char *buffer, webvtt_uint *pos, webvtt_uint len )
{
  while( *pos < len ) {
    if( buffer[ *pos ] == '\r' || buffer[ *pos ] == '\n' ) {
      return 1;
    } else {
      ( *pos )++;
    }
  }
  return -1;
}

/**
 * basic strnstr-ish routine
 */
static webvtt_status
find_bytes( const char *buffer, webvtt_uint len,
    const char *sbytes, webvtt_uint slen )
{
  webvtt_uint slen2;
  // check params for integrity
  if( !buffer || len < 1 || !sbytes || slen < 1 ) {
    return WEBVTT_INVALID_PARAM;
  }

  slen2 = slen - 1;
  while( len-- >= slen && *buffer ){
    if( *buffer == *sbytes && memcmp( buffer + 1, sbytes + 1, slen2 ) == 0 ) {
      return WEBVTT_SUCCESS;
    }
    buffer++;
  }

  return WEBVTT_NO_MATCH_FOUND;
}

/**
 * Helpers to figure out what state we're on
 */
#define SP (self->top)
#define AT_BOTTOM (self->top == self->stack)
#define ON_HEAP (self->stack_alloc == sizeof(p->astack) / sizeof(p->astack[0]))
#define STACK_SIZE ((webvtt_uint)(self->top - self->stack))
#define FRAME(i) (self->top - (i))
#define FRAMEUP(i) (self->top + (i))
#define RECHECK goto _recheck;
#define BACK (SP->back)
/**
 * More state stack helpers
 */
WEBVTT_INTERN webvtt_status
do_push( webvtt_parser self, webvtt_uint token, webvtt_uint back,
         webvtt_uint state, void *data, webvtt_state_value_type type,
         webvtt_uint line, webvtt_uint column )
{
  if( STACK_SIZE + 1 >= self->stack_alloc ) {
    webvtt_state *stack =
        ( webvtt_state * )webvtt_alloc0( (webvtt_uint)sizeof( webvtt_state ) *
                                         ( self->stack_alloc << 1 ) ), *tmp;
    if( !stack ) {
      ERROR( WEBVTT_ALLOCATION_FAILED );
      return WEBVTT_OUT_OF_MEMORY;
    }
    memcpy( stack, self->stack, sizeof( webvtt_state ) * self->stack_alloc );
    tmp = self->stack;
    self->stack = stack;
    self->top = stack + ( self->top - tmp );
    if( tmp != self->astack ) {
      webvtt_free( tmp );
    }
  }
  ++self->top;
  self->top->state = state;
  self->top->flags = 0;
  self->top->type = type;
  self->top->token = ( webvtt_token )token;
  self->top->line = line;
  self->top->back = back;
  self->top->column = column;
  self->top->v.cue = ( webvtt_cue * )data;
  return WEBVTT_SUCCESS;
}
static int
do_pop( webvtt_parser self )
{
  int count = self->top->back;
  self->top -= count;
  self->top->back = 0;
  self->popped = 1;
  return count;
}

#define PUSH0(S,V,T) \
do { \
    self->popped = 0; \
    if( do_push(self,token,BACK+1,(S),(void*)(V),T,last_line, last_column) \
      == WEBVTT_OUT_OF_MEMORY ) \
      return WEBVTT_OUT_OF_MEMORY; \
  } while(0)

#define PUSH(S,B,V,T) \
do { \
  self->popped = 0; \
  if( do_push(self,token,(B),(S),(void*)(V),T,last_line, last_column) \
    == WEBVTT_OUT_OF_MEMORY ) \
    return WEBVTT_OUT_OF_MEMORY; \
  } while(0)

#define POP() \
do \
{ \
  --(self->top); \
  self->popped = 1; \
} while(0)
#define POPBACK() do_pop(self)

/**
 * Read a timestamp into 'result' field, following the rules of the cue-times
 * section of the draft:
 * http://dev.w3.org/html5/webvtt/#collect-webvtt-cue-timings-and-settings
 *
 * - Ignore whitespace
 * - Return immediately after having parsed a timestamp
 * - Return error if timestamp invalid.
 */
WEBVTT_INTERN webvtt_status
webvtt_collect_timestamp( webvtt_parser self, webvtt_timestamp *result,
                          const webvtt_string *input, int *position )
{
  int column = self->column;
  int line = self->line;
  int len;
  int rv = webvtt_parse_timestamp( webvtt_string_text(input) + *position, &len, result );
  if( !rv ) {
    if( BAD_TIMESTAMP(*result) ) {
      ERROR_AT( WEBVTT_EXPECTED_TIMESTAMP, line, column );
      return WEBVTT_BAD_CUE;  
    } else {
      /* Time is malformed, but still usable. Let the UA decide. */
      ERROR_AT_OR( WEBVTT_MALFORMED_TIMESTAMP, line, column, WEBVTT_BAD_CUE );
    }
  }

  /* Move column ahead */
  self->column += len;
  *position += len;

  return WEBVTT_SUCCESS;
}

/**
 * http://dev.w3.org/html5/webvtt/#dfn-collect-webvtt-cue-timings-and-settings
 */
WEBVTT_INTERN webvtt_status
webvtt_collect_timings_and_settings( webvtt_parser self,
                                     const webvtt_string *line,
                                     webvtt_cue *cue )
{
  webvtt_status s;
  webvtt_string settings;

  /* 1. Let input be the string being parsed. */
  const webvtt_string *input = line;

  /**
   * 2. Let position be a pointer to input, initially pointing at the start of
   * the string
   */
  int position = 0;

  /* 3. Skip whitespace */
  webvtt_string_skip_whitespace( line, &position );

  /**
   * 4. Collect a WebVTT timestamp. If that algorithm fails, then abort these
   * steps and return failure. Otherwise, let cue's text track cue start time
   * be the collected time.
   */
  if( WEBVTT_FAILED( s = webvtt_collect_timestamp( self, &cue->from, input,
                     &position ) ) ) {
    return s;
  }

  /* 5. Skip whitespace */
  self->column += webvtt_string_skip_whitespace( line, &position );

  /**
   * 6. If the character at position is not a U+002D HYPHEN-MINUS character (-)
   * then abort these steps and return failure. Otherwise, move position forward
   * one character.
   *
   * 7. If the character at position is not a U+002D HYPHEN-MINUS character (-)
   * then abort these steps and return failure. Otherwise, move position forward
   * one character.
   *
   * 8. If the character at position is not a U+003E GREATER-THAN SIGN character
   * (>) then abort these steps and return failure. Otherwise, move position
   * forwards one character.
   */
  if( strstr( webvtt_string_text( input ) + position, "-->" )
      != webvtt_string_text( input ) + position ) {
    return WEBVTT_PARSE_ERROR;
  }

  /* Skip separator */
  position += 3;
  self->column += 3;

  /* 9. Skip whitespace */
  self->column += webvtt_string_skip_whitespace( input, &position );

  /**
   * 10. Collect a WebVTT timestamp. If that algorithm fails, then abort these
   * steps and return failure. Otherwise, let cue's text track cue end time be
   * the collected time.
   */
  if( WEBVTT_FAILED( s = webvtt_collect_timestamp( self, &cue->until, input,
                     &position ) ) ) {
    return s;
  }

  /**
   * http://dev.w3.org/html5/webvtt/#dfn-webvtt-cu
   * 3. Optionally, either a U+0020 SPACE character or a U+0009 CHARACTER
   *    TABULATION (tab) character followed by any number of characters that are
   *    not U+000A LINE FEED (LF) or U+000D CARRIAGE RETURN (CR) characters.
   *
   * Emit warning if whitespace is not present here.
   */
  if( webvtt_string_text( input )[position] &&
      !webvtt_isspace( webvtt_string_text( input )[position] ) ) {
    ERROR( WEBVTT_EXPECTED_WHITESPACE );
  }

  /**
   * 11. Let remainder be the trailing substring of input starting at position.
   */
  webvtt_create_string_with_text( &settings,
                                  webvtt_string_text( input ) + position, -1 );
  webvtt_cue_validate_set_settings( self, cue, &settings );
  webvtt_release_string( &settings );

  return WEBVTT_SUCCESS;
}

WEBVTT_INTERN webvtt_status
webvtt_proc_cueline( webvtt_parser self, webvtt_cue *cue,
                     webvtt_string *line )
{
  SAFE_ASSERT( cue != NULL );
  const char *text;
  webvtt_uint length;
  DIE_IF( line == NULL );
  length = webvtt_string_length( line );
  text = webvtt_string_text( line );
  /* backup the column */
  self->column = 1;
  if( find_bytes( text, length, separator, sizeof( separator ) )
      == WEBVTT_SUCCESS) {
    /* It's not a cue id, we found '-->'. It can't be a second
       cueparams line, because if we had it, we would be in
       a different state. */
    int v;
    self->cuetext_line = self->line + 1;
    if( ( v = webvtt_collect_timings_and_settings( self,
                                                   line, cue ) ) < 0 ) {
        if( v == WEBVTT_PARSE_ERROR ) {
          return WEBVTT_PARSE_ERROR;
        }
        self->mode = M_SKIP_CUE;
      } else {
        cue->flags |= CUE_HAVE_CUEPARAMS;
        self->mode = M_CUETEXT;
      }
  } else {
    /* It is a cue-id */
    if( cue && cue->flags & CUE_HAVE_ID ) {
      /**
       * This isn't actually a cue-id, because we already
       * have one. It seems to be cuetext, which is occurring
       * before cue-params
       */
      webvtt_release_string( line );
      ERROR( WEBVTT_CUE_INCOMPLETE );
      self->mode = M_SKIP_CUE;
      return WEBVTT_SUCCESS;
    } else {
      webvtt_uint last_column = self->column;
      webvtt_uint last_line = self->line;
      webvtt_token token = UNFINISHED;
      self->column += length;
      self->cuetext_line = self->line;
      if( WEBVTT_FAILED( webvtt_string_append( &cue->id, text,
                                               length ) ) ) {
        webvtt_release_string( line );
        ERROR( WEBVTT_ALLOCATION_FAILED );
        return WEBVTT_OUT_OF_MEMORY;
      }
      cue->flags |= CUE_HAVE_ID;

      /* Read cue-params line */
      PUSH0( T_CUEREAD, 0, V_NONE );
      webvtt_init_string( &SP->v.text );
      SP->type = V_TEXT;
    }
  }

  webvtt_release_string( line );
  return WEBVTT_SUCCESS;
}

static webvtt_status
parse_webvtt( webvtt_parser self, const char *buffer, webvtt_uint *ppos,
              webvtt_uint len, int finish )
{
  webvtt_status status = WEBVTT_SUCCESS;
  webvtt_token token = 0;
  webvtt_uint pos = *ppos;
  int skip_error = 0;

  while( pos < len ) {
    webvtt_uint last_column, last_line;
    skip_error = 0;
    last_column = self->column;
    last_line = self->line;

    /**
     * If we're in certain states, we don't want to get a token and just
     * want to read text instead.
     */
    if( SP->state == T_CUEREAD ) {
      DIE_IF( SP->type != V_TEXT );
      if( SP->flags == 0 ) {
        int v;
        if( ( v = webvtt_string_getline( &SP->v.text, buffer, &pos, len, 0,
                                         finish ) ) != 0 ) {
          if( v < 0 ) {
            webvtt_release_string( &SP->v.text );
            SP->type = V_NONE;
            POP();
            ERROR( WEBVTT_ALLOCATION_FAILED );
            status = WEBVTT_OUT_OF_MEMORY;
            goto _finish;
          }
          /* replace '\0' with u+fffd */
          if( WEBVTT_FAILED( status = webvtt_string_replace_all( &SP->v.text,
                                                                 "\0", 1,
                                                                 replacement,
                                                                 3 ) ) ) {
            webvtt_release_string( &SP->v.text );
            SP->type = V_NONE;
            POP();
            ERROR( WEBVTT_ALLOCATION_FAILED );
            goto _finish;
          }
          SP->flags = 1;
        }
      }
      if( SP->flags ) {
        webvtt_token t = webvtt_lex_newline( self, buffer, &pos, len,
                                                 self->finished );
        if( t == NEWLINE ) {
          POP();
          continue;
        }
      }
    }

    /**
     * Get the next token from the stream
     *
     * If the token is 'UNFINISHED', but we are at the end of our input
     * data, change it to BADTOKEN because it will never be finished.
     *
     * Otherwise, if we are expecting further data at some point, and have
     * an unfinished token, return and let the next chunk deal with it.
     */
    if( SP->state != T_CUE ||
        !( self->popped && FRAMEUP( 1 )->state == T_CUEREAD ) ) {
      /**
       * We don't tokenize in certain states
       */
      token = webvtt_lex( self, buffer, &pos, len, finish );
      if( token == UNFINISHED ) {
        if( finish ) {
          token = BADTOKEN;
        } else if( pos == len ) {
          goto _finish;
        }
      }
    }
_recheck:
    switch( SP->state ) {
      default:
        /* Should never happen */
        break;

      case T_INITIAL:
        /**
         * In the initial state:
         * We should have WEBVTT as the first token returned,
         * otherwise this isn't really a valid file.
         *
         * If we get 'WEBVTT', push us into the TAG state, where we
         * check for a tag comment (arbitrary text following a whitespace
         * after the WEBVTT token) until a newline
         *
         * If WEBVTT is not the first token, then report error and
         * abort parsing.
         */
        if( token == WEBVTT ) {
          PUSH0( T_TAG, 0, V_NONE );
          break;
        } else if( token != UNFINISHED ) {
          ERROR_AT( WEBVTT_MALFORMED_TAG, 1, 1 );
          status = WEBVTT_PARSE_ERROR;
          goto _finish;
        }
        break;

      case T_TAG:
        /**
         * If we have a WHITESPACE following the WEBVTT token,
         * switch to T_TAGCOMMENT state and skip the comment.
         * Otherwise, if it's a NEWLINE, we can just skip to
         * the T_BODY state.
         *
         * Otherwise, we didn't actually have a WEBVTT token,
         * and should feel ashamed.
         */
        if( token == WHITESPACE ) {
          /* switch to comment skipper */
          PUSH0( T_TAGCOMMENT, 0, V_NONE );
        } else if( token == NEWLINE ) {
          /* switch to NEWLINE counter */
          POPBACK();
          self->popped = 0;
          SP->state = T_BODY;
          PUSH0( T_EOL, 1, V_INTEGER );
          break;
        } else {
          /**
           * This wasn't preceded by an actual WEBVTT token, it's more
           * like WEBVTTasdasd, which is not valid. Report an error,
           * which should be considered fatal.
           */
          if( !skip_error ) {
            ERROR_AT_COLUMN( WEBVTT_MALFORMED_TAG, FRAME( 1 )->column );
            skip_error = 1;
            status = WEBVTT_PARSE_ERROR;
            goto _finish;
          }
        }
        break;

        /**
         * COMMENT -- Read until EOL, ignore everything else
         */
      case T_TAGCOMMENT:
        switch( token ) {
          case NEWLINE:
            /**
             * If we encounter a newline, switch to NEWLINE mode,
             * and set up so that when we POPBACK() we are in the
             * T_BODY state.
             */
            POPBACK();
            SP->state = T_BODY;
            PUSH0( T_EOL, 1, V_INTEGER );
            break;
          default:
            find_newline( buffer, &pos, len );
            continue;
        }
        break;

      case T_CUEID:
        switch( token ) {
            /**
             * We're only really expecting a newline here --
             * The cue id should have been read already
             */
          case NEWLINE:
            SP->state = T_FROM;
            break;

          default:
            break;
        }

        /**
         * Count EOLs, POP when finished
         */
      case T_EOL:
        switch( token ) {
          case NEWLINE:
            SP->v.value++;
            break;
          default:
            POPBACK();
            RECHECK
        }
        break;

      case T_BODY:
        if( self->popped && FRAMEUP( 1 )->state == T_EOL ) {
          if( FRAMEUP( 1 )->v.value < 2 ) {
            ERROR_AT_COLUMN( WEBVTT_EXPECTED_EOL, 1 );
          }
          FRAMEUP( 1 )->state = 0;
          FRAMEUP( 1 )->v.cue = NULL;
        }
        if( token != NEWLINE ) {
          webvtt_cue *cue = 0;
          webvtt_string tk = { 0 };
          if( WEBVTT_FAILED( status = webvtt_create_cue( &cue ) ) ) {
            if( status == WEBVTT_OUT_OF_MEMORY ) {
              ERROR( WEBVTT_ALLOCATION_FAILED );
            }
            goto _finish;
          }
          if( WEBVTT_FAILED( status = webvtt_create_string_with_text( &tk,
            self->token, self->token_pos ) ) ) {
            if( status == WEBVTT_OUT_OF_MEMORY ) {
              ERROR( WEBVTT_ALLOCATION_FAILED );
            }
            webvtt_release_cue( &cue );
            goto _finish;
          }
          PUSH0( T_CUE, cue, V_CUE );
          PUSH0( T_CUEREAD, 0, V_TEXT );
          SP->v.text.d = tk.d;
        }
        break;


      case T_CUE:
      {
        webvtt_cue *cue;
        webvtt_state *st;
        webvtt_string text;
        SAFE_ASSERT( self->popped && FRAMEUP( 1 )->state == T_CUEREAD );
        /**
         * We're expecting either cue-id (contains '-->') or cue
         * params
         */
        cue = SP->v.cue;
        st = FRAMEUP( 1 );
        text.d = st->v.text.d;

        st->type = V_NONE;
        st->v.cue = NULL;

        /* FIXME: guard inconsistent state */
        if (!cue) {
          ERROR( WEBVTT_PARSE_ERROR );
          status = WEBVTT_PARSE_ERROR;
          goto _finish;
        }

        status = webvtt_proc_cueline( self, cue, &text );
        ++self->line;
        if( self->mode != M_WEBVTT ) {
          goto _finish;
        }
        self->popped = 0;
      }
      break;
    }

    /**
     * reset token pos
     */
    self->token_pos = 0;
  }


_finish:
  if( status == WEBVTT_OUT_OF_MEMORY ) {
    cleanup_stack( self );
  }
  *ppos = pos;
  return status;
}

WEBVTT_INTERN webvtt_status
webvtt_read_cuetext( webvtt_parser self, const char *b,
                     webvtt_uint *ppos, webvtt_uint len, webvtt_bool finish )
{
  webvtt_status status = WEBVTT_SUCCESS;
  webvtt_uint pos = *ppos;
  int finished = 0;
  int flags = 0;
  webvtt_cue *cue;

  /* Ensure that we have a cue to work with */
  SAFE_ASSERT( self->top->type == V_CUE );
  cue = self->top->v.cue;

  /**
   * Hack to support lines spanning multiple buffers.
   *
   * TODO: Do this some better way. This is not good!
   */
  if( self->line_buffer.d != 0 && self->line_buffer.d->text[
      self->line_buffer.d->length - 1 ] == '\n' ) {
    flags = 1;
  }

  do {
    if( !flags ) {
      int v;
      if( ( v = webvtt_string_getline( &self->line_buffer, b, &pos, len,
                                       &self->truncate, finish ) ) != 0 ) {
        if( v < 0 || WEBVTT_FAILED( webvtt_string_putc( &self->line_buffer,
                                                        '\n' ) ) ) {
          ERROR( WEBVTT_ALLOCATION_FAILED );
          status = WEBVTT_OUT_OF_MEMORY;
          goto _finish;
        }
        /* replace '\0' with u+fffd */
        if( WEBVTT_FAILED( status =
                           webvtt_string_replace_all( &self->line_buffer,
                                                      "\0", 1, replacement,
                                                      3 ) ) ) {
          ERROR( WEBVTT_ALLOCATION_FAILED );
          goto _finish;
        }

        flags = 1;
      }
    }
    if( flags ) {
      webvtt_token token = webvtt_lex_newline( self, b, &pos, len, finish );
      if( token == NEWLINE ) {
        self->token_pos = 0;
        self->line++;

        SAFE_ASSERT(self->line_buffer.d);
        /* Remove the '\n' that we appended to determine that we're in state 1
         */
        self->line_buffer.d->text[ --self->line_buffer.d->length ] = 0;
        /**
         * We've encountered a line without any cuetext on it, i.e. there is no
         * newline character and len is 0 or there is and len is 1, therefore,
         * the cue text is finished.
         */
        if( self->line_buffer.d->length == 0 ) {
          webvtt_release_string( &self->line_buffer );
          finished = 1;
        } else if( find_bytes( webvtt_string_text( &self->line_buffer ),
                   webvtt_string_length( &self->line_buffer ), separator,
                   sizeof( separator ) ) == WEBVTT_SUCCESS ) {
          /**
           * Line contains cue-times separator, and thus we treat it as a
           * separate cue. Trick program into thinking that T_CUEREAD had read
           * this line.
           */
          do_push( self, 0, 0, T_CUEREAD, 0, V_NONE, self->line, self->column );
          webvtt_copy_string( &SP->v.text, &self->line_buffer );
          webvtt_release_string( &self->line_buffer );
          SP->type = V_TEXT;
          POP();
          finished = 1;
        } else {
          /**
           * If it's not the end of a cue, simply append it to the cue's payload
           * text.
           */
          if( webvtt_string_length( &cue->body ) &&
              WEBVTT_FAILED( webvtt_string_putc( &cue->body, '\n' ) ) ) {
            status = WEBVTT_OUT_OF_MEMORY;
            goto _finish;
          }
          webvtt_string_append_string( &cue->body, &self->line_buffer );
          webvtt_release_string( &self->line_buffer );
          flags = 0;
        }
      }
    }
  } while( pos < len && !finished );
_finish:
  *ppos = pos;
  if( finish ) {
    finished = 1;
  }

  /**
   * If we didn't encounter 2 successive EOLs, and it's not the final buffer in
   * the file, notify the caller.
   */
  if( !finish && pos >= len && !WEBVTT_FAILED( status ) && !finished ) {
    status = WEBVTT_UNFINISHED;
  }
  return status;
}

WEBVTT_INTERN webvtt_status
webvtt_proc_cuetext( webvtt_parser self, const char *b,
                     webvtt_uint *ppos, webvtt_uint len, webvtt_bool finish )
{
  webvtt_status status;
  webvtt_cue *cue;
  SAFE_ASSERT( ( self->mode == M_CUETEXT || self->mode == M_SKIP_CUE )
               && self->top->type == V_CUE );
  cue = self->top->v.cue;
  SAFE_ASSERT( cue != 0 );
  status  = webvtt_read_cuetext( self, b, ppos, len, finish );

  if( status == WEBVTT_SUCCESS ) {
    if( self->mode != M_SKIP_CUE ) {
      /**
       * Once we've successfully read the cuetext into line_buffer, call the
       * cuetext parser from cuetext.c
       */
      status = webvtt_parse_cuetext( self, cue, &cue->body,
                                     self->finished );

      /**
       * return the cue to the user, if possible.
       */
      finish_cue( self, &cue );
    } else {
      webvtt_release_cue( &cue );
    }

    self->top->type = V_NONE;
    self->top->state = 0;
    self->top->v.cue = 0;

    if( (self->top+1)->type == V_NONE ) {
      (self->top+1)->state = 0;
      /* Pop from T_CUE state */
      POP();
    } else {
      /**
       * If we found '-->', we need to create another cue and remain
       * in T_CUE state
       */
      webvtt_create_cue( &self->top->v.cue );
      self->top->type = V_CUE;
      self->top->state = T_CUE;
    }
    self->mode = M_WEBVTT;
  }
  return status;
}

WEBVTT_EXPORT webvtt_status
webvtt_parse_chunk( webvtt_parser self, const void *buffer, webvtt_uint len )
{
  webvtt_status status;
  webvtt_uint pos = 0;
  const char *b = ( const char * )buffer;

  while( pos < len ) {
    switch( self->mode ) {
      case M_WEBVTT:
        if( WEBVTT_FAILED( status = parse_webvtt( self, b, &pos, len,
                                                  self->finished ) ) ) {
          return status;
        }
        break;

      case M_CUETEXT:
        /**
         * read in cuetext
         */
        if( WEBVTT_FAILED( status = webvtt_proc_cuetext( self, b, &pos, len,
                                                         self->finished ) ) ) {
          if( status == WEBVTT_UNFINISHED ) {
            /* Make an exception here, because this isn't really a failure. */
            return WEBVTT_SUCCESS;
          }
          return status;
        }

        /* If we failed to parse cuetext, return the error */
        if( WEBVTT_FAILED( status ) ) {
          return status;
        }
        break;

      case M_SKIP_CUE:
        if( WEBVTT_FAILED( status = webvtt_proc_cuetext( self, b, &pos, len,
                                                         self->finished ) ) ) {
          return status;
        }
        break;
    }
  }

  return WEBVTT_SUCCESS;
}

#undef SP
#undef AT_BOTTOM
#undef ON_HEAP
#undef STACK_SIZE
#undef FRAME
#undef PUSH
#undef POP

/**
 * Get an integer value from a series of digits.
 */
WEBVTT_INTERN webvtt_int64
webvtt_parse_int( const char **pb, int *pdigits )
{
  int digits = 0;
  webvtt_int64 result = 0;
  webvtt_int64 mul = 1;
  const char *b = *pb;
  while( *b ) {
    char ch = *b;
    if( webvtt_isdigit( ch ) ) {
      /**
       * Digit character, carry on
       */
      result = result * 10 + ( (webvtt_int64)ch - '0' );
      ++digits;
    } else if( mul == 1 && digits == 0 && ch == '-' ) {
      mul = -1;
    } else {
      break;
    }
    ++b;
  }
  *pb = b;
  if( pdigits ) {
    *pdigits = digits;
  }
  return result * mul;
}

/**
 * Turn the token of a TIMESTAMP tag into something useful, and returns non-zero
 * returns 0 if it fails
 */
WEBVTT_INTERN int
webvtt_parse_timestamp( const char *b, int *tokenLength, webvtt_timestamp *result )
{
  webvtt_int64 tmp;
  int have_hours = 0;
  int digits;
  int malformed = 0;
  webvtt_int64 v[4];
  int n = 0;
  if ( !webvtt_isdigit( *b ) ) {
    goto not_timestamp;
  }

  /* get sequence of digits */
  v[0] = webvtt_parse_int( &b, &digits );

  /**
   * assume v[0] contains hours if more or less than 2 digits, or value is
   * greater than 59
   */
  if ( digits != 2 || v[0] > 59 ) {
    have_hours = 1;
  }

  n += digits;

  /* fail if missing colon ':' character */
  if ( !*b || *b++ != ':' ) {
    malformed = 1;
    goto not_timestamp;
  }
  ++n;

  /* fail if end of data reached, or byte is not an ASCII digit */
  if ( !*b || !webvtt_isdigit( *b ) ) {
    malformed = 1;
    goto not_timestamp;
  }

  /* get another integer value, fail if digits is not equal to 2 */
  v[1] = webvtt_parse_int( &b, &digits );
  if( digits != 2 ) {
    malformed = 1;
  }
  n += digits;

  /* if we already know there's an hour component, or if the next byte is a
     colon ':', read the next value */
  if ( have_hours || ( *b == ':' ) ) {
    if( *b == '.' ) {
      /* If this byte is '.', then have_hours is incorrectly set to true.
         the timestamp is technically malformed, but still understandable,
         so far. */
      malformed = 1;
      goto shift_over;
    } else if( *b++ != ':' ) {
      /* If ':' is not found, this is not a timestamp. */
      goto not_timestamp;
    }
    ++n;
    if( !*b || !webvtt_isdigit( *b ) ) {
      malformed = 1;
      v[2] = 0;
    } else {
      v[2] = webvtt_parse_int( &b, &digits );
      if( digits != 2 ) {
        malformed = 1;
      }
      n += digits;
    }
  } else {
shift_over:
    /* Otherwise, if there is no hour component, shift everything over */
    v[2] = v[1];
    v[1] = v[0];
    v[0] = 0;
  }

  /* collect the manditory seconds-frac component. fail if there is no FULL_STOP
     '.' or if there is no ascii digit following it */
  if( *b++ != '.' || !webvtt_isdigit( *b ) ) {
    goto not_timestamp;
  }
  ++n;
  v[3] = webvtt_parse_int( &b, &digits );
  if( digits != 3 ) {
    malformed = 1;
  }
  n += digits;

  /* Ensure that minutes and seconds are acceptable values */
  if( v[3] > 999 ) {
#define MILLIS_PER_SEC (1000)
    tmp = v[3];
    v[2] += tmp / MILLIS_PER_SEC;
    v[3] = tmp % MILLIS_PER_SEC;
    malformed = 1;
  }
  if( v[2] > 59 ) {
#define SEC_PER_MIN (60)
    tmp = v[2];
    v[1] += tmp / SEC_PER_MIN;
    v[2] = tmp % SEC_PER_MIN;
    malformed = 1;
  }
  if( v[1] > 59 ) {
#define MIN_PER_HOUR (60)
    tmp = v[1];
    v[0] += tmp / MIN_PER_HOUR;
    v[1] = tmp % MIN_PER_HOUR;
    malformed = 1;
  }

  *result = ( webvtt_timestamp )( v[0] * MSECS_PER_HOUR )
            + ( v[1] * MSECS_PER_MINUTE )
            + ( v[2] * MSECS_PER_SECOND )
            + ( v[3] );

  if( tokenLength ) {
    *tokenLength = n;
  }

  if( malformed ) {
    return 0;
  }
  return 1;

not_timestamp:
  *result = 0xFFFFFFFFFFFFFFFF;
  if( tokenLength ) {
    *tokenLength = n;
  }
  return 0;
}

WEBVTT_INTERN webvtt_bool
token_in_list( webvtt_token token, const webvtt_token list[] )
{
  int i = 0;
  webvtt_token t;
  while( ( t = list[ i++ ] ) != 0 ) {
    if( token == t ) {
      return 1;
    }
  }
  return 0;
}

WEBVTT_INTERN int
find_token( webvtt_token token, const webvtt_token list[] )
{
  int i = 0;
  webvtt_token t;
  while( ( t = list[ i ] ) != 0 ) {
    webvtt_token masked = t & TF_TOKEN_MASK;
    if( token == masked ) {
      return i;
    }
    ++i;
  }
  return -1;
}
