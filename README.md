# aws_sigv4

aws_sigv4 is a small C library for creating an Amazon Web Services signature
version 4.

## Pardon our dust

This library is still in pre-release Alpha state. It does not use the GNU build
tools yet and has only been compiled on Mac OS X. For now it is assumed that
liburiparser is installed on your system.

Furthermore, the API might change between now and the beta. Non-GET requests may
be signed incorrectly now.

Be warned.

## Usage

For now, ```make``` or ```make dev``` is enough.

The library will be saved in ./build.

In your code include the library:

    #include <aws_sigv4.h>

Then link the library when you compile:

    gcc -o yourprogram yourprogram.c -L/path/to/libaws_sigv4.a -laws_sigv4 -luriparser -I/path/to/aws_sigv4/src

# Licenses

## aws_sigv4

This library is released under the BSD License.

## The Better String Library

### BSD License

Copyright (c) 2002-2008 Paul Hsieh
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, 
    this list of conditions and the following disclaimer. 

    Redistributions in binary form must reproduce the above copyright notice, 
    this list of conditions and the following disclaimer in the documentation 
    and/or other materials provided with the distribution. 

    Neither the name of bstrlib nor the names of its contributors may be used 
    to endorse or promote products derived from this software without 
    specific prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

## uriparser

### BSD License

uriparser - RFC 3986 URI parsing library

Copyright (C) 2007, Weijia Song <songweijia@gmail.com>
Copyright (C) 2007, Sebastian Pipping <webmaster@hartwork.org>
All rights reserved.

Redistribution  and use in source and binary forms, with or without
modification,  are permitted provided that the following conditions
are met:

    * Redistributions   of  source  code  must  retain  the   above
      copyright  notice, this list of conditions and the  following
      disclaimer.

    * Redistributions  in  binary  form must  reproduce  the  above
      copyright  notice, this list of conditions and the  following
      disclaimer   in  the  documentation  and/or  other  materials
      provided with the distribution.

    * Neither  the name of the <ORGANIZATION> nor the names of  its
      contributors  may  be  used to endorse  or  promote  products
      derived  from  this software without specific  prior  written
      permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT  NOT
LIMITED  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS
FOR  A  PARTICULAR  PURPOSE ARE DISCLAIMED. IN NO EVENT  SHALL  THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL,    SPECIAL,   EXEMPLARY,   OR   CONSEQUENTIAL   DAMAGES
(INCLUDING,  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES;  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT  LIABILITY,  OR  TORT (INCLUDING  NEGLIGENCE  OR  OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.
