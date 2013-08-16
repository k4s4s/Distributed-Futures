include_directories(/opt/intel/mkl/include)

add_executable(lu kernels.cpp lu.cpp)

target_link_libraries (lu ${EXTRA_LIBS}
													-L/opt/intel/mkl/lib/intel64
    											-L/opt/intel/lib/intel64
-lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lpthread)

