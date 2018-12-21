#include "V3DRender.h"

// - added double buffering

/*
 */
V3DRender::V3DRender(int width, int height) : resWidth(width), resHeight(height) {
	primaryBuffer	= new V3d::pixel[width * height];
	secondaryBuffer = new V3d::pixel[width * height];
	currentBuffer = primaryBuffer;
}

/*
 */
V3DRender::~V3DRender() {

	currentBuffer = NULL;

	if (primaryBuffer != NULL) {
		delete[] primaryBuffer;
		primaryBuffer = NULL;
	}

	if (secondaryBuffer != NULL) {
		delete[] secondaryBuffer;
		secondaryBuffer = NULL;
	}
}

/*
 */
void V3DRender::Clear(V3d::pixel &pix) {
	if (currentBuffer == NULL)
		return;

	for (int i = 0; i < resWidth * resHeight; i++) {
		currentBuffer[i] = pix;
	}
}

/*
 */
void V3DRender::SwapBuffers() {
	if (bufferFlag ^= 1) currentBuffer = primaryBuffer;
	else currentBuffer = secondaryBuffer;
}

/*
 */
char *V3DRender::GetRender() {
	if (currentBuffer == primaryBuffer) return (char*)secondaryBuffer;
	else return (char*)primaryBuffer;
}

/*
 */ 
void V3DRender::RenderPixel(int x, int y, V3d::pixel pix) {
	if (currentBuffer == NULL)
		return;

	int location = y * resWidth + x;

	if (location < 0 || location >= (resWidth * resHeight))
		return;
	
	currentBuffer[location] = pix;
}
void V3DRender::RenderPixel(V3d::vec2 p0, V3d::pixel pix) {
	RenderPixel(p0.x, p0.y, pix);
}

/*
 */
void V3DRender::RenderClip(int &x, int &y) {
	if (x < 0) x = 0;
	if (x >= resWidth) x = resWidth;
	if (y < 0) y = 0;
	if (y >= resHeight) y = resHeight;
}

/*
 */
void V3DRender::RenderFill(int x1, int y1, int x2, int y2, V3d::pixel pix) {
	RenderClip(x1, y1);
	RenderClip(x2, y2);
	for (int x = x1; x < x2; x++) {
		for (int y = y1; y < y2; y++) {
			RenderPixel(x, y, pix);
		}
	}
}
void V3DRender::RenderFill(V3d::vec2 p0, V3d::vec2 p1, V3d::pixel pix) {
	RenderFill(p0.x, p0.y, p1.x, p1.y, pix);
}

/*
 */
void V3DRender::RenderLine(int x1, int y1, int x2, int y2, V3d::pixel pix) {
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;

	dx = x2 - x1; dy = y2 - y1;
	dx1 = abs(dx); dy1 = abs(dy);
	px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;

	if (dy1 <= dx1) {
		if (dx >= 0) {
			x = x1; y = y1; xe = x2;
		}
		else {
			x = x2; y = y2; xe = x1;
		}

		RenderPixel(x, y, pix);

		for (i = 0; x<xe; i++) {
			x = x + 1;
			if (px<0) px = px + 2 * dy1;
			else {
				if ((dx<0 && dy<0) || (dx>0 && dy>0)) y = y + 1; else y = y - 1;
				px = px + 2 * (dy1 - dx1);
			}

			RenderPixel(x, y, pix);

		}
	}
	else {
		if (dy >= 0) {
			x = x1; y = y1; ye = y2;
		}
		else {
			x = x2; y = y2; ye = y1;
		}

		RenderPixel(x, y, pix);

		for (i = 0; y<ye; i++) {
			y = y + 1;
			if (py <= 0) py = py + 2 * dx1;
			else {
				if ((dx<0 && dy<0) || (dx>0 && dy>0)) x = x + 1; else x = x - 1;
				py = py + 2 * (dx1 - dy1);
			}

			RenderPixel(x, y, pix);

		}
	}
}
void V3DRender::RenderLine(V3d::vec2 p0, V3d::vec2 p1, V3d::pixel pix) {
	RenderLine(p0.x, p0.y, p1.x, p1.y, pix);
}

/*
 */
void V3DRender::RenderTriangle(int x1, int y1, int x2, int y2, int x3, int y3, V3d::pixel pix) {
	RenderLine(x1, y1, x2, y2, pix);
	RenderLine(x2, y2, x3, y3, pix);
	RenderLine(x3, y3, x1, y1, pix);
}
void V3DRender::RenderTriangle(V3d::vec2 p0, V3d::vec2 p1, V3d::vec2 p2, V3d::pixel pix) {
	RenderTriangle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, pix);
}

/* https://www.avrfreaks.net/sites/default/files/triangles.c
 */
