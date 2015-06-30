/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2015 André Müller
 *
 *****************************************************************************/

//#include "angle.h"
//#include "biquaternion.h"
//#include "bounded_angles.h"
//#include "bounded.h"
//#include "choice.h"
//#include "constant.h"
//#include "constants.h"
//#include "conversion.h"
//#include "dual_quaternion.h"
//#include "dual.h"
//#include "equality.h"
//#include "interval.h"
//#include "limits.h"
//#include "narrowing.h"
//#include "natural_interval.h"
//#include "natural.h"
//#include "param.h"
//#include "quaternion.h"
//#include "rational.h"
//#include "rounded.h"
//#include "scomplex.h"
//#include "sequ_combined.h"
//#include "sequ_fibonacci.h"
//#include "sequ_geometric.h"
//#include "sequ_linear.h"
//#include "sequ_repeated.h"
//#include "split_biquaternion.h"
//#include "traits.h"


#ifdef AM_USE_TESTS
    void numeric_correctness();
#endif

int main()
{
    #ifdef AM_USE_TESTS
        numeric_correctness();
    #endif
}
