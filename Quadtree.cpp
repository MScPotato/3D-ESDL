#include "pch.h"
#include "Quadtree.h"


Quadtree::Quadtree()
{
	rootNode = 0;
}


Quadtree::~Quadtree()
{
}

void Quadtree::InitTree(ModelHandler* objHandler, float width)
{
	float centerX = 0.f;
	float centerZ = 0.f;

	rootNode = new Node;
	
	CreateTreeNode(objHandler, rootNode, centerX, centerZ, width);
}

void Quadtree::CreateTreeNode(ModelHandler* objHandler, Node* node, float posX, float posZ, float width)
{
	int numObj, count;
	float offsetX, offsetZ;
	node->positionX = posX;
	node->positionZ = posZ;
	node->width = width;

	node->nrOfObj = 0; 
	
	node->ObjVertexBuffer = nullptr;

	node->nodeChild[0] = nullptr;
	node->nodeChild[1] = nullptr;
	node->nodeChild[2] = nullptr;
	node->nodeChild[3] = nullptr;

	numObj = countObj(objHandler, posX, posZ, width);

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
			count = countObj(objHandler, (posX + offsetX), (posZ + offsetZ), (width / 2.f));//countObj funktion istället för 0

			if (count > 0)
			{
				node->nodeChild[i] = new Node;

				CreateTreeNode(objHandler, node->nodeChild[i], (posX + offsetX), (posZ + offsetZ), (width / 2.f));
			}
		}
		return;
	}

	//Case 3: If this node is not empty and the obj count is less then max
	node->nrOfObj = numObj; 
	//vi behöver id för obj för att vet vilken som ska renderas i varje quad
}

int Quadtree::countObj(ModelHandler* objHandler, float posX, float posZ, float width)
{
	int count = 0;
	bool result;
	int nrOfObj = objHandler->getNrOfModels();//ska få infon från modelHandlern

	for (int i = 0; i < nrOfObj; i++)
	{
		result = isObjContained(objHandler->getModelAt(i), posX, posZ, width);
		if (result)
		{
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