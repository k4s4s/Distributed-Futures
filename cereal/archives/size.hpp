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
#ifndef CEREAL_ARCHIVES_SIZE_HPP_
#define CEREAL_ARCHIVES_SIZE_HPP_

#include <cereal/cereal.hpp>

namespace cereal
{

  // ######################################################################
  //! An output archive designed to count the bytes of the data to be serialized
  /*! This archive only counts the data to be serialized

      \ingroup Archives */
  class SizeOutputArchive : public OutputArchive<SizeOutputArchive, AllowEmptyClassElision>
  {
    public:
      SizeOutputArchive() :
        OutputArchive<SizeOutputArchive, AllowEmptyClassElision>(this),
				total_bytes(0)
    { }

      //! Counts data bytes
      void countBytes(size_t size) {
				total_bytes += size;
      }

			size_t size() {
				return total_bytes;
			}

    private:
    	size_t total_bytes;
  };

  // ######################################################################
  //! An input archive designed to count the bytes of the serialized data
  /*! This archive only counts the data of a serialized archive

      \ingroup Archives */
class SizeInputArchive : public InputArchive<SizeInputArchive, AllowEmptyClassElision>
  {
    public:
      //! Construct, loading from the provided stream
      SizeInputArchive() :
        InputArchive<SizeInputArchive, AllowEmptyClassElision>(this),
				total_bytes(0)
    { }
      //! Counts data bytes
      void countBytes(size_t size) {
				total_bytes += size;
      }

			size_t size() {
				return total_bytes;
			}

    private:
			size_t total_bytes;
  };

  // ######################################################################
  // Common BinaryArchive serialization functions

  //! Saving for POD types to binary
  template<class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  save(SizeOutputArchive & ar, T const & t)
  {
    ar.countBytes(sizeof(t));
  }

  //! Loading for POD types from binary
  template<class T> inline
  typename std::enable_if<std::is_arithmetic<T>::value, void>::type
  load(SizeInputArchive & ar, T & t)
  {
    ar.countBytes(sizeof(t));
  }


  //! Serializing NVP types to binary
  template <class Archive, class T> inline
  CEREAL_ARCHIVE_RESTRICT(SizeInputArchive, SizeOutputArchive)
  serialize( Archive & ar, NameValuePair<T> & t )
  {
    //ar( t.value );
  }

  //! Serializing SizeTags to binary
  template <class Archive, class T> inline
  CEREAL_ARCHIVE_RESTRICT(SizeInputArchive, SizeOutputArchive)
  serialize( Archive & ar, SizeTag<T> & t )
  {
    //ar( t.size );
  }

  //! Saving binary data
  template <class T> inline
  void save(SizeOutputArchive & ar, BinaryData<T> const & bd)
  {
    ar.countBytes(bd.size);
  }

  //! Loading binary data
  template <class T> inline
  void load(SizeInputArchive & ar, BinaryData<T> & bd)
  {
    ar.countBytes(bd.size);
  }

} // namespace cereal

// register archives for polymorphic support
CEREAL_REGISTER_ARCHIVE(cereal::SizeOutputArchive);

#endif // CEREAL_ARCHIVES_RAW_HPP_
