#ifndef SINGLEVALUE_TENSOR_H
#define SINGLEVALUE_TENSOR_H

#include <tensor/Tensor.h>
#include <tensor/TensorIO.h>
#include <meta/tensor_meta.h>
#include <meta/tensor_post_meta.h>

namespace Fastor {

namespace internal {
template<typename T, typename U, size_t N>
constexpr std::array<T,N>  _filll_one_value(std::array<T,N> seq, U val) {
    for (int i=0; i<N; ++i) seq[i]=val;
    return seq;
}
}


template<typename T, size_t ...Rest>
class SingleValueTensor : public AbstractTensor<SingleValueTensor<T,Rest...>,sizeof...(Rest)> {
    // const FASTOR_ALIGN std::array<T,prod<Rest...>::value> _data;
    const FASTOR_ALIGN std::array<T,1> _data;
public:
    typedef T scalar_type;
    using Dimension_t = std::integral_constant<FASTOR_INDEX, sizeof...(Rest)>;
    static constexpr FASTOR_INDEX Dimension = sizeof...(Rest);
    static constexpr FASTOR_INDEX Size = prod<Rest...>::value;
    static constexpr FASTOR_INDEX Stride = stride_finder<T>::value;
    static constexpr FASTOR_INDEX Remainder = prod<Rest...>::value % sizeof(T);
    static constexpr FASTOR_INLINE FASTOR_INDEX rank() {return Dimension;}
    static constexpr FASTOR_INLINE FASTOR_INDEX size() {return Size;}
    FASTOR_INLINE FASTOR_INDEX dimension(FASTOR_INDEX dim) const {
#ifndef NDEBUG
        FASTOR_ASSERT(dim>=0 && dim < sizeof...(Rest), "TENSOR SHAPE MISMATCH");
#endif
        const FASTOR_INDEX DimensionHolder[sizeof...(Rest)] = {Rest...};
        return DimensionHolder[dim];
    }

    template<typename U=int>
    SingleValueTensor(U num) : _data{internal::_filll_one_value(_data,num)} {}
    SingleValueTensor(const SingleValueTensor<T,Rest...> &a) : _data{internal::_filll_one_value(_data,a.data()[0])} {}
    FASTOR_INLINE SingleValueTensor(const AbstractTensor<SingleValueTensor<T,Rest...>,sizeof...(Rest)>& src_)
    : _data{internal::_filll_one_value(_data,0)} {
    }

    constexpr FASTOR_INLINE T* data() const { return const_cast<T*>(this->_data.data());}
    // constexpr FASTOR_INLINE T* data() const { return const_cast<T*>(&this->_data);}


    // Scalar indexing const
    //----------------------------------------------------------------------------------------------------------//
// #undef SCALAR_INDEXING_CONST_H
//     #include <tensor/ScalarIndexing.h>
// #define SCALAR_INDEXING_CONST_H
    //----------------------------------------------------------------------------------------------------------//

    //----------------------------------------------------------------------------------------------------------//
    template<typename... Args, typename std::enable_if<sizeof...(Args)==1
                            && sizeof...(Args)==Dimension_t::value && is_arithmetic_pack<Args...>::value,bool>::type =0>
    FASTOR_INLINE const T& operator()(Args ... args) const {
#ifdef BOUNDSCHECK
        constexpr int M = get_value<1,Rest...>::value;
        const int i = get_index<0>(args...) < 0 ? M + get_index<0>(args...) : get_index<0>(args...);
        assert( ( (i>=0 && i<M)) && "INDEX OUT OF BOUNDS");
#endif
        return _data[0];
    }

