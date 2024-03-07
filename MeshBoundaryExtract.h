#pragma once
#ifndef _MeshBoundaryExtract_
#define _MeshBoundaryExtract_
#include"Graphics/sn3dmeshmodel.h"


class MeshBoundaryExtract
{
public:

	// �������߽�Ķ�������
	int FindLargestBoundary(sn3DMeshData * pMesh, int * & vlist);

	// �������������е����б߽磬��˳����� bindices �߽綥������ index_num �߽綥����Ŀ
	int  FindBoundaries(sn3DMeshData * model, int ** & bindexArray, int * & bnumArray);

	void SortBoundaries(int b_num, int * index_num, int * b_sequence);

	void SortBoundaries(sn3DMeshData * mesh,int b_num, int **  bindices, int * index_num, int * b_sequence);

	float BoundartLength(sn3DMeshData * mesh, int * indices, int num); //����߽糤��

private:

	int  TrackBoundary(sn3DMeshData *pMesh, Face *pSeed, int * & pBoundary); // ׷�ٵ���

};
#endif