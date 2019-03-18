#include "pch.h"
#include "Quadtree.h"


Quadtree::Quadtree()
{
	rootNode = 0;
}


Quadtree::~Quadtree()
{
}

void Quadtree::InitTree(ID3D11DeviceContext* gDeviceContext, ID3D11ShaderResourceView* gTextureSRV, ID3D11Buffer* constantBuffer, ModelHandler* objHandler, float width)
{
	this->gDeviceContext = gDeviceContext;
	this->gTextureSRV = gTextureSRV;
	this->constantBuffer = constantBuffer;

	float centerX = 0.f;
	float centerZ = 0.f;

	rootNode = new Node;
	//rootNode->objIndex.resize(objHandler->getNrOfModels());
	
	CreateTreeNode(objHandler, rootNode, centerX, centerZ, width);
}

void Quadtree::CreateTreeNode(ModelHandler* objHandler, Node* node, float posX, float posZ, float width)
{
	// numObj how many objs are there in current quad, count determines if we need to split again
	int numObj, count;
	// offsets gives us the "origin" of next quad
	float offsetX, offsetZ;
	node->positionX = posX; // "origin" of current quad
	node->positionZ = posZ;
	node->width = width; // width of current quad

	// vadan är denna till???????????
	//node->nrOfObj = 0; 
	
	// behövs???? till render?
	//node->ObjVertexBuffer = nullptr;

	// The children
	node->nodeChild[0] = nullptr;
	node->nodeChild[1] = nullptr;
	node->nodeChild[2] = nullptr;
	node->nodeChild[3] = nullptr;

	// how many objs
	numObj = countObj(objHandler, node, posX, posZ, width);

	//Case 1: If there is no obj in the node
	if (numObj == 0)
		return;

	//Case 2: If there is too many obj in the node
	if (numObj > 1) //Max obj i en quad
	{
		for (int i = 0; i < 4; i++)
		{
			//calc the pos offsets for new child node
			//if (i % 2 < 1)
			//	offsetX = -1.f * (width / 4.f);;
			//else
			//	offsetX = 1.f * (width / 4.f);;

			//if (i % 2 < 1)
			//	offsetZ = -1.f * (width / 4.f);;
			//else
			//	offsetZ = 1.f * (width / 4.f);;

			offsetX = (((i % 2) < 1) ? -1.f : 1.f) * (width / 4.f);
			offsetZ = (((i % 4) < 2) ? -1.f : 1.f) * (width / 4.f);

			//see if there are any obj in the new node
			node->nodeChild[i] = new Node;
			count = countObj(objHandler, node->nodeChild[i], (posX + offsetX), (posZ + offsetZ), (width / 2.f));//countObj funktion istället för 0

			if (count > 0)
			{
				CreateTreeNode(objHandler, node->nodeChild[i], (posX + offsetX), (posZ + offsetZ), (width / 2.f));
			}
			else
			{
				delete node->nodeChild[i];
				node->nodeChild[i] = nullptr;
			}
		}
		return;
	}

	//Case 3: If this node is not empty and the obj count is less then max
	node->objInQuad = &objHandler->getModelAt(node->objIndex[0]);//inte säker på att detta kommer funka
	//vi behöver id för obj för att vet vilken som ska renderas i varje quad
	
}

int Quadtree::countObj(ModelHandler* objHandler, Node* node, float posX, float posZ, float width)
{
	int count = 0;
	bool result;
	int nrOfObj = objHandler->getNrOfModels();//ska få infon från modelHandlern

	for (int i = 0; i < nrOfObj; i++)
	{
		result = isObjContained(objHandler->getModelAt(i), posX, posZ, width);
		if (result)
		{
			node->objIndex.push_back(i);
			count++;
		}
	}
	return count;
}

bool Quadtree::isObjContained(Model Obj, float posX, float posZ, float width)
{
	float radius = width / 2.f;
	float x = Obj.getWorldCoords().x;
	float z = Obj.getWorldCoords().z;
	//sätt x och z värden mha modelHandlern
	
	if (x > (posX + radius))
		return false;
	
	if (x < (posX - radius))
		return false;
	
	if (z > (posZ + radius))
		return false;

	if (z < (posZ - radius))
		return false;

	return true;
}

void Quadtree::render(float x, Constantbuffer &constBuffData, bool shadow)
{
	drawCount = 0;

	renderNode(rootNode, x, constBuffData, shadow);
}

void Quadtree::renderNode(Node* node, float x, Constantbuffer &constBuffData, bool shadow)
{
	bool result = false;
	int count = 0;

	if (node->positionX > x) //x placeholders el något sånt
		result = true;

	if (!result)
		return;

	for (int i = 0; i < 4; i++)
	{
		if (node->nodeChild[i] != nullptr)
		{
			count++;
			renderNode(node->nodeChild[i], x, constBuffData, shadow);
		}
	}

	if (count != 0)
		return;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	gDeviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	constBuffData.world = node->objInQuad->getModelSpace();
	memcpy(mappedResource.pData, &constBuffData, sizeof(Constantbuffer));
	gDeviceContext->Unmap(constantBuffer, 0);
	gDeviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

	node->objInQuad->updateMTL();
	node->objInQuad->setVertexBuffer();
	if (!shadow)
	{
		gTextureSRV = node->objInQuad->getTexture();
		gDeviceContext->PSGetShaderResources(0, 1, &gTextureSRV);
	}
	drawCount++;
	gDeviceContext->Draw(node->objInQuad->getMesh().size(), 0);
}