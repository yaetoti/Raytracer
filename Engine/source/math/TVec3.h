#pragma once

#include <cassert>
#include <ostream>

template <typename T>
struct TVec4;

template <typename T>
struct TVec3 final {
    union {
        T x, r, s;
    };
    union {
        T y, g, t;
    };
    union {
        T z, b, p;
    };

    // Constructors
    TVec3()
    : TVec3(0) {
    }

    TVec3(const TVec3& v)
    : x(v.x), y(v.y), z(v.z) {
    }

    // Value constructors
    explicit TVec3(const T& v1)
    : x(v1), y(v1), z(v1) {
    }

    explicit TVec3(const T& v1, const T& v2, const T& v3)
    : x(v1), y(v2), z(v3) {
    }

    // Conversion constructors
    template <typename U>
    explicit TVec3(const TVec3<U>& v)
    : x(v.x), y(v.y), z(v.z) {
    }

    template <typename U>
    explicit TVec3(const TVec4<U>& v)
        : x(v.x), y(v.y), z(v.z) {
    }

    // Access
    T& operator[](size_t i) {
        assert(i < 3);
        switch (i) {
        default: // fallthrough
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        }
    }

    const T& operator[](size_t i) const {
        assert(i < 3);
        switch (i) {
        default: // fallthrough
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        }
    }

    // Unary arithmetic operators
    TVec3& operator=(const TVec3& v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    template <typename U>
    TVec3& operator=(const TVec3<U>& v) {
        x = static_cast<T>(v.x);
        y = static_cast<T>(v.y);
        z = static_cast<T>(v.z);
        return *this;
    }

    template <typename U>
    TVec3& operator=(const TVec4<U>& v) {
        x = static_cast<T>(v.x);
        y = static_cast<T>(v.y);
        z = static_cast<T>(v.z);
        return *this;
    }

    TVec3& operator+() const {
        return *this;
    }

    TVec3 operator-() const {
        return TVec3(-x, -y, -z);
    }

    // Sum
    TVec3& operator+=(const T& scalar) {
        x += scalar;
        y += scalar;
        z += scalar;
        return *this;
    }

    template <typename U>
    TVec3& operator+=(const TVec3<U>& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    // Sub
    TVec3& operator-=(const T& scalar) {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        return *this;
    }

    template <typename U>
    TVec3& operator-=(const TVec3<U>& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    // Mul
    TVec3& operator*=(const T& scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    template <typename U>
    TVec3& operator*=(const TVec3<U>& v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }

    // Div
    TVec3& operator/=(const T& scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    template <typename U>
    TVec3& operator/=(const TVec3<U>& v) {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }

    // IO operators
    friend std::ostream& operator<<(std::ostream& out, const TVec3& vec) {
        out << "{ " << vec.x << ", " << vec.y << ", " << vec.z << " }";
        return out;
    }

    friend std::wostream& operator<<(std::wostream& out, const TVec3& vec) {
        out << L"{ " << vec.x << L", " << vec.y << L", " << vec.z << L" }";
        return out;
    }

    // Functions
    T Length() const {
        return sqrt(Dot(*this));
    }

    TVec3& Normalize() {
        T scale = 1 / Length();
        *this *= scale;
        return *this;
    }

    TVec3 Normalized() const {
        return TVec3(*this).Normalize();
    }

    template <typename U>
    T Dot(const TVec3<U>& v) const {
        return x * static_cast<T>(v.x) + y * static_cast<T>(v.y) + z * static_cast<T>(v.z);
    }

    template <typename U>
    TVec3 Cross(const TVec3<U>& v) const {
        return TVec3(
            y * v.z - z * v.y,
            x * v.z - z * v.x,
            x * v.y - y * v.x
        );
    }
};

// Binary arithmetic operators
// Sum
template <typename T>
TVec3<T> operator+(const TVec3<T>& v, const T& scalar) {
    return TVec3<T>(v) += scalar;
}

template <typename T>
TVec3<T> operator+(const T& scalar, const TVec3<T>& v) {
    return TVec3<T>(v) += scalar;
}

template <typename T, typename U>
TVec3<T> operator+(const TVec3<T>& v1, const TVec3<U>& v2) {
    return TVec3<T>(v1) += v2;
}

// Sub
template <typename T>
TVec3<T> operator-(const TVec3<T>& v, const T& scalar) {
    return TVec3<T>(v) -= scalar;
}

template <typename T, typename U>
TVec3<T> operator-(const TVec3<T>& v1, const TVec3<U>& v2) {
    return TVec3<T>(v1) -= v2;
}

// Mul
template <typename T>
TVec3<T> operator*(const TVec3<T>& v, const T& scalar) {
    return TVec3<T>(v) *= scalar;
}

template <typename T>
TVec3<T> operator*(const T& scalar, const TVec3<T>& v) {
    return TVec3<T>(v) *= scalar;
}

template <typename T, typename U>
TVec3<T> operator*(const TVec3<T>& v1, const TVec3<U>& v2) {
    return TVec3<T>(v1) *= v2;
}

// Div
template <typename T>
TVec3<T> operator/(const TVec3<T>& v, const T& scalar) {
    return TVec3<T>(v) /= scalar;
}

template <typename T, typename U>
TVec3<T> operator/(const TVec3<T>& v1, const TVec3<U>& v2) {
    return TVec3<T>(v1) /= v2;
}
