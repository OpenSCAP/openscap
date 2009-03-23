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

void cce_parse(char *docname,  struct CCE* cce, char* id)
{
    xmlTextReaderPtr reader;
    xmlDocPtr doc;
    int ret;
    doc = xmlParseFile(docname);
    reader = xmlNewTextReaderFilename(docname);
    if (reader != NULL)
    {
        ret = xmlTextReaderRead(reader);
        while(ret == 1)
        {
            processNode(reader, cce, id);
            ret = xmlTextReaderRead(reader);

        }
    }
}
