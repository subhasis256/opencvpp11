#ifndef __OPENCVPP11_TRANSFORM__
#define __OPENCVPP11_TRANSFORM__

#include <opencv2/opencv.hpp>
#include <common_checker.hpp>

using namespace std;
using namespace cv;

/**
 * a simple function_traits class to get argument and result types of functions
 */

template <typename T>
struct function_traits
    : public function_traits<decltype(&T::operator())>
{};
// For generic types, directly use the result of the signature of its 'operator()'


template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const>
// we specialize for pointers to member function
{
    typedef ReturnType result_type;
    typedef typename std::tuple_element<0, std::tuple<Args...>>::type argument_type;
};


/**
 * This is the workhorse of per-pixel transforms and the like
 * An IntermediateValue stores the current intermediate Mat in its val
 * and can be freely cast to or from a Mat instance
 *
 * It provides two methods:
 * pp_transform(func), which is equivalent to
 *
 * Mat output = Mat(val.size(), <return type of func>)
 * for(each pixel in val) {
 *      output[pixel] = func(input[pixel]);
 * }
 * return IntermediateValue(output);
 *
 * pp_transform can also be called with an explicit output
 * type, such as: 
 * pp_transform(CV_32FC3, func);
 *
 * One can directly obtain an IntermediateValue by calling
 * pp_transform(image, func)
 *
 * Since a pp_transform returns a IntermediateValue, one can create a chain
 * of pp_transforms such as
 * pp_transform(image, func).
 * pp_transform(func2).
 * pp_transform(func3);
 *
 * Also, another similar method is total_transform, which simply calls
 * func on the Mat itself, like:
 *
 * Mat output = func(val);
 * return IntermediateValue(output);
 *
 * This method is meant to be used for OpenCV implemented transforms such
 * as edge detection etc. An example of edge detection on the R-channel of a
 * normalized color image is below
 *
 * Mat output = pp_transform(image, [](Vec3b pixel) { // get normalized R channel
 *     float sum = pixel[0] + pixel[1] + pixel[2];
 *     return saturate_cast<uint8_t>(pixel[2]/sum*255);
 * }).total_transform([](Mat input) {
 *     Mat output;
 *     Canny(input, output, 20, 60);
 *     return output;
 * });
 */
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
