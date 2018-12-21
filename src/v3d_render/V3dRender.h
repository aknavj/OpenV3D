#ifndef __V3D_RENDER_H__
#define __V3D_RENDER_H__

#ifdef _WIN32
	#include <windows.h>
#else

#endif

#include "MathLib.h"

/*
 */
class V3DRender {

	public:

		V3DRender(int width, int height);
		virtual ~V3DRender();

		void Clear(V3d::pixel &pix);
		void SwapBuffers();

		void RenderPixel(int x, int y, V3d::pixel pix);
		void RenderPixel(V3d::vec2 p0, V3d::pixel pix);

		void RenderClip(int &x, int &y);

		void RenderFill(int x1, int y1, int x2, int y2, V3d::pixel pix);
		void RenderFill(V3d::vec2 p0, V3d::vec2 p1, V3d::pixel pix);

		void RenderLine(int x1, int y1, int x2, int y2, V3d::pixel pix);
		void RenderLine(V3d::vec2 p0, V3d::vec2 p1, V3d::pixel pix);

		void RenderTriangle(int x1, int y1, int x2, int y2, int x3, int y3, V3d::pixel pix);
		void RenderTriangle(V3d::vec2 p0, V3d::vec2 p1, V3d::vec2 p2, V3d::pixel pix);

		void RenderFillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, V3d::pixel pix);
		void RenderFillTriangle(V3d::vec2 p0, V3d::vec2 p1, V3d::vec2 p2, V3d::pixel pix);

		void RenderTextureTriangle(int x1, int y1, int x2, int y2, int x3, int y3, V3d::texture tex);
		void RenderTextureTriangle(V3d::vec2 p0, V3d::vec2 p1, V3d::vec2 p2, V3d::texture tex);

		void RenderCircle(int x, int y, int r, V3d::pixel pix);
		void RenderCircle(V3d::vec2 p0, int r, V3d::pixel pix);

		void RenderFillCircle(int x, int y, int r, V3d::pixel pix);
		void RenderFillCircle(V3d::vec2 p0, int r, V3d::pixel pix);

		char *GetRender();

		int GetWidth();
		int GetHeight();

	private:
		V3d::pixel *primaryBuffer;
		V3d::pixel *secondaryBuffer;
		V3d::pixel *currentBuffer;

		char bufferFlag;

		int resWidth, resHeight;
};

#endif /* __V3D_RENDER_H__ */