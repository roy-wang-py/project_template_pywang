#ifndef __LOG_WRAPPER_HXX__
#define __LOG_WRAPPER_HXX__
#include "easylogging++.h"

#define LOG_USR_IF(l,pre) LOG_IF(pre,l)
#define LOG_USR_DEBUG() LOG_D()
#define OUT_NAME_VAL(X) "\t"<<#X <<" = "<< (X)
#define OUT_BRIEF_STR_VAL(S,X) (S)<<" : "<< (X)<<"\t"

#define LOG_T() LOG(TRACE)
#define LOG_D() LOG(DEBUG)
#define LOG_I() LOG(INFO)
#define LOG_W() LOG(WARNING)
#define LOG_E() LOG(ERROR)
#define LOG_F() LOG(FATAL)
#define LOG_A() LOG(ERROR)
#define LOG_T_N(x) LOG_EVERY_N((x),TRACE)
#define LOG_D_N(x) LOG_EVERY_N((x),DEBUG)
#define LOG_I_N(x) LOG_EVERY_N((x),INFO)
#define LOG_W_N(x) LOG_EVERY_N((x),WARNING)
#define LOG_E_N(x) LOG_EVERY_N((x),ERROR)
#define LOG_F_N(x) LOG_EVERY_N((x),FATAL)

#endif //__LOG_WRAPPER_HXX__.hxx