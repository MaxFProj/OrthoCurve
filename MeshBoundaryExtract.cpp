
#include"MeshBoundaryExtract.h"
#include"Graphics/sn3DTopology.h"

using namespace Common;
int  MeshBoundaryExtract::FindLargestBoundary(sn3DMeshData * pMesh, int * & vlist)
{
	MeshBoundaryExtract extract;
	int ** bindices = 0;
	int *  index_num = 0;

	// find mesh boundaries
	int b_num = this->FindBoundaries(pMesh, bindices, index_num);

	int * b_sequence;
	int bsize;
	if (b_num > 1) 
	{
		b_sequence = new int[b_num];
		extract.SortBoundaries(b_num, index_num, b_sequence);
		vlist = bindices[b_sequence[0]];
		bsize = index_num[b_sequence[0]];
		delete b_sequence;
		return bsize;
	}
	else if (b_num == 1)
	{
		vlist = *bindices;
		bsize = *index_num;
		return bsize;
	}
	return 0;
}
void MeshBoundaryExtract::SortBoundaries(sn3DMeshData * mesh,int b_num, int **  bindices,int * index_num, int * b_sequence)
{
	if (b_num <= 1) return;

	bool * visit = new bool[b_num];
	float * length = new float[b_num];
	Topology::VertexFace(*mesh);

	for (int i = 0; i < b_num; i++)
	{
		visit[i] = false;
		length[i] = 0.0;
	}
	for (int i = 0; i < b_num; i++)
	{
		length[i] = 0.0;
		int cnt = index_num[i];
		int * boundary = bindices[i];

		for (int j = 0; j < cnt; j++)
		{
			float dis = 0.0;
			Vertex * v0 = mesh->V(boundary[j]);
			Vertex * v1 = mesh->V(boundary[(j + 1) % cnt]);
			VFIterator vfi(v0);
			bool found = false;
			for (; !vfi.End(); ++vfi)
			{
				if (vfi.F()->V((vfi.z + 1) % 3) == v1)
					found = true;
				if (vfi.F()->V((vfi.z + 2) % 3) == v1)
					found = true;
			}
			if (!found)
				found = found;

			dis = (v0->P() - v1->P()).Norm();
			length[i] += dis;
		}
	}

	for (int i = 0; i<b_num; i++)
	{
		int max_index = -1;
		float max_len = 0;
		for (int j = 0; j<b_num; j++)
		{
			if (visit[j]) continue;
			if (length[j]>max_len)
			{
				max_len = length[j];
				max_index = j;
			}
		}
		visit[max_index] = true;
		b_sequence[i] = max_index; 
	}

	delete visit;
}
void MeshBoundaryExtract::SortBoundaries(int b_num, int * index_num, int * b_sequence)
{
	if (b_num <= 1) return;

	bool * visit = new bool[b_num];
	for (int i = 0; i < b_num; i++)
	{
	   visit[i] = false;
	}

	for (int i=0;i<b_num;i++)
	{
		int max_index = -1;
		int max_num = 0;
		for (int j=0;j<b_num;j++)
		{
			if (visit[j]) continue;
			if (index_num[j]>max_num)
			{
				max_num = index_num[j];
				max_index = j;
			}
		}
		visit[max_index] = true;
		b_sequence[i] = max_index; 
	}

	delete visit;
}
int  MeshBoundaryExtract::FindBoundaries(sn3DMeshData * pMesh, int ** & bindices, int *& index_num)
{
	int bnum = 0; 
	int fn = pMesh->fn;
	int numBorderTris = 0; 

	int memsize = 1024;
	bindices = new int *[memsize];
	index_num = new int[memsize];

	Topology::VertexFace(*pMesh);
	Topology::FaceFace(*pMesh);

	for (int i = 0; i < fn; i++) 
	{
		Face * pTri = pMesh->F(i);

		if (pTri->FFp(0) != pTri&& pTri->FFp(1) != pTri&& pTri->FFp(2) != pTri)
		{
			pTri->ClearM();
		}
		else
		{
			pTri->SetM();
			numBorderTris++;
		}
	}

	if (numBorderTris == 0)
	{
		return 0;
	}

	for (int seedIdx = 0; seedIdx < fn; seedIdx++)
	{
		Face * pSeed = pMesh->F(seedIdx);
		if (!pSeed->IsM()) 
			continue;

		int * loop = 0; 
		int   inum = 0; 

		inum = TrackBoundary(pMesh, pSeed,loop);

		if (inum == 0)
		{
			continue;
		}

		if (bnum >= memsize) 
		{
			memsize += 1024;
			int ** buffer0 = bindices;
			int * buffer1 = index_num;
			bindices = new int *[memsize];
			index_num = new int[memsize];
			memcpy(bindices, buffer0, bnum);
			memcpy(index_num, buffer1, bnum);
			delete buffer0;
			delete buffer1;
		}

		bindices[bnum] = loop;
		index_num[bnum] = inum;
		bnum++;
	}
	return bnum;
}
int  MeshBoundaryExtract::TrackBoundary(sn3DMeshData *pMesh, Face *pSeed, int * & pBoundary)
{
	int edgeIdx, count = 0;

	Vertex *pPt;
	Face *pTri = pSeed;

	for (edgeIdx = 0; edgeIdx < 3; edgeIdx++)
		if (pTri->FFp(edgeIdx) == pTri)
			break;
	
	int memsize = 1024;
	int mempointer = 0;
	int * tempLoop = new int[memsize];

	
	do {
		pTri->ClearM();
		pPt = pTri->V(edgeIdx);
		if (mempointer > 0 && tempLoop[mempointer] == tempLoop[mempointer - 1]) //by wuzq
		{
			delete tempLoop;
			return 0;
		}
		tempLoop[mempointer] = pPt->Index(); 
		mempointer++;
		if (mempointer>=memsize)
		{
			int *buffer = tempLoop;
			memsize += 1024;
			tempLoop = new int[memsize];
			memcpy(tempLoop,buffer,sizeof(int)*(mempointer+1));
			delete buffer;
		}

		Pos pos(pTri, edgeIdx);
		pos.NextB();
		pTri = pos.f;
		edgeIdx = pos.z;

		if (pTri == NULL)
		{
			delete tempLoop;
			return 0;
		}
	} while (pTri != pSeed);

	if (mempointer < 3)
	{
		delete tempLoop;
		return 0;
	}

	pBoundary = tempLoop;

	return mempointer;
}
float MeshBoundaryExtract::BoundartLength(sn3DMeshData * mesh, int * boundary, int cnt)
{
	float length = 0.0;
	for (int j = 0; j < cnt; j++)
	{
		float dis = 0.0;
		Vertex * v0 = mesh->V(boundary[j]);
		Vertex * v1 = mesh->V(boundary[(j + 1) % cnt]);
		VFIterator vfi(v0);
		bool found = false;
		for (; !vfi.End(); ++vfi)
		{
			if (vfi.F()->V((vfi.z + 1) % 3) == v1)
				found = true;
			if (vfi.F()->V((vfi.z + 2) % 3) == v1)
				found = true;
		}
		if (!found)
			found = found;

		dis = (v0->P() - v1->P()).Norm();
		length += dis;
	}
	return length;
}

