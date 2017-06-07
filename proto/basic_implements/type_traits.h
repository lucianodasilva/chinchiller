#pragma once
#ifndef _type_traits_h_
#define _type_traits_h_

#include <stdlib.h>

namespace common {

    struct __sfinae_types
    {
        typedef char __one;
        typedef struct { char __arr[2]; } __two;
    };

    template<class _Tp>
    struct __in_array
    : public __sfinae_types
    {
    private:
        template<class _Up>
        static __one __test(_Up(*)[1]);
        template<class>
        static __two __test(...);
    
    public:
        static const bool __value = sizeof(__test<_Tp>(0)) == 1;
    };

    #define _DEFINE_SPEC_BODY(_Value)                                    \
        : public integral_constant<bool, _Value> { };

    #define _DEFINE_SPEC_0_HELPER(_Spec, _Value)                         \
        template<>                                                       \
        struct _Spec                                                     \
            _DEFINE_SPEC_BODY(_Value)

    #define _DEFINE_SPEC_1_HELPER(_Spec, _Value)                         \
        template<class _Tp>                                           \
        struct _Spec                                                     \
            _DEFINE_SPEC_BODY(_Value)
      
    #define _DEFINE_SPEC_2_HELPER(_Spec, _Value)                         \
        template<class _Tp, class _Cp>                             \
        struct _Spec                                                     \
            _DEFINE_SPEC_BODY(_Value)

    #define _DEFINE_SPEC(_Order, _Trait, _Type, _Value)                  \
        _DEFINE_SPEC_##_Order##_HELPER(_Trait<_Type>, _Value)            \
        _DEFINE_SPEC_##_Order##_HELPER(_Trait<_Type const>, _Value)      \
        _DEFINE_SPEC_##_Order##_HELPER(_Trait<_Type volatile>, _Value)   \
        _DEFINE_SPEC_##_Order##_HELPER(_Trait<_Type const volatile>, _Value)


    template < class _Tp, _Tp __v >
    struct integral_constant
    {
        static const _Tp                    value = __v;
        using value_type =                  _Tp;
        using type =                        integral_constant < _Tp, __v >;
    };

    using true_type =   integral_constant < bool, true >;
    using false_type =  integral_constant < bool, false >;

    template< class _Tp, _Tp __v >
    const _Tp integral_constant<_Tp, __v>::value;

    template < class >
    struct is_void
        : public false_type { };
    _DEFINE_SPEC(0, is_void, void, true)

    template < class >
    struct is_integral
        : public false_type { };
    _DEFINE_SPEC(0, is_integral, bool, true)
    _DEFINE_SPEC(0, is_integral, char, true)
    _DEFINE_SPEC(0, is_integral, signed char, true)
    _DEFINE_SPEC(0, is_integral, unsigned char, true)
    _DEFINE_SPEC(0, is_integral, short, true)
    _DEFINE_SPEC(0, is_integral, unsigned short, true)
    _DEFINE_SPEC(0, is_integral, int, true)
    _DEFINE_SPEC(0, is_integral, unsigned int, true)
    _DEFINE_SPEC(0, is_integral, long, true)
    _DEFINE_SPEC(0, is_integral, unsigned long, true)
    _DEFINE_SPEC(0, is_integral, long long, true)
    _DEFINE_SPEC(0, is_integral, unsigned long long, true)

    template<class>
    struct is_floating_point
        : public false_type { };
    _DEFINE_SPEC(0, is_floating_point, float, true)
    _DEFINE_SPEC(0, is_floating_point, double, true)
    _DEFINE_SPEC(0, is_floating_point, long double, true)

    template<class>
    struct is_array
        : public false_type { };

    template<class _Tp, size_t _Size>
    struct is_array<_Tp[_Size]>
        : public true_type { };

    template<class _Tp>
    struct is_array<_Tp[]>
        : public true_type { };

    template<class>
    struct is_pointer
        : public false_type { };
    _DEFINE_SPEC(1, is_pointer, _Tp*, true)
 
    template<class>
    struct is_reference
        : public false_type { };

    template<class _Tp>
    struct is_reference<_Tp&>
        : public true_type { };

        template<typename _Tp>
    struct __is_union_or_class_helper
    : public __sfinae_types
    {
    private:
      template<typename _Up>
        static __one __test(int _Up::*);
      template<typename>
        static __two __test(...);
    
    public:
      static const bool __value = sizeof(__test<_Tp>(0)) == 1;
    };

