#pragma once

#include"Graphics/sn3dmeshmodel.h"
#include<vector>
#include<set>
using namespace std;
class MeshGrid 
{
	class CGridCube
	{
	public:
		CGridCube(){m_iX=m_iY=m_iZ=m_iTriIdx=m_iNumTris=0; m_iNextCube=-1;}
		int m_iX, m_iY, m_iZ;	
		int m_iNextCube;		
		int m_iTriIdx;			
		int m_iNumTris;			
	};

public:
	MeshGrid(sn3DMeshData *pTriMesh, float resolution);
	virtual ~MeshGrid();
	int m_iPR_ProcessedCubes;
	int m_iPR_ProcessedTris;

public:
	float m_fResolution; 			
	sn3DMeshData *m_pMesh; 							
	float m_fXMin, m_fXMax, m_fYMin, m_fYMax, m_fZMin, m_fZMax;
	int m_iDimX, m_iDimY, m_iDimZ;	
	int m_iHashSize; 									
	int m_iHashDimSize;						

	vector<CGridCube> m_vCubes;	
	vector<int> m_vTableIdx;		
	vector<int> m_vCubeTris;		

	vector<Face*> m_vTmpTris;
	vector<CGridCube*> m_vTmpCubes;	
	vector<Vertex*> m_vTmpPts; 

	int m_iNumThreadsMax;
	vector< vector<bool> > m_vFlags;
	vector< vector<int> > m_vTmpIdx;

	void InsertTriangles();
	void InsertTrianglesParallel();
	void GetTriCubeRange(int triIdx, int CubeIdxMin[3], int CubeIdxMax[3]);
	int GetHashIdx(int gx, int gy, int gz);
	CGridCube* GetCreateCube(int gx, int gy, int gz);
	CGridCube* GetCube(int gx, int gy, int gz);

	int   GetCubePos(CGridCube* pCube){return (int)(pCube - &(m_vCubes.at(0)));}
	bool  GetCubeIdx(float x, float y, float z, int &gx, int &gy, int &gz);
	void  PR_ProcessCube(float p[3], float qrad, int gx, int gy, int gz, Face **pMinTri, float &qdist, float &s, float &t, int &footCase);
	void  PR_ProcessCube(float p[3], float nv[3], float qrad, int gx, int gy, int gz, Face **pMinTri, float &qdist, float &s, float &t, int &footCase);
	void  GetBoxCubes(float x, float y, float z, float rad, int& mingx, int& maxgx, int& mingy, int& maxgy, int& mingz, int& maxgz);
	int   PR_CalcSign(float q[3], Face *pMinTri, float s, float t, int footCase);
	int   PR_GetSignProjection(float q[3], float foot[3], Face *pTri);
	bool  CheckOverlapCubeTriangle(float cubePt[3], float boxSize[3], float triPt1[3], float triPt2[3], float triPt3[3]);
	float CalcSqrDistPtTri(float q[3], float t1[3], float t2[3], float t3[3], float &s, float &t, int &footCase);
	float ScalarProduct(float a[3], float b[3]);
	void  VectorProduct(float v1[3], float v2[3], float res[3]);
	float SqrDist(float a[3], float b[3]);
	float SqrDist(float pax, float pay, float paz, float pbx, float pby, float pbz);
	float CalcInnerTriAngle(float q1[3], float q2[3], float q3[3]);
	int   Normalize(float& nx, float& ny, float& nz);
	float CalcAngle(float dir1[3], float dir2[3]);
	int   GetTrisOfPt(int ptIdx, vector<Face*>& vNbTris);
	float DistPt2Plane(float pt[3], float planePt[3], float planeNrv[3]);
	bool  PlaneBoxOverlap(float nrv[3], float d, float boxhalfsize[3]);
	float ProjPt2Plane(float pt[3], float planePt[3], float planeNrv[3], float projPt[3]);
	
	void GetPtCube(float x, float y, float z, int& gx, int& gy, int& gz)
	{
		gx = (int) floor( (x - m_fXMin ) / m_fResolution );
		gy = (int) floor( (y - m_fYMin ) / m_fResolution );
		gz = (int) floor( (z - m_fZMin ) / m_fResolution );
	}
	float GetResolution() {return m_fResolution;}
	Face* PR_ProjectPoint(float q[3], float rad, float &s, float &t, float &dist, int &footCase, bool bSigned);
	Face* PR_ProjectPoint(float q[3], float nv[3], float rad, float &s, float &t, float &dist, int &footCase, bool bSigned);
	
};
