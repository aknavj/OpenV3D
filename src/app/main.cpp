#include "V3dRender.h"
#include "V3dMesh.h"

#define WINDOW_NAME		L"WIN32 Aknav: V3d Software Rendering"
#define WINDOW_WIDTH	1280
#define WINDOW_HEIGHT	768

/*
 */
V3DRender v3d(WINDOW_WIDTH, WINDOW_HEIGHT);
V3d::mat4 projection;
V3d::mat4 modelview;

V3d::mesh mesh;
V3d::vec3 camera;

float fTheta;
int xpos = 0, step = 5;

/*
 */
bool Initialize() {
	
	mesh = v3d_mesh_load_obj("ship.obj");

	projection = MatrixProjection(90.0f, (float)WINDOW_HEIGHT / (float)WINDOW_WIDTH, 0.1f, 1000.0f);

	return true;
}

void UpdateScene() {

}

/*
 */
void RenderScene() {
	v3d.Clear(V3d::pixel(0, 0, 0));

#ifdef LIBRARY_TEST
	xpos += step;

	if (xpos < 0 || xpos > WINDOW_WIDTH)
	step *= -1;

	v3d.RenderFill(10, 10, v3d.GetWidth(), v3d.GetHeight(), pixel(0, 255, 0));

	v3d.RenderLine(50, 100, 300, 300, pixel(0, 0, 255));

	v3d.RenderFillCircle(350, 350, 50, pixel(45, 45, 45));

	v3d.RenderFillTriangle(xpos + 100, 200, xpos - 100, 200, xpos, 300, pixel(255, 0, 255));
#else
	// set up rotation matrices
	V3d::mat4 matRotZ, matRotX;
	fTheta += 0.01f;
	
	// rotation Z
	matRotZ.m[0][0] = cosf(fTheta);
	matRotZ.m[0][1] = sinf(fTheta);
	matRotZ.m[1][0] = -sinf(fTheta);
	matRotZ.m[1][1] = cosf(fTheta);
	matRotZ.m[2][2] = 1;
	matRotZ.m[3][3] = 1;

	// rotation X
	matRotX.m[0][0] = 1;
	matRotX.m[1][1] = cosf(fTheta * 0.5f);
	matRotX.m[1][2] = sinf(fTheta * 0.5f);
	matRotX.m[2][1] = -sinf(fTheta * 0.5f);
	matRotX.m[2][2] = cosf(fTheta * 0.5f);
	matRotX.m[3][3] = 1;

#ifdef NO_SORT
	// draw triangles
	for (auto tri : mesh.triangles) {

		triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

		// rotate in Z-Axis
		MatrixMultiplyVector(tri.p[0], triRotatedZ.p[0], matRotZ);
		MatrixMultiplyVector(tri.p[1], triRotatedZ.p[1], matRotZ);
		MatrixMultiplyVector(tri.p[2], triRotatedZ.p[2], matRotZ);

		// rotate in X-Axis
		MatrixMultiplyVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
		MatrixMultiplyVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
		MatrixMultiplyVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

		// offset into the screen
		triTranslated = triRotatedZX;
		triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
		triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
		triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

		// project triangles from 3D --> 2D
		MatrixMultiplyVector(triTranslated.p[0], triProjected.p[0], projection);
		MatrixMultiplyVector(triTranslated.p[1], triProjected.p[1], projection);
		MatrixMultiplyVector(triTranslated.p[2], triProjected.p[2], projection);

		// scale into view
		triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
		triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
		triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
		triProjected.p[0].x *= 0.5f * (float)WINDOW_WIDTH;
		triProjected.p[0].y *= 0.5f  * (float)WINDOW_HEIGHT;
		triProjected.p[1].x *= 0.5f  * (float)WINDOW_WIDTH;
		triProjected.p[1].y *= 0.5f  * (float)WINDOW_HEIGHT;
		triProjected.p[2].x *= 0.5f  * (float)WINDOW_WIDTH;
		triProjected.p[2].y *= 0.5f  * (float)WINDOW_HEIGHT;

		// rasterize triangle
		v3d.RenderTriangle(triProjected.p[0].x, triProjected.p[0].y,
			triProjected.p[1].x, triProjected.p[1].y,
			triProjected.p[2].x, triProjected.p[2].y,
			pixel(255, 255, 255));

	}
#else
	// store triagles for rastering later
	std::vector<V3d::triangle> vecTrianglesToRaster;

	// draw triangles
	for (auto tri : mesh.triangles)
	{
		V3d::triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

		// rotate in Z-Axis
		MatrixMultiplyVector(tri.p[0], triRotatedZ.p[0], matRotZ);
		MatrixMultiplyVector(tri.p[1], triRotatedZ.p[1], matRotZ);
		MatrixMultiplyVector(tri.p[2], triRotatedZ.p[2], matRotZ);

		// rotate in X-Axis
		MatrixMultiplyVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
		MatrixMultiplyVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
		MatrixMultiplyVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

		// offset into the screen
		triTranslated = triRotatedZX;
		triTranslated.p[0].z = triRotatedZX.p[0].z + 8.0f;
		triTranslated.p[1].z = triRotatedZX.p[1].z + 8.0f;
		triTranslated.p[2].z = triRotatedZX.p[2].z + 8.0f;

		// use Cross-Product to get surface normal
		V3d::vec3 normal, line1, line2;

		line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
		line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
		line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

		line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
		line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
		line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

		normal.x = line1.y * line2.z - line1.z * line2.y;
		normal.y = line1.z * line2.x - line1.x * line2.z;
		normal.z = line1.x * line2.y - line1.y * line2.x;

		// It's normally normal to normalise the normal
		float l = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
		normal.x /= l; normal.y /= l; normal.z /= l;

		//if (normal.z < 0)
		if (normal.x * (triTranslated.p[0].x - camera.x) +
			normal.y * (triTranslated.p[0].y - camera.y) +
			normal.z * (triTranslated.p[0].z - camera.z) < 0.0f)
		{
			// illumination
			V3d::vec3 light_direction = V3d::vec3(0.0f, 0.0f, -1.0f);
			float l = sqrtf(light_direction.x*light_direction.x + light_direction.y*light_direction.y + light_direction.z*light_direction.z);
			light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

			// how similar is normal to light direction
			float dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;
			V3d::pixel color = V3d::pixel(dp * 255.0f, dp * 255.0f, dp * 255.0f);
			triTranslated.c = color;

			// project triangles from 3D --> 2D
			MatrixMultiplyVector(triTranslated.p[0], triProjected.p[0], projection);
			MatrixMultiplyVector(triTranslated.p[1], triProjected.p[1], projection);
			MatrixMultiplyVector(triTranslated.p[2], triProjected.p[2], projection);
			triProjected.c = triTranslated.c;

			// scale into view
			triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
			triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
			triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
			triProjected.p[0].x *= 0.5f  * (float)WINDOW_WIDTH;
			triProjected.p[0].y *= 0.5f  * (float)WINDOW_HEIGHT;
			triProjected.p[1].x *= 0.5f  * (float)WINDOW_WIDTH;
			triProjected.p[1].y *= 0.5f  * (float)WINDOW_HEIGHT;
			triProjected.p[2].x *= 0.5f  * (float)WINDOW_WIDTH;
			triProjected.p[2].y *= 0.5f  * (float)WINDOW_HEIGHT;

			// store triangle for sorting
			vecTrianglesToRaster.push_back(triProjected);
		}
	}

	// sort triangles from back to front
	std::sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](V3d::triangle &t1, V3d::triangle &t2)
	{
		float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
		float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
		return z1 > z2;
	});

	for (auto &triProjected : vecTrianglesToRaster)
	{
		// rasterize triangle
#define RENDER_SHADED
#ifdef RENDER_SHADED
		v3d.RenderFillTriangle(triProjected.p[0].x, triProjected.p[0].y,
			triProjected.p[1].x, triProjected.p[1].y,
			triProjected.p[2].x, triProjected.p[2].y,
			triProjected.c);
#elif defined RENDER_WIREFRAME
		v3d.RenderTriangle(triProjected.p[0].x, triProjected.p[0].y,
			triProjected.p[1].x, triProjected.p[1].y,
			triProjected.p[2].x, triProjected.p[2].y,
			triProjected.c);
#else
		v3d.RenderTriangle(triProjected.p[0].x, triProjected.p[0].y,
			triProjected.p[1].x, triProjected.p[1].y,
			triProjected.p[2].x, triProjected.p[2].y,
			pixel(255,0,0));
#endif
	}
