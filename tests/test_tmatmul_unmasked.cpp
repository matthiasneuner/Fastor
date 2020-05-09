#include <Fastor/Fastor.h>

using namespace Fastor;

#define Tol 1e-12



template<typename T, size_t M, size_t K, size_t N>
Tensor<T,M,N> matmul_ref(const Tensor<T,M,K> &a, const Tensor<T,K,N> &b) {

    Tensor<T,M,N> out; out.zeros();
    for (size_t i=0; i<M; ++i) {
        for (size_t j=0; j<N; ++j) {
            for (size_t k=0; k<K; ++k) {
                out(i,j) += a(i,k)*b(k,j);
            }
        }
    }
    return out;
}

template<typename T, size_t M, size_t N>
T matmul_check(const Tensor<T,M,N> &a, const Tensor<T,M,N> &b) {

    T sum = 0;
    for (size_t i=0; i<M; ++i) {
        for (size_t j=0; j<N; ++j) {
            T value = a(i,j) - b(i,j);
            if ( std::abs(value) > 0 ) {
                return value;
            }
            // sum += a(i,j) - b(i,j);
        }
    }
    return sum;
}


// Register the matmul function to be testes
template<typename LhsType = matrix_type::general, typename RhsType = matrix_type::general, typename T, size_t M, size_t K, size_t N>
Tensor<T,M,N> matmul_registered_func(const Tensor<T,M,K> &am, const Tensor<T,K,N> &bm) {
    Tensor<T,M,N> outm; // outm.zeros();
    internal::_tmatmul_base<T,M,K,N,LhsType,RhsType>(am.data(),bm.data(),outm.data());
    return outm;
}



// Compile time loop over M, K and N
template<size_t from, size_t to>
struct check_lhs_lt_M {

    template<typename T, size_t K, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general,
        enable_if_t_<is_same_v_<LhsType,matrix_type::lower_tri> && is_same_v_<RhsType,matrix_type::general>,bool> = false >
    static inline void Do() {
        Tensor<T,from,K> am; am.iota(3);
        am = tril(am);
        Tensor<T,K,N> bm; bm.iota(5);
        Tensor<T,from,N> cm = matmul_registered_func<LhsType,RhsType>(am,bm);
        auto refm = matmul_ref(am,bm);
        T value = matmul_check(cm,refm);
        FASTOR_EXIT_ASSERT(std::abs(value) < Tol, "FAILED AT " +  std::to_string(from) + " " + std::to_string(K)+ " " + std::to_string(N));
        check_lhs_lt_M<from+1,to>::template Do<T,K,N,LhsType,RhsType>();
    }

    template<typename T, size_t K, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general,
        enable_if_t_<is_same_v_<LhsType,matrix_type::upper_tri> && is_same_v_<RhsType,matrix_type::general>,bool> = false >
    static inline void Do() {
        Tensor<T,from,K> am; am.iota(3);
        am = triu(am);
        Tensor<T,K,N> bm; bm.iota(5);
        Tensor<T,from,N> cm = matmul_registered_func<LhsType,RhsType>(am,bm);
        auto refm = matmul_ref(am,bm);
        T value = matmul_check(cm,refm);
        FASTOR_EXIT_ASSERT(std::abs(value) < Tol, "FAILED AT " +  std::to_string(from) + " " + std::to_string(K)+ " " + std::to_string(N));
        check_lhs_lt_M<from+1,to>::template Do<T,K,N,LhsType,RhsType>();
    }

    template<typename T, size_t K, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general,
        enable_if_t_<is_same_v_<LhsType,matrix_type::general> && is_same_v_<RhsType,matrix_type::lower_tri>,bool> = false >
    static inline void Do() {
        Tensor<T,from,K> am; am.iota(3);
        Tensor<T,K,N> bm; bm.iota(5);
        bm = tril(bm);
        Tensor<T,from,N> cm = matmul_registered_func<LhsType,RhsType>(am,bm);
        auto refm = matmul_ref(am,bm);
        T value = matmul_check(cm,refm);
        FASTOR_EXIT_ASSERT(std::abs(value) < Tol, "FAILED AT " +  std::to_string(from) + " " + std::to_string(K)+ " " + std::to_string(N));
        check_lhs_lt_M<from+1,to>::template Do<T,K,N,LhsType,RhsType>();
    }

