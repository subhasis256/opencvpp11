opencvpp11
==========

OpenCV++11: A library for adding C++11 and functional features to OpenCV.

This library is an effort to add C++11 iteration features to OpenCV Mat classes and introduces new features such as per-pixel transformations with lambda expressions to OpenCV. A few examples:

1. Using the `iterate<T>` iterator over `Mat`s. This function outputs a warning if there is a mismatch between `T` and the internal `Mat` type.
```C++
Mat image = imread("foo.jpg");
for(auto f : iterate<Vec3b>(image)) {
  cout << f << endl;
}
```

2. Using the `enumerate<T>` iterator over `Mat`s. Same as before, except the iterator also outputs a `(x,y)` co-ordinate with every element.
```C++
Mat image = imread("foo.jpg");
for(auto p : enumerate<Vec3b>(image)) {
  cout << p.x << " " << p.y << " " << p.val << endl;
}
```
3. Using the `pp_transform` and the `total_transform` functions for per-pixel and full-image transformations of Mat. Like before, they output a warning if the function accepts different typed arguments than the image's internal type.
```C++
Mat image = imread("foo.jpg");

Mat output = pp_transform(image, [](Vec3b pixel) { // do an RGB normalization
  float sum = pixel[0] + pixel[1] + pixel[2];
  return Vec3f(pixel[0]/sum, pixel[1]/sum, pixel[2]/sum);
}).pp_transform([](Vec3f pixel) { // pp_transforms can be chained
  return pixel[2] - min(pixel);
});
```
