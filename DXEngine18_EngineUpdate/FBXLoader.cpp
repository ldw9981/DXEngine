#include "FBXLoader.h"

namespace FBXLoader
{
	FbxManager* fbxManager;

	HRESULT LoadFBX(const char * fileName, std::vector<Vertex>* pOutVertices, std::vector<DWORD>* pOutIndices)
	{
		// fbxManager가 생성되지 않았으면 생성.
		if (fbxManager == NULL)
		{
			// 초기 SDK Manager 생성.
			fbxManager = FbxManager::Create();

			FbxIOSettings* pIOSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
			fbxManager->SetIOSettings(pIOSettings);
		}

		FbxGeometryConverter gConverter(fbxManager);
		FbxImporter* importer
			= FbxImporter::Create(fbxManager, "");
		FbxScene* fbxScene
			= FbxScene::Create(fbxManager, "");

		// 임포터 초기화.
		if (importer->Initialize(fileName, -1, fbxManager->GetIOSettings()) == false)
		{
			return E_FAIL;
		}

		// 3D 씬 임포트.
		if (importer->Import(fbxScene) == false)
		{
			return E_FAIL;
		}

		// 임포터 해제.
		importer->Destroy();

		// FBX Node 읽기.
		FbxNode* fbxRootNode = fbxScene->GetRootNode();
		if (fbxRootNode == NULL)
			return E_FAIL;

		// 모델링 폴리곤 기본 구조를 삼각면으로 설정.
		gConverter.Triangulate(fbxScene, true);

		int childCount = fbxRootNode->GetChildCount();
		for (int ix = 0; ix < childCount; ++ix)
		{
			// 자식 계층 노드 읽어오기.
			FbxNode* fbxChildNode
				= fbxRootNode->GetChild(ix);

			// 속성이 없는 노드는 패스.
			if (fbxChildNode->GetNodeAttribute() == NULL)
				continue;

			// 노드 속성 타입 확인.
			FbxNodeAttribute::EType arrtType
				= fbxChildNode->GetNodeAttribute()->GetAttributeType();

			// 노드 타입이 메시가 아닌 경우 패스.
			if (arrtType != FbxNodeAttribute::EType::eMesh)
				continue;

			// 메시 정보 읽기.
			FbxMesh* fbxMesh = fbxChildNode->GetMesh();

			// 컨트롤 포인트 배열 얻어오기 (정점).
			FbxVector4* vertices = fbxMesh->GetControlPoints();
			// 폴리곤 수 확인.
			int polyCount = fbxMesh->GetPolygonCount();
			int vertexCounter = 0;

			for (int jx = 0; jx < polyCount; ++jx)
			{
				// 폴리곤 구성 정점 개수 확인.
				int vertexCount = fbxMesh->GetPolygonSize(jx);

				for (int kx = 0; kx < vertexCount; ++kx)
				{
					// 삼각면(폴리곤) 번호, 정점 번호를 통해 정점 읽기.
					int vertexIndex = fbxMesh->GetPolygonVertex(jx, kx);

					// 정점 정보 읽기.
					Vertex vertex;
					vertex.position.x = (float)vertices[vertexIndex].mData[0];
					vertex.position.y = (float)vertices[vertexIndex].mData[1];
					vertex.position.z = (float)vertices[vertexIndex].mData[2];

					// UV 읽기.
					vertex.texCoord = ReadUV(fbxMesh, vertexIndex, vertexCounter);

					// 노멀 읽기.
					vertex.normal = ReadNormal(fbxMesh, vertexIndex, vertexCounter);

					// 탄젠트 읽기.
					bool hasNoTangent
						= fbxMesh->GetElementTangentCount() < 1;
					if (hasNoTangent)
					{
						MessageBox(NULL, L"탄젠트 없음", L"오류", MB_OK);
					}

					vertex.tangent = hasNoTangent ? XMFLOAT3(0.0f, 0.0, 0.0f) : ReadTangent(fbxMesh, vertexIndex, vertexCounter);

					// 바이노멀 읽기.
					bool hasNoBinormal
						= fbxMesh->GetElementBinormalCount() < 1;
					if (hasNoBinormal)
						MessageBox(NULL, L"바이노멀 없음", L"오류", MB_OK);

					vertex.binormal = hasNoBinormal ? XMFLOAT3(0.0f, 0.0f, 0.0f) :
						ReadBinormal(fbxMesh, vertexIndex, vertexCounter);

					// 정점 배열에 데이터 추가.
					pOutVertices->push_back(vertex);
					// 인덱스 배열에 데이터 추가.
					pOutIndices->push_back(vertexCounter);
					++vertexCounter;
				}
			}
		}


		return S_OK;
	}