    template<typename T, size_t K, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general,
        enable_if_t_<is_same_v_<LhsType,matrix_type::general> && is_same_v_<RhsType,matrix_type::upper_tri>,bool> = false >
    static inline void Do() {
        Tensor<T,from,K> am; am.iota(3);
        Tensor<T,K,N> bm; bm.iota(5);
        bm = triu(bm);
        Tensor<T,from,N> cm = matmul_registered_func<LhsType,RhsType>(am,bm);
        auto refm = matmul_ref(am,bm);
        T value = matmul_check(cm,refm);
        FASTOR_EXIT_ASSERT(std::abs(value) < Tol, "FAILED AT " +  std::to_string(from) + " " + std::to_string(K)+ " " + std::to_string(N));
        check_lhs_lt_M<from+1,to>::template Do<T,K,N,LhsType,RhsType>();
    }

    template<typename T, size_t K, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general,
        enable_if_t_<is_same_v_<LhsType,matrix_type::lower_tri> && is_same_v_<RhsType,matrix_type::lower_tri>,bool> = false >
    static inline void Do() {
        Tensor<T,from,K> am; am.iota(3);
        am = tril(am);
        Tensor<T,K,N> bm; bm.iota(5);
        bm = tril(bm);
        Tensor<T,from,N> cm = matmul_registered_func<LhsType,RhsType>(am,bm);
        auto refm = matmul_ref(am,bm);
        T value = matmul_check(cm,refm);
        FASTOR_EXIT_ASSERT(std::abs(value) < Tol, "FAILED AT " +  std::to_string(from) + " " + std::to_string(K)+ " " + std::to_string(N));
        check_lhs_lt_M<from+1,to>::template Do<T,K,N,LhsType,RhsType>();
    }

    template<typename T, size_t K, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general,
        enable_if_t_<is_same_v_<LhsType,matrix_type::lower_tri> && is_same_v_<RhsType,matrix_type::upper_tri>,bool> = false >
    static inline void Do() {
        Tensor<T,from,K> am; am.iota(3);
        am = tril(am);
        Tensor<T,K,N> bm; bm.iota(5);
        bm = triu(bm);
        Tensor<T,from,N> cm = matmul_registered_func<LhsType,RhsType>(am,bm);
        auto refm = matmul_ref(am,bm);
        T value = matmul_check(cm,refm);
        FASTOR_EXIT_ASSERT(std::abs(value) < Tol, "FAILED AT " +  std::to_string(from) + " " + std::to_string(K)+ " " + std::to_string(N));

        check_lhs_lt_M<from+1,to>::template Do<T,K,N,LhsType,RhsType>();
    }

    template<typename T, size_t K, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general,
        enable_if_t_<is_same_v_<LhsType,matrix_type::upper_tri> && is_same_v_<RhsType,matrix_type::lower_tri>,bool> = false >
    static inline void Do() {
        Tensor<T,from,K> am; am.iota(3);
        am = triu(am);
        Tensor<T,K,N> bm; bm.iota(5);
        bm = tril(bm);
        Tensor<T,from,N> cm = matmul_registered_func<LhsType,RhsType>(am,bm);
        auto refm = matmul_ref(am,bm);
        T value = matmul_check(cm,refm);
        FASTOR_EXIT_ASSERT(std::abs(value) < Tol, "FAILED AT " +  std::to_string(from) + " " + std::to_string(K)+ " " + std::to_string(N));

        check_lhs_lt_M<from+1,to>::template Do<T,K,N,LhsType,RhsType>();
    }

    template<typename T, size_t K, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general,
        enable_if_t_<is_same_v_<LhsType,matrix_type::upper_tri> && is_same_v_<RhsType,matrix_type::upper_tri>,bool> = false >
    static inline void Do() {
        Tensor<T,from,K> am; am.iota(3);
        am = triu(am);
        Tensor<T,K,N> bm; bm.iota(5);
        bm = triu(bm);
        Tensor<T,from,N> cm = matmul_registered_func<LhsType,RhsType>(am,bm);
        auto refm = matmul_ref(am,bm);
        T value = matmul_check(cm,refm);
        FASTOR_EXIT_ASSERT(std::abs(value) < Tol, "FAILED AT " +  std::to_string(from) + " " + std::to_string(K)+ " " + std::to_string(N));

        check_lhs_lt_M<from+1,to>::template Do<T,K,N,LhsType,RhsType>();
    }
};
template<size_t from>
struct check_lhs_lt_M<from,from> {

    template<typename T, size_t K, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general,
        enable_if_t_<is_same_v_<LhsType,matrix_type::lower_tri> && is_same_v_<RhsType,matrix_type::general>,bool> = false >
    static inline void Do() {
        Tensor<T,from,K> am; am.iota(3);
        am = tril(am);
        Tensor<T,K,N> bm; bm.iota(5);
        Tensor<T,from,N> cm = matmul_registered_func<LhsType,RhsType>(am,bm);
        auto refm = matmul_ref(am,bm);
        T value = matmul_check(cm,refm);
        FASTOR_EXIT_ASSERT(std::abs(value) < Tol, "FAILED AT " +  std::to_string(from) + " " + std::to_string(K)+ " " + std::to_string(N));
    }

