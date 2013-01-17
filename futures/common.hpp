#include "stats/stats.hpp"

#ifndef DEBUG
#define DPRINT_VAR(m, x)
#define DPRINT_MESSAGE(x)
#else
#define DPRINT_VAR(m, x) \
	std::cout << m << #x << ":" << x << std::endl;
#define DPRINT_MESSAGE(x) \
	std::cout << x << std::endl;
#endif

#ifndef STATS

#define INIT_STATS
#define START_TIMER(x)
#define STOP_TIMER(x)
#define PRINT_STATS(x)
#define FIN_STATS

#else

#define INIT_STATS
#define START_TIMER(x)
#define STOP_TIMER(x)
#define PRINT_STATS(x)
#define FIN_STATS

#endif
