#ifdef _WIN32

/*
  Nobody's written a decent free C++11 thread library implementation yet,
  so we'll have to just use boost in the mean time.  
*/
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

#else

//other platforms can use the C++11 standard library
#include <thread>
#include <chrono>

#endif

namespace internal {
	#ifdef _WIN32
		using namespace boost::chrono;
		using namespace boost;
	#else
		using namespace std;
	#endif
}