	XMFLOAT2 ReadUV(FbxMesh * mesh, int controlPointIndex, int vertexCounter)
	{
		// UV가 있는지 확인.
		if (mesh->GetElementUVCount() < 1)
		{
			MessageBox(NULL, L"UV가 없습니다.", L"오류", MB_OK);
			return NULL;
		}

		// 반환용 데이터 선언.
		XMFLOAT2 texCoord(0.0f, 0.0f);

		// UV 전체 배열 읽기.
		FbxGeometryElementUV* vertexUV = mesh->GetElementUV(0);
		const bool isUsingIndex
			= vertexUV->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int indexCount = isUsingIndex ? vertexUV->GetIndexArray().GetCount() : 0;

		// 모드 확인.
		switch (vertexUV->GetMappingMode())
		{
			// 현재 정점이 제어점 유형인 경우.
		case FbxGeometryElement::eByControlPoint:
		{
			// 현재 UV 값을 읽어올 인덱스 얻어오기.
			int index = isUsingIndex ? vertexUV->GetIndexArray().GetAt(controlPointIndex) : controlPointIndex;

			/*if (isUsingIndex == true)
				index = vertexUV->GetIndexArray().GetAt(controlPointIndex);
			else
				index = controlPointIndex;*/

				// UV 값 읽어오기. (DirectX와 FBX의 UV의 V좌표는 서로 반대.)
			texCoord.x = (float)vertexUV->GetDirectArray().GetAt(index).mData[0];
			texCoord.y = 1.0f - (float)vertexUV->GetDirectArray().GetAt(index).mData[1];

			// UV 값 반환.
			return texCoord;
		}

		case FbxGeometryElement::eByPolygonVertex:
		{
			// 현재 UV 값을 읽어올 인덱스 얻어오기.
			int index = isUsingIndex ? vertexUV->GetIndexArray().GetAt(vertexCounter) : vertexCounter;

			// UV 값 읽어오기. (DirectX와 FBX의 UV의 V좌표는 서로 반대.)
			texCoord.x = (float)vertexUV->GetDirectArray().GetAt(index).mData[0];
			texCoord.y = 1.0f - (float)vertexUV->GetDirectArray().GetAt(index).mData[1];

			// UV 값 반환.
			return texCoord;
		}

		default:
		{
			MessageBox(NULL, L"UV 값이 유효하지 않습니다", L"오류", MB_OK);
			return NULL;
		}
		}

		return NULL;
	}

	XMFLOAT3 ReadNormal(FbxMesh * mesh, int controlPointIndex, int vertexCounter)
	{
		XMFLOAT3 normal(0.0f, 0.0f, 0.0f);

		// 노멀이 있는지 확인.
		if (mesh->GetElementNormalCount() < 1)
		{
			MessageBox(NULL, L"노멀이 없습니다.", L"오류", MB_OK);
			return NULL;
		}

		FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(0);
		const bool isUsingIndex
			= vertexNormal->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int indexCount = isUsingIndex ? vertexNormal->GetIndexArray().GetCount() : 0;

		switch (vertexNormal->GetMappingMode())
		{
			// 현재 정점이 제어점 유형인 경우.
		case FbxGeometryElement::eByControlPoint:
		{
			// 현재 노멀 값을 읽어올 인덱스 얻어오기.
			int index = isUsingIndex ? vertexNormal->GetIndexArray().GetAt(controlPointIndex) : controlPointIndex;

			// 노멀 값 읽어오기. (DirectX와 FBX의 UV의 V좌표는 서로 반대.)
			normal.x = (float)vertexNormal->GetDirectArray().GetAt(index).mData[0];
			normal.y = (float)vertexNormal->GetDirectArray().GetAt(index).mData[1];
			normal.z = (float)vertexNormal->GetDirectArray().GetAt(index).mData[2];

			// 노멀 값 반환.
			return normal;
		}

		case FbxGeometryElement::eByPolygonVertex:
		{
			// 현재 노멀 값을 읽어올 인덱스 얻어오기.
			int index = isUsingIndex ? vertexNormal->GetIndexArray().GetAt(vertexCounter) : vertexCounter;

			// 노멀 값 읽어오기. (DirectX와 FBX의 UV의 V좌표는 서로 반대.)
			normal.x = (float)vertexNormal->GetDirectArray().GetAt(index).mData[0];
			normal.y = (float)vertexNormal->GetDirectArray().GetAt(index).mData[1];
			normal.z = (float)vertexNormal->GetDirectArray().GetAt(index).mData[2];

			// 노멀 값 반환.
			return normal;
		}

		default:
		{
			MessageBox(NULL, L"노멀 값이 유효하지 않습니다", L"오류", MB_OK);
			return NULL;
		}
		}

		return NULL;
	}

