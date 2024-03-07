#pragma once
#ifndef _MeshBoundaryExtract_
#define _MeshBoundaryExtract_
#include"Graphics/sn3dmeshmodel.h"


class MeshBoundaryExtract
{
public:

	// 查找最大边界的顶点序列
	int FindLargestBoundary(sn3DMeshData * pMesh, int * & vlist);

	// 查找网格曲面中的所有边界，按顺序输出 bindices 边界顶点索引 index_num 边界顶点数目
	int  FindBoundaries(sn3DMeshData * model, int ** & bindexArray, int * & bnumArray);

	void SortBoundaries(int b_num, int * index_num, int * b_sequence);

	void SortBoundaries(sn3DMeshData * mesh,int b_num, int **  bindices, int * index_num, int * b_sequence);

	float BoundartLength(sn3DMeshData * mesh, int * indices, int num); //计算边界长度

private:

	int  TrackBoundary(sn3DMeshData *pMesh, Face *pSeed, int * & pBoundary); // 追踪单个

};
#endif