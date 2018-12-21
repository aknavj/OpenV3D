#ifndef __V3D_MESH_H__
#define __V3D_MESH_H__

#include "MathLib.h"

/*
 */
namespace V3d {

	struct mesh {
		std::vector<triangle> triangles;
		//std::vector<vec2> texcoord;

		/*
		*/
		bool load_obj(std::string sFilename, bool hasTexture = false) {
			std::ifstream f(sFilename);
			if (!f.is_open())
				return false;

			// Local cache of verts
			std::vector<vec3> verts;
			std::vector<vec2> texcoords;

			while (!f.eof()) {
				char line[128];
				f.getline(line, 128);

				std::strstream s;
				s << line;

				char junk;

				if (line[0] == 'v') {
					if (line[1] == 't') {
						vec2 v;
						s >> junk >> junk >> v.x >> v.y;
						texcoords.push_back(v);
					} else {
						vec3 v;
						s >> junk >> v.x >> v.y >> v.z;
						verts.push_back(v);
					}
				}

				if (!hasTexture) {
					if (line[0] == 'f') {
						int f[3];
						s >> junk >> f[0] >> f[1] >> f[2];
						triangles.push_back(triangle(verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]));
					}
				} else {
					if (line[0] == 'f') {
						s >> junk;

						std::string tokens[6];
						int token_count = -1;

						while (!s.eof()) {
							char c = s.get();
							if (c == ' ' || c == '/')
								token_count++;
							else
								tokens[token_count].append(1, c);
						}

						tokens[token_count].pop_back();

						triangles.push_back(
							triangle(
								verts[stoi(tokens[0]) - 1], 
								verts[stoi(tokens[2]) - 1], 
								verts[stoi(tokens[4]) - 1],
								texcoords[stoi(tokens[1]) - 1], 
								texcoords[stoi(tokens[3]) - 1], 
								texcoords[stoi(tokens[5]) - 1]
							)
						);

					}
				}
			}

			return true;
		}

	};

}

#endif /* __V3D_MESH_H__ */