# pcg.h
Single header, 100 line implementation of 32bit [PCG random number generator](http://www.pcg-random.org/).


# API

Constructor

- Based on nanosecond clock and thread id
    
- Should guarantee different rng values in most usecases
 - You can also manually seed by providing two uint64_t
 - The first being the seed, second the sequence id

Generating rng values
   
- next() generates a uniformly distributed uint32_t

- next_bounded(uint32_t) generates a bounded uint32_t (exclusive)

- next_double() generates a double in [0,1)
    
Requires C++11 for default seeding. Specifically <thread> and <chrono>.
  
# Example

    #include "pcg.h"
    
    int main() {
        // Create new instance of RNG
        // Default seed is thread safe and nanosecond-time based
        pcg::pcg32 rng;

        for (int i = 0; i < 99; ++i) {
            std::cout << rng.next() << std::endl;
            std::cout << rng.next_double() << std::endl;
            std::cout << rng.next_bounded(101) << std::endl;
        }

        return 0;
     }
      
