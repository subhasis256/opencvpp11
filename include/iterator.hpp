#ifndef __OPENCVPP11_ITERATOR__
#define __OPENCVPP11_ITERATOR__

#include <opencv2/opencv.hpp>
#include <common_checker.hpp>

using namespace cv;
using namespace std;

template<class Element> class IterableMat : public Mat {
    public:
        struct Iterator {
            MatIterator_<Element> it;

            Iterator& operator++ () {
                ++it;
                return *this;
            }

            Iterator(MatIterator_<Element> _it):
                it(_it) {}

            bool operator != (const Iterator &other) {
                return it != other.it;
            }

            Element &operator*() {
                return *it;
            }
        };

        Iterator begin() {
            return Iterator(Mat::begin<Element>());
        }

        Iterator end() {
            return Iterator(Mat::end<Element>());
        }

        IterableMat(Mat &x) :
            Mat(x) {}
};

template<class Element>
IterableMat<Element> iterate(Mat &x) {
    CHECKER_ASSERT(DataType<Element>::type == x.type(), "WARNING: Data type mismatch in iterable, may lead to wrong results\n");
    return IterableMat<Element>(x);
}

template<class Element> class EnumerableMat : public Mat {
    public:
        struct Enumeration {
            int x;
            int y;
            Element &val;

            Enumeration(int _x, int _y, Element &_val):
                x(_x), y(_y), val(_val) {}
        };

        struct EnumerationIterator {
            int x;
            int y;

            int rows;
            int cols;

            MatIterator_<Element> it;

            EnumerationIterator& operator++ () {
                if(x < cols-1)
                    ++x;
                else {
                    x = 0;
                    ++y;
                }
                ++it;
                return *this;
            }

            EnumerationIterator(MatIterator_<Element> _it, int _x, int _y, int _rows, int _cols):
                it(_it), x(_x), y(_y), rows(_rows), cols(_cols) {}

            bool operator != (const EnumerationIterator &other) {
                return it != other.it;
            }

            Enumeration operator*() {
                return Enumeration(x, y, *it);
            }
        };

        EnumerationIterator begin() {
            return EnumerationIterator(Mat::begin<Element>(), 0, 0, Mat::rows, Mat::cols);
        }

        EnumerationIterator end() {
            return EnumerationIterator(Mat::end<Element>(), Mat::cols, Mat::rows, Mat::rows, Mat::cols);
        }

        EnumerableMat(Mat &x) :
            Mat(x) {}
};

template<class Element>
EnumerableMat<Element> enumerate(Mat &x) {
    CHECKER_ASSERT(DataType<Element>::type == x.type(), "WARNING: Data type mismatch in enumerable, may lead to wrong results\n");
    return EnumerableMat<Element>(x);
}

#endif

