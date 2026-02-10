#pragma once

////////////////
// Run Results /
//////////////////////
// tracks run score //
//////////////////////
struct RunResults
{
    int perfect = 0;
    int good    = 0;
    int late    = 0;
    int missed  = 0;
};

extern RunResults run_results;
extern RunResults run_results_p2;
