/***************************************************************************
    copyright            : (C) 2002 - 2008 by Scott Wheeler
    email                : wheeler@kde.org
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

#include "tagunion.h"
#include "tstringlist.h"
#include "tpropertymap.h"

#include "id3v1tag.h"
#include "id3v2tag.h"
#include "apetag.h"
#include "xiphcomment.h"
#include "infotag.h"

#include <array>

using namespace TagLib;

#define stringUnion(method)                                          \
  if(tag(0) && !tag(0)->method().isEmpty())                          \
    return tag(0)->method();                                         \
  if(tag(1) && !tag(1)->method().isEmpty())                          \
    return tag(1)->method();                                         \
  if(tag(2) && !tag(2)->method().isEmpty())                          \
    return tag(2)->method();                                         \
  return String();                                                   \

#define numberUnion(method)                                          \
  if(tag(0) && tag(0)->method() > 0)                                 \
    return tag(0)->method();                                         \
  if(tag(1) && tag(1)->method() > 0)                                 \
    return tag(1)->method();                                         \
  if(tag(2) && tag(2)->method() > 0)                                 \
    return tag(2)->method();                                         \
  return 0

#define setUnion(method, value)                                      \
  if(tag(0))                                                         \
    tag(0)->set##method(value);                                      \
  if(tag(1))                                                         \
    tag(1)->set##method(value);                                      \
  if(tag(2))                                                         \
    tag(2)->set##method(value);                                      \

class TagUnion::TagUnionPrivate
{
public:
  TagUnionPrivate() = default;
  ~TagUnionPrivate()
  {
    for(auto &tag : tags)
      delete tag;
  }

  TagUnionPrivate(const TagUnionPrivate &) = delete;
  TagUnionPrivate &operator=(const TagUnionPrivate &) = delete;

  std::array<Tag *, 3> tags { nullptr, nullptr, nullptr };
};

TagUnion::TagUnion(Tag *first, Tag *second, Tag *third) :
  d(std::make_unique<TagUnionPrivate>())
{
  d->tags = { first, second, third };
}

TagUnion::~TagUnion() = default;

Tag *TagUnion::operator[](int index) const
{
  return tag(index);
}

Tag *TagUnion::tag(int index) const
{
  return d->tags[index];
}

void TagUnion::set(int index, Tag *tag)
{
  delete d->tags[index];
  d->tags[index] = tag;
}

PropertyMap TagUnion::properties() const
{
  for(const auto &tag : d->tags) {
    if(tag && !tag->isEmpty()) {
      return tag->properties();
    }
  }

  return PropertyMap();
}

void TagUnion::removeUnsupportedProperties(const StringList &unsupported)
{
  for(const auto &tag : d->tags) {
    if(tag) {
      tag->removeUnsupportedProperties(unsupported);
    }
  }
}

String TagUnion::title() const
{
  stringUnion(title);
}

String TagUnion::artist() const
{
  stringUnion(artist);
}

String TagUnion::album() const
{
  stringUnion(album);
}

String TagUnion::comment() const
{
  stringUnion(comment);
}

String TagUnion::genre() const
{
  stringUnion(genre);
}

unsigned int TagUnion::year() const
{
  numberUnion(year);
}

unsigned int TagUnion::track() const
{
  numberUnion(track);
}

void TagUnion::setTitle(const String &s)
{
  setUnion(Title, s);
}

void TagUnion::setArtist(const String &s)
{
  setUnion(Artist, s);
}

void TagUnion::setAlbum(const String &s)
{
  setUnion(Album, s);
}

void TagUnion::setComment(const String &s)
{
  setUnion(Comment, s);
}

void TagUnion::setGenre(const String &s)
{
  setUnion(Genre, s);
}

void TagUnion::setYear(unsigned int i)
{
  setUnion(Year, i);
}

void TagUnion::setTrack(unsigned int i)
{
  setUnion(Track, i);
}

bool TagUnion::isEmpty() const
{
  return std::none_of(d->tags.begin(), d->tags.end(), [](auto t) { return t && !t->isEmpty(); });
}
