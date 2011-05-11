/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * OpenScap CPE URI Module Test Suite Helper
 *
 * Authors:
 *      Ondrej Moris <omoris@redhat.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cpeuri.h>

void print_usage(const char *, FILE *);

struct cpe_name *cpe_example_creation(int, const char *, const char *, const char *,
				      const char *, const char *, const char *);

void cpe_print(struct cpe_name *);

int main(int argc, char **argv)
{
  struct cpe_name *name = NULL, *candidate_cpe = NULL, **cpes = NULL;
  FILE *f;
  int ret_val = 1, i;
  char *cpe_uri = NULL;
  bool match_result_a = false, match_result_b = false, match_result_c = false;

  if (argc == 2 && !strcmp(argv[1], "--help")) {   
    print_usage(argv[0], stdout);
    ret_val = 0;
  }

  // Create a new CPE name from given information and store it to given file.
  else if (argc == 9 && strcmp(argv[1], "--creation") == 0) {    
    
    // Create CPE from given information.
    if ((name = cpe_example_creation(atoi(argv[2]), argv[3], argv[4], argv[5], argv[6], argv[7], argv[8])) != NULL) {	
      
      // Extract URI of created CPE.
      if ((cpe_uri = cpe_name_get_uri(name)) != NULL) {

	// Check if URI is valid.
	if (cpe_name_check(cpe_uri)) {	    
	  
	  // Write CPE URI to the screen.
	  if (cpe_name_write(name, stdout) > 0)
	    ret_val = 0;
	  else 	    
	    fprintf(stderr, "Unable to write CPE to output!\n");
	} else 
	  fprintf(stderr, "CPE URI is not valid!\n");	  
	free(cpe_uri);	
      } else 
	fprintf(stderr, "Unable to get CPE uri!\n");
      cpe_name_free(name);
    } else 
      fprintf(stderr, "Unable to create CPE name!\n");          
  }

  // Parse given CPE URI.
  else if (argc == 4 && strcmp(argv[1], "--parsing") == 0) {    

    // Check if URI is valid.
    if (cpe_name_check(argv[2])) {	    

      // Create CPE from URI string.
      if ((name = cpe_name_new(argv[2])) != NULL) {
	
	// Extract URI from created CPE.
	if ((cpe_uri = cpe_name_get_uri(name)) != NULL) {
      
	  // Compare it to the original URI.
	  if (!strcmp(cpe_uri, argv[2])) {
 
	    // Write CPE to file.
	    if ((f = fopen(argv[3], "w")) == NULL) 
	      fprintf(stderr, "Unable to open or create file %s!\n", argv[3]);
	    else 
	      cpe_name_write(name, f), fclose(f);
	    
	    // Print CPE to the screen.
	    cpe_print(name);	    
	    ret_val = 0;
	  }
	  cpe_name_free(name);
	} else
	  fprintf(stderr, "Not enough memory!\n");
      }
      else 
	fprintf(stderr, "Parse error!\n");
    } else
      fprintf(stderr, "CPE URI is not valid!\n");	  
  }
  
  // Match candidate CPE URI agains the others.
  else if (argc >= 4 && strcmp(argv[1], "--matching") == 0) {

    // Check match directly using CPE URIs.
    match_result_a = cpe_name_match_strs(argv[2], argc - 2, &(argv[3])) >= 0 ? true : false;
    
    // Load candidate CPE.
    if ((candidate_cpe = cpe_name_new(argv[2])) != NULL) {
      if ((cpes = (struct cpe_name **) malloc((argc - 3) * sizeof(struct cpe_name *))) != NULL) {
	for (i = 0; i < argc - 3; i++) {

	  // Load CPE to check match.
	  cpes[i] = cpe_name_new(argv[i+3]);	  

	  // Match check (against last CPE).
	  match_result_b = match_result_b || cpe_name_match_one(candidate_cpe, cpes[i]);
	}

	// Match check agains all loaded CPEs.
	match_result_c = cpe_name_match_cpes(candidate_cpe, argc - 3, cpes);

	// Report result - all should be the same.
	if (match_result_a && match_result_b && match_result_c) 
	  printf("Match\n"), ret_val = 0;
	if (!match_result_a && !match_result_b && !match_result_c) 
	  printf("Mismatch\n"), ret_val = 0;
	
      } else 
	fprintf(stderr, "Not enough memory!\n");
      
      for (i = 0; i < argc - 3; i++) 
	cpe_name_free(cpes[i]);      
      free(cpes);
      cpe_name_free(candidate_cpe);

    } else
      fprintf(stderr, "Parse error!\n");
  }
  
  else if (argc == 2 && strcmp(argv[1], "--smoke-test") == 0) {
    if ((name = cpe_name_new(NULL)) != NULL) {
      cpe_name_free(name);
      ret_val = 0;
    }
  }

  // Print usage only, invalid parameters.
  else 
    print_usage(argv[0], stderr);
  
  return ret_val;
}      

// Print usage.
void print_usage(const char *program_name, FILE *out) 
{
  fprintf(out, 
	  "Usage: \n\n"
	  "  %s --help\n"
	  "  %s --creation PART VENDOR PRODUCT VERSION UPDATE EDITION LANGUAGE\n"
	  "  %s --parsing  URI FILE\n"
	  "  %s --matching CANDIDATE_URI URI_1 ... URI_n\n"
	  "  %s --smoke-test\n",
	  program_name, program_name, program_name, program_name, program_name);
}

// Create a new CPE according to given information.
struct cpe_name *cpe_example_creation(int part, 
				      const char *vendor,
				      const char *product,
				      const char *version,
				      const char *update,
				      const char *edition,
				      const char *language)
{
  struct cpe_name *cpe = NULL;
  
  if ((cpe = cpe_name_new(NULL)) != NULL) {
    
    // Set fields.
    cpe_name_set_part(cpe, part);
    cpe_name_set_vendor (cpe, vendor);
    cpe_name_set_product(cpe, product);
    cpe_name_set_version(cpe, version);
    cpe_name_set_update(cpe, update);
    cpe_name_set_edition(cpe, edition);
    cpe_name_set_language(cpe, language);
  }

  return cpe;
}

// Parse CPE URI string and print all contained information.
void cpe_print(struct cpe_name *name)
{

  // Print out all contained information.
  printf(" %d\n", cpe_name_get_part(name));
  printf(" %s\n", cpe_name_get_vendor(name) ? cpe_name_get_vendor(name) : "");
  printf(" %s\n", cpe_name_get_product(name) ? cpe_name_get_product(name) : "");
  printf(" %s\n", cpe_name_get_version(name) ? cpe_name_get_version(name) : "");
  printf(" %s\n", cpe_name_get_update(name) ? cpe_name_get_update(name) : "");
  printf(" %s\n", cpe_name_get_edition(name) ? cpe_name_get_edition(name) : "");
  printf(" %s\n", cpe_name_get_language(name) ? cpe_name_get_language(name) : "");
}
