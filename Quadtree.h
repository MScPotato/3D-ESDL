#pragma once
#include "ModelHandler.h"
#include "Frustum.h"

class Quadtree
{
	struct Node
	{
		float positionX, positionZ, width; //center of scen and the distans to the sides.
		//int nrOfObj; //id?
		//ID3D11Buffer* ObjVertexBuffer;
		Model* objInQuad = nullptr;
		std::vector<int> objIndex;
		Node* nodeChild[4];
	};
private:
	ID3D11DeviceContext* gDeviceContext;
	ID3D11ShaderResourceView* gTextureSRV;
	ID3D11Buffer* constantBuffer;

	Node* rootNode;
	int drawCount;
public:
	Quadtree();
	~Quadtree();
	void InitTree(ID3D11DeviceContext* gDeviceContext, ID3D11ShaderResourceView* gTextureSRV, ID3D11Buffer* constantBuffer, ModelHandler* objHandler, float width);

	void CreateTreeNode(ModelHandler* objHandler, Node* node, float posX, float posZ, float width);
	int countObj(ModelHandler* objHandler, Node* node, float posX, float posZ, float width);
	bool isObjContained(Model Obj, float posX, float posZ, float width);

	void render(Frustum* fov, Constantbuffer &constBuffData, bool shadow);
	void renderNode(Node* node, Frustum* fov, Constantbuffer &constBuffData, bool shadow);
};

