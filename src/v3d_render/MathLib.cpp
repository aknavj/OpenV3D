#include "mathlib.h"

/*
 */
void MatrixMultiply(V3d::vec3 &i, V3d::vec3 &o, V3d::mat4 &m) {

}

/*
 */
V3d::vec3 MatrixMultiplyVector(V3d::mat4 &m, V3d::vec3 &i) {
	V3d::vec3 v;
	v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
	v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
	v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
	v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
	return v;
}

/*
 */
V3d::mat4 MatrixIdentity() {
	V3d::mat4 m;
	m.m[0][0] = 1.0f;
	m.m[1][1] = 1.0f;
	m.m[2][2] = 1.0f;
	m.m[3][3] = 1.0f;
	return m;
}

/*
 */
V3d::mat4 MatrixRotationX(float fAngleRad) {
	V3d::mat4 m;
	m.m[0][0] = 1.0f;
	m.m[1][1] = cosf(fAngleRad);
	m.m[1][2] = sinf(fAngleRad);
	m.m[2][1] = -sinf(fAngleRad);
	m.m[2][2] = cosf(fAngleRad);
	m.m[3][3] = 1.0f;
	return m;
}

/*
 */
V3d::mat4 MatrixRotationY(float fAngleRad) {
	V3d::mat4 m;
	m.m[0][0] = cosf(fAngleRad);
	m.m[0][2] = sinf(fAngleRad);
	m.m[2][0] = -sinf(fAngleRad);
	m.m[1][1] = 1.0f;
	m.m[2][2] = cosf(fAngleRad);
	m.m[3][3] = 1.0f;
	return m;
}

/*
 */
V3d::mat4 MatrixRotationZ(float fAngleRad) {
	V3d::mat4 m;
	m.m[0][0] = cosf(fAngleRad);
	m.m[0][1] = sinf(fAngleRad);
	m.m[1][0] = -sinf(fAngleRad);
	m.m[1][1] = cosf(fAngleRad);
	m.m[2][2] = 1.0f;
	m.m[3][3] = 1.0f;
	return m;
}

/*
 */
V3d::mat4 MatrixTranslation(float x, float y, float z) {
	V3d::mat4 m;
	m.m[0][0] = 1.0f;
	m.m[1][1] = 1.0f;
	m.m[2][2] = 1.0f;
	m.m[3][3] = 1.0f;
	m.m[3][0] = x;
	m.m[3][1] = y;
	m.m[3][2] = z;
	return m;
}

/*
 */
V3d::mat4 MatrixProjection(float fov, float aspect_ratio, float near, float far) {
	V3d::mat4 m;
	float fov_rad = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159f);
	m.m[0][0] = aspect_ratio * fov_rad;
	m.m[1][1] = fov_rad;
	m.m[2][2] = far / (far - near);
	m.m[3][2] = (-far * near) / (far - near);
	m.m[2][3] = 1.0f;
	m.m[3][3] = 0.0f;
	return m;
}

/*
 */
V3d::mat4 MatrixMultiply(V3d::mat4 &m1, V3d::mat4 &m2) {
	V3d::mat4 m;
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			m.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
	return m;
}

V3d::mat4 MatrixPointAt(V3d::vec3 &pos, V3d::vec3 &target, V3d::vec3 &up) {

	// calculate new forward direction
	V3d::vec3 newForward = VectorSub(target, pos);
	newForward = VectorNormalize(newForward);

	// calculate new Up direction
	V3d::vec3 a = VectorMul(newForward, VectorDotProduct(up, newForward));
	V3d::vec3 newUp = VectorSub(up, a);
	newUp = VectorNormalize(newUp);

	// new right direction is easy, its just cross product
	V3d::vec3 newRight = VectorCrossProduct(newUp, newForward);

	// construct dimensioning and Translation Matrix	
	V3d::mat4 matrix;
	matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
	return matrix;

}

/* Only for Rotation/Translation Matrices
 */
