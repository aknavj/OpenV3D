#include "mathlib.h"

/*
 */
void MatrixMultiplyVector(V3d::vec3 &i, V3d::vec3 &o, V3d::mat4 &m) {
	o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
	o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
	o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];

	float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

	if (w != 0.0f) {
		o.x /= w; o.y /= w; o.z /= w;
	}
}

/*
 */
V3d::mat4 MatrixProjection(float fov, float aspectratio, float near, float far) {
	V3d::mat4 m;
	float rad = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159f);

	m.m[0][0] = aspectratio * rad;
	m.m[1][1] = rad;
	m.m[2][2] = far / (far - near);
	m.m[3][2] = (-far * near) / (far - near);
	m.m[2][3] = 1.0f;
	m.m[3][3] = 0.0f;

	return m;
}

/*
 */