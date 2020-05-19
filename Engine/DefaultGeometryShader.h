#pragma once
#include "Triangle.h"

template<typename V>
class DefaultGeometryShader
{
public:
    typedef V Output;
public:
    Triangle<Output> operator()(const V &input0, const V &input1, const V &input2,
        size_t triangle_idx) const
    {
        return { input0, input1, input2 };
    }
};
