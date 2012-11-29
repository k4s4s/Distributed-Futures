#ifndef DEBUG
	#define DPRINT_VAR(m, x)
	#define DPRINT_MESSAGE(x)
#else
	#define DPRINT_VAR(m, x) \
	std::cout << m << #x << ":" << x << std::endl;
#define DPRINT_MESSAGE(x) \
	std::cout << x << std::endl;
#endif