    template<typename... Args, typename std::enable_if<sizeof...(Args)==2
                            && sizeof...(Args)==Dimension_t::value && is_arithmetic_pack<Args...>::value,bool>::type =0>
    FASTOR_INLINE const T& operator()(Args ... args) const {
#ifdef BOUNDSCHECK
        constexpr int M = get_value<1,Rest...>::value;
        constexpr int N = get_value<2,Rest...>::value;
        const int i = get_index<0>(args...) < 0 ? M + get_index<0>(args...) : get_index<0>(args...);
        const int j = get_index<1>(args...) < 0 ? N + get_index<1>(args...) : get_index<1>(args...);
        assert( ( (i>=0 && i<M) && (j>=0 && j<N)) && "INDEX OUT OF BOUNDS");
#endif
        return _data[0];
    }

    template<typename... Args, typename std::enable_if<sizeof...(Args)>=3
                            && sizeof...(Args)==Dimension_t::value && is_arithmetic_pack<Args...>::value,bool>::type =0>
    FASTOR_INLINE const T& operator()(Args ... args) const {
#ifdef BOUNDSCHECK
        int largs[sizeof...(Args)] = {args...};
        constexpr int DimensionHolder[Dimension] = {Rest...};
        for (int i=0; i<Dimension; ++i) {
            if (largs[i]==-1) largs[i] += DimensionHolder[i];
            assert( (largs[i]>=0 && largs[i]<DimensionHolder[i]) && "INDEX OUT OF BOUNDS");
        }
#endif
        return _data[0];
    }

    // Expression templates evaluators
    //----------------------------------------------------------------------------------------------------------//
// #undef TENSOR_EVALUATOR_H
//     #include "tensor/TensorEvaluator.h"
// #define TENSOR_EVALUATOR_H

    // Expression templates evaluators
    //----------------------------------------------------------------------------------------------------------//
    template<typename U=T>
    FASTOR_INLINE SIMDVector<T,DEFAULT_ABI> eval(FASTOR_INDEX i) const {
#ifdef BOUNDSCHECK
        // This is a generic evaluator and not for 1D cases only
        FASTOR_ASSERT((i>=0 && i<Size), "INDEX OUT OF BOUNDS");
#endif
        return SIMDVector<T,DEFAULT_ABI>(_data[0]);
    }
    template<typename U=T>
    FASTOR_INLINE T eval_s(FASTOR_INDEX i) const {
    #ifdef BOUNDSCHECK
        // This is a generic evaluator and not for 1D cases only
        FASTOR_ASSERT((i>=0 && i<Size), "INDEX OUT OF BOUNDS");
    #endif
        return _data[0];
    }
    template<typename U=T>
    FASTOR_INLINE SIMDVector<T,DEFAULT_ABI> eval(FASTOR_INDEX i, FASTOR_INDEX j) const {
        static_assert(Dimension==2,"INDEXING TENSOR WITH INCORRECT NUMBER OF ARGUMENTS");
        constexpr int N = get_value<2,Rest...>::value;
    #ifdef BOUNDSCHECK
        constexpr int M = get_value<1,Rest...>::value;
        FASTOR_ASSERT((i>=0 && i<M && j>=0 && j<N), "INDEX OUT OF BOUNDS");
    #endif
        return SIMDVector<T,DEFAULT_ABI>(_data[0]);
    }
    template<typename U=T>
    FASTOR_INLINE T eval_s(FASTOR_INDEX i, FASTOR_INDEX j) const {
        static_assert(Dimension==2,"INDEXING TENSOR WITH INCORRECT NUMBER OF ARGUMENTS");
    #ifdef BOUNDSCHECK
        constexpr int M = get_value<1,Rest...>::value;
        constexpr int N = get_value<2,Rest...>::value;
        FASTOR_ASSERT((i>=0 && i<M && j>=0 && j<N), "INDEX OUT OF BOUNDS");
    #endif
        return _data[0];
    }

    // This is purely for smart ops
    constexpr FASTOR_INLINE T eval(T i, T j) const {
        return _data[0];
    }
   // ----------------------------------------------------------------------------------------------------------//

    //----------------------------------------------------------------------------------------------------------//

