#include "V3dMesh.h"

/*
 */
V3d::mesh v3d_mesh_load_obj(std::string filename) {

	V3d::mesh m;

	std::ifstream f(filename);
	if (!f.is_open()) return m;

	// cache of verts
	std::vector<V3d::vec3> verts;

	while (!f.eof()) {
		char line[128];
		f.getline(line, 128);

		std::strstream s;
		s << line;

		char c;

		if (line[0] == 'v') {
			V3d::vec3 v;
			s >> c >> v.x >> v.y >> v.z;
			verts.push_back(v);
		}

		if (line[0] == 'f') {
			int f[3];
			s >> c >> f[0] >> f[1] >> f[2];
			m.triangles.push_back(V3d::triangle(verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]));
		}
	}

	return m;
}
