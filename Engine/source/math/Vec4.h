#pragma once

#include <cassert>
#include <ostream>

template <typename T>
struct Vec3;

template <typename T>
struct Vec4 final {
    union {
        T x, r, s;
    };
    union {
        T y, g, t;
    };
    union {
        T z, b, p;
    };
    union {
        T w, a, r;
    };

    // Constructors
    Vec4()
        : Vec4(0) {
    }

    Vec4(const Vec4& v)
        : x(v.x), y(v.y), z(v.z), w(v.w) {
    }

    // Value constructors
    explicit Vec4(const T& v1)
        : x(v1), y(v1), z(v1), w(v1) {
    }

    explicit Vec4(const T& v1, const T& v2, const T& v3, const T& v4)
        : x(v1), y(v2), z(v3), w(v4) {
    }

    // Conversion constructors
    template <typename U>
    explicit Vec4(const Vec4<U>& v)
        : x(v.x), y(v.y), z(v.z), w(v.w) {
    }

    template <typename U>
    explicit Vec4(const Vec3<U>& v)
        : x(v.x), y(v.y), z(v.z), w(0) {
    }

    // Access
    T& operator[](size_t i) {
        assert(i < 4);
        switch (i) {
        default: // fallthrough
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        }
    }

    const T& operator[](size_t i) const {
        assert(i < 4);
        switch (i) {
        default: // fallthrough
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        }
    }

    // Unary arithmetic operators
    Vec4& operator=(const Vec4& v) {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        return *this;
    }

    template <typename U>
    Vec4& operator=(const Vec4<U>& v) {
        x = static_cast<T>(v.x);
        y = static_cast<T>(v.y);
        z = static_cast<T>(v.z);
        w = static_cast<T>(v.w);
        return *this;
    }

    template <typename U>
    Vec4& operator=(const Vec3<U>& v) {
        x = static_cast<T>(v.x);
        y = static_cast<T>(v.y);
        z = static_cast<T>(v.z);
        w = 0;
        return *this;
    }

    Vec4& operator+() const {
        return *this;
    }

    Vec4 operator-() const {
        return Vec4(-x, -y, -z, -w);
    }

    // Sum
    Vec4& operator+=(const T& scalar) {
        x += scalar;
        y += scalar;
        z += scalar;
        w += scalar;
        return *this;
    }

    template <typename U>
    Vec4& operator+=(const Vec4<U>& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }

    // Sub
    Vec4& operator-=(const T& scalar) {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        w -= scalar;
        return *this;
    }

    template <typename U>
    Vec4& operator-=(const Vec4<U>& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    // Mul
    Vec4& operator*=(const T& scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    template <typename U>
    Vec4& operator*=(const Vec4<U>& v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
        return *this;
    }

    // Div
    Vec4& operator/=(const T& scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }

    template <typename U>
    Vec4& operator/=(const Vec4<U>& v) {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        w /= v.w;
        return *this;
    }

    // IO operators
    friend std::ostream& operator<<(std::ostream& out, const Vec4& vec) {
        out << "{ " << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << " }";
        return out;
    }

    friend std::wostream& operator<<(std::wostream& out, const Vec4& vec) {
        out << L"{ " << vec.x << L", " << vec.y << L", " << vec.z << L", " << vec.w << L" }";
        return out;
    }

    // Functions
    T Length() const {
        return sqrt(SquaredLength());
    }

    T SquaredLength() const {
        return Dot(*this);
    }

    Vec4& Normalize() {
        T scale = 1 / Length();
        *this *= scale;
        return *this;
    }

    Vec4 Normalized() const {
        return Vec4(*this).Normalize();
    }

    template <typename U>
    T Dot(const Vec4<U>& v) const {
        return x * static_cast<T>(v.x) + y * static_cast<T>(v.y) + z * static_cast<T>(v.z) + w * static_cast<T>(v.w);
    }

    template <typename U>
    Vec4 Cross(const Vec4<U>& v) const {
        return Vec4(
            y * v.z - z * v.y,
            x * v.z - z * v.x,
            x * v.y - y * v.x,
            0
        );
    }

    // TODO: What to do with w
    template <typename U>
    Vec4 Reflect(const Vec4<U>& normal) const {
        return *this - 2 * Dot(normal) * normal;
    }
};

// Binary arithmetic operators
// Sum
template <typename T, typename U>
Vec4<T> operator+(const Vec4<T>& v, const U& scalar) {
    return Vec4<T>(v) += scalar;
}

template <typename T, typename U>
Vec4<T> operator+(const U& scalar, const Vec4<T>& v) {
    return Vec4<T>(v) += scalar;
}

template <typename T, typename U>
Vec4<T> operator+(const Vec4<T>& v1, const Vec4<U>& v2) {
    return Vec4<T>(v1) += v2;
}

// Sub
template <typename T, typename U>
Vec4<T> operator-(const Vec4<T>& v, const U& scalar) {
    return Vec4<T>(v) -= scalar;
}

template <typename T, typename U>
Vec4<T> operator-(const Vec4<T>& v1, const Vec4<U>& v2) {
    return Vec4<T>(v1) -= v2;
}

// Mul
template <typename T, typename U>
Vec4<T> operator*(const Vec4<T>& v, const U& scalar) {
    return Vec4<T>(v) *= scalar;
}

template <typename T, typename U>
Vec4<T> operator*(const U& scalar, const Vec4<T>& v) {
    return Vec4<T>(v) *= scalar;
}

template <typename T, typename U>
Vec4<T> operator*(const Vec4<T>& v1, const Vec4<U>& v2) {
    return Vec4<T>(v1) *= v2;
}

// Div
template <typename T, typename U>
Vec4<T> operator/(const Vec4<T>& v, const U& scalar) {
    return Vec4<T>(v) /= scalar;
}

template <typename T, typename U>
Vec4<T> operator/(const Vec4<T>& v1, const Vec4<U>& v2) {
    return Vec4<T>(v1) /= v2;
}
