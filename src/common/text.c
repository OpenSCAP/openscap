/*
 * text.c
 *
 *  Created on: Jan 14, 2010
 *      Author: david.niemoller
 */

#include "util.h"
#include "list.h"
#include <config.h>
#include <string.h>
#include <wchar.h>
#include <stdio.h>
#ifdef HAVE_ENDIAN_H
# include <endian.h>
#else
/* XXX: workaround */
# ifdef HAVE_SYS_ENDIAN_H
# include <sys/endian.h>
#  if _BYTE_ORDER == _LITTLE_ENDIAN
#   define __LONG_LONG_PAIR(HI, LO) LO, HI
#  elif _BYTE_ORDER == _BIG_ENDIAN
#   define __LONG_LONG_PAIR(HI, LO) HI, LO
#  endif
# endif
#endif

OSCAP_ITERATOR_GEN_T(const struct oscap_text *, oscap_text)
OSCAP_ITERATOR_REMOVE_T(const struct oscap_text *, oscap_text, oscap_text_free)

typedef struct oscap_text {
	char *lang;
	char *encoding;
	wchar_t *text;
} oscap_text_t;

#define _FREE(MNAME) if(text->MNAME)free(text->MNAME);text->MNAME = NULL;
void oscap_text_free(struct oscap_text *text)
{
	_FREE(lang)
	_FREE(encoding)
	_FREE(text)
	free(text);
}

#define _GET(MTYPE, MNAME)\
		const MTYPE oscap_text_get_##MNAME(const struct oscap_text *text){return text->MNAME;}

_GET(char *, lang)
_GET(char *, encoding)
_GET(wchar_t *, text);

static const size_t _wlen(const wchar_t *text)
{
	size_t len;for(len=0;text[len]!=0;len++);
	return len;
}

size_t oscap_text_get_len(const struct oscap_text *text)
{
	return _wlen(oscap_text_get_text(text));
}

static wchar_t *_clone_wchar(const wchar_t *text)
{
	size_t len = (_wlen(text)+1)*sizeof(wchar_t);
	wchar_t *clone = (wchar_t *)malloc(len);
	memcpy(clone, text, len);
	return clone;
}

struct oscap_text *oscap_text_new(const char *lang, const char *encoding, const wchar_t *text)
{
	struct oscap_text *oscap_text = (oscap_text_t *)malloc(sizeof(oscap_text_t));
	oscap_text->lang     = (lang    )?strdup(lang):NULL;
	oscap_text->encoding = (encoding)?strdup(encoding):NULL;
	oscap_text->text     = (text    )?_clone_wchar(text):NULL;
	return oscap_text;
}

static wchar_t *_convert_UTF_8_to_UTF_16(const char *string)
{
	int slen = strlen(string)+1;
	wchar_t *utf_16 = (wchar_t *)malloc(2*slen);
	wchar_t *tmp_16 = utf_16;

	int i;for(i=0;i<slen;i++){
		char tmp_8[2] = {__LONG_LONG_PAIR((char)0, string[i])};
		*tmp_16 = *((wchar_t *)tmp_8);
		tmp_16++;
	}

	return utf_16;
}

struct oscap_text *oscap_text_from_string(const char *lang, const char *string)
{
	wchar_t *wstring = _convert_UTF_8_to_UTF_16(string);
	struct oscap_text *text = oscap_text_new(lang, "UTF-16", wstring);
	free(wstring);
	return text;
}