    // Tensor methods
    //----------------------------------------------------------------------------------------------------------//
#undef TENSOR_METHODS_CONST_H
    #include "tensor/TensorMethods.h"
#define TENSOR_METHODS_CONST_H
    //----------------------------------------------------------------------------------------------------------//

    // Converters
    //----------------------------------------------------------------------------------------------------------//
#undef PODCONVERTERS_H
    #include "tensor/PODConverters.h"
#define PODCONVERTERS_H
    //----------------------------------------------------------------------------------------------------------//

    // Boolean functions
    //----------------------------------------------------------------------------------------------------------//
    constexpr FASTOR_INLINE bool is_uniform() const {
        //! A tensor is uniform if it spans equally in all dimensions,
        //! i.e. generalisation of square matrix to n dimension
        return no_of_unique<Rest...>::value==1 ? true : false;
    }

    template<typename U, size_t ... RestOther>
    FASTOR_INLINE bool is_equal(const SingleValueTensor<U,RestOther...> &other, const double Tol=PRECI_TOL) const {
        //! Two tensors are equal if they have the same type, rank, size and elements
        if(!std::is_same<T,U>::value) return false;
        if(sizeof...(Rest)!=sizeof...(RestOther)) return false;
        if(prod<Rest...>::value!=prod<RestOther...>::value) return false;
        const T *other_data = other.data();
        if (std::fabs(_data[0]-other_data[0])>Tol) return false;
        return true;
    }

    template<typename U, size_t ... RestOther>
    FASTOR_INLINE bool operator ==(const SingleValueTensor<U,RestOther...> &other) const {
        //! Two tensors are equal if they have the same type, rank, size and elements
            return is_equal(other);
    }