#endif

#endif

	v3d.SwapBuffers();
}

/*
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp) {

	HDC hdc, canvasHdc;
	PAINTSTRUCT paintStruct;
	HBITMAP canvasBitmap;

	switch (m)
	{
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &paintStruct);
		canvasBitmap = CreateCompatibleBitmap(hdc, v3d.GetWidth(), v3d.GetHeight());

		BITMAPINFO bm;
		memset(&bm.bmiHeader, 0, sizeof(BITMAPINFOHEADER));
		bm.bmiHeader.biBitCount = 24;
		bm.bmiHeader.biCompression = BI_RGB;
		bm.bmiHeader.biHeight = v3d.GetHeight();
		bm.bmiHeader.biPlanes = 1;
		bm.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bm.bmiHeader.biWidth = v3d.GetWidth();

		SetDIBits(hdc, canvasBitmap, 0, v3d.GetHeight(), (void*)v3d.GetRender(), &bm, DIB_RGB_COLORS);
		canvasHdc = CreateCompatibleDC(hdc);
		SelectObject(canvasHdc, canvasBitmap);

		BitBlt(hdc, 0, 0, v3d.GetWidth(), v3d.GetHeight(), canvasHdc, 0, 0, SRCCOPY);

		DeleteDC(canvasHdc);
		DeleteObject(canvasBitmap);
		EndPaint(hwnd, &paintStruct);
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	case WM_KEYDOWN:
		switch (wp)
		{
		case VK_ESCAPE:
				PostQuitMessage(0);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	// Pass remaining messages to default handler.
	return (DefWindowProc(hwnd, m, wp, lp));
}

/*
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prev, LPSTR cmd, int show) {
	MSG msg;

	// Describes a window.
	WNDCLASSEX windowClass;
	memset(&windowClass, 0, sizeof(WNDCLASSEX));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WndProc;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	windowClass.lpszClassName = L"APPCLASS";
	windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&windowClass))
		return 0;

	// Create the window.
	HWND hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
		L"APPCLASS",
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW |
		WS_VISIBLE | WS_SYSMENU |
		WS_CLIPCHILDREN |
		WS_CLIPSIBLINGS,
		100, 100, WINDOW_WIDTH,
		WINDOW_HEIGHT, 0, 0,
		hInstance,
		NULL);

	if (!hwnd)
		return 0;

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	// If initialize fail, then we don't want the program to run.
	if (!Initialize()) {
		MessageBox(NULL, L"Error in initialize!", L"Error", MB_OK);
	} else {
		// This is the messsage loop.
		while (1) {
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				// If a quit message then break;
					if (msg.message == WM_QUIT)
						break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			} else {

				UpdateScene();
				RenderScene();

				InvalidateRect(hwnd, NULL, false);
			}
		}
	}
	// Unregister the class.
	UnregisterClass(L"APPCLASS", windowClass.hInstance);
	return (int)msg.wParam;
}