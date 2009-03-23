/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "cce_priv.h"


bool cce_validate(const char *filename) {
    xmlParserCtxtPtr ctxt; /* the parser context */
    xmlDocPtr doc; /* the resulting document tree */
	bool ret = false;
    /* create a parser context */
    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL)
		return false;
    /* parse the file, activating the DTD validation option */
    doc = xmlCtxtReadFile(ctxt, filename, NULL, XML_PARSE_DTDATTR);
    /* check if parsing suceeded */
    if (doc == NULL) {
		xmlFreeParserCtxt(ctxt);
        return false;
    }
	/* check if validation suceeded */
	if (ctxt->valid)
		ret = true;
	xmlFreeDoc(doc);
    /* free up the parser context */
    xmlFreeParserCtxt(ctxt);
	return ret;

}