    template<typename U, size_t ... RestOther>
    FASTOR_INLINE bool operator !=(const SingleValueTensor<U,RestOther...> &other) const {
        //! Two tensors are equal if they have the same type, rank, size and elements
            return !is_equal(other);
    }

};

// template<typename T, size_t ...Rest>
// constexpr const T SingleValueTensor<T,Rest...>::_data[prod<Rest...>::value];

template<typename T, size_t ... Rest>
struct tensor_type_finder<SingleValueTensor<T,Rest...>> {
    using type = SingleValueTensor<T,Rest...>;
};

template<typename T, size_t ... Rest>
struct scalar_type_finder<SingleValueTensor<T,Rest...>> {
    using type = T;
};


OS_STREAM_TENSOR0(SingleValueTensor)
OS_STREAM_TENSOR1(SingleValueTensor)
OS_STREAM_TENSOR2(SingleValueTensor)
OS_STREAM_TENSORn(SingleValueTensor)


template<typename T, size_t M, size_t N>
FASTOR_INLINE SingleValueTensor<T,N,M> transpose(const SingleValueTensor<T,M,N> &a) {
    return SingleValueTensor<T,N,M>(a(0,0));
}

template<typename T, size_t M>
T trace(const SingleValueTensor<T,M,M> &a) {
    return M*a(0,0);
}

template<typename T, size_t M>
FASTOR_INLINE T determinant(const SingleValueTensor<T,M,M> &a) {
    return std::pow(a(0,0),M);
}

template<typename T, size_t M, size_t N>
FASTOR_INLINE double norm(const SingleValueTensor<T,M,N> &a) {
    return a(0,0)*std::sqrt(double(M*N));
}

template<typename T, size_t I>
FASTOR_INLINE Tensor<T,I,I> inverse(const SingleValueTensor<T,I,I> &a) {
    Tensor<T,I,I> out; Tensor<T,I,I> in(a(0,0));
    _inverse<T,I>(in.data(),out.data());
    return out;
}

template<typename T, size_t M, size_t K, size_t N>
FASTOR_INLINE Tensor<T,M,N> matmul(const Tensor<T,M,K> &a, const SingleValueTensor<T,K,N> &b) {

    using V = SIMDVector<T,DEFAULT_ABI>;

    Tensor<T,M,N> out;
    T *out_data = out.data();
    const T *a_data = a.data();
    const T b_value = b(0,0);

    for (size_t i=0; i<M; ++i) {
        V vec_out;
        size_t j=0;
        for (; j<ROUND_DOWN(K,(int)V::Size); j+=V::Size) {
            vec_out = vec_out + V(&a_data[i*K+j])*b_value;
        }
        T out_value = 0.;
        for (; j<K; j++) {
            out_value += a_data[i*K+j]*b_value;
        }
        out_value += vec_out.sum();
        V out_vec_value(out_value);

        j=0;
        for (; j<ROUND_DOWN(N,(int)V::Size); j+=V::Size) {
            out_vec_value.store(&out_data[i*N+j],false);
        }
        for (; j<N; ++j) {
            out_data[i*N+j] = out_value;
        }
    }

    return out;
}

template<typename T, size_t M, size_t K, size_t N>
FASTOR_INLINE Tensor<T,M,N> matmul(const SingleValueTensor<T,M,K> &a, const Tensor<T,K,N> &b) {
    return transpose(matmul(transpose(b),transpose(a)));
}

template<typename T, size_t M, size_t K, size_t N>
FASTOR_INLINE SingleValueTensor<T,M,N> matmul(const SingleValueTensor<T,M,K> &a, const SingleValueTensor<T,K,N> &b) {

    const T a_value = a(0,0);
    const T b_value = b(0,0);
    // matmul is just this
    SingleValueTensor<T,M,N> out(a_value*b_value*K);

    // Not necessary
    // using V = SIMDVector<T,DEFAULT_ABI>;
    // V vec_out;
    // size_t j=0;
    // for (; j<ROUND_DOWN(K,(int)V::Size); j+=V::Size) {
    //     vec_out = vec_out + V(a_value)*b_value;
    // }
    // T out_value = 0.;
    // for (; j<K; j++) {
    //     out_value += a_value*b_value;
    // }
    // out_value += vec_out.sum();
    // SingleValueTensor<T,M,N> out(out_value);

    return out;
}




// This one is almost like a compile time einsum
template<class Index_I, class Index_J, typename T, size_t ... Rest0, size_t ... Rest1>
FASTOR_INLINE
typename contraction_impl<typename concat_<Index_I,Index_J>::type,SingleValueTensor<T,Rest0...,Rest1...>,
                typename std_ext::make_index_sequence<sizeof...(Rest0)+sizeof...(Rest1)>::type>::type
einsum(const SingleValueTensor<T,Rest0...> &a, const SingleValueTensor<T,Rest1...> &b) {

    static_assert(einsum_index_checker<typename concat_<Index_I,Index_J>::type>::value,
                  "INDICES FOR EINSUM FUNCTION CANNOT APPEAR MORE THAN TWICE. USE CONTRACTION INSTEAD");

    std::array<size_t,Index_I::NoIndices> idx0; std::copy_n(Index_I::_IndexHolder,Index_I::NoIndices,idx0.begin());
    std::array<size_t,Index_J::NoIndices> idx1; std::copy_n(Index_J::_IndexHolder,Index_J::NoIndices,idx1.begin());
    std::array<size_t,Index_I::NoIndices> dims0 = {Rest0...};

    // n^2 but is okay as this is a small loop with compile time spans
    size_t total = 1;
    for (int i=0; i<idx0.size(); ++i) {
        for (int j=0; j<idx1.size(); ++j) {
            if (idx0[i]==idx1[j]) {
                total *= dims0[i];
            }
        }
    }
    const T a_value = a.eval_s(0);
    const T b_value = b.eval_s(0);
    const T out_value = total*a_value*b_value;

    using OutTensor = typename contraction_impl<typename concat_<Index_I,Index_J>::type,SingleValueTensor<T,Rest0...,Rest1...>,
                typename std_ext::make_index_sequence<sizeof...(Rest0)+sizeof...(Rest1)>::type>::type;
    OutTensor out(out_value);
    return out;
}


}


#endif // SINGLEVALUE_TENSOR_H


