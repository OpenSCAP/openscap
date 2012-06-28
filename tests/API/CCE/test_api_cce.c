
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <cce.h>

void print_usage(char *, FILE *);
void print_entry(struct cce_entry *);

int main(int argc, char *argv[])
{
  int ret_val = 0;
  struct cce *cce = NULL;
  struct cce_entry *entry = NULL;
  struct cce_entry_iterator *entry_it = NULL;

  if (argc == 1) {
    print_usage(argv[0], stdout);
    ret_val = 0;
  }

  else if (argc == 3 && !strcmp(argv[1], "--parse")) {

    if (!cce_validate(argv[2])) {
      fprintf(stderr, "Not valid CCE!\n");
      ret_val = 1;
    } else {

      if ((cce = cce_new(argv[2])) == NULL) {
	fprintf(stderr, "Unable to create new CCE!\n");
	ret_val = 1;
      } else {

	entry_it = cce_get_entries(cce);
	while (cce_entry_iterator_has_more(entry_it)) {
	  entry = cce_entry_iterator_next(entry_it);
	  print_entry(entry);	    
	}
	cce_entry_iterator_free(entry_it);

	cce_free(cce);
	oscap_cleanup();

	ret_val = 0;
      }
    }
  }

  else if (argc == 4 && !strcmp(argv[1], "--search")) {

    if (!cce_validate(argv[2])) {
      fprintf(stderr, "Not valid CCE!\n");
      ret_val = 1;
    } else {

      if ((cce = cce_new(argv[2])) == NULL) {
	fprintf(stderr, "Unable to create new CCE!\n");
	ret_val = 1;
      } else {

	if ((entry = cce_get_entry(cce, argv[3])) == NULL) {
	  fprintf(stderr, "Not found!\n");
	  ret_val = 1;
	} else {	  
	  print_entry(entry);
	  ret_val = 0;
	}

	cce_free(cce);	
	oscap_cleanup();

      }
    }
  }
  
  else if (argc == 3 && !strcmp(argv[1], "--validate")) {

    if (!cce_validate(argv[2])) {
      fprintf(stderr, "Not valid!\n");
      ret_val = 1;
    }

  }

  else if (argc == 2 && !strcmp(argv[1], "--smoke-test")) {

    if ((cce = cce_new(NULL)) == NULL) {
      fprintf(stderr, "Unable to create new CCE!\n");
      ret_val = 1;      
    } else {
      cce_free(cce);
    }

  }
  
  else {
    print_usage(argv[0], stderr);
  }

  return ret_val;


  }
  
void print_usage(char *program_name, FILE *out) 
{
  fprintf(out, 
	  "Usage:\n\n"
	  "%s --parse    file.xml\n"
	  "%s --search   file.xml CCE-xxxx-x\n"
	  "%s --validate file.xml\n"
	  "%s --smoke-test",
	  program_name, program_name, program_name, program_name);
}

void print_entry(struct cce_entry *entry) 
{
  struct cce_reference_iterator *reference_it = NULL;
  struct cce_reference *reference = NULL;
  struct oscap_string_iterator *tech_mech_it = NULL;
  struct oscap_string_iterator *param_it = NULL;
  const char *tech_mech = NULL, *param = NULL;

  printf("ID: %s\n", cce_entry_get_id(entry));
  printf("Description: %s\n", cce_entry_get_description(entry));

  reference_it = cce_entry_get_references(entry);
  while (cce_reference_iterator_has_more(reference_it)) {
    reference = cce_reference_iterator_next(reference_it);
    printf("Reference Source: %s\n", cce_reference_get_source(reference));
    printf("Reference Value: %s\n", cce_reference_get_value(reference));
  }
  cce_reference_iterator_free(reference_it);

  tech_mech_it = cce_entry_get_tech_mechs(entry);
  while (oscap_string_iterator_has_more(tech_mech_it)) {
    tech_mech = oscap_string_iterator_next(tech_mech_it);
    printf("Technical Mechanism: %s\n", (char *) tech_mech);
  }
  oscap_string_iterator_free(tech_mech_it);

  param_it = cce_entry_get_params(entry);
  while (oscap_string_iterator_has_more(param_it)) {
    param = oscap_string_iterator_next(param_it);
    printf("Available Parameter Choices: %s\n", param);
  }
  oscap_string_iterator_free(param_it);
  putchar('\n');
}
