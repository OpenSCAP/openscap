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


#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include "list_cstring.h"

void list_cstring_clear(struct list_cstring* list)
{
	struct list_cstring* current = list;
	struct list_cstring* next = current;
	do
	{
		current = next;
		if (current->value) xmlFree(current->value);
		next = current->next;
		free(current);
	} while(NULL != next);
	return;
}
