#pragma once

#include "model.h"
#include <vector>
#include <span>
#include <ranges>

struct triangle{
	vertex &A, B, C;
};

struct indexTriangle{
	unsigned int A, B, C;
};

triangle createTriangle(std::span<unsigned int> indices, std::vector<vertex> &vertices){
	return {vertices[indices[0]], vertices[indices[1]], vertices[indices[2]]};
}

auto toTriangles(std::span<unsigned int> indices){
	auto const triangleArray = std::views::iota(0u, indices.size() / 3u) | std::views::transform
    (
        [pos = indices.data()](const unsigned int i)
        {
            return indexTriangle{
                pos[3 * i    ],
                pos[3 * i + 1],
                pos[3 * i + 2],
            };
        }
    );
    return triangleArray;
}

// indexTriangle nthTriangle(unsigned int i, std::span<unsigned int> indices){
// 	return {
// 		indices[3 * i],
// 		indices[3 * i + 1],
// 		indices[3 * i + 2]
// 	};
// }

struct box{
	glm::vec3 min;
	glm::vec3 max;	
};

box createBox(triangle t){
	return{
		{
			std::min(t.A.pos.x, std::min(t.B.pos.x, t.C.pos.x)),
			std::min(t.A.pos.y, std::min(t.B.pos.y, t.C.pos.y)),
			std::min(t.A.pos.z, std::min(t.B.pos.z, t.C.pos.z))
		},
		{
			std::max(t.A.pos.x, std::max(t.B.pos.x, t.C.pos.x)),
			std::max(t.A.pos.y, std::max(t.B.pos.y, t.C.pos.y)),
			std::max(t.A.pos.z, std::max(t.B.pos.z, t.C.pos.z))
		}
	};
}

box operator+(box b1, box b2){
	return{
		{
			std::min(b1.min.x, b2.min.x),
			std::min(b1.min.y, b2.min.y),
			std::min(b1.min.z, b2.min.z)
		},
		{
			std::max(b1.min.x, b2.min.x),
			std::max(b1.min.y, b2.min.y),
			std::max(b1.min.z, b2.min.z)
		}
	};
}

box createBox(vertex v){
	return {v.pos, v.pos};
}

box createBox(std::span<unsigned int> indices, std::vector<vertex> &vertices){
	box result = createBox(vertices[indices[0]]);

	for (int i = 0; i < indices.size(); ++i){
		result = result + createBox(vertices[indices[i]]);
	}
	return result;
}

box createBox(model m){
	box result = createBox(std::span(m.meshes[0].indices.data(), m.meshes[0].indices.size()), m.meshes[0].vertices);

	for (int i = 1; i < m.meshes.size(); ++i){
		result = result + createBox(std::span(m.meshes[i].indices.data(), m.meshes[i].indices.size()), m.meshes[i].vertices);
	}
	return result;
}

std::pair<box, box> splitBox(std::span<unsigned int> indices, std::vector<vertex> &vertices, box origin){

}

struct tree{
	std::vector<box> boxes;
	std::vector<triangle> triangles;
};

tree continueTree(std::span<unsigned int> indices, std::vector<vertex> &vertices, tree t){
	if(indices.size() == 3){
		t.triangles.push_back(createTriangle(indices, vertices));
	}	
}