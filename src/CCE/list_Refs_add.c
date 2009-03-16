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


#include "list_Refs.h"

void list_Refs_add(struct list_Refs* list, char* source, char* value)
{
	if (!list->value)
	{
		list->value = value;
		list->source = source;
		return;
	}
	struct list_Refs* current = list;
	while(current->next) current = current->next;
	struct list_Refs* newitem = (struct list_Refs*)malloc(sizeof(struct list_Refs));
	newitem->value = value;
	newitem->source = source;
	newitem->next = NULL;
	current->next = newitem;
	return;
}
