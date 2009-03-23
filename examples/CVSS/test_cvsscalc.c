
#include "config.h"

#include <stdio.h>
#include <cvsscalc.h>

int main()
{
	double bs, is, es, ts, envs;

	baseScore(AV_LOCAL, AC_HIGH, AU_NONE, CI_PARTIAL, II_NONE, AI_NONE, &bs, &is, &es);
	printf("bs: %f, is: %f, es: %f\n", bs, is, es); 

	tempScore(EX_UNPROVEN, RL_WORKAROUND, RC_CONFIRMED, bs, &ts);
	printf("ts: %f\n", ts); 

	envScore(CD_LOW, TD_LOW, CR_LOW, IR_LOW, AR_LOW, &envs,
		 AV_LOCAL, AC_HIGH, AU_NONE, CI_PARTIAL, II_NONE, AI_NONE,
		 EX_UNPROVEN, RL_WORKAROUND, RC_CONFIRMED);
	printf("envs: %f\n", envs);

	return 0;
}
