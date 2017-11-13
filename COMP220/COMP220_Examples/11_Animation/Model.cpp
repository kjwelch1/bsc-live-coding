#include "Model.h"

// https://gamedev.stackexchange.com/questions/26382/i-cant-figure-out-how-to-animate-my-loaded-model-with-assimp
//
//https://www.youtube.com/watch?v=F-kcaonjHf8&index=2&list=PLRIWtICgwaX2tKWCxdeB7Wv_rTET9JtWW
int currentBoneID = 0;
std::map<std::string, int> BoneMap;

bool loadModelFromFile(const std::string& filename, GLuint VBO, GLuint EBO, unsigned int& numVerts, unsigned int& numIndices)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filename, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);

	if (!scene)
	{
		printf("Model Loading Error - %s\n", importer.GetErrorString());
		return false;
	}

	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh *currentMesh=scene->mMeshes[i];

		for (int v = 0; v < currentMesh->mNumVertices; v++)
		{
			aiVector3D currentModelVertex = currentMesh->mVertices[v];
			aiVector3D currentTextureCoordinates = currentMesh->mTextureCoords[0][v];
			aiVector3D currentNormals = currentMesh->mNormals[v];

			Vertex currentVertex = { currentModelVertex.x,currentModelVertex.y,currentModelVertex.z,
				1.0f,1.0f,1.0f,1.0f,
				currentTextureCoordinates.x,currentTextureCoordinates.y,
				currentNormals.x,currentNormals.y,currentNormals.z
			};

			vertices.push_back(currentVertex);
		}

		for (int f = 0; f < currentMesh->mNumFaces; f++)
		{
			aiFace currentModelFace = currentMesh->mFaces[f];
			indices.push_back(currentModelFace.mIndices[0]);
			indices.push_back(currentModelFace.mIndices[1]);
			indices.push_back(currentModelFace.mIndices[2]);
		}
	}

	numVerts = vertices.size();
	numIndices = indices.size();

	// Give our vertices to OpenGL.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, numVerts * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	return true;
}

bool loadMeshFromFile(const std::string & filename, std::vector<Mesh*>& meshes, Joint **pRootJoint)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filename, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);
	BoneMap.clear();

	if (!scene)
	{
		printf("Model Loading Error - %s\n", importer.GetErrorString());
		return false;
	}
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh *currentMesh = scene->mMeshes[i];
		Mesh *pMesh = new Mesh();
		pMesh->init();

		for (int v = 0; v < currentMesh->mNumVertices; v++)
		{
			
			aiVector3D currentModelVertex = currentMesh->mVertices[v];

			aiVector3D currentTextureCoordinates = aiVector3D(0.0f);
			aiVector3D currentNormals = aiVector3D(0.0f);

			if (currentMesh->HasNormals())
			{
				currentNormals=currentMesh->mNormals[v];
			}
			if (currentMesh->HasTextureCoords(0))
			{
				currentTextureCoordinates = currentMesh->mTextureCoords[0][v];
			}



			Vertex currentVertex = { currentModelVertex.x,currentModelVertex.y,currentModelVertex.z,
				1.0f,1.0f,1.0f,1.0f,
				currentTextureCoordinates.x,currentTextureCoordinates.y,
				currentNormals.x,currentNormals.y,currentNormals.z
			};

			vertices.push_back(currentVertex);
		}

		for (int f = 0; f < currentMesh->mNumFaces; f++)
		{
			aiFace currentModelFace = currentMesh->mFaces[f];
			indices.push_back(currentModelFace.mIndices[0]);
			indices.push_back(currentModelFace.mIndices[1]);
			indices.push_back(currentModelFace.mIndices[2]);
		}

		printf("Bones %d\n", currentMesh->mNumBones);
		if (currentMesh->HasBones())
		{
			for (int i = 0; i < currentMesh->mNumBones; i++)
			{
				aiBone * currentBone = currentMesh->mBones[i];
				std::string currentBoneName = std::string(currentBone->mName.C_Str());
				printf("Current Bone %d %s\n", currentBoneID, currentBoneName.c_str());
				if (BoneMap.find(currentBoneName) == BoneMap.end()) {
					BoneMap[std::string(currentBone->mName.C_Str())] = currentBoneID;
					currentBoneID++;
				}
				for (int w = 0; w < 4; w++)
				{

					int vertexID=currentBone->mWeights[w].mVertexId;
					
					vertices[vertexID].boneIDs[w] = currentBoneID;
					vertices[vertexID].boneWeights[w] = currentBone->mWeights[w].mWeight;
				}
			}

		}

		pMesh->copyBufferData(vertices.data(), vertices.size(), indices.data(), indices.size());

		meshes.push_back(pMesh);
		vertices.clear();
		indices.clear();
	}

	currentBoneID = 0;
	aiNode * sceneRootNode = scene->mRootNode;
	
	std::string animationRootName;
	for (auto boneMapItem : BoneMap)
	{
		if (boneMapItem.second == 0)
		{
			animationRootName = boneMapItem.first;
		}
	}

	aiNode * animationRootNode= sceneRootNode->FindNode(animationRootName.c_str());
	std::string jointName = std::string(animationRootNode->mName.C_Str());
	glm::mat4 transformation = ASSMIPMatrixToGLM(animationRootNode->mTransformation);
	(*pRootJoint) = new Joint(currentBoneID, jointName, transformation);
	(*pRootJoint)->calculateInverseBindTransform(glm::mat4(1.0f));
	processNode(animationRootNode,(*pRootJoint));
	return true;
}

bool loadAnimationFromFile(const std::string & filename, AnimationClip ** clip)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filename, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);

	if (!scene)
	{
		printf("Model Loading Error - %s\n", importer.GetErrorString());
		return false;
	}

	if (!scene->HasAnimations())
		return false;

	//the file we are using has one animation per file
	aiAnimation * currentAnimation = scene->mAnimations[0];
	
	for (int i = 0; i < currentAnimation->mNumChannels; i++)
	{
		aiNodeAnim *currentNode = currentAnimation->mChannels[i];
		//printf("Animation Node %s\n", currentNode->mNodeName.C_Str());

		aiVector3D position=currentNode->mPositionKeys[0].mValue;
		aiVector3D scale = currentNode->mScalingKeys[0].mValue;
		aiQuaternion rotation = currentNode->mRotationKeys[0].mValue;
	}


	return true;
}


void processNode(aiNode * parentNode,Joint *parentJoint)
{
	Joint * pJoint = parentJoint;
	for (int i = 0; i < parentNode->mNumChildren; i++)
	{
		std::string nodeName = std::string(parentNode->mChildren[i]->mName.C_Str());
		if (BoneMap.find(nodeName) != BoneMap.end())
		{
			pJoint = new Joint(BoneMap[nodeName], nodeName, parentJoint->getBindTransform());
			parentJoint->addChildJoint(pJoint);
		}

		processNode(parentNode->mChildren[i], pJoint);
	}
}