	XMFLOAT3 ReadTangent(FbxMesh * mesh, int controlPointIndex, int vertexCounter)
	{
		// UV가 있는지 확인.
		if (mesh->GetElementTangentCount() < 1)
		{
			MessageBox(NULL, L"Tangent 값이 유효하지 않습니다", L"오류", MB_OK);
			return NULL;
		}

		// 리턴용 변수 선언.
		XMFLOAT3 tangent(0.0f, 0.0f, 0.0f);
		// UV 전체 배열 얻어오기.
		FbxGeometryElementTangent* vertexTangent = mesh->GetElementTangent(0);
		const bool isUsingIndex = vertexTangent->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int indexCount = isUsingIndex ? vertexTangent->GetIndexArray().GetCount() : 0;

		switch (vertexTangent->GetMappingMode())
		{
			// UV 속성이 제어점 유형인 경우.
		case FbxGeometryElement::eByControlPoint:
		{
			// 현재 UV 값을 읽어올 인덱스 얻어오기.
			int index = isUsingIndex ? vertexTangent->GetIndexArray().GetAt(controlPointIndex) : controlPointIndex;

			// UV 값 읽어오기. (DirectX와 FBX의 UV V좌표는 서로 반대).
			tangent.x = (float)vertexTangent->GetDirectArray().GetAt(index).mData[0];
			tangent.y = (float)vertexTangent->GetDirectArray().GetAt(index).mData[1];
			tangent.z = (float)vertexTangent->GetDirectArray().GetAt(index).mData[2];

			// UV 값 반환.
			return tangent;
		}

		// UV 속성이 정점 유형인 경우.
		case FbxGeometryElement::eByPolygonVertex:
		{
			// 현재 UV 값을 읽어올 인덱스 얻어오기. 삼항연산자.
			int index = isUsingIndex == true ? vertexTangent->GetIndexArray().GetAt(vertexCounter) : vertexCounter;

			// UV 값 읽어오기. (DirectX와 FBX의 UV V좌표는 서로 반대).
			tangent.x = (float)vertexTangent->GetDirectArray().GetAt(index).mData[0];
			tangent.y = (float)vertexTangent->GetDirectArray().GetAt(index).mData[1];
			tangent.z = (float)vertexTangent->GetDirectArray().GetAt(index).mData[2];

			// UV 값 반환.
			return tangent;
		}

		default:
		{
			MessageBox(NULL, L"Tangent 값이 유효하지 않습니다", L"오류", MB_OK);
			return NULL;
		}
		}

		return NULL;
	}

	XMFLOAT3 ReadBinormal(FbxMesh * mesh, int controlPointIndex, int vertexCounter)
	{
		// 바이노멀이 있는지 확인.
		if (mesh->GetElementBinormalCount() < 1)
		{
			MessageBox(NULL, L"Binormal 값이 유효하지 않습니다", L"오류", MB_OK);
			return NULL;
		}

		// 리턴용 변수 선언.
		XMFLOAT3 binormal(0.0f, 0.0f, 0.0f);
		// UV 전체 배열 얻어오기.
		FbxGeometryElementBinormal* vertexBinormal = mesh->GetElementBinormal(0);
		const bool isUsingIndex = vertexBinormal->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int indexCount = isUsingIndex ? vertexBinormal->GetIndexArray().GetCount() : 0;

		switch (vertexBinormal->GetMappingMode())
		{
			// UV 속성이 제어점 유형인 경우.
		case FbxGeometryElement::eByControlPoint:
		{
			// 현재 UV 값을 읽어올 인덱스 얻어오기.
			int index = isUsingIndex ? vertexBinormal->GetIndexArray().GetAt(controlPointIndex) : controlPointIndex;

			// UV 값 읽어오기. (DirectX와 FBX의 UV V좌표는 서로 반대).
			binormal.x = (float)vertexBinormal->GetDirectArray().GetAt(index).mData[0];
			binormal.y = (float)vertexBinormal->GetDirectArray().GetAt(index).mData[1];
			binormal.z = (float)vertexBinormal->GetDirectArray().GetAt(index).mData[2];

			// UV 값 반환.
			return binormal;
		}

		// UV 속성이 정점 유형인 경우.
		case FbxGeometryElement::eByPolygonVertex:
		{
			// 현재 UV 값을 읽어올 인덱스 얻어오기. 삼항연산자.
			int index = isUsingIndex == true ? vertexBinormal->GetIndexArray().GetAt(vertexCounter) : vertexCounter;

			// UV 값 읽어오기. (DirectX와 FBX의 UV V좌표는 서로 반대).
			binormal.x = (float)vertexBinormal->GetDirectArray().GetAt(index).mData[0];
			binormal.y = (float)vertexBinormal->GetDirectArray().GetAt(index).mData[1];
			binormal.z = (float)vertexBinormal->GetDirectArray().GetAt(index).mData[2];

			// UV 값 반환.
			return binormal;
		}

		default:
		{
			MessageBox(NULL, L"Binormal 값이 유효하지 않습니다", L"오류", MB_OK);
			return NULL;
		}
		}

		return NULL;
	}
}