#include <vector>
#include "Floor.hh"

Floor::Floor(int size, int numcell)
{
	this->size = size;
	this->ncell = numcell;

	setup(size, numcell);
}

void Floor::setup(float size, int nSquares)
{
	std::vector <glm::vec3> vlists;
	std::vector <glm::vec3> clists;

	// parameters:
	float maxX = size / 2, maxY = size / 2;
	float minX = -size / 2, minY = -size / 2;

	int x, y, v[3], i;
	float xp, yp, xd, yd;
	v[2] = 0;
	xd = (maxX - minX) / ((float)nSquares);
	yd = (maxY - minY) / ((float)nSquares);

	for (x = 0, xp = minX; x < nSquares; x++, xp += xd) {
		for (y = 0, yp = minY, i = x; y < nSquares; y++, i++, yp += yd) {
			clists.push_back(glm::vec3(0, 1, 0));
			clists.push_back(glm::vec3(0, 1, 0));
			clists.push_back(glm::vec3(0, 1, 0));
			clists.push_back(glm::vec3(0, 1, 0));
			clists.push_back(glm::vec3(0, 1, 0));
			clists.push_back(glm::vec3(0, 1, 0));

			vlists.push_back(glm::vec3(xp, -0.1, yp));
			vlists.push_back(glm::vec3(xp, -0.1, yp + yd));
			vlists.push_back(glm::vec3(xp + xd, -0.1, yp + yd));

			vlists.push_back(glm::vec3(xp, -0.1, yp));
			vlists.push_back(glm::vec3(xp + xd, -0.1, yp + yd));
			vlists.push_back(glm::vec3(xp + xd, -0.1, yp));
		} // end of for j
	}// end of for i

	nbVerts = vlists.size();

	//create vao
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	//create vbo for vertices
	glGenBuffers(1, &_vboPositions);
	glBindBuffer(GL_ARRAY_BUFFER, _vboPositions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vlists.size() * 3, vlists.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		0, // attribute
		3,                 // number of elements per vertex, here (x,y,z,1)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
	);
	glEnableVertexAttribArray(0);

	//create vbo for colors
	glGenBuffers(1, &vbo_cube_colors);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*clists.size() * 3, clists.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		1, // attribute
		3,                 // number of elements per vertex, here (R,G,B)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
	);
	glEnableVertexAttribArray(1);

	vlists.clear();
	clists.clear();

	glBindVertexArray(0);
}

void Floor::resize(int size, int numcell)
{
	setup(size, numcell);
}

void Floor::draw()
{
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, nbVerts * 3);
	glBindVertexArray(0);
}