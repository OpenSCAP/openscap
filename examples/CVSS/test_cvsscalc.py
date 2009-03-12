#! /usr/bin/python


# set up path for: 
# "_cvsscalc.so" 
# "cvsscalc.py"
import sys
sys.path.append("../swig")
sys.path.append("../swig/.libs")

import cvsscalc

BS = cvsscalc.new_doublep()
IS = cvsscalc.new_doublep()
ES = cvsscalc.new_doublep()
TS = cvsscalc.new_doublep()
ENVS = cvsscalc.new_doublep()

cvsscalc.baseScore(cvsscalc.AV_LOCAL, cvsscalc.AC_HIGH, cvsscalc.AU_NONE, 
                   cvsscalc.CI_PARTIAL, cvsscalc.II_NONE, cvsscalc.AI_NONE,
                    BS, IS, ES);

print "BS: ", cvsscalc.doublep_value(BS), "IS: ", cvsscalc.doublep_value(IS), "ES: ", cvsscalc.doublep_value(ES)

cvsscalc.tempScore(cvsscalc.EX_UNPROVEN, 
                   cvsscalc.RL_WORKAROUND, 
                   cvsscalc.RC_CONFIRMED, 
                   cvsscalc.doublep_value(BS), TS);
print "TS: ", cvsscalc.doublep_value(TS)
 
cvsscalc.envScore(cvsscalc.CD_LOW,      cvsscalc.TD_LOW,        cvsscalc.CR_LOW, 
                  cvsscalc.IR_LOW,      cvsscalc.AR_LOW,        ENVS,
                  cvsscalc.AV_LOCAL,    cvsscalc.AC_HIGH,       cvsscalc.AU_NONE, 
                  cvsscalc.CI_PARTIAL,  cvsscalc.II_NONE,       cvsscalc.AI_NONE,
                  cvsscalc.EX_UNPROVEN, cvsscalc.RL_WORKAROUND, cvsscalc.RC_CONFIRMED);

print "ENVS: ", cvsscalc.doublep_value(ENVS)

