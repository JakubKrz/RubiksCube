#pragma once
#include <cmath>
#include <cstring>

struct Vec2 {
	float x, y;
	Vec2(float x = 0, float y = 0) : x(x), y(y) {}
};

struct Vec3 {
	float x, y, z;
	Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
	float Length() const {
		return std::sqrt(x * x + y * y + z * z);
	}
	void Normalize() {
		float len = Length();
		if (len > 0.00001f) {
			x /= len;
			y /= len;
			z /= len;
		}
	}
	Vec3 Normalized() const {
		float len = Length();
		if (len > 0.00001f) {
			return Vec3(x / len, y / len, z / len);
		}
		return Vec3(0.0f, 0.0f, 0.0f);
	}
	Vec3 operator*(const float& other)
	{
		return Vec3(x * other, y * other, z * other);
	}
};

struct Mat4 {
	float m[16];

	Mat4() { std::memset(m, 0, sizeof(m)); }

	static Mat4 Identity() {
		Mat4 res;
		res.m[0] = 1.0f;
		res.m[5] = 1.0f;
		res.m[10] = 1.0f;
		res.m[15] = 1.0f;
		return res;
	}

	Mat4 operator*(const Mat4& other) const {
		Mat4 res;
		for (int col = 0; col < 4; col++) {
			for (int row = 0; row < 4; row++) {
				res.m[col * 4 + row] =
					m[0 * 4 + row] * other.m[col * 4 + 0] +
					m[1 * 4 + row] * other.m[col * 4 + 1] +
					m[2 * 4 + row] * other.m[col * 4 + 2] +
					m[3 * 4 + row] * other.m[col * 4 + 3];
			}
		}
		return res;
	}


	static Mat4 Translation(const Vec3& v) {
		Mat4 res = Identity();
		res.m[12] = v.x; res.m[13] = v.y; res.m[14] = v.z;
		return res;
	}

	static Mat4 RotationY(float angleDeg) {
		float rad = angleDeg * 0.01745329f;
		Mat4 res = Identity();
		res.m[0] = cosf(rad);  res.m[8] = sinf(rad);
		res.m[2] = -sinf(rad); res.m[10] = cosf(rad);
		return res;
	}

	static Mat4 Perspective(float fovDeg, float aspect, float zNear, float zFar) {
		float fovRad = fovDeg * 0.01745329f;
		float tanHalfFov = tanf(fovRad / 2.0f);
		Mat4 res;
		res.m[0] = 1.0f / (aspect * tanHalfFov);
		res.m[5] = 1.0f / tanHalfFov;
		res.m[10] = -(zFar + zNear) / (zFar - zNear);
		res.m[11] = -1.0f;
		res.m[14] = -(2.0f * zFar * zNear) / (zFar - zNear);
		return res;
	}
};

struct Quat {
	float x, y, z, w;

	Quat(float x = 0, float y = 0, float z = 0, float w = 1) : x(x), y(y), z(z), w(w) {}

	static Quat Identity() { return Quat(0, 0, 0, 1); }

	static Quat FromAxisAngle(const Vec3& axis, float angleDeg) {
		float rad = angleDeg * 0.01745329f * 0.5f;
		float s = sinf(rad);
		return Quat(axis.x * s, axis.y * s, axis.z * s, cosf(rad));
	}

	Quat operator*(const Quat& q) const {
		return Quat(
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y + y * q.w + z * q.x - x * q.z,
			w * q.z + z * q.w + x * q.y - y * q.x,
			w * q.w - x * q.x - y * q.y - z * q.z
		);
	}

	Mat4 ToMat4() const {
		Mat4 res = Mat4::Identity();
		float xx = x * x, yy = y * y, zz = z * z;
		float xy = x * y, xz = x * z, yz = y * z;
		float wx = w * x, wy = w * y, wz = w * z;

		res.m[0] = 1.0f - 2.0f * (yy + zz);
		res.m[1] = 2.0f * (xy + wz);
		res.m[2] = 2.0f * (xz - wy);

		res.m[4] = 2.0f * (xy - wz);
		res.m[5] = 1.0f - 2.0f * (xx + zz);
		res.m[6] = 2.0f * (yz + wx);

		res.m[8] = 2.0f * (xz + wy);
		res.m[9] = 2.0f * (yz - wx);
		res.m[10] = 1.0f - 2.0f * (xx + yy);

		return res;
	}

	static Quat Slerp(Quat a, Quat b, float t) {
		float cosHalfTheta = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
		if (cosHalfTheta < 0) {
			b = Quat(-b.x, -b.y, -b.z, -b.w);
			cosHalfTheta = -cosHalfTheta;
		}
		if (abs(cosHalfTheta) >= 1.0f) return a;

		float halfTheta = acosf(cosHalfTheta);
		float sinHalfTheta = sqrtf(1.0f - cosHalfTheta * cosHalfTheta);

		if (abs(sinHalfTheta) < 0.001f) {
			return Quat(a.x * 0.5f + b.x * 0.5f, a.y * 0.5f + b.y * 0.5f,
				a.z * 0.5f + b.z * 0.5f, a.w * 0.5f + b.w * 0.5f);
		}

		float ratioA = sinf((1 - t) * halfTheta) / sinHalfTheta;
		float ratioB = sinf(t * halfTheta) / sinHalfTheta;
		return Quat(a.x * ratioA + b.x * ratioB, a.y * ratioA + b.y * ratioB,
			a.z * ratioA + b.z * ratioB, a.w * ratioA + b.w * ratioB);
	}
};

struct DualQuat {
	Quat real;
	Quat dual;

	DualQuat() : real(0, 0, 0, 1), dual(0, 0, 0, 0) {}

	static DualQuat FromRotationTranslation(const Quat& r, const Vec3& t) {
		DualQuat dq;
		dq.real = r;

		Quat tQuat(t.x, t.y, t.z, 0);
		dq.dual = (tQuat * r);
		dq.dual.x *= 0.5f; dq.dual.y *= 0.5f; dq.dual.z *= 0.5f; dq.dual.w *= 0.5f;
		return dq;
	}

	void Normalize() {
		float mag = sqrtf(real.w * real.w + real.x * real.x + real.y * real.y + real.z * real.z);
		real.x /= mag; real.y /= mag; real.z /= mag; real.w /= mag;
		dual.x /= mag; dual.y /= mag; dual.z /= mag; dual.w /= mag;
	}

	DualQuat operator*(const DualQuat& other) const {
		DualQuat res;
		res.real = real * other.real;

		Quat r1d2 = real * other.dual;
		Quat d1r2 = dual * other.real;
		res.dual = Quat(r1d2.x + d1r2.x, r1d2.y + d1r2.y, r1d2.z + d1r2.z, r1d2.w + d1r2.w);
		return res;
	}

	Mat4 ToMat4() const {
		Mat4 res = real.ToMat4();

		Quat r_conj(-real.x, -real.y, -real.z, real.w);
		Quat t = (dual * r_conj);

		res.m[12] = t.x * 2.0f;
		res.m[13] = t.y * 2.0f;
		res.m[14] = t.z * 2.0f;

		return res;
	}
};