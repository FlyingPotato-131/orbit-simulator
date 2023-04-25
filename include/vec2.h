#pragma once

struct vec2{
	float x, y;
};

inline vec2 operator+(vec2 const &v1, vec2 const &v2)
{
    return
    {
        v1.x + v2.x,
        v1.y + v2.y
    };
}
inline vec2 operator-(vec2 const &v1, vec2 const &v2)
{
    return
    {
        v1.x - v2.x,
        v1.y - v2.y
    };
}
inline vec2 operator-(vec2 const &v)
{
    return {-v.x, -v.y};
}
inline vec2 operator*(vec2 const &v, float const f)
{
    return
    {
        v.x * f,
        v.y * f
    };
}
inline vec2 operator/(vec2 const &v, float const f)
{
    return
    {
        v.x / f,
        v.y / f
    };
}
inline float dot(vec2 const &v1, vec2 const &v2)
{
    return v1.x * v2.x
         + v1.y * v2.y;
}

inline float length(vec2 const &v)
{
    return std::sqrt(dot(v, v));
}

inline vec2 normalize(vec2 const &v)
{
    return v / length(v);
}
