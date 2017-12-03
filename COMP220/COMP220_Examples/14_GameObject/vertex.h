#pragma once

struct Vertex
{
	float x, y, z;
	float r, g, b, a;
	float tu, tv;
	float normalX, normalY, normalZ;
	int boneIDs[4];
	float boneWeights[4];
};