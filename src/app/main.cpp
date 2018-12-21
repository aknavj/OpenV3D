#include "V3dRender.h"
#include "V3dMesh.h"

#define WINDOW_NAME		L"WIN32 Aknav: V3d Software Rendering"
#define WINDOW_WIDTH	1280
#define WINDOW_HEIGHT	768

/*
 */
V3DRender v3d(WINDOW_WIDTH, WINDOW_HEIGHT);
V3d::mat4 projection_matrix;
V3d::mat4 world_matrix;

V3d::mesh mesh;
V3d::vec3 camera;
V3d::vec3 look_dir;

float fTheta;
float fYaw;
int xpos = 0, step = 5;

/*
 */
bool Initialize() {
	
	mesh.load_obj("ship.obj");

	projection_matrix = MatrixProjection(90.0f, (float)WINDOW_HEIGHT / (float)WINDOW_WIDTH, 0.1f, 1000.0f);

	return true;
}

void UpdateScene() {

}

/*
 */
void RenderScene() {

	// set up rotation matrices
	V3d::mat4 matRotZ, matRotX, matRotY;
	fTheta += 0.01f;
	
	matRotZ = MatrixRotationZ(0.0f);
	matRotX = MatrixRotationX(16.0f);
	matRotY = MatrixRotationY(fTheta);

	V3d::mat4 translation;
	translation = MatrixTranslation(0.0f, 0.0f, 10.0f);

	world_matrix = MatrixIdentity();
	world_matrix = MatrixMultiply(matRotZ, matRotX);
	world_matrix = MatrixMultiply(world_matrix, matRotY);
	world_matrix = MatrixMultiply(world_matrix, translation);

	V3d::vec3 up = { 0,1,0 };
	V3d::vec3 target = { 0,0,1 };
	V3d::mat4 camera_rot = MatrixRotationY(fYaw);
	look_dir = MatrixMultiplyVector(camera_rot, target);
	target = VectorAdd(camera, look_dir);
	V3d::mat4 camera_matrix = MatrixPointAt(camera, target, up);

	// make view matrix from camera
	V3d::mat4 view_matrix = MatrixQuickInverse(camera_matrix);

	// store triagles for rastering later
	std::vector<V3d::triangle> vecTrianglesToRaster;

	// draw triangles
	for (auto tri : mesh.triangles) {
		V3d::triangle triProjected, triTransformed, triViewed;

		// world Matrix Transform
		triTransformed.p[0] = MatrixMultiplyVector(world_matrix, tri.p[0]);
		triTransformed.p[1] = MatrixMultiplyVector(world_matrix, tri.p[1]);
		triTransformed.p[2] = MatrixMultiplyVector(world_matrix, tri.p[2]);
		triTransformed.t[0] = tri.t[0];
		triTransformed.t[1] = tri.t[1];
		triTransformed.t[2] = tri.t[2];

		// use Cross-Product to get surface normal
		V3d::vec3 normal, line1, line2;

		// get lines either side of triangle
		line1 = VectorSub(triTransformed.p[1], triTransformed.p[0]);
		line2 = VectorSub(triTransformed.p[2], triTransformed.p[0]);

		// take cross product of lines to get normal to triangle surface
		normal = VectorCrossProduct(line1, line2);

		// you normally need to Normalize a normal!
		normal = VectorNormalize(normal);

		// get Ray from triangle to camera
		V3d::vec3 camera_ray = VectorSub(triTransformed.p[0], camera);

		//if (normal.z < 0)
		if (VectorDotProduct(normal, camera_ray) < 0.0f) {
			// illumination
			V3d::vec3 light_direction = { 0.0f, 1.0f, -1.0f };
			light_direction = VectorNormalize(light_direction);

			// how "aligned" are light direction and triangle surface normal?
			float dp = max(0.1f, VectorDotProduct(light_direction, normal));

			V3d::pixel color = V3d::pixel(dp * 255.0f, dp * 255.0f, dp * 255.0f);
			triTransformed.c = color;

			// project triangles from 3D --> 2D
			triViewed.p[0] = MatrixMultiplyVector(view_matrix, triTransformed.p[0]);
			triViewed.p[1] = MatrixMultiplyVector(view_matrix, triTransformed.p[1]);
			triViewed.p[2] = MatrixMultiplyVector(view_matrix, triTransformed.p[2]);
			triViewed.t[0] = triTransformed.t[0];
			triViewed.t[1] = triTransformed.t[1];
			triViewed.t[2] = triTransformed.t[2];
			triViewed.c = triTransformed.c;

			// clipping triangles
			int nClippedTriangles = 0;
			V3d::triangle clipped[2];
			nClippedTriangles = TriangleClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

			// we may end up with multiple triangles form the clip, so project as
			// required
			for (int n = 0; n < nClippedTriangles; n++) {

				// Project triangles from 3D --> 2D
				triProjected.p[0] = MatrixMultiplyVector(projection_matrix, clipped[n].p[0]);
				triProjected.p[1] = MatrixMultiplyVector(projection_matrix, clipped[n].p[1]);
				triProjected.p[2] = MatrixMultiplyVector(projection_matrix, clipped[n].p[2]);
				triProjected.c = clipped[n].c;


				triProjected.t[0] = clipped[n].t[0];
				triProjected.t[1] = clipped[n].t[1];
				triProjected.t[2] = clipped[n].t[2];

				// U
				triProjected.t[0].x = triProjected.t[0].x / triProjected.p[0].w;
				triProjected.t[1].x = triProjected.t[1].x / triProjected.p[1].w;
				triProjected.t[2].x = triProjected.t[2].x / triProjected.p[2].w;

				// V
				triProjected.t[0].y = triProjected.t[0].y / triProjected.p[0].w;
				triProjected.t[1].y = triProjected.t[1].y / triProjected.p[1].w;
				triProjected.t[2].y = triProjected.t[2].y / triProjected.p[2].w;

				// W
				triProjected.t[0].w = 1.0f / triProjected.p[0].w;
				triProjected.t[1].w = 1.0f / triProjected.p[1].w;
				triProjected.t[2].w = 1.0f / triProjected.p[2].w;

				// scale into view, we moved the normalising into cartesian space
				// out of the matrix.vector function from the previous videos, so
				// do this manually
				triProjected.p[0] = VectorDiv(triProjected.p[0], triProjected.p[0].w);
				triProjected.p[1] = VectorDiv(triProjected.p[1], triProjected.p[1].w);
				triProjected.p[2] = VectorDiv(triProjected.p[2], triProjected.p[2].w);

				// X/Y are inverted so put them back
				triProjected.p[0].x *= -1.0f;
				triProjected.p[1].x *= -1.0f;
				triProjected.p[2].x *= -1.0f;
				triProjected.p[0].y *= -1.0f;
				triProjected.p[1].y *= -1.0f;
				triProjected.p[2].y *= -1.0f;

				// offset verts into visible Normalized space
				V3d::vec3 vOffsetView = { 1,1,0 };
				triProjected.p[0] = VectorAdd(triProjected.p[0], vOffsetView);
				triProjected.p[1] = VectorAdd(triProjected.p[1], vOffsetView);
				triProjected.p[2] = VectorAdd(triProjected.p[2], vOffsetView);
				triProjected.p[0].x *= 0.5f * (float)WINDOW_WIDTH;
				triProjected.p[0].y *= 0.5f * (float)WINDOW_HEIGHT;
				triProjected.p[1].x *= 0.5f * (float)WINDOW_WIDTH;
				triProjected.p[1].y *= 0.5f * (float)WINDOW_HEIGHT;
				triProjected.p[2].x *= 0.5f * (float)WINDOW_WIDTH;
				triProjected.p[2].y *= 0.5f * (float)WINDOW_HEIGHT;

				// store triangle for sorting
				vecTrianglesToRaster.push_back(triProjected);
			}
		}
	}

	// sort triangles from back to front
	std::sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](V3d::triangle &t1, V3d::triangle &t2)
	{
		float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
		float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
		return z1 > z2;
	});

	v3d.Clear(V3d::pixel(0, 0, 0));

	for (auto &triToRaster : vecTrianglesToRaster)
	{
		// clip triangles against all four screen edges, this could yield
		// a bunch of triangles, so create a queue that we traverse to 
		//  ensure we only test new triangles generated against planes
		V3d::triangle clipped[2];
		std::list<V3d::triangle> listTriangles;

		// add initial triangle
		listTriangles.push_back(triToRaster);
		int nNewTriangles = 1;

		for (int p = 0; p < 4; p++)
		{
			int nTrisToAdd = 0;
			while (nNewTriangles > 0)
			{
				// take triangle from front of queue
				V3d::triangle test = listTriangles.front();
				listTriangles.pop_front();
				nNewTriangles--;

				// clip it against a plane. We only need to test each 
				// subsequent plane, against subsequent new triangles
				// as all triangles after a plane clip are guaranteed
				// to lie on the inside of the plane. I like how this
				// comment is almost completely and utterly justified
				switch (p)
				{
					case 0:	nTrisToAdd = TriangleClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 1:	nTrisToAdd = TriangleClipAgainstPlane({ 0.0f, (float)WINDOW_HEIGHT - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 2:	nTrisToAdd = TriangleClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 3:	nTrisToAdd = TriangleClipAgainstPlane({ (float)WINDOW_WIDTH - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				}

				// clipping may yield a variable number of triangles, so
				// add these new ones to the back of the queue for subsequent
				// clipping against next planes
				for (int w = 0; w < nTrisToAdd; w++)
					listTriangles.push_back(clipped[w]);
			}
			nNewTriangles = listTriangles.size();
		}


		// draw the transformed, viewed, clipped, projected, sorted, clipped triangles
		for (auto &t : listTriangles)
		{
			/*
			TexturedTriangle(t.p[0].x, t.p[0].y, t.t[0].u, t.t[0].v, t.t[0].w,
				t.p[1].x, t.p[1].y, t.t[1].u, t.t[1].v, t.t[1].w,
				t.p[2].x, t.p[2].y, t.t[2].u, t.t[2].v, t.t[2].w, sprTex1);
				*/

			v3d.RenderFillTriangle(t.p[0].x, t.p[0].y,
				t.p[1].x, t.p[1].y,
				t.p[2].x, t.p[2].y,
				t.c);
		}
	}

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