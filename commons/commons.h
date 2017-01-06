#ifndef COMMONS_H
#define COMMONS_H


#ifdef __GNUC__
    #ifndef __clang__
        #ifndef __INTEL_COMPILER
            #define FASTOR_GCC
        #endif
    #endif
#endif

#ifdef __INTEL_COMPILER
    #define FASTOR_INTEL
#endif

#ifdef __clang__
    #define FASTOR_CLANG
#endif

#if defined(_MSC_VER)
    #define FASTOR_MSC
#endif

#if defined(_MSC_VER)
    #if _MSC_VER < 1800
       #error FASTOR REQUIRES AN ISO C++11 COMPLIANT COMPILER
    #endif
#elif defined(__GNUC__) || defined(__GNUG__)
    #if __cplusplus <= 199711L
        #error FASTOR REQUIRES AN ISO C++11 COMPLIANT COMPILER
    #endif
#endif

#if defined(__GNUC__) || defined(__GNUG__)
    #define FASTOR_INLINE inline __attribute__((always_inline))
    #define FASTOR_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
    #define FASTOR_INLINE __forceinline
    #define FASTOR_NOINLINE __declspec(noinline)
#endif

#if defined(__GNUC__) || defined(__GNUG__)
    #define FASTOR_ALIGN __attribute__((aligned(0x20)))
#elif defined(_MSC_VER)
    #define FASTOR_ALIGN __declspec(align(32))
#endif

// Define this if hadd seems beneficial
//#define USE_HADD

// ADDITIONAL MACROS DEFINED THROUGHOUT FASTOR
//-----------------------------------------------
// Bounds checking - on by default
#define BOUNDSCHECK
//#define FASTOR_DONT_VECTORISE
//#define FASTOR_DONT_PERFORM_OP_MIN
//#define COPY_SMART_EXPR
#define SHAPE_CHECK
#define DepthFirst -200
#define NoDepthFirst -201
//-----------------------------------------------

#include <cstdlib>
#include <cassert>

#ifdef __SSE2__
#include <emmintrin.h>
#endif
#ifdef __AVX__
#include <immintrin.h>
#endif


// FASTOR CONSTRUCTS
#define Symmetric -100
#define NonSymmetric -101
#define AntiSymmetric -102
#define Identity -103
#define One -104
#define Zero -105
#define Voigt -106

#define ThreeD -150
#define TwoD -151
#define PlaneStrain -152
#define PlaneStress -153

#define SSE 128
#define AVX 256
#define Scalar 64
#define Double 64
#define Single 32



#ifdef __SSE4_2__
#define ZEROPS (_mm_set1_ps(0.f))
#define ZEROPD (_mm_set1_pd(0.0))
// minus/negative version
#define MZEROPS (_mm_set1_ps(-0.f))
#define MZEROPD (_mm_set1_pd(-0.0))
#define ONEPS (_mm_set1_ps(1.f))
#define ONEPD (_mm_set1_pd(1.0))
#define HALFPS (_mm_set1_ps(0.5f))
#define HALFPD (_mm_set1_pd(0.5))
#endif
#ifdef __AVX__
#define VZEROPS (_mm256_set1_ps(0.f))
#define VZEROPD (_mm256_set1_pd(0.0))
// minus/negative version
#define MVZEROPS (_mm256_set1_ps(-0.f))
#define MVZEROPD (_mm256_set1_pd(-0.0))
#define VONEPS (_mm256_set1_ps(1.f))
#define VONEPD (_mm256_set1_pd(1.0))
#define VHALFPS (_mm256_set1_ps(0.5f))
#define VHALFPD (_mm256_set1_pd(0.5))
#endif

using FASTOR_INDEX = size_t;
using Int64 = long long int;
using DEFAULT_FLOAT_TYPE = double;
using DFT = DEFAULT_FLOAT_TYPE;
using FASTOR_VINDEX = volatile size_t;


#define PRECI_TOL 1e-14

void FASTOR_ASSERT(bool cond, const std::string &x) {
    if (cond==true) {
        return;
    }
    else {
        std::cout << x << std::endl;
        exit(EXIT_FAILURE);
    }
}

void FASTOR_WARN(bool cond, const std::string &x) {
    if (cond==true) {
        return;
    }
    else {
        std::cout << x << std::endl;
    }
}

#define _FASTOR_TOSTRING(X) #X
#define FASTOR_TOSTRING(X) _FASTOR_TOSTRING(X)


#if defined(__GNUC__)
    #define DEPRECATE(foo, msg) foo __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
    #define DEPRECATE(foo, msg) __declspec(deprecated(msg)) foo
#else
    #error FASTOR STATIC WARNING DOES NOT SUPPORT THIS COMPILER
#endif

#define PP_CAT(x,y) PP_CAT1(x,y)
#define PP_CAT1(x,y) x##y

namespace useless
{
    struct true_type {};
    struct false_type {};
    template <int test> struct converter : public true_type {};
    template <> struct converter<0> : public false_type {};
}

#define FASTOR_STATIC_WARN(cond, msg) \
struct PP_CAT(static_warning,__LINE__) { \
  DEPRECATE(void _(::useless::false_type const& ),msg) {}; \
  void _(::useless::true_type const& ) {}; \
  PP_CAT(static_warning,__LINE__)() {_(::useless::converter<(cond)>());} \
}



//
#define FASTOR_ISALIGNED(POINTER, BYTE_COUNT) \
    (((uintptr_t)(const void *)(POINTER)) % (BYTE_COUNT) == 0)


#include "extended_algorithms.h"


#endif // COMMONS_H

