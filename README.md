AM numeric
==========

Header-only numeric facilities for C++14.


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


## Requirements
  - requires C++14 conforming compiler
  - tested with g++ {5.4, 7.2} and clang++ 5.0.2
