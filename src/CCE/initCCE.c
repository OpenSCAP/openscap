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

void initCCE(struct CCE* cce)
{
	cce->id = NULL;
	cce->description = NULL;
	cce->parameters = (struct list_cstring*)malloc(sizeof(struct list_cstring));
	cce->technicalmechanisms = (struct list_cstring*)malloc(sizeof(struct list_cstring));
	cce->parameters->next = NULL;
	cce->parameters->value = NULL;
	cce->technicalmechanisms->next = NULL;
	cce->technicalmechanisms->value = NULL;
	cce->references = (struct list_Refs*)malloc(sizeof(struct list_Refs));
	cce->references->next = NULL;
	cce->references->value = NULL;
	cce->references->source = NULL;
	return;
}
