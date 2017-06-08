# pcg.h
Single header, 100 line implementation of 32bit [PCG random number generator](http://www.pcg-random.org/).

**Advantages**

- Blazingly fast generation of uint32 and 32bit floats in [0,1)

- Each rng class instance only takes 128bits of memory (two uint64)

**Disadvantages**

Doesn't conform to full C++11 rng engine requirements. It's a very simple rng that produces output fast.

# API

Constructor

- Based on nanosecond clock and thread id
    
- Should guarantee different rng values in most usecases
 - You can also manually seed by providing two uint64_t
 - The first being the seed, second the sequence id

Generating rng values
   
- next() generates a uniformly distributed uint32_t

- next_bounded(uint32_t) generates a bounded uint32_t (exclusive)

- next_float() generates a float in [0,1)
    
Requires C++11 for default seeding. Specifically <thread> and <chrono>.
  
# Example

    #include "pcg.h"
    
    int main() {
        // Create new instance of RNG
        // Default seed is thread safe and nanosecond-time based
        pcg::pcg32 rng;

        for (int i = 0; i < 99; ++i) {
            std::cout << rng.next() << std::endl;
            std::cout << rng.next_float() << std::endl;
            std::cout << rng.next_bounded(101) << std::endl;
        }

        return 0;
     }
      