void V3DRender::RenderFillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, V3d::pixel pix) {

	auto SWAP = [](int &x, int &y) { int t = x; x = y; y = t; };
	auto lambda_RenderLine = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) RenderPixel(i, ny, pix); };

	int t1x, t2x, y, minx, maxx, t1xp, t2xp;
	bool changed1 = false;
	bool changed2 = false;
	int signx1, signx2, dx1, dy1, dx2, dy2;
	int e1, e2;

	// sort vertices
	if (y1>y2) { SWAP(y1, y2); SWAP(x1, x2); }
	if (y1>y3) { SWAP(y1, y3); SWAP(x1, x3); }
	if (y2>y3) { SWAP(y2, y3); SWAP(x2, x3); }

	// starting points
	t1x = t2x = x1; y = y1;
	dx1 = (int)(x2 - x1); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y2 - y1);

	dx2 = (int)(x3 - x1); if (dx2<0) { dx2 = -dx2; signx2 = -1; }
	else signx2 = 1;
	dy2 = (int)(y3 - y1);

	// swap values
	if (dy1 > dx1) {
		SWAP(dx1, dy1);
		changed1 = true;
	}

	// swap values
	if (dy2 > dx2) {
		SWAP(dy2, dx2);
		changed2 = true;
	}

	e2 = (int)(dx2 >> 1);

	// flat top, just process the second half
	if (y1 == y2) goto next;
	e1 = (int)(dx1 >> 1);

	for (int i = 0; i < dx1;) {
		t1xp = 0; t2xp = 0;
		if (t1x<t2x) {
			minx = t1x; maxx = t2x;
		}
		else {
			minx = t2x; maxx = t1x;
		}

		// process first line until y value is about to change
		while (i<dx1) {
			i++;
			e1 += dy1;

			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) t1xp = signx1; //t1x += signx1;
				else          goto next1;
			}

			if (changed1) break;
			else t1x += signx1;
		}
		// move line
	next1:
		// process second line until y value is about to change
		while (1) {
			e2 += dy2;

			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;//t2x += signx2;
				else          goto next2;
			}

			if (changed2)     break;
			else              t2x += signx2;
		}

	next2:
		if (minx>t1x) minx = t1x; if (minx>t2x) minx = t2x;
		if (maxx<t1x) maxx = t1x; if (maxx<t2x) maxx = t2x;

		lambda_RenderLine(minx, maxx, y);    // draw line from min to max points found on the y

		// now increase y
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y == y2) break;

	}

next:
	// second half
	dx1 = (int)(x3 - x2); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y3 - y2);
	t1x = x2;

	if (dy1 > dx1) {   // swap values
		SWAP(dy1, dx1);
		changed1 = true;
	}
	else changed1 = false;

	e1 = (int)(dx1 >> 1);

	for (int i = 0; i <= dx1; i++) {
		t1xp = 0; t2xp = 0;
		if (t1x<t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i<dx1) {
			e1 += dy1;

			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) { t1xp = signx1; break; } //t1x += signx1;
				else          goto next3;
			}

			if (changed1) break;
			else   	   	  t1x += signx1;
			if (i<dx1) i++;
		}

	next3:
		// process second line until y value is about to change
		while (t2x != x3) {
			e2 += dy2;

			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;
				else          goto next4;
			}

			if (changed2)     break;
			else              t2x += signx2;
		}

	next4:

		if (minx>t1x) minx = t1x; if (minx>t2x) minx = t2x;
		if (maxx<t1x) maxx = t1x; if (maxx<t2x) maxx = t2x;

		lambda_RenderLine(minx, maxx, y);

		if (!changed1) t1x += signx1;
		t1x += t1xp;

		if (!changed2) t2x += signx2;
		t2x += t2xp;

		y += 1;
		if (y>y3) return;
	}
}
void V3DRender::RenderFillTriangle(V3d::vec2 p0, V3d::vec2 p1, V3d::vec2 p2, V3d::pixel pix) {
	RenderFillTriangle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, pix);
}

/*
 */
void V3DRender::RenderTextureTriangle(int x1, int y1, int x2, int y2, int x3, int y3, V3d::texture tex) {

}
void V3DRender::RenderTextureTriangle(V3d::vec2 p0, V3d::vec2 p1, V3d::vec2 p2, V3d::texture tex) {
	RenderTextureTriangle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, tex);
}

/*
 */
void V3DRender::RenderCircle(int x, int y, int r, V3d::pixel pix) {

	int _x = 0;
	int _y = r;

	int p = 3 - 2 * r;
	if (!r) return;

	// only formulate 1/8 of circle
	while (_y >= _x) {
		RenderPixel(x - _x, y - _y, pix); // upper left left
		RenderPixel(x - _y, y - _x, pix); // upper upper left
		RenderPixel(x + _y, y - _x, pix); // upper upper right
		RenderPixel(x + _x, y - _y, pix); // upper right right
		RenderPixel(x - _x, y + _y, pix); // lower left left
		RenderPixel(x - _y, y + _x, pix); // lower lower left
		RenderPixel(x + _y, y + _x, pix); // lower lower right
		RenderPixel(x + _x, y + _y, pix); // lower right right

		if (p < 0) p += 4 * _x++ + 6;
		else p += 4 * (_x++ - _y--) + 10;
	}
}
void V3DRender::RenderCircle(V3d::vec2 p0, int r, V3d::pixel pix) {
	RenderCircle(p0.x, p0.y, r, pix);
}

/*
 */
void V3DRender::RenderFillCircle(int x, int y, int r, V3d::pixel pix) {

	// taken from wikipedia
	int _x = 0;
	int _y = r;
	int p = 3 - 2 * r;
	if (!r) return;

	auto lambda_RenderLine = [&](int sx, int ex, int ny) {
		for (int i = sx; i <= ex; i++)
			RenderPixel(i, ny, pix);
	};

	while (_y >= _x) {

		// modified to draw scan-lines instead of edges
		lambda_RenderLine(x - _x, x + _x, y - _y);
		lambda_RenderLine(x - _y, x + _y, y - _x);
		lambda_RenderLine(x - _x, x + _x, y + _y);
		lambda_RenderLine(x - _y, x + _y, y + _x);

		if (p < 0) p += 4 * _x++ + 6;
		else p += 4 * (_x++ - _y--) + 10;
	}
}
void V3DRender::RenderFillCircle(V3d::vec2 p0, int r, V3d::pixel pix) {
	RenderFillCircle(p0.x, p0.y, r, pix);
}

/*
 */
int V3DRender::GetWidth() {
	return resWidth;
}

/*
 */
int V3DRender::GetHeight() {
	return resHeight;
}
