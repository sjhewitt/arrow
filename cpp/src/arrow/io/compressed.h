// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

// Compressed stream implementations

#ifndef ARROW_IO_COMPRESSED_H
#define ARROW_IO_COMPRESSED_H

#include <memory>
#include <string>

#include "arrow/io/concurrency.h"
#include "arrow/io/interfaces.h"
#include "arrow/util/visibility.h"

namespace arrow {

class MemoryPool;
class Status;

namespace util {

class Codec;

}  // namespace util

namespace io {

class ARROW_EXPORT CompressedOutputStream : public OutputStream {
 public:
  ~CompressedOutputStream() override;

  /// \brief Create a compressed output stream wrapping the given output stream.
  static Status Make(util::Codec* codec, const std::shared_ptr<OutputStream>& raw,
                     std::shared_ptr<CompressedOutputStream>* out);
  static Status Make(MemoryPool* pool, util::Codec* codec,
                     const std::shared_ptr<OutputStream>& raw,
                     std::shared_ptr<CompressedOutputStream>* out);

  // OutputStream interface

  /// \brief Close the compressed output stream.  This implicitly closes the
  /// underlying raw output stream.
  Status Close() override;
  Status Abort() override;
  bool closed() const override;

  Status Tell(int64_t* position) const override;

  Status Write(const void* data, int64_t nbytes) override;
  /// \cond FALSE
  using Writable::Write;
  /// \endcond
  Status Flush() override;

  /// \brief Return the underlying raw output stream.
  std::shared_ptr<OutputStream> raw() const;

 private:
  ARROW_DISALLOW_COPY_AND_ASSIGN(CompressedOutputStream);

  CompressedOutputStream() = default;

  class ARROW_NO_EXPORT Impl;
  std::unique_ptr<Impl> impl_;
};

class ARROW_EXPORT CompressedInputStream
    : public internal::InputStreamConcurrencyWrapper<CompressedInputStream> {
 public:
  ~CompressedInputStream() override;

  /// \brief Create a compressed input stream wrapping the given input stream.
  static Status Make(util::Codec* codec, const std::shared_ptr<InputStream>& raw,
                     std::shared_ptr<CompressedInputStream>* out);
  static Status Make(MemoryPool* pool, util::Codec* codec,
                     const std::shared_ptr<InputStream>& raw,
                     std::shared_ptr<CompressedInputStream>* out);

  // InputStream interface

  bool closed() const override;

  /// \brief Return the underlying raw input stream.
  std::shared_ptr<InputStream> raw() const;

 private:
  friend InputStreamConcurrencyWrapper<CompressedInputStream>;
  ARROW_DISALLOW_COPY_AND_ASSIGN(CompressedInputStream);

  CompressedInputStream() = default;

  /// \brief Close the compressed input stream.  This implicitly closes the
  /// underlying raw input stream.
  Status DoClose();
  Status DoAbort() override;
  Status DoTell(int64_t* position) const;
  Status DoRead(int64_t nbytes, int64_t* bytes_read, void* out);
  Status DoRead(int64_t nbytes, std::shared_ptr<Buffer>* out);

  class ARROW_NO_EXPORT Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace io
}  // namespace arrow

#endif  // ARROW_IO_COMPRESSED_H