V3d::mat4 MatrixQuickInverse(V3d::mat4 &m) {
	V3d::mat4 matrix;
	matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

/*
 */
V3d::vec3 VectorAdd(V3d::vec3 &v1, V3d::vec3 &v2) {
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

/*
 */
V3d::vec3 VectorSub(V3d::vec3 &v1, V3d::vec3 &v2) {
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

/*
 */
V3d::vec3 VectorMul(V3d::vec3 &v1, float k) {
	return { v1.x * k, v1.y * k, v1.z * k };
}

/*
 */
V3d::vec3 VectorDiv(V3d::vec3 &v1, float k) {
	return { v1.x / k, v1.y / k, v1.z / k };
}

/*
 */
float VectorDotProduct(V3d::vec3 &v1, V3d::vec3 &v2) {
	return v1.x*v2.x + v1.y*v2.y + v1.z * v2.z;
}

/*
 */
float VectorLength(V3d::vec3 &v) {
	return sqrtf(VectorDotProduct(v, v));
}

/*
 */
V3d::vec3 VectorNormalize(V3d::vec3 &v) {
	float l = VectorLength(v);
	return { v.x / l, v.y / l, v.z / l };
}

/*
 */
V3d::vec3 VectorCrossProduct(V3d::vec3 &v1, V3d::vec3 &v2) {
	V3d::vec3 v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	return v;
}

/*
 */
V3d::vec3 VectorIntersectPlane(V3d::vec3 &plane_p, V3d::vec3 &plane_n, V3d::vec3 &lineStart, V3d::vec3 &lineEnd) {
	plane_n = VectorNormalize(plane_n);
	float plane_d = -VectorDotProduct(plane_n, plane_p);
	float ad = VectorDotProduct(lineStart, plane_n);
	float bd = VectorDotProduct(lineEnd, plane_n);
	float t = (-plane_d - ad) / (bd - ad);

	V3d::vec3 lineStartToEnd = VectorSub(lineEnd, lineStart);
	V3d::vec3 lineToIntersect = VectorMul(lineStartToEnd, t);

	return VectorAdd(lineStart, lineToIntersect);
}

/*
 */
int TriangleClipAgainstPlane(V3d::vec3 plane_p, V3d::vec3 plane_n, V3d::triangle &in_tri, V3d::triangle &out_tri1, V3d::triangle &out_tri2) {
	// sure plane normal is indeed normal
	plane_n = VectorNormalize(plane_n);

	// return signed shortest distance from point to plane, plane normal must be Normalized
	auto dist = [&](V3d::vec3 &p) {
		V3d::vec3 n = VectorNormalize(p);
		return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - VectorDotProduct(plane_n, plane_p));
	};

	// create two temporary storage arrays to classify points either side of plane
	// if distance sign is positive, point lies on "inside" of plane
	V3d::vec3* inside_points[3];  int nInsidePointCount = 0;
	V3d::vec3* outside_points[3]; int nOutsidePointCount = 0;

	// Get signed distance of each point in triangle to plane
	float d0 = dist(in_tri.p[0]);
	float d1 = dist(in_tri.p[1]);
	float d2 = dist(in_tri.p[2]);

	if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[0]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.p[0]; }
	if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[1]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.p[1]; }
	if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[2]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.p[2]; }

	// now classify triangle points, and break the input triangle into 
	// smaller output triangles if required. There are four possible
	// outcomes...

	if (nInsidePointCount == 0) {
		// all points lie on the outside of plane, so clip whole triangle
		// it ceases to exist

		return 0; // No returned triangles are valid
	}

	if (nInsidePointCount == 3) {
		// all points lie on the inside of plane, so do nothing
		// and allow the triangle to simply pass through
		out_tri1 = in_tri;

		return 1; // Just the one returned original triangle is valid
	}

	if (nInsidePointCount == 1 && nOutsidePointCount == 2) {
		// triangle should be clipped. As two points lie outside
		// the plane, the triangle simply becomes a smaller triangle

		// copy appearance info to new triangle
		out_tri1.c = in_tri.c;

		// the inside point is valid, so keep that...
		out_tri1.p[0] = *inside_points[0];

		// but the two new points are at the locations where the 
		// original sides of the triangle (lines) intersect with the plane
		out_tri1.p[1] = VectorIntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
		out_tri1.p[2] = VectorIntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

		return 1; // return the newly formed single triangle
	}

	if (nInsidePointCount == 2 && nOutsidePointCount == 1) {
		// triangle should be clipped. As two points lie inside the plane,
		// the clipped triangle becomes a "quad". Fortunately, we can
		// represent a quad with two new triangles

		// copy appearance info to new triangles
		out_tri1.c = in_tri.c;
		out_tri2.c = in_tri.c;

		// the first triangle consists of the two inside points and a new
		// point determined by the location where one side of the triangle
		// intersects with the plane
		out_tri1.p[0] = *inside_points[0];
		out_tri1.p[1] = *inside_points[1];
		out_tri1.p[2] = VectorIntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

		// the second triangle is composed of one of he inside points, a
		// new point determined by the intersection of the other side of the 
		// triangle and the plane, and the newly created point above
		out_tri2.p[0] = *inside_points[1];
		out_tri2.p[1] = out_tri1.p[2];
		out_tri2.p[2] = VectorIntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

		return 2; // return two newly formed triangles which form a quad
	}
}