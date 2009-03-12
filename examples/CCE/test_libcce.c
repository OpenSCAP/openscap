/* 
 * File:   test_libcce.c
 * Author: riley.porter@g2-inc.com
 * This program is an example of how to use the OpenScap libCCE
 * The libCCE takes an CCE xml filename followed by a CCE ID number
 * that you wish to get information on.
 *
 * The return type is the struct CCE.. Illustrated below in the print statements.
 *
 * Created on March 3, 2009, 10:29 AM
 *
 * This program is free software: you can redistribute it and/or modify
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


#include <stdio.h>
#include <stdlib.h>
#include <cce.h>
#include <list_Refs.h>
#include <list_xmlChar.h>



/*
 * 
 */
int main(int argc, char** argv)
{
    char* docname = argv[1]; //The CCE XML file to be parsed. (searched)
    char* id = argv[2]; //2nd Argument is the CCE ID to be searched for.
    struct CCE cce;
    initCCE(&cce);
    printf("Searching for %s\n\n",id);

        validateFile(docname);
        parseDoc (docname, &cce, id);

        //Example of struct data returned.
        printf("\nID: %s\n",cce.id);
        printf("Description: %s\n",cce.description);

        struct list_XMLCHAR* current = cce.references;
        while(current)
            {
                printf("Ref Source: %s\n",cce.references->source);
                printf("Ref Value: %s\n",cce.references->value);
                current = current->next;
            }

        current = cce.technicalmechanisms;
            while(current)
            {
                printf("Technical Mech: %s\n",current->value);
                current = current->next;
            }

        current = cce.parameters;
            while(current)
            {
                printf("Available Parameter Choices: %s\n",current->value);
                current = current->next;

            }

	clearCCE(&cce);
    return (EXIT_SUCCESS);
}

