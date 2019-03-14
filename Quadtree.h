#pragma once
class Quadtree
{
	struct Node
	{
		float positionX, positionZ, width;
		int nrOfTriangles;
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;

		Node* nodeChild[4];
	};
private:

public:
	Quadtree();
	~Quadtree();

};

