#pragma once

#include <cassert>
#include <ostream>

template <typename T>
struct TVec3;

template <typename T>
struct TVec4 final {
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
    TVec4()
        : TVec4(0) {
    }

    TVec4(const TVec4& v)
        : x(v.x), y(v.y), z(v.z), w(v.w) {
    }

    // Value constructors
    explicit TVec4(const T& v1)
        : x(v1), y(v1), z(v1), w(v1) {
    }

    explicit TVec4(const T& v1, const T& v2, const T& v3, const T& v4)
        : x(v1), y(v2), z(v3), w(v4) {
    }

    // Conversion constructors
    template <typename U>
    explicit TVec4(const TVec4<U>& v)
        : x(v.x), y(v.y), z(v.z), w(v.w) {
    }

    template <typename U>
    explicit TVec4(const TVec3<U>& v)
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
    TVec4& operator=(const TVec4& v) {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        return *this;
    }

    template <typename U>
    TVec4& operator=(const TVec4<U>& v) {
        x = static_cast<T>(v.x);
        y = static_cast<T>(v.y);
        z = static_cast<T>(v.z);
        w = static_cast<T>(v.w);
        return *this;
    }

    template <typename U>
    TVec4& operator=(const TVec3<U>& v) {
        x = static_cast<T>(v.x);
        y = static_cast<T>(v.y);
        z = static_cast<T>(v.z);
        w = 0;
        return *this;
    }

    TVec4& operator+() const {
        return *this;
    }

    TVec4 operator-() const {
        return TVec4(-x, -y, -z, -w);
    }

    // Sum
    TVec4& operator+=(const T& scalar) {
        x += scalar;
        y += scalar;
        z += scalar;
        w += scalar;
        return *this;
    }

    template <typename U>
    TVec4& operator+=(const TVec4<U>& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }

    // Sub
    TVec4& operator-=(const T& scalar) {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        w -= scalar;
        return *this;
    }

    template <typename U>
    TVec4& operator-=(const TVec4<U>& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    // Mul
    TVec4& operator*=(const T& scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    template <typename U>
    TVec4& operator*=(const TVec4<U>& v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
        return *this;
    }

    // Div
    TVec4& operator/=(const T& scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }

    template <typename U>
    TVec4& operator/=(const TVec4<U>& v) {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        w /= v.w;
        return *this;
    }

    // IO operators
    friend std::ostream& operator<<(std::ostream& out, const TVec4& vec) {
        out << "{ " << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << " }";
        return out;
    }

    friend std::wostream& operator<<(std::wostream& out, const TVec4& vec) {
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

    TVec4& Normalize() {
        T scale = 1 / Length();
        *this *= scale;
        return *this;
    }

    TVec4 Normalized() const {
        return TVec4(*this).Normalize();
    }

    template <typename U>
    T Dot(const TVec4<U>& v) const {
        return x * static_cast<T>(v.x) + y * static_cast<T>(v.y) + z * static_cast<T>(v.z) + w * static_cast<T>(v.w);
    }

    template <typename U>
    TVec4 Cross(const TVec4<U>& v) const {
        return TVec4(
            y * v.z - z * v.y,
            x * v.z - z * v.x,
            x * v.y - y * v.x,
            0
        );
    }

    // TODO: What to do with w
    template <typename U>
    TVec4 Reflect(const TVec4<U>& normal) {
        return TVec4(
            x * (1 - 2 * normal.x),
            y * (1 - 2 * normal.y),
            z * (1 - 2 * normal.z),
            w
        );
    }
};

// Binary arithmetic operators
// Sum
template <typename T, typename U>
TVec4<T> operator+(const TVec4<T>& v, const U& scalar) {
    return TVec4<T>(v) += scalar;
}

template <typename T, typename U>
TVec4<T> operator+(const U& scalar, const TVec4<T>& v) {
    return TVec4<T>(v) += scalar;
}

template <typename T, typename U>
TVec4<T> operator+(const TVec4<T>& v1, const TVec4<U>& v2) {
    return TVec4<T>(v1) += v2;
}

// Sub
template <typename T, typename U>
TVec4<T> operator-(const TVec4<T>& v, const U& scalar) {
    return TVec4<T>(v) -= scalar;
}

template <typename T, typename U>
TVec4<T> operator-(const TVec4<T>& v1, const TVec4<U>& v2) {
    return TVec4<T>(v1) -= v2;
}

// Mul
template <typename T, typename U>
TVec4<T> operator*(const TVec4<T>& v, const U& scalar) {
    return TVec4<T>(v) *= scalar;
}

template <typename T, typename U>
TVec4<T> operator*(const U& scalar, const TVec4<T>& v) {
    return TVec4<T>(v) *= scalar;
}

template <typename T, typename U>
TVec4<T> operator*(const TVec4<T>& v1, const TVec4<U>& v2) {
    return TVec4<T>(v1) *= v2;
}

// Div
template <typename T, typename U>
TVec4<T> operator/(const TVec4<T>& v, const U& scalar) {
    return TVec4<T>(v) /= scalar;
}

template <typename T, typename U>
TVec4<T> operator/(const TVec4<T>& v1, const TVec4<U>& v2) {
    return TVec4<T>(v1) /= v2;
}
