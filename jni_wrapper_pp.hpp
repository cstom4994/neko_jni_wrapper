
#if (!defined NEKO_PP_FOR_EACH || !defined NEKO_PP_PARAMS)


// 横向迭代, NEKO_PP_PARAMS(x, 3) => x1, x2, x3

#define NEKO_PP_PARAMS_0(x)
#define NEKO_PP_PARAMS_1(x) x##1
#define NEKO_PP_PARAMS_2(x) NEKO_PP_PARAMS_1(x), x##2
#define NEKO_PP_PARAMS_3(x) NEKO_PP_PARAMS_2(x), x##3
#define NEKO_PP_PARAMS_4(x) NEKO_PP_PARAMS_3(x), x##4
#define NEKO_PP_PARAMS_5(x) NEKO_PP_PARAMS_4(x), x##5
#define NEKO_PP_PARAMS_6(x) NEKO_PP_PARAMS_5(x), x##6
#define NEKO_PP_PARAMS_7(x) NEKO_PP_PARAMS_6(x), x##7
#define NEKO_PP_PARAMS_8(x) NEKO_PP_PARAMS_7(x), x##8
#define NEKO_PP_PARAMS_9(x) NEKO_PP_PARAMS_8(x), x##9
#define NEKO_PP_PARAMS_10(x) NEKO_PP_PARAMS_9(x), x##10
#define NEKO_PP_PARAMS_11(x) NEKO_PP_PARAMS_10(x), x##11
#define NEKO_PP_PARAMS_12(x) NEKO_PP_PARAMS_11(x), x##12
#define NEKO_PP_PARAMS_13(x) NEKO_PP_PARAMS_12(x), x##13
#define NEKO_PP_PARAMS_14(x) NEKO_PP_PARAMS_13(x), x##14
#define NEKO_PP_PARAMS_15(x) NEKO_PP_PARAMS_14(x), x##15

#define NEKO_PP_PARAMS(x, N) NEKO_PP_PARAMS_##N(x)

// 横向迭代 NEKO_PP_CALL_PARAMS(f, func, 3) => f(1), f(2), f(3)

#define NEKO_PP_CALL_PARAMS_0(f, func)
#define NEKO_PP_CALL_PARAMS_1(f, func) f(func, 1)
#define NEKO_PP_CALL_PARAMS_2(f, func) NEKO_PP_CALL_PARAMS_1(f, func), f(func, 2)
#define NEKO_PP_CALL_PARAMS_3(f, func) NEKO_PP_CALL_PARAMS_2(f, func), f(func, 3)
#define NEKO_PP_CALL_PARAMS_4(f, func) NEKO_PP_CALL_PARAMS_3(f, func), f(func, 4)
#define NEKO_PP_CALL_PARAMS_5(f, func) NEKO_PP_CALL_PARAMS_4(f, func), f(func, 5)
#define NEKO_PP_CALL_PARAMS_6(f, func) NEKO_PP_CALL_PARAMS_5(f, func), f(func, 6)
#define NEKO_PP_CALL_PARAMS_7(f, func) NEKO_PP_CALL_PARAMS_6(f, func), f(func, 7)
#define NEKO_PP_CALL_PARAMS_8(f, func) NEKO_PP_CALL_PARAMS_7(f, func), f(func, 8)
#define NEKO_PP_CALL_PARAMS_9(f, func) NEKO_PP_CALL_PARAMS_8(f, func), f(func, 9)
#define NEKO_PP_CALL_PARAMS_10(f, func) NEKO_PP_CALL_PARAMS_9(f, func), f(func, 10)
#define NEKO_PP_CALL_PARAMS_11(f, func) NEKO_PP_CALL_PARAMS_10(f, func), f(func, 11)
#define NEKO_PP_CALL_PARAMS_12(f, func) NEKO_PP_CALL_PARAMS_11(f, func), f(func, 12)
#define NEKO_PP_CALL_PARAMS_13(f, func) NEKO_PP_CALL_PARAMS_12(f, func), f(func, 13)
#define NEKO_PP_CALL_PARAMS_14(f, func) NEKO_PP_CALL_PARAMS_13(f, func), f(func, 14)
#define NEKO_PP_CALL_PARAMS_15(f, func) NEKO_PP_CALL_PARAMS_14(f, func), f(func, 15)

#define NEKO_PP_CALL_PARAMS(f, func, N) NEKO_PP_CALL_PARAMS_##N(f, func)

#endif