    // Extension.
    template<typename _Tp>
    struct __is_union_or_class
        : public integral_constant<bool, __is_union_or_class_helper<_Tp>::__value>
    { };

    template<class _Tp>
    struct is_function
    : public integral_constant<bool, !(__in_array<_Tp>::__value
				       || __is_union_or_class<_Tp>::value
				       || is_reference<_Tp>::value
				       || is_void<_Tp>::value)>
    {};

    template<class>
    struct is_member_object_pointer
        : public false_type { };
    _DEFINE_SPEC(2, is_member_object_pointer, _Tp _Cp::*, !is_function<_Tp>::value)

    template<class>
    struct is_member_function_pointer
        : public false_type { };
    _DEFINE_SPEC(2, is_member_function_pointer, _Tp _Cp::*, is_function<_Tp>::value)

    template<class _Tp>
    struct is_member_pointer
    : public integral_constant<bool,
			       (is_member_object_pointer<_Tp>::value
				|| is_member_function_pointer<_Tp>::value)>
    { };

    template<class _Tp>
    struct is_arithmetic
    : public integral_constant<bool, (is_integral<_Tp>::value
				      || is_floating_point<_Tp>::value)>
    { };

    template<class _Tp>
    struct is_fundamental
    : public integral_constant<bool, (is_arithmetic<_Tp>::value
				      || is_void<_Tp>::value)>
    { };

    template<class _Tp>
    struct is_enum
        : public integral_constant<bool, !(is_fundamental<_Tp>::value
				       || is_array<_Tp>::value
				       || is_pointer<_Tp>::value
				       || is_reference<_Tp>::value
				       || is_member_pointer<_Tp>::value
				       || is_function<_Tp>::value
				       || __is_union_or_class<_Tp>::value)>
    {};

    template < class >
    struct is_union { };

    template < class >
    struct is_class { };

    template<class _Tp>
    struct is_object
    : public integral_constant<bool, !(is_function<_Tp>::value
				       || is_reference<_Tp>::value
				       || is_void<_Tp>::value)>
    { };

    template<class _Tp>
    struct is_scalar
    : public integral_constant<bool, (is_arithmetic<_Tp>::value
				      || is_enum<_Tp>::value
				      || is_pointer<_Tp>::value
				      || is_member_pointer<_Tp>::value)>
    { };

    template<class _Tp>
    struct is_compound
    : public integral_constant<bool, !is_fundamental<_Tp>::value> { };
  
    template<typename>
    struct is_const
        : public false_type { };

    template<typename _Tp>
    struct is_const<_Tp const>
        : public true_type { };
  
    template<typename>
    struct is_volatile
        : public false_type { };

    template<typename _Tp>
    struct is_volatile<_Tp volatile>
        : public true_type { };

    template<typename _Tp>
    struct remove_all_extents
    { typedef _Tp     type; };

    template<typename _Tp, size_t _Size>
    struct remove_all_extents<_Tp[_Size]>
    { typedef typename remove_all_extents<_Tp>::type     type; };

    template<typename _Tp>
    struct remove_all_extents<_Tp[]>
    { typedef typename remove_all_extents<_Tp>::type     type; };

    template<typename _Tp>
    struct is_pod
        : public integral_constant<bool, (is_void<_Tp>::value
				      || is_scalar<typename
				      remove_all_extents<_Tp>::type>::value)>
    { };

    template<typename _Tp>
    struct has_trivial_constructor
        : public integral_constant<bool, is_pod<_Tp>::value> { };

    template<typename _Tp>
    struct has_trivial_copy
        : public integral_constant<bool, is_pod<_Tp>::value> { };

    template<typename _Tp>
    struct has_trivial_assign
        : public integral_constant<bool, is_pod<_Tp>::value> { };

    template<typename _Tp>
    struct has_trivial_destructor
        : public integral_constant<bool, is_pod<_Tp>::value> { };

    template<typename _Tp>
    struct has_nothrow_constructor
        : public integral_constant<bool, is_pod<_Tp>::value> { };

    template<typename _Tp>
    struct has_nothrow_copy
        : public integral_constant<bool, is_pod<_Tp>::value> { };

    template<typename _Tp>
    struct has_nothrow_assign
        : public integral_constant<bool, is_pod<_Tp>::value> { };

    template<typename>
    struct has_virtual_destructor
        : public false_type { };

    template< class _t > struct remove_reference        { using type = _t;};
    template< class _t > struct remove_reference<_t&>   { using type = _t;};
    template< class _t > struct remove_reference<_t&&>  { using type = _t;};
}

#endif