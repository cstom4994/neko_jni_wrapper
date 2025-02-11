#pragma once

#include <jni.h>

#include <tuple>
#include <type_traits>
#include <utility>

#include "jni_wrapper_pp.hpp"

template <typename CType, typename JType>
struct ConvertedArg {
    using CppType = CType;
    using JNIType = JType;
    CType converted;
    JType original;
};

template <typename T>
struct JNITypeConverter;

template <>
struct JNITypeConverter<void*> {
    using CppType = void*;
    using JNIType = jlong;

    static ConvertedArg<CppType, JNIType> fromJNI(JNIEnv* env, JNIType value) { return {reinterpret_cast<void*>(value), value}; }

    static void release(JNIEnv*, const ConvertedArg<CppType, JNIType>&) {}
    static constexpr bool needRelease = false;
};

template <>
struct JNITypeConverter<const void*> {
    using CppType = const void*;
    using JNIType = jlong;

    static ConvertedArg<CppType, JNIType> fromJNI(JNIEnv* env, JNIType value) { return {reinterpret_cast<const void*>(value), value}; }

    static void release(JNIEnv*, const ConvertedArg<CppType, JNIType>&) {}
    static constexpr bool needRelease = false;
};

template <>
struct JNITypeConverter<int*> {
    using CppType = int*;
    using JNIType = jlong;

    static ConvertedArg<CppType, JNIType> fromJNI(JNIEnv* env, JNIType value) { return {reinterpret_cast<int*>(value), value}; }

    static void release(JNIEnv*, const ConvertedArg<CppType, JNIType>&) {}
    static constexpr bool needRelease = false;
};

template <>
struct JNITypeConverter<const char*> {
    using CppType = const char*;
    using JNIType = jstring;

    static ConvertedArg<CppType, JNIType> fromJNI(JNIEnv* env, JNIType value) {
        const char* cstr = value ? env->GetStringUTFChars(value, nullptr) : nullptr;
        return {cstr, value};
    }

    static void release(JNIEnv* env, const ConvertedArg<CppType, JNIType>& arg) {
        if (arg.original && arg.converted) {
            env->ReleaseStringUTFChars(arg.original, arg.converted);
        }
    }

    static constexpr bool needRelease = true;
};

#define BASIC_TYPE_CONVERSION(CType, JType)                                                                                        \
    template <>                                                                                                                    \
    struct JNITypeConverter<CType> {                                                                                               \
        using CppType = CType;                                                                                                     \
        using JNIType = JType;                                                                                                     \
        static ConvertedArg<CppType, JNIType> fromJNI(JNIEnv* env, JNIType value) { return {static_cast<CppType>(value), value}; } \
        static void release(JNIEnv*, const ConvertedArg<CppType, JNIType>&) {}                                                     \
        static constexpr bool needRelease = false;                                                                                 \
    };

BASIC_TYPE_CONVERSION(int, jint)
BASIC_TYPE_CONVERSION(long, jlong)
BASIC_TYPE_CONVERSION(long long, jlong)
BASIC_TYPE_CONVERSION(size_t, jlong)
BASIC_TYPE_CONVERSION(bool, jboolean)
BASIC_TYPE_CONVERSION(double, jdouble)
BASIC_TYPE_CONVERSION(jobject, jobject)

template <typename T>
struct JNIReturnConverter;

#define RETURN_TYPE_CONVERSION(CType, JType)                                                 \
    template <>                                                                              \
    struct JNIReturnConverter<CType> {                                                       \
        using CppType = CType;                                                               \
        using JNIType = JType;                                                               \
        static JType toJNI(JNIEnv* env, CppType value) { return static_cast<JType>(value); } \
    };

RETURN_TYPE_CONVERSION(int, jint)
RETURN_TYPE_CONVERSION(long long, jlong)
RETURN_TYPE_CONVERSION(unsigned long long, jlong)
RETURN_TYPE_CONVERSION(bool, jboolean)
RETURN_TYPE_CONVERSION(float, jfloat)
RETURN_TYPE_CONVERSION(double, jdouble)
RETURN_TYPE_CONVERSION(jclass, jclass)

template <>
struct JNIReturnConverter<void*> {
    using CppType = void*;
    using JNIType = jlong;
    static jlong toJNI(JNIEnv* env, CppType value) { return reinterpret_cast<jlong>(value); }
};

template <>
struct JNIReturnConverter<const void*> {
    using CppType = const void*;
    using JNIType = jlong;
    static jlong toJNI(JNIEnv* env, CppType value) { return reinterpret_cast<jlong>(value); }
};

template <>
struct JNIReturnConverter<jobject> {
    using CppType = jobject;
    using JNIType = jobject;
    static jobject toJNI(JNIEnv* env, CppType value) { return value; }
};

template <>
struct JNIReturnConverter<const char*> {
    using CppType = const char*;
    using JNIType = jstring;
    static jstring toJNI(JNIEnv* env, CppType value) {
        if (value == nullptr) {
            return nullptr;
        }
        return env->NewStringUTF(value);
    }
};