    template<typename T, size_t K, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general,
        enable_if_t_<is_same_v_<LhsType,matrix_type::upper_tri> && is_same_v_<RhsType,matrix_type::general>,bool> = false >
    static inline void Do() {
        Tensor<T,from,K> am; am.iota(3);
        am = triu(am);
        Tensor<T,K,N> bm; bm.iota(5);
        Tensor<T,from,N> cm = matmul_registered_func<LhsType,RhsType>(am,bm);
        auto refm = matmul_ref(am,bm);
        T value = matmul_check(cm,refm);
        FASTOR_EXIT_ASSERT(std::abs(value) < Tol, "FAILED AT " +  std::to_string(from) + " " + std::to_string(K)+ " " + std::to_string(N));
    }

    template<typename T, size_t K, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general,
        enable_if_t_<is_same_v_<LhsType,matrix_type::general> && is_same_v_<RhsType,matrix_type::lower_tri>,bool> = false >
    static inline void Do() {
        Tensor<T,from,K> am; am.iota(3);
        Tensor<T,K,N> bm; bm.iota(5);
        bm = tril(bm);
        Tensor<T,from,N> cm = matmul_registered_func<LhsType,RhsType>(am,bm);
        auto refm = matmul_ref(am,bm);
        T value = matmul_check(cm,refm);
        FASTOR_EXIT_ASSERT(std::abs(value) < Tol, "FAILED AT " +  std::to_string(from) + " " + std::to_string(K)+ " " + std::to_string(N));
    }

    template<typename T, size_t K, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general,
        enable_if_t_<is_same_v_<LhsType,matrix_type::general> && is_same_v_<RhsType,matrix_type::upper_tri>,bool> = false >
    static inline void Do() {
        Tensor<T,from,K> am; am.iota(3);
        Tensor<T,K,N> bm; bm.iota(5);
        bm = triu(bm);
        Tensor<T,from,N> cm = matmul_registered_func<LhsType,RhsType>(am,bm);
        auto refm = matmul_ref(am,bm);
        T value = matmul_check(cm,refm);
        FASTOR_EXIT_ASSERT(std::abs(value) < Tol, "FAILED AT " +  std::to_string(from) + " " + std::to_string(K)+ " " + std::to_string(N));
    }

    template<typename T, size_t K, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general,
        enable_if_t_<is_same_v_<LhsType,matrix_type::lower_tri> && is_same_v_<RhsType,matrix_type::lower_tri>,bool> = false >
    static inline void Do() {
        Tensor<T,from,K> am; am.iota(3);
        am = tril(am);
        Tensor<T,K,N> bm; bm.iota(5);
        bm = tril(bm);
        Tensor<T,from,N> cm = matmul_registered_func<LhsType,RhsType>(am,bm);
        auto refm = matmul_ref(am,bm);
        T value = matmul_check(cm,refm);
        FASTOR_EXIT_ASSERT(std::abs(value) < Tol, "FAILED AT " +  std::to_string(from) + " " + std::to_string(K)+ " " + std::to_string(N));
    }

    template<typename T, size_t K, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general,
        enable_if_t_<is_same_v_<LhsType,matrix_type::lower_tri> && is_same_v_<RhsType,matrix_type::upper_tri>,bool> = false >
    static inline void Do() {
        Tensor<T,from,K> am; am.iota(3);
        am = tril(am);
        Tensor<T,K,N> bm; bm.iota(5);
        bm = triu(bm);
        Tensor<T,from,N> cm = matmul_registered_func<LhsType,RhsType>(am,bm);
        auto refm = matmul_ref(am,bm);
        T value = matmul_check(cm,refm);
        FASTOR_EXIT_ASSERT(std::abs(value) < Tol, "FAILED AT " +  std::to_string(from) + " " + std::to_string(K)+ " " + std::to_string(N));
    }

    template<typename T, size_t K, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general,
        enable_if_t_<is_same_v_<LhsType,matrix_type::upper_tri> && is_same_v_<RhsType,matrix_type::lower_tri>,bool> = false >
    static inline void Do() {
        Tensor<T,from,K> am; am.iota(3);
        am = triu(am);
        Tensor<T,K,N> bm; bm.iota(5);
        bm = tril(bm);
        Tensor<T,from,N> cm = matmul_registered_func<LhsType,RhsType>(am,bm);
        auto refm = matmul_ref(am,bm);
        T value = matmul_check(cm,refm);
        FASTOR_EXIT_ASSERT(std::abs(value) < Tol, "FAILED AT " +  std::to_string(from) + " " + std::to_string(K)+ " " + std::to_string(N));
    }

