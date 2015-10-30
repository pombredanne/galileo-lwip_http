
/*VL: for Viper*/

#ifdef PERF
#define PERF_START  do {
/*keep the ;*/
#define PERF_STOP(x)  ;} while(0)

#else /* PERF */
#define PERF_START    /* null definition */
#define PERF_STOP(x)  /* null definition */
#endif /* PERF */