template <>
struct JNIReturnConverter<void> {
    using CppType = void;
    using JNIType = void;
    static void toJNI() {}
};

template <typename Func>
struct FuncTraits : public FuncTraits<decltype(&Func::operator())> {};

template <typename Ret, typename... Args>
struct FuncTraits<Ret (*)(Args...)> {
    using ReturnType = Ret;
    using ArgTuple = std::tuple<Args...>;
    static constexpr size_t ArgCount = sizeof...(Args);
    template <size_t I>
    using ArgType = typename std::tuple_element<I, ArgTuple>::type;
};

template <typename ClassType, typename Ret, typename... Args>
struct FuncTraits<Ret (ClassType::*)(Args...) const> {
    using ReturnType = Ret;
    using ArgTuple = std::tuple<Args...>;
    static constexpr size_t ArgCount = sizeof...(Args);

    template <size_t I>
    using ArgType = typename std::tuple_element<I, ArgTuple>::type;
};

template <typename FuncPtrT>
struct JNIWrapper;

template <typename Ret, typename... Args>
struct JNIWrapper<Ret (*)(Args...)> {
private:
    using ConvertedArgs = std::tuple<ConvertedArg<Args, typename JNITypeConverter<Args>::JNIType>...>;

    static auto convertArgs(JNIEnv* env, typename JNITypeConverter<Args>::JNIType... rawArgs) { return std::make_tuple(JNITypeConverter<Args>::fromJNI(env, rawArgs)...); }

    template <size_t... Is>
    static auto getCallArgsImpl(const ConvertedArgs& args, std::index_sequence<Is...>) {
        return std::make_tuple(std::get<Is>(args).converted...);
    }

    static auto getCallArgs(const ConvertedArgs& args) { return getCallArgsImpl(args, std::index_sequence_for<Args...>{}); }

    template <size_t... Is>
    static void releaseResourcesImpl(JNIEnv* env, ConvertedArgs& args, std::index_sequence<Is...>) {
        (releaseResource<Is>(env, args), ...);
    }

    template <size_t I>
    static void releaseResource(JNIEnv* env, ConvertedArgs& args) {
        using ArgType = typename std::tuple_element<I, ConvertedArgs>::type;
        using Converter = JNITypeConverter<typename ArgType::CppType>;
        if constexpr (Converter::needRelease) {
            Converter::release(env, std::get<I>(args));
        }
    }

public:
    template <typename Callable>
    static auto call(Callable&& func, JNIEnv* env, jobject obj, typename JNITypeConverter<Args>::JNIType... rawArgs) -> typename JNIReturnConverter<Ret>::JNIType {

        ConvertedArgs convertedArgs = convertArgs(env, rawArgs...);

        auto callFunc = [&]() { return std::apply(func, getCallArgs(convertedArgs)); };

        if constexpr (std::is_void_v<Ret>) {
            callFunc();
            releaseResourcesImpl(env, convertedArgs, std::index_sequence_for<Args...>{});
            return;
        } else {
            auto result = callFunc();
            releaseResourcesImpl(env, convertedArgs, std::index_sequence_for<Args...>{});
            return JNIReturnConverter<Ret>::toJNI(env, result);
        }
    }
};

#define JNI_PACKAGE_NAME dev_kaoruxun_any
#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)
#define JNI_FUNCTION_NAME(func) CONCAT(CONCAT(Java_, JNI_PACKAGE_NAME), CONCAT(_, func))

#define NEKO_JNI_PARAMS_GEN(funcptr, N) typename JNITypeConverter<typename FuncTraits<decltype(funcptr)>::template ArgType<N - 1>>::JNIType arg##N

#define NEKO_JNI_FUNC(func, _FUNCTION_NAME, N)                                                                                          \
    extern "C" JNIEXPORT typename JNIReturnConverter<typename FuncTraits<decltype(&func)>::ReturnType>::JNIType JNICALL _FUNCTION_NAME( \
            JNIEnv* env, jobject obj NEKO_PP_CALL_PARAMS(NEKO_JNI_PARAMS_GEN, &func, N)) {                                              \
        return JNIWrapper<decltype(&func)>::call(func, env, obj NEKO_PP_PARAMS(arg, N));                                                \
    }

#define NEKO_JNI_LAMBDA(func, _FUNCTION_NAME, N)                                                                                                                                                       \
    extern "C" JNIEXPORT typename JNIReturnConverter<typename FuncTraits<decltype(func)>::ReturnType>::JNIType JNICALL _FUNCTION_NAME(JNIEnv* env,                                                     \
                                                                                                                                      jobject obj NEKO_PP_CALL_PARAMS(NEKO_JNI_PARAMS_GEN, func, N)) { \
        return JNIWrapper<decltype(+func)>::call(func, env, obj NEKO_PP_PARAMS(arg, N));                                                                                                               \
    }
