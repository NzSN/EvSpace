#define DEFINE_TRAIT_HAS_METHOD(M)                   \
  template<typename T, typename F>                                      \
  struct has_##M {                                                        \
    static_assert(std::integral_constant<F, false>::value,              \
                  "The second parameter of has_method<T,F> must be a function type"); \
  };                                                                    \
  template<typename T, typename R, typename... Args>                    \
  struct has_##M<T, R(Args...)> {                                         \
  private:                                                              \
  template<typename U>                                                  \
  static constexpr auto check(U*) -> typename                           \
    std::is_same<decltype(std::declval<U>().M(std::declval<Args>()...)), R>::type;      \
  };
