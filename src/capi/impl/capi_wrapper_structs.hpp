#ifndef TURI_CAPI_WRAPPER_STRUCTS_HPP_
#define TURI_CAPI_WRAPPER_STRUCTS_HPP_

#include <capi/TuriCore.h>
#include <unity/lib/variant.hpp>
#include <unity/lib/variant_converter.hpp>
#include <flexible_type/flexible_type.hpp>
#include <unity/lib/extensions/model_base.hpp>
#include <unity/lib/gl_sarray.hpp>
#include <unity/lib/gl_sframe.hpp>
#include <utility>

// BEGIN EXPERIMENTAL IMPLEMENTATION

// Specializations of this template define the inner C++ type for each public
// C-API struct T
template <typename T>
struct tc_wrapped_type;

// Abstract base class for objects actually returned to clients. The virtual
// destructor could facilitate a unified release method...
struct tc_impl_base {
  virtual ~tc_impl_base() = default;
};

// Templated derived class for each C API struct T. Inherits from tc_impl_base
// to enable dynamic_cast. Inherits from T to enable conversions.
template <typename T>
struct tc_impl : public tc_impl_base, public T {
  template <typename... Args>
  tc_impl(Args&&... args) : value(std::forward<Args>(args)...) {
    // Set the "type field" of the public struct.
    T::impl = this;
  }

  // Contain an instance of the wrapped type.
  typename tc_wrapped_type<T>::type value;
};

// Convenient helpers for extracting a reference to the wrapped value given only
// a pointer to public C-API struct. Performs type checking in case the client
// performed some invalid cast.
template <typename T>
typename tc_wrapped_type<T>::type& get_value(T* wrapper) {
  auto* impl = dynamic_cast<tc_impl<T>*>(wrapper->impl);
  if (impl == nullptr) {
    log_and_throw(std::string(typeid(T).name()) +
                  " value does not contain implementation of expected type " +
                  typeid(tc_impl<T>).name());
  }
  return impl->value;
}
template <typename T>
const typename tc_wrapped_type<T>::type& get_value(const T* wrapper) {
  const auto* impl = dynamic_cast<const tc_impl<T>*>(wrapper->impl);
  if (impl == nullptr) {
    log_and_throw(std::string(typeid(T).name()) +
                  " value does not contain implementation of expected type " +
                  typeid(tc_impl<T>).name());
  }
  return impl->value;
}

// Must be defined for each type... worth defining a macro?
template <>
struct tc_wrapped_type<tc_datetime> {
  using type = turi::flex_date_time;
};

// Probably not worth defining this... Call sites should just instantiate the
// tc_impl template, right?
template <typename... Args>
tc_datetime* new_tc_datetime(Args&&... args) {
  return new tc_impl<tc_datetime>(std::forward<Args>(args)...);
}

// END EXPERIMENTAL IMPLEMENTATION

struct capi_struct_type_info {
  virtual const char* name() const = 0;
  virtual void free(const void* v) = 0;

  // TODO: memory pool implementations, etc.
};

// Each C API wrapper struct is intended to match a c++ container type that
// allows it to interface to pure C easily.  As such, each wrapper struct has the
// an instance of the wrapping type stored as the value field, and type_info to hold
// some type information associated with it for error checking and, in the 
// future, memory management.
//
// Creation of a wrapping struct should be done with new_* methods.

#define DECLARE_CAPI_WRAPPER_STRUCT(struct_name, wrapping_type)                 \
  struct capi_struct_type_info_##struct_name;                                   \
                                                                                \
  /* The typeinfo executor needs to have a singleton instance. */               \
  extern capi_struct_type_info_##struct_name capi_struct_type_info_##struct_name##_inst; \
                                                                                \
  extern "C" {                                                                  \
    struct struct_name##_struct {                                               \
      capi_struct_type_info* type_info = nullptr;                               \
      wrapping_type value;                                                      \
    };                                                                          \
                                                                                \
    typedef struct struct_name##_struct struct_name;                            \
  }                                                                             \
                                                                                \
  struct capi_struct_type_info_##struct_name : public capi_struct_type_info {   \
    const char* name() const { return #struct_name; }                           \
    void free(const void* v) {                                                  \
      const struct_name* vv = static_cast<const struct_name*>(v);               \
      ASSERT_TRUE(vv->type_info == this);                                       \
      delete vv;                                                                \
    }                                                                           \
  };                                                                            \
                                                                                \
  static inline struct_name* new_##struct_name() {                              \
    struct_name* ret = new struct_name##_struct();                              \
    ret->type_info = &(capi_struct_type_info_##struct_name##_inst);             \
    return ret;                                                                 \
  }                                                                             \
                                                                                \
  template <typename... Args>                                                      \
  static inline struct_name* new_##struct_name(Args&&... args) {                   \
    struct_name* ret = new_##struct_name();                                     \
    ret->value = wrapping_type(std::forward<Args>(args)...);                                        \
    return ret;                                                                 \
  }


// TODO: make this more full featured than just a string error message.
DECLARE_CAPI_WRAPPER_STRUCT(tc_error, std::string);

DECLARE_CAPI_WRAPPER_STRUCT(tc_flex_dict, turi::flex_dict);
DECLARE_CAPI_WRAPPER_STRUCT(tc_flex_list, turi::flex_list);
DECLARE_CAPI_WRAPPER_STRUCT(tc_flex_image, turi::flex_image);
DECLARE_CAPI_WRAPPER_STRUCT(tc_flexible_type, turi::flexible_type);
DECLARE_CAPI_WRAPPER_STRUCT(tc_flex_enum_list, std::vector<turi::flex_type_enum>);
DECLARE_CAPI_WRAPPER_STRUCT(tc_sarray, turi::gl_sarray);
DECLARE_CAPI_WRAPPER_STRUCT(tc_sframe, turi::gl_sframe);
DECLARE_CAPI_WRAPPER_STRUCT(tc_variant, turi::variant_type);
DECLARE_CAPI_WRAPPER_STRUCT(tc_parameters, turi::variant_map_type);
DECLARE_CAPI_WRAPPER_STRUCT(tc_model, std::shared_ptr<turi::model_base>);

#endif
