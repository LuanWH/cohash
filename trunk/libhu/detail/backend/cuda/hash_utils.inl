#include <iostream>

#include <thrust/iterator/iterator_traits.h>
#include <thrust/detail/type_traits.h>

#include <thrust/gather.h>
#include <thrust/reverse.h>
#include <thrust/sequence.h>
#include <thrust/iterator/iterator_traits.h>
#include <thrust/detail/raw_buffer.h>

#include <libhu/detail/backend/cuda/detail/hash_utils.h>

namespace libhu
{
namespace detail
{
namespace backend
{
namespace cuda
{

namespace second_dispatch
{
    // second level of the dispatch decision tree

template<typename T_RAND_ACCESS_ITERATOR1,
         typename T_RAND_ACCESS_ITERATOR2,
         typename T_RAND_ACCESS_ITERATOR3,
         typename T_HASH_FUNCTOR>
  void access(T_RAND_ACCESS_ITERATOR1 search_keys_begin,
              T_RAND_ACCESS_ITERATOR1 search_keys_end,
              T_RAND_ACCESS_ITERATOR2 hash_table_begin,
              T_RAND_ACCESS_ITERATOR2 hash_table_end,
              T_RAND_ACCESS_ITERATOR3 output_values_begin,
              T_HASH_FUNCTOR          hf,
              bool                    constrained_hash_access,
              libhu::U32              max_age,
              thrust::detail::true_type)
    {
      libhu::detail::backend::cuda::detail::access(search_keys_begin, search_keys_end, hash_table_begin, hash_table_end, output_values_begin, hf, constrained_hash_access, max_age);
    }
    

} // end namespace second_dispatch

namespace first_dispatch
{
    // first level of the dispatch decision tree

template<typename T_RAND_ACCESS_ITERATOR1,
         typename T_RAND_ACCESS_ITERATOR2,
         typename T_RAND_ACCESS_ITERATOR3,
         typename T_HASH_FUNCTOR>
  void access(T_RAND_ACCESS_ITERATOR1 search_keys_begin,
              T_RAND_ACCESS_ITERATOR1 search_keys_end,
              T_RAND_ACCESS_ITERATOR2 hash_table_begin,
              T_RAND_ACCESS_ITERATOR2 hash_table_end,
              T_RAND_ACCESS_ITERATOR3 output_values_begin,
              T_HASH_FUNCTOR          hf,
              bool                    constrained_hash_access,
              libhu::U32              max_age,
              thrust::detail::true_type)
    {
    
      // decide whether to sort keys indirectly
      typedef typename thrust::iterator_traits<T_RAND_ACCESS_ITERATOR1>::value_type KeyType;
      static const bool hash_keys_indirectly = sizeof(KeyType) == 4;  

      // XXX WAR nvcc 3.0 unused variable warning
      (void) hash_keys_indirectly;
      
      second_dispatch::access(search_keys_begin, search_keys_end, hash_table_begin, hash_table_end, output_values_begin, hf, constrained_hash_access, max_age,
        thrust::detail::integral_constant<bool, hash_keys_indirectly>());
        
    }

} // end namespace first_dispatch

template<typename T_RAND_ACCESS_ITERATOR1,
         typename T_RAND_ACCESS_ITERATOR2,
         typename T_RAND_ACCESS_ITERATOR3,
         typename T_HASH_FUNCTOR>
  void access(T_RAND_ACCESS_ITERATOR1 search_keys_begin,
              T_RAND_ACCESS_ITERATOR1 search_keys_end,
              T_RAND_ACCESS_ITERATOR2 hash_table_begin,
              T_RAND_ACCESS_ITERATOR2 hash_table_end,
              T_RAND_ACCESS_ITERATOR3 output_values_begin,
              T_HASH_FUNCTOR          hf,
              bool                    constrained_hash_access,
              libhu::U32              max_age)
    {

  // we're attempting to launch a kernel, assert we're compiling with nvcc
  // ========================================================================
  // X Note to the user: If you've found this line due to a compiler error, X
  // X you need to compile your code using nvcc, rather than g++ or cl.exe  X
  // ========================================================================
  THRUST_STATIC_ASSERT( (thrust::detail::depend_on_instantiation<T_RAND_ACCESS_ITERATOR1, THRUST_DEVICE_COMPILER == THRUST_DEVICE_COMPILER_NVCC>::value) );
  
  // dispatch on whether we can use radix_sort_by_key
  typedef typename thrust::iterator_traits<T_RAND_ACCESS_ITERATOR1>::value_type KeyType;
  static const bool use_hash_key_type = thrust::detail::is_arithmetic<KeyType>::value;

  // XXX WAR nvcc 3.0 unused variable warning
  (void) use_hash_key_type;
  
  first_dispatch::access(search_keys_begin, search_keys_end, hash_table_begin, hash_table_end, output_values_begin, hf, constrained_hash_access, max_age,
    thrust::detail::integral_constant<bool, use_hash_key_type>());

}

} // end namespace cuda
} // end namespace backend
} // end namespace detail
} // end namespace libhu
