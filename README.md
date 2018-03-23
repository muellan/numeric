AM numeric
==========

Header-only numeric facilities for C++11.


## Quick Overview

### Generic classes
  - safe angle (that is tagged with its unit)
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
  - random number distribution adapter
  
### Other
  - number conversion factories
  - number concept checking
  - checking for narrowing
  - some tests (not very thorough at the moment)


## Requirements
  - requires C++11 conforming compiler
  - tested with g++ 4.7.2, 4.9.2, 5.1.0, 5.3.1, 7.2 and clang++ 5.0.1
