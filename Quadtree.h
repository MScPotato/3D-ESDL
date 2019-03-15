#pragma once
#include "ModelHandler.h"

class Quadtree
{
	struct Node
	{
		float positionX, positionZ, width; //center of scen and the distans to the sides.
		int nrOfObj; //id?
		ID3D11Buffer* ObjVertexBuffer;

		//std::vector<int> objIndex;
		Node* nodeChild[4];
	};
private:
	Node* rootNode;
public:
	Quadtree();
	~Quadtree();
	void InitTree(ModelHandler* objHandler, float width);

	void CreateTreeNode(ModelHandler* objHandler, Node* node, float posX, float posZ, float width);
	int countObj(ModelHandler* objHandler, float posX, float posZ, float width);
	bool isObjContained(Model Obj, float posX, float posZ, float width);
};

