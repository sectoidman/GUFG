#ifdef _BOOST
#include <boost/chrono.hpp>
#else
//other platforms can use the C++11 standard library
#include <chrono>
#endif


namespace internal {
	#ifdef _BOOST
		using namespace boost::chrono;
		using namespace boost;
	#else
		using namespace std::chrono;
		using namespace std;
	#endif
}

