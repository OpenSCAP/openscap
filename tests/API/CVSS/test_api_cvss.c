
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <cvss_score.h>

static void print_score(float s)
{
    if (isnan(s)) printf("/-");
    else printf("/%.1f", s);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s cvss_vector\n", argv[0]);
        return -1;
    }

    struct cvss_impact *imp = cvss_impact_new_from_vector(argv[1]);

    if (imp == NULL) {
        printf("NULL");
        return 0;
    }

    print_score(cvss_impact_base_score(imp));
    print_score(cvss_impact_temporal_score(imp));
    print_score(cvss_impact_environmental_score(imp));
    printf("/\n");

    return 0;
}
