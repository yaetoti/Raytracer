#pragma once

#include <cassert>
#include <ostream>

template <typename T>
struct Vec4;

template <typename T>
struct Vec3 final {
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
    Vec3()
    : Vec3(0) {
    }

    Vec3(const Vec3& v)
    : x(v.x), y(v.y), z(v.z) {
    }

    // Value constructors
    explicit Vec3(const T& v1)
    : x(v1), y(v1), z(v1) {
    }

    explicit Vec3(const T& v1, const T& v2, const T& v3)
    : x(v1), y(v2), z(v3) {
    }

    // Conversion constructors
    template <typename U>
    explicit Vec3(const Vec3<U>& v)
    : x(v.x), y(v.y), z(v.z) {
    }

    template <typename U>
    explicit Vec3(const Vec4<U>& v)
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
    Vec3& operator=(const Vec3& v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    template <typename U>
    Vec3& operator=(const Vec3<U>& v) {
        x = static_cast<T>(v.x);
        y = static_cast<T>(v.y);
        z = static_cast<T>(v.z);
        return *this;
    }

    template <typename U>
    Vec3& operator=(const Vec4<U>& v) {
        x = static_cast<T>(v.x);
        y = static_cast<T>(v.y);
        z = static_cast<T>(v.z);
        return *this;
    }

    Vec3& operator+() const {
        return *this;
    }

    Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }

    // Sum
    Vec3& operator+=(const T& scalar) {
        x += scalar;
        y += scalar;
        z += scalar;
        return *this;
    }

    template <typename U>
    Vec3& operator+=(const Vec3<U>& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    // Sub
    Vec3& operator-=(const T& scalar) {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        return *this;
    }

    template <typename U>
    Vec3& operator-=(const Vec3<U>& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    // Mul
    Vec3& operator*=(const T& scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    template <typename U>
    Vec3& operator*=(const Vec3<U>& v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }

    // Div
    Vec3& operator/=(const T& scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    template <typename U>
    Vec3& operator/=(const Vec3<U>& v) {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }

    // IO operators
    friend std::ostream& operator<<(std::ostream& out, const Vec3& vec) {
        out << "{ " << vec.x << ", " << vec.y << ", " << vec.z << " }";
        return out;
    }

    friend std::wostream& operator<<(std::wostream& out, const Vec3& vec) {
        out << L"{ " << vec.x << L", " << vec.y << L", " << vec.z << L" }";
        return out;
    }

    // Functions
    T Length() const {
        return sqrt(SquaredLength());
    }

    T SquaredLength() const {
        return Dot(*this);
    }

    // TODO: inconsistent naming (infinitive = create new vector in other methods)
    Vec3& Normalize() {
        T scale = 1 / Length();
        *this *= scale;
        return *this;
    }

    Vec3 Normalized() const {
        return Vec3(*this).Normalize();
    }

    template <typename U>
    T Dot(const Vec3<U>& v) const {
        return x * static_cast<T>(v.x) + y * static_cast<T>(v.y) + z * static_cast<T>(v.z);
    }

    template <typename U>
    Vec3 Cross(const Vec3<U>& v) const {
        return Vec3(
            y * v.z - z * v.y,
            x * v.z - z * v.x,
            x * v.y - y * v.x
        );
    }

    template <typename U>
    Vec3 Reflect(const Vec3<U>& normal) const {
        return *this - 2 * Dot(normal) * normal;
    }

    static Vec3 RandomInUnitSphere(std::uniform_real_distribution<float>& rand, std::mt19937& gen) {
        Vec3 v;
        do {
            v = 2.0 * Vec3(rand(gen), rand(gen), rand(gen)) - Vec3(1.0);
        } while (v.SquaredLength() > 1.0);

        return v;
    }
};

// Binary arithmetic operators
// Sum
template <typename T, typename U>
Vec3<T> operator+(const Vec3<T>& v, const U& scalar) {
    return Vec3<T>(v) += scalar;
}

template <typename T, typename U>
Vec3<T> operator+(const U& scalar, const Vec3<T>& v) {
    return Vec3<T>(v) += scalar;
}

template <typename T, typename U>
Vec3<T> operator+(const Vec3<T>& v1, const Vec3<U>& v2) {
    return Vec3<T>(v1) += v2;
}

// Sub
template <typename T, typename U>
Vec3<T> operator-(const Vec3<T>& v, const U& scalar) {
    return Vec3<T>(v) -= scalar;
}

template <typename T, typename U>
Vec3<T> operator-(const Vec3<T>& v1, const Vec3<U>& v2) {
    return Vec3<T>(v1) -= v2;
}

// Mul
template <typename T, typename U>
Vec3<T> operator*(const Vec3<T>& v, const U& scalar) {
    return Vec3<T>(v) *= scalar;
}

template <typename T, typename U>
Vec3<T> operator*(const U& scalar, const Vec3<T>& v) {
    return Vec3<T>(v) *= scalar;
}

template <typename T, typename U>
Vec3<T> operator*(const Vec3<T>& v1, const Vec3<U>& v2) {
    return Vec3<T>(v1) *= v2;
}

// Div
template <typename T, typename U>
Vec3<T> operator/(const Vec3<T>& v, const U& scalar) {
    return Vec3<T>(v) /= scalar;
}

template <typename T, typename U>
Vec3<T> operator/(const Vec3<T>& v1, const Vec3<U>& v2) {
    return Vec3<T>(v1) /= v2;
}
