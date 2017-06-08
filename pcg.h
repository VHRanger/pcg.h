#pragma once
#include <cstdint>
#include <cmath>
#include <chrono> // high resolution clock for default seed
#include <thread> // for default constructor to differ by thread

namespace pcg {
	using std::uint64_t;
	using std::uint32_t;
//
// PCG32 basic reworked in a class
// 	
//	API:
//		Constructor
//			Based on nanosecond clock and thread id
//			Should guarantee different rng values in most usecases
//			--You can also manually seed by providing two uint64_t
//			--The first being the seed, second the sequence id
//
//		Generating rng values
//			next() generates a uniformly distributed uint32_t
//			next_bounded(uint32_t) generates a bounded uint32_t
//			next_double() generates a double in [0,1)
class pcg32 {
	// RNG state.  All values are possible.
	uint64_t state;
	// Controls which RNG sequence (stream) is
	// selected. Must *always* be odd.
	uint64_t inc;     
public:
	void seed(
		uint64_t initstate, 
		uint64_t initseq)
	{
		this->state = 0U;
		this->inc = (initseq << 1U) | 1U;
		next();
		this->state += initstate;
		next();
	}
		

	pcg32() 
	{
		// high resolution clock to avoid same seed on 
		uint64_t sd = std::chrono::high_resolution_clock::
			now().time_since_epoch().count();
		// hash function to get uint from thread id
		std::hash<std::thread::id> pcg_hasher;
		// seed on high res clock and thread id
		seed((uint64_t) sd, 
			 2 * (uint64_t) pcg_hasher(std::this_thread::get_id()) + 1 );
	}


	pcg32(uint64_t sd, uint64_t sequence)
	{
		seed(sd, sequence);
	}


	//Generate a uniformly distributed 32-bit random number
#pragma warning (push)
#pragma warning (disable : 4146) //pragmas disable error on intended "unsigned out of range" behavior
#pragma warning (disable : 4244) 
	uint32_t next()
	{
		uint64_t oldstate = this->state;
		this->state = oldstate * 6364136223846793005ULL + this->inc;
		uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
		uint32_t rot = oldstate >> 59u;
		return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
	}
#pragma warning (pop)
		
	//     Generate a uniformly distributed number, r, where 0 <= r < bound
	uint32_t next_bounded(uint32_t bound)
	{
		// To avoid bias, we need to make the range of the RNG a multiple of
		// bound, which we do by dropping output less than a threshold.
		// A naive scheme to calculate the threshold would be to do
		//
		//     uint32_t threshold = 0x100000000ull % bound;
		//
		// but 64-bit div/mod is slower than 32-bit div/mod (especially on
		// 32-bit platforms).  In essence, we do
		//
		//     uint32_t threshold = (0x100000000ull-bound) % bound;
		//
		// because this version will calculate the same modulus, but the LHS
		// value is less than 2^32.

		uint32_t threshold = -bound % bound;

		// Uniformity guarantees that this loop will terminate.  In practice, it
		// should usually terminate quickly; on average (assuming all bounds are
		// equally likely), 82.25% of the time, we can expect it to require just
		// one iteration.  In the worst case, someone passes a bound of 2^31 + 1
		// (i.e., 2147483649), which invalidates almost 50% of the range.  In 
		// practice, bounds are typically small and only a tiny amount of the range
		// is eliminated.
		for (;;) {
			uint32_t r = next();
			if (r >= threshold)
				return r % bound;
		}
	}
		
		
	// Generate next number as a double in [0,1)
	// Precise to 1/2^32
	double next_double()
	{
		return std::ldexp(next(), -32);
	}	


	// Different version of next_double
	double next_double2()
	{
		return (next() >> 11) * (1. / (UINT64_C(1) << 53));
	}

};
}
