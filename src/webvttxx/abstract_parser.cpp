//
// Copyright (c) 2013 Mozilla Foundation and Contributors
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//  - Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//  - Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include <webvttxx/abstract_parser>
#include <webvttxx/cue>

namespace WebVTT
{

AbstractParser::AbstractParser()
{
  webvtt_status status;
  if(WEBVTT_FAILED(status = webvtt_create_parser( &__parsedCue, &__reportError,
                                                  this, &parser ) ) ) {
    /**
     * TODO: Throw error
     */
  }
}

AbstractParser::~AbstractParser()
{
  webvtt_delete_parser( parser );
}

::webvtt_status
AbstractParser::finishParsing()
{
  return webvtt_finish_parsing( parser );
}

::webvtt_status
AbstractParser::parseChunk( const void *chunk, webvtt_uint length )
{
  return webvtt_parse_chunk( parser, chunk, length );
}

void WEBVTT_CALLBACK
AbstractParser::__parsedCue( void *userdata, webvtt_cue *pcue )
{
  Cue cue(pcue);
  /**
   * Cue object increases the reference count of pcue, so we can dereference it
   */
  webvtt_release_cue( &pcue );

  AbstractParser *self = static_cast<AbstractParser *>( userdata );
  self->parsedCue( cue );
}

int WEBVTT_CALLBACK
AbstractParser::__reportError( void *userdata, webvtt_uint line,
                               webvtt_uint col, webvtt_error error )
{
  AbstractParser *self = static_cast<AbstractParser *>( userdata );
  Error err( line, col, error );
  if( !self->reportError( err ) ) {
    return -1;
  }
  return 0;
}

}
