#include <stdio.h>
#include <cce.h>

int main(int argc, char *argv[])
{
	printf("CCE XML Parsing Program:\n");
	struct cce cce;
	cce_init(&cce);

	if (argc != 3) {
		printf
		    ("Invalid Syntax Usage:\n Syntax: ./cce_parse filen.xml CCE-xxxx-x\n");
		return 1;
	}

	char *docname = argv[1];	/* The CCE XML file to be parsed. (searched) */
	char *id = argv[2];	/* 2nd Argument is the CCE ID to be searched for. */

	printf("Searching for %s\n\n", id);

	if (!cce_validate(docname)) {
		printf("File '%s' does not seem to be a valid CCE\n", docname);
		return 1;
	}
	cce_parse(docname, &cce, id);

	/* Example of struct data returned. */
	printf("\nID: %s\n", cce.id);
	printf("Description: %s\n", cce.description);

	{
		struct list_refs *current = cce.references;
		while (current) {
			printf("Ref Source: %s\n", cce.references->source);
			printf("Ref Value: %s\n", cce.references->value);
			current = current->next;
		}
	}

	{
		struct list_cstring *current = cce.technicalmechanisms;
		while (current) {
			printf("Technical Mech: %s\n", current->value);
			current = current->next;
		}

		current = cce.parameters;
		while (current) {
			printf("Available Parameter Choices: %s\n",
			       current->value);
			current = current->next;

		}
	}

	cce_clear(&cce);	/* cce is a struct that contains all the data that the ID string contained. */

	return 0;
}
