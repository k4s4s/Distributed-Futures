/*! \file binary.hpp
    \brief Binary input and output archives */
/*
  Copyright (c) 2013, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of cereal nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES OR SHANE GRANT BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_ARCHIVES_RAW_HPP_
#define CEREAL_ARCHIVES_RAW_HPP_

#include <cereal/cereal.hpp>
#include <vector>
#include <sstream>
#include <cstring>
#include <iostream>

namespace cereal
{

	typedef std::vector<char> buffer_type;
  // ######################################################################
  //! An output archive designed to save data in a compact binary representation
  /*! This archive outputs data to a stream in an extremely compact binary
      representation with as little extra metadata as possible.

      This archive does nothing to ensure that the endianness of the saved
      and loaded data is the same.  If you need to have portability over
      architectures with different endianness, use PortableBinaryOutputArchive.

      \ingroup Archives */
  class RawOutputArchive : public OutputArchive<RawOutputArchive, AllowEmptyClassElision>
  {
    public:
      //! Construct, outputting to the provided stream
      /*! @param stream The stream to output to.  Can be a stringstream, a file stream, or
                        even cout! */
      RawOutputArchive() :
        OutputArchive<RawOutputArchive, AllowEmptyClassElision>(this)
    { }

      //! Writes size bytes of data to the output stream
      void saveBinary(const void* data, size_t size) {
				const char *data_ptr = reinterpret_cast<const char*>(data);
      	_raw_buffer.insert(_raw_buffer.end(), data_ptr, data_ptr+size);
      }

			int size() {
				return _raw_buffer.size();
			}

			void const *address() {
				return &_raw_buffer.front();
			}

    private:
    	buffer_type _raw_buffer; //FIXME:is the size correct?
  };

  // ######################################################################
  //! An input archive designed to load data saved using BinaryOutputArchive
  /*  This archive does nothing to ensure that the endianness of the saved
      and loaded data is the same.  If you need to have portability over
      architectures with different endianness, use PortableBinaryOutputArchive.

      \ingroup Archives */
  class RawInputArchive : public InputArchive<RawInputArchive, AllowEmptyClassElision>
  {
    public:
      //! Construct, loading from the provided stream
      RawInputArchive() :
        InputArchive<RawInputArchive, AllowEmptyClassElision>(this),
				position(0)
    { }

      //! Reads size bytes of data from the input stream
      void loadBinary(void* const data, size_t size)
      {
				//std::cout << endl()
     	 	if(position+size > static_cast<int>(_raw_buffer.size())) {
					throw Exception("Failed to read " + std::to_string(size) + " bytes from buffer. Read " + std::to_string(position-1) + "bytes");
				}
      	std::memcpy(data, &_raw_buffer[position], size);
      	position += size;
			}

			void resize(size_t size) {
				_raw_buffer.resize(size);
			}

			int size() {
				return _raw_buffer.size();
			}

			void const *address() {
				return &_raw_buffer.front();
			}

    private:
      buffer_type _raw_buffer;
			int position;
  };

  // ######################################################################
  // Common BinaryArchive serialization functions

  //! Saving for POD types to binary
  template<class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  save(RawOutputArchive & ar, T const & t)
  {
    ar.saveBinary(std::addressof(t), sizeof(t));
  }

  //! Loading for POD types from binary
  template<class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  load(RawInputArchive & ar, T & t)
  {
    ar.loadBinary(std::addressof(t), sizeof(t));
  }

  //! Serializing NVP types to binary
  template <class Archive, class T> inline
  CEREAL_ARCHIVE_RESTRICT(RawInputArchive, RawOutputArchive)
  serialize( Archive & ar, NameValuePair<T> & t )
  {
    ar( t.value );
  }

  //! Serializing SizeTags to binary
  template <class Archive, class T> inline
  CEREAL_ARCHIVE_RESTRICT(RawInputArchive, RawOutputArchive)
  serialize( Archive & ar, SizeTag<T> & t )
  {
    ar( t.size );
  }

  //! Saving binary data
  template <class T> inline
  void save(RawOutputArchive & ar, BinaryData<T> const & bd)
  {
    ar.saveBinary(bd.data, bd.size);
  }

  //! Loading binary data
  template <class T> inline
  void load(RawInputArchive & ar, BinaryData<T> & bd)
  {
    ar.loadBinary(bd.data, bd.size);
  }
} // namespace cereal

// register archives for polymorphic support
CEREAL_REGISTER_ARCHIVE(cereal::RawOutputArchive);
CEREAL_REGISTER_ARCHIVE(cereal::RawInputArchive);

#endif // CEREAL_ARCHIVES_RAW_HPP_
