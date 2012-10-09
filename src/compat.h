#ifndef COMPAT_H
#define COMPAT_H

#ifdef _APPLE
	#include <SDL_mixer/SDL_mixer.h>
	#include <SDL_image/SDL_image.h>
#else
	#include <SDL/SDL_mixer.h>
	#include <SDL/SDL_image.h>
#endif

/* 
 * Switch between boost::chrono and c++11 chrono for platforms
 * without a good C++11 implementation
 */

#ifdef _BOOST
	#include <boost/chrono.hpp>
#else
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

#endif /* ndef COMPAT_H */
