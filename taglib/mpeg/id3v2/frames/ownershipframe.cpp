/***************************************************************************
    copyright            : (C) 2012 by Rupert Daniel
    email                : rupert@cancelmonday.com
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#include <tdebug.h>
#include <tstringlist.h>
#include <id3v2tag.h>

#include "ownershipframe.h"

//JBH <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef JBH_USE_EMBEDDED_UNICODE_ENCODER
#include "charsetdetector.h"
#include "charsetconverter.h"
#endif
//JBH >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

using namespace TagLib;
using namespace ID3v2;

class OwnershipFrame::OwnershipFramePrivate
{
public:
  String pricePaid;
  String datePurchased;
  String seller;
  String::Type textEncoding;
};

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

//JBH ==========================================================================<
#ifdef JBH_USE_EMBEDDED_UNICODE_ENCODER
OwnershipFrame::OwnershipFrame(String::Type encoding, std::string orgCharSet, float orgCharSetConfidence) :
  Frame("OWNE", orgCharSet, orgCharSetConfidence),
  d(new OwnershipFramePrivate())
{
  d->textEncoding = encoding;
}

OwnershipFrame::OwnershipFrame(const ByteVector &data, std::string orgCharSet, float orgCharSetConfidence) :
  Frame(data, orgCharSet, orgCharSetConfidence),
  d(new OwnershipFramePrivate())
{
  setData(data);
}
#else
OwnershipFrame::OwnershipFrame(String::Type encoding) :
  Frame("OWNE"),
  d(new OwnershipFramePrivate())
{
  d->textEncoding = encoding;
}

OwnershipFrame::OwnershipFrame(const ByteVector &data) :
  Frame(data),
  d(new OwnershipFramePrivate())
{
  setData(data);
}
#endif
//JBH ==========================================================================>

OwnershipFrame::~OwnershipFrame()
{
  delete d;
}

String OwnershipFrame::toString() const
{
  return "pricePaid=" + d->pricePaid + " datePurchased=" + d->datePurchased + " seller=" + d->seller;
}

String OwnershipFrame::pricePaid() const
{
  return d->pricePaid;
}

void OwnershipFrame::setPricePaid(const String &s)
{
  d->pricePaid = s;
}

String OwnershipFrame::datePurchased() const
{
  return d->datePurchased;
}

void OwnershipFrame::setDatePurchased(const String &s)
{
  d->datePurchased = s;
}

String OwnershipFrame::seller() const
{
  return d->seller;
}

void OwnershipFrame::setSeller(const String &s)
{
  d->seller = s;
}

String::Type OwnershipFrame::textEncoding() const
{
  return d->textEncoding;
}

void OwnershipFrame::setTextEncoding(String::Type encoding)
{
  d->textEncoding = encoding; //JBH: possible enum values: {Latin1, UTF16, UTF16BE, UTF8, UTF16LE}
}

////////////////////////////////////////////////////////////////////////////////
// protected members
////////////////////////////////////////////////////////////////////////////////

void OwnershipFrame::parseFields(const ByteVector &data)
{
  int pos = 0;

  // Get the text encoding
  d->textEncoding = String::Type(data[0]); //JBH: The first byte of a field is always the encoding type in id3v2 by the spec?
  pos += 1;

  // Read the price paid this is a null terminate string
  d->pricePaid = readStringField(data, String::Latin1, &pos);

  // If we don't have at least 8 bytes left then don't parse the rest of the
  // data
  if(data.size() - pos < 8) {
    return;
  }

  // Read the date purchased YYYYMMDD
  d->datePurchased = String(data.mid(pos, 8));
  pos += 8;

  // Read the seller
  if(d->textEncoding == String::Latin1)
  {
//JBH <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef JBH_USE_EMBEDDED_UNICODE_ENCODER
    int res;
    std::string charset;
    float confidence;
    String encoded_string;

    //confidence = CHARSETDETECTOR::detectCharSet(data, charset);
    //confidence = CHARSETDETECTOR::detectCharSet(data.mid(pos), charset);
    charset    = getOrgCharSet();
    confidence = getOrgCharSetConfidence();
    if ( (confidence > CHARSETDETECTOR_CONFIDENCE_THRESHOLD) && (charset != "UNDETECTED") && (charset != "UNKNOWN") && (charset != "NOUSE") && (charset != "ASCII") && (charset != "WINDOWS-1252") && (charset != "UTF-8") )
    {
      res = CHARSETCONVERTER::encodeByteVectorToUTF16(data.mid(pos), encoded_string, charset, CHARSETCONVERTER_TO_CHARSET);
      if (res ==0)
      {
        d->seller = encoded_string;
      }
      else
      {
        //fallback upon failure
        d->seller = Tag::latin1StringHandler()->parse(data.mid(pos));
      }
    }
    else
    {
      d->seller = Tag::latin1StringHandler()->parse(data.mid(pos));
    }
#else
    /*
     * JBH: callback the client-side-defined unicode encoder, if registered at the init stage of TagLib.
     *      EX: at taglibfile.cpp@kid3, TagLib::ID3v2::Tag::setLatin1StringHandler(m_textCodecStringHandlerForID3v2); //JBH: register our own unicode encoder to TagLib, so that TagLib will call back.
     *
     * JBH: All "Non-Unicode" strings, such as EUC-KR and real Latin1, are marked "Latin1" in taglib.
     */
    d->seller = Tag::latin1StringHandler()->parse(data.mid(pos));
#endif
//JBH >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>        
  }
  else
  {
    d->seller = String(data.mid(pos), d->textEncoding);
  }
}

ByteVector OwnershipFrame::renderFields() const
{
  StringList sl;
  sl.append(d->seller);

  const String::Type encoding = checkTextEncoding(sl, d->textEncoding);

  ByteVector v;

  v.append(char(encoding));
  v.append(d->pricePaid.data(String::Latin1));
  v.append(textDelimiter(String::Latin1));
  v.append(d->datePurchased.data(String::Latin1));
  v.append(d->seller.data(encoding));

  return v;
}

////////////////////////////////////////////////////////////////////////////////
// private members
////////////////////////////////////////////////////////////////////////////////

//JBH ==========================================================================<
#ifdef JBH_USE_EMBEDDED_UNICODE_ENCODER
OwnershipFrame::OwnershipFrame(const ByteVector &data, Header *h, std::string orgCharSet, float orgCharSetConfidence) :
  Frame(h, orgCharSet, orgCharSetConfidence),
  d(new OwnershipFramePrivate())
{
  parseFields(fieldData(data));
}
#else
OwnershipFrame::OwnershipFrame(const ByteVector &data, Header *h) :
  Frame(h),
  d(new OwnershipFramePrivate())
{
  parseFields(fieldData(data));
}
#endif
//JBH ==========================================================================>
