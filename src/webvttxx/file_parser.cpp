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

#include <stdlib.h>
#include <webvttxx/file_parser>

namespace WebVTT
{

FileParser::FileParser( const char *fPath )
 : filePath( fPath )
{
  reader.open( fPath, std::ios::in | std::ios::binary );

  if( !reader.good() ) {
    // TODO: Throw
  }
}

FileParser::~FileParser()
{
  try {
    if( reader.is_open() ) {
      reader.close();
    }
  }
  catch (...) {
  }
}

bool
FileParser::parse()
{
  if( !reader.good() ) {
    return false;
  }

  const ::webvtt_status status = [this] {
    char buffer[0x1000];
    bool final = false;
    ::webvtt_status status = WEBVTT_FAIL;
    do
    {
      reader.read( buffer, sizeof buffer );
      const uint len = static_cast<uint>(reader.gcount());
      final = reader.eof();
      status = parseChunk( buffer, len );
    } while( !final && !WEBVTT_FAILED(status) );
    return ( status == WEBVTT_UNFINISHED ) ? WEBVTT_SUCCESS : status;
  }();

  const ::webvtt_status finishStatus = finishParsing();
  return !( WEBVTT_FAILED(status) || WEBVTT_FAILED(finishStatus) );
}

}