    template<typename T, size_t K, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general,
        enable_if_t_<is_same_v_<LhsType,matrix_type::upper_tri> && is_same_v_<RhsType,matrix_type::upper_tri>,bool> = false >
    static inline void Do() {
        Tensor<T,from,K> am; am.iota(3);
        am = triu(am);
        Tensor<T,K,N> bm; bm.iota(5);
        bm = triu(bm);
        Tensor<T,from,N> cm = matmul_registered_func<LhsType,RhsType>(am,bm);
        auto refm = matmul_ref(am,bm);
        T value = matmul_check(cm,refm);
        FASTOR_EXIT_ASSERT(std::abs(value) < Tol, "FAILED AT " +  std::to_string(from) + " " + std::to_string(K)+ " " + std::to_string(N));

    }
};

template<size_t from, size_t to>
struct check_lhs_lt_K {
    template<typename T, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general>
    static inline void Do() {
        check_lhs_lt_M<from,to>::template Do<T,from,N,LhsType,RhsType>();
        check_lhs_lt_K<from+1,to>::template Do<T,N,LhsType,RhsType>();
    }
};
template<size_t from>
struct check_lhs_lt_K<from,from> {
    template<typename T, size_t N, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general>
    static inline void Do() {
        check_lhs_lt_M<from,from>::template Do<T,from,N,LhsType,RhsType>();
    }
};

template<size_t from, size_t to>
struct check_lhs_lt_N {
    template<typename T, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general>
    static inline void Do() {
        check_lhs_lt_K<from,to>::template Do<T,from,LhsType,RhsType>();
        check_lhs_lt_N<from+1,to>::template Do<T,LhsType,RhsType>();
    }
};
template<size_t from>
struct check_lhs_lt_N<from,from> {
    template<typename T, typename LhsType = matrix_type::general, typename RhsType = matrix_type::general>
    static inline void Do() {
        check_lhs_lt_K<from,from>::template Do<T,from,LhsType,RhsType>();
    }
};


template<typename T>
void test_tmatmul() {

    {
        constexpr size_t start_size = 1UL;
        constexpr size_t end_size   = 13UL;

        print(FBLU(BOLD("Testing triangular matmul: lower-general")));
        check_lhs_lt_N<start_size,end_size>::template Do<T,matrix_type::lower_tri,matrix_type::general>();
        print(FGRN(BOLD("All tests passed successfully")));
        print(FBLU(BOLD("Testing triangular matmul: upper-general")));
        check_lhs_lt_N<start_size,end_size>::template Do<T,matrix_type::upper_tri,matrix_type::general>();
        print(FGRN(BOLD("All tests passed successfully")));
        print(FBLU(BOLD("Testing triangular matmul: general-lower")));
        check_lhs_lt_N<start_size,end_size>::template Do<T,matrix_type::general,matrix_type::lower_tri>();
        print(FGRN(BOLD("All tests passed successfully")));
        print(FBLU(BOLD("Testing triangular matmul: general-upper")));
        check_lhs_lt_N<start_size,end_size>::template Do<T,matrix_type::general,matrix_type::upper_tri>();
        print(FGRN(BOLD("All tests passed successfully")));
    }

    {
        constexpr size_t start_size = 1UL;
        constexpr size_t end_size   = 6UL;

        print(FBLU(BOLD("Testing triangular matmul: lower-lower")));
        check_lhs_lt_N<start_size,end_size>::template Do<T,matrix_type::lower_tri,matrix_type::lower_tri>();
        print(FGRN(BOLD("All tests passed successfully")));
        print(FBLU(BOLD("Testing triangular matmul: lower-upper")));
        check_lhs_lt_N<start_size,end_size>::template Do<T,matrix_type::lower_tri,matrix_type::upper_tri>();
        print(FGRN(BOLD("All tests passed successfully")));
        print(FBLU(BOLD("Testing triangular matmul: upper-lower")));
        check_lhs_lt_N<start_size,end_size>::template Do<T,matrix_type::upper_tri,matrix_type::lower_tri>();
        print(FGRN(BOLD("All tests passed successfully")));
        print(FBLU(BOLD("Testing triangular matmul: upper-upper")));
        check_lhs_lt_N<start_size,end_size>::template Do<T,matrix_type::upper_tri,matrix_type::upper_tri>();
        print(FGRN(BOLD("All tests passed successfully")));
    }

}

int main() {

    // We will test only double precision here
    // print(FBLU(BOLD("Testing triangular matmul: double precision")));
    test_tmatmul<double>();

    return 0;
}
