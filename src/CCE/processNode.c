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


#include "cce.h"
void processNode(xmlTextReaderPtr reader, struct CCE* cce, char* id )
{
	xmlChar* name;
	xmlChar* value;

	if (!xmlStrcmp(xmlTextReaderName(reader), (const xmlChar*)"item") && 
		xmlTextReaderNodeType(reader) == 1)
	{
		if (!xmlStrcmp(xmlTextReaderGetAttribute(reader, (const xmlChar*)"id"), (const xmlChar*) id)) // if id matches CCE id
		{

			cce->id = (char*) xmlTextReaderGetAttribute(reader, (const xmlChar*)"id");
			xmlTextReaderRead(reader);
			xmlChar* name = xmlTextReaderName(reader);
			int type = xmlTextReaderNodeType(reader);
			while(xmlStrcmp(xmlTextReaderName(reader), (const xmlChar*)"item"))
			{
				if (!xmlStrcmp(xmlTextReaderName(reader), (const xmlChar*)"description") &&
					xmlTextReaderNodeType(reader) == 1)
				{
					processDescription(reader, cce);
				}
				else if (!xmlStrcmp(xmlTextReaderName(reader), (const xmlChar*)"parameter") &&
						 xmlTextReaderNodeType(reader) == 1)
				{
					processParameter(reader, cce);
				}
				else if (!xmlStrcmp(xmlTextReaderName(reader), (const xmlChar*)"technicalmechanisms") &&
						 xmlTextReaderNodeType(reader) == 1)
				{
					processTechMech(reader, cce);
				}
				else if (!xmlStrcmp(xmlTextReaderName(reader), (const xmlChar*)"ref") &&
						 xmlTextReaderNodeType(reader) == 1)
				{
					processRefs(reader, cce);
				}
				xmlTextReaderRead(reader);
				name = xmlTextReaderName(reader);
				type = xmlTextReaderNodeType(reader);
			}
		}
	}
}
