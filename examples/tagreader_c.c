/* Copyright (C) 2003 Scott Wheeler <wheeler@kde.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <tag_c.h>

#ifndef FALSE
#define FALSE 0
#endif

int main(int argc, char *argv[])
{
  int i;
  int seconds;
  int minutes;
  TagLib_File *file;
  TagLib_Tag *tag;
  const TagLib_AudioProperties *properties;
  char **propertiesMap;
  char **propertyValues;
  char **pp;
  char **pp1;

  taglib_set_strings_unicode(1);

  for(i = 1; i < argc; i++) {
    printf("******************** \"%s\" ********************\n", argv[i]);

    file = taglib_file_new(argv[i]);

    if(file == NULL)
      break;

    tag = taglib_file_tag(file);
    properties = taglib_file_audioproperties(file);
    propertiesMap = taglib_property_keys(file);

    if(tag != NULL) {
      printf("-- TAG --\n");
      printf("title   - \"%s\"\n", taglib_tag_title(tag));
      printf("artist  - \"%s\"\n", taglib_tag_artist(tag));
      printf("album   - \"%s\"\n", taglib_tag_album(tag));
      printf("year    - \"%i\"\n", taglib_tag_year(tag));
      printf("comment - \"%s\"\n", taglib_tag_comment(tag));
      printf("track   - \"%i\"\n", taglib_tag_track(tag));
      printf("genre   - \"%s\"\n", taglib_tag_genre(tag));
    }

    if(properties != NULL) {
      seconds = taglib_audioproperties_length(properties) % 60;
      minutes = (taglib_audioproperties_length(properties) - seconds) / 60;

      printf("-- AUDIO --\n");
      printf("bitrate     - %i\n", taglib_audioproperties_bitrate(properties));
      printf("sample rate - %i\n", taglib_audioproperties_samplerate(properties));
      printf("channels    - %i\n", taglib_audioproperties_channels(properties));
      printf("length      - %i:%02i\n", minutes, seconds);
    }

    if(propertiesMap)
    {
      printf("-- PROPERITES --\n");
      pp = propertiesMap;
      while (*pp)
      {
        printf("\"%s\"\n", *pp);
	propertyValues = pp1 = taglib_property_get(file, *pp);

	while (pp1 && *pp1) {
	  printf("  \"%s\"\n", *pp1);
	  ++pp1;
	}
	taglib_property_free(propertyValues);

        ++pp;
      }
    }

    {
      printf("-- SET PROPERITES --\n");
      const char* key = "taglibreader_c";
      taglib_property_set(file, key, "new value");
      taglib_property_set_append(file, key, "another value");

	printf("== added 2x values\n");
	propertyValues = pp1 = taglib_property_get(file, key);
	while (pp1 && *pp1) {
	  printf("  \"%s\"\n", *pp1);
	  ++pp1;
	}
	taglib_property_free(propertyValues);

      taglib_property_set(file, key, NULL);
	propertyValues = taglib_property_get(file, key);
	printf("== removed value, should be null - %x\n", propertyValues);

	printf("== appending to empty\n");
      taglib_property_set_append(file, key, "append value");
	propertyValues = pp1 = taglib_property_get(file, key);
	while (pp1 && *pp1) {
	  printf("  \"%s\"\n", *pp1);
	  ++pp1;
	}
	taglib_property_free(propertyValues);
    }

    taglib_property_free(propertiesMap);
    taglib_tag_free_strings();
    taglib_file_free(file);
  }

  return 0;
}
