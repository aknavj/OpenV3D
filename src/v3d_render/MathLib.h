#ifndef __V3D_MATHLIB_H__
#define __V3D_MATHLIB_H__

#include "Base.h"

/* V3d namespace
 */
namespace V3d {

	/* pixel struct
	 */
	struct pixel {

		pixel() : r(0), g(0), b(0) { }
		pixel(float R, float G, float B) : r(R), g(G), b(B) { }
		~pixel() { }

		void operator=(pixel &pix) {
			r = pix.r;
			g = pix.g;
			b = pix.b;
		}

		char r;
		char g;
		char b;
	};

	/* texture struct
	 */
	struct texture {
		std::vector<pixel> t;
	};

	/* 2d vector struct
	 */
	struct vec2 {
		float x = 0;
		float y = 0;
		float w = 1;
	};

	/* 3d vector struct
	 */
	struct vec3 {

		vec3() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) { }
		vec3(float X, float Y, float Z, float W = 1.0f) : x(X), y(Y), z(Z), w(W) { }
		~vec3() { }

		void operator=(vec3 &v) {
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;
		}

		float x;
		float y;
		float z;
		float w;
	};

	/* triangle struct
	 */
	struct triangle {

		// triangle clear
		triangle() {
			memset(p, 0, sizeof(vec3)); pixel(0, 0, 0);
		}

		// triangle points
		triangle(vec3 p0, vec3 p1, vec3 p2) {
			p[0] = p0; p[1] = p1; p[2] = p2; c = pixel(255, 255, 255);
		}

		// triangle points with color
		triangle(vec3 p0, vec3 p1, vec3 p2, pixel pix) {
			p[0] = p0; p[1] = p1; p[2] = p2; c = pix;
		}

		// triangle points with UV tex coords
		triangle(vec3 p0, vec3 p1, vec3 p2, vec2 u, vec2 v, vec2 w) {
			p[0] = p0; p[1] = p1; p[2] = p2; t[0] = u; t[1] = v; t[3] = w;
		}

		// triangle points with normal and color
		triangle(vec3 p0, vec3 p1, vec3 p2, vec3 n0, vec3 n1, vec3 n2, pixel pix) {
			p[0] = p0; p[1] = p1; p[2] = p2; n[0] = n0; n[1] = n1; n[2] = n2; c = pix;
		}
	
		// triangle points with normal and UV tex coords
		triangle(vec3 p0, vec3 p1, vec3 p2, vec3 n0, vec3 n1, vec3 n2, vec2 u, vec2 v) {
			p[0] = p0; p[1] = p1; p[2] = p2; n[0] = n0; n[1] = n1; n[2] = n2; t[0] = u; t[1] = v;
		}

		vec3 p[3];	// points
		vec3 n[3];	// normals
		vec2 t[3];	// uvs
		pixel c;	// color
	};

	/* mat4 struct
	 */
	struct mat4 {
		float m[4][4] = { 0 };
	};
} /* v3d namespace end */

/* MathLib functions
 */

// matrix functions
void MatrixMultiply(V3d::vec3 &i, V3d::vec3 &o, V3d::mat4 &m);
V3d::vec3 MatrixMultiplyVector(V3d::mat4 &m, V3d::vec3 &i);
V3d::mat4 MatrixIdentity();
V3d::mat4 MatrixRotationX(float angle);
V3d::mat4 MatrixRotationY(float angle);
V3d::mat4 MatrixRotationZ(float angle);
V3d::mat4 MatrixTranslation(float x, float y, float z);
V3d::mat4 MatrixProjection(float fov, float aspectratio, float near, float far);
V3d::mat4 MatrixMultiply(V3d::mat4 &m1, V3d::mat4 &m2);
V3d::mat4 MatrixPointAt(V3d::vec3 &pos, V3d::vec3 &target, V3d::vec3 &up);
V3d::mat4 MatrixQuickInverse(V3d::mat4 &m);

// vector functions
V3d::vec3 VectorAdd(V3d::vec3 &v1, V3d::vec3 &v2);
V3d::vec3 VectorSub(V3d::vec3 &v1, V3d::vec3 &v2);
V3d::vec3 VectorMul(V3d::vec3 &v1, float k);
V3d::vec3 VectorDiv(V3d::vec3 &v1, float k);
float VectorDotProduct(V3d::vec3 &v1, V3d::vec3 &v2);
float VectorLength(V3d::vec3 &v1);
V3d::vec3 VectorNormalize(V3d::vec3 &v);
V3d::vec3 VectorCrossProduct(V3d::vec3 &v1, V3d::vec3 &v2);
V3d::vec3 VectorIntersectPlane(V3d::vec3 &plane_p, V3d::vec3 &plane_n, V3d::vec3 &lineStart, V3d::vec3 &lineEnd);
int TriangleClipAgainstPlane(V3d::vec3 plane_p, V3d::vec3 plane_n, V3d::triangle &in_tri, V3d::triangle &out_tri1, V3d::triangle &out_tri2);

#endif /* __V3D_MATHLIB_H__ */