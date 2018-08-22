/**
 * Copyright (c) 2012, Thibault Signor <tibsou@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PWMSGPACK_H
#define PWMSGPACK_H

#include <msgpack.hpp>
#include <sstream>
#include <QtNetwork>

#define ENABLE_MSGPACK(T) friend struct msgpack::v1::adaptor::convert<T, void>

typedef msgpack::type::tuple<uint16_t, msgpack::type::raw_ref> NetMsg;

namespace msgpack {

  template <typename T>
  size_t pack(QDataStream* s, const T& v) {
    std::stringstream buffer;
    msgpack::pack(buffer, v);
    buffer.seekg(0);
    std::string str = buffer.str();
    return s->writeRawData(str.data(), str.size());
  }

  template <typename T>
  size_t pack(QDataStream& s, const T& v) {
    return pack(&s, v);
  }

  object_handle unpack(QDataStream* s, std::size_t len);
  object_handle unpack(QDataStream& s, std::size_t len);

}

typedef struct _BaseUser {

  float x, y, z;
  float pitch, yaw;
  
  MSGPACK_DEFINE_MAP(x, y, z, pitch, yaw);
  
} BaseUser;

#endif // PWMSGPACK_H
