#ifndef __OPENCVPP11_TRANSFORM__
#define __OPENCVPP11_TRANSFORM__

#include <opencv2/opencv.hpp>
#include <common_checker.hpp>

using namespace std;
using namespace cv;

template <typename T>
struct function_traits
    : public function_traits<decltype(&T::operator())>
{};
// For generic types, directly use the result of the signature of its 'operator()'


template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const>
// we specialize for pointers to member function
{
    enum { arity = sizeof...(Args) };
    // arity is the number of arguments.

    typedef ReturnType result_type;
    typedef typename std::tuple_element<0, std::tuple<Args...>>::type argument_type;

    template <size_t i>
        struct arg
        {
            typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
            // the i-th argument is equivalent to the i-th tuple element of a tuple
            // composed of those arguments.
        };
};

struct IntermediateValue {
    Mat val;

    IntermediateValue(Mat x) {
        val = x;
    }

    operator Mat() {
        return val;
    }

    template<class Function> IntermediateValue pp_transform(Function fn) {
        Size sz = val.size();

        typedef function_traits<decltype(fn)> traits;

        CHECKER_ASSERT(DataType<typename traits::argument_type>::type == val.type(),
                "WARNING: Data type mismatch in transform, may lead to wrong results\n");

        Mat output = Mat(sz, DataType<typename traits::result_type>::type);

        if(val.isContinuous()) {
            sz.width *= sz.height;
            sz.height = 1;
        }

        for(int i = 0; i < sz.height; i++) {
            const typename traits::argument_type* ptr = val.ptr<typename traits::argument_type>(i);
            typename traits::result_type* outPtr = output.ptr<typename traits::result_type>(i);

            for(int j = 0; j < sz.width; j++) {
                outPtr[j] = fn(ptr[j]);
            }
        }

        return IntermediateValue(output);
    }

    template<class Function> IntermediateValue total_transform(Function fn) {
        return IntermediateValue(fn(val));
    }
};

template<class Function>
IntermediateValue pp_transform(const Mat &input, Function fn)
{
    return IntermediateValue(input).pp_transform(fn);
}

template<class Function>
IntermediateValue total_transform(const Mat &input, Function fn)
{
    return IntermediateValue(input).total_transform(fn);
}


#endif
