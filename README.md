AM numeric
==========

Collection of numeric facilities for C++

**Note**: All classes and functions are work in progress.
Interfaces are still likely to change over time and documentation is limited to some comments so far. 

All components are header-only.


## Generic classes
  - safe angle 
  - choice (provides arithmetic modulo N)
  - interval (incl. interval arithmetic)
  - natural number adapter (provides unsigned integer with bounds check and infinity type)
  - bounded number adapter (+ aliases for clipped and wrapped numbers)
  - rounded number adapter 
  - rational number
  - dual number
  - split-complex number
  - quaternion  
  - ordinary biquaternion
  - split-biquaternion
  - dual quaternion (study biquaternion)  
  - sequences (linear, geometric, repeated, fibonacci)
  
## Other
  - number conversion factories
  - number concept checking
  - checking for narrowing
  - some tests (not very thorough at the moment)


## Requirements
  - requires (mostly) C++11 conforming compiler
  - tested with g++ 4.7.2, g++ 4.9.2, g++ 5.1.0
