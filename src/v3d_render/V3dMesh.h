#ifndef __V3D_MESH_H__
#define __V3D_MESH_H__

#include "MathLib.h"

/*
 */
namespace V3d {

	struct mesh {
		std::vector<triangle> triangles;
		std::vector<vec2> texcoord;
	};

}

/*
 */
V3d::mesh v3d_mesh_load_obj(std::string filename);

#endif /* __V3D_MESH_H__ */