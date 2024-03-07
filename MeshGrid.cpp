
#include "MeshGrid.h"
#include"Graphics/sn3DTopology.h"

using namespace Common;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define EPS (0.001f*m_fResolution)
#define MG_INSERTSERIAL
#define TRI_DELETED			-1
#define TRI_OK				0
#define TRI_UNTOUCHED		1
#define TRI_DONE			2
#define TRI_BORDER			3
#define TRI_NOBORDER		4
#define TRI_ORIENTATED		5
#define TRI_NOORIENTATED	6
#define TRI_NRVINVERTED		7
#define TRI_INTERSECTING	8

#define PI                  3.14159265f
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

MeshGrid::MeshGrid(sn3DMeshData *pTriMesh, float resolution)
{
	float eps;
	
	if (resolution <= 0)
		resolution = 1;

	Topology::VertexFace(*pTriMesh);
	Topology::FaceFace(*pTriMesh);

	m_iPR_ProcessedCubes = m_iPR_ProcessedTris = 0;

	m_fResolution = resolution;
	m_pMesh = pTriMesh;

	Point3f max = Point3f(-100000, -100000, -100000);
	Point3f min = Point3f(100000, 100000, 100000);

	for (int i = 0; i<(int)pTriMesh->vn; i++)
	{
		if (min.X() > pTriMesh->V(i)->m_coord.X()) min.X() = pTriMesh->V(i)->m_coord.X();
		if (min.Y() > pTriMesh->V(i)->m_coord.Y()) min.Y() = pTriMesh->V(i)->m_coord.Y();
		if (min.Z() > pTriMesh->V(i)->m_coord.Z()) min.Z() = pTriMesh->V(i)->m_coord.Z();
		if (max.X() < pTriMesh->V(i)->m_coord.X()) max.X() = pTriMesh->V(i)->m_coord.X();
		if (max.Y() < pTriMesh->V(i)->m_coord.Y()) max.Y() = pTriMesh->V(i)->m_coord.Y();
		if (max.Z() < pTriMesh->V(i)->m_coord.Z()) max.Z() = pTriMesh->V(i)->m_coord.Z();
	}

	m_fXMin = min[0];
	m_fXMax = max[0];
	m_fYMin = min[1];
	m_fYMax = max[1];
	m_fZMin = min[2];
	m_fZMax = max[2];

	eps = 0.001f*m_fResolution;
	m_fXMin -= eps;
	m_fXMax += eps;
	m_fYMin -= eps;
	m_fYMax += eps;
	m_fZMin -= eps;
	m_fZMax += eps;

	m_iDimX = max(1, (int) ceil((m_fXMax - m_fXMin) / resolution));
	m_iDimY = max(1, (int) ceil((m_fYMax - m_fYMin) / resolution));
	m_iDimZ = max(1, (int) ceil((m_fZMax - m_fZMin) / resolution));

	m_vTmpTris.reserve(1000);

	m_iHashSize = 262144;
	m_iHashDimSize = 64;

	InsertTriangles();

	m_iNumThreadsMax = 1;
	m_vFlags.resize(m_iNumThreadsMax, vector<bool>(m_pMesh->fn, false));

	m_vTmpIdx.resize(m_iNumThreadsMax);

}

MeshGrid::~MeshGrid()
{

}

void MeshGrid::InsertTriangles()
{
	int i, j, j1, j2, numTris, cubeIdxMin[3], cubeIdxMax[3]; 
	int gx, gy, gz, posTri, count;
	float cubeBase[3], cubeSize[3], p1[3], p2[3], p3[3];
	Vertex *pPt1, *pPt2, *pPt3;
	CGridCube *pCube;
	vector<int> vNumCubes;

	numTris = m_pMesh->fn;

	vector<int> vTri2Grid;
	vTri2Grid.reserve(1000);

	m_vTableIdx.resize(m_iHashSize, -1);

	m_vCubes.reserve(1000);

	vNumCubes.resize(numTris, 0);

	cubeSize[0] = cubeSize[1] = cubeSize[2] = m_fResolution;


	for (i=0; i < numTris; i++)
	{
		Face * f = m_pMesh->F(i);
		pPt1 = f->V(0);
		pPt2 = f->V(1);
		pPt3 = f->V(2);
		
		
		p1[0] = pPt1->P()[0];
		p1[1] = pPt1->P()[1];
		p1[2] = pPt1->P()[2];

		p2[0] = pPt2->P()[0];
		p2[1] = pPt2->P()[1];
		p2[2] = pPt2->P()[2];
		
		p3[0] = pPt3->P()[0];
		p3[1] = pPt3->P()[1];
		p3[2] = pPt3->P()[2];

		GetTriCubeRange(i, cubeIdxMin, cubeIdxMax);

		for (gx=cubeIdxMin[0]; gx <= cubeIdxMax[0]; gx++)
		{
			cubeBase[0] = m_fXMin + gx * m_fResolution;
			
			for (gy=cubeIdxMin[1]; gy <= cubeIdxMax[1]; gy++)
			{
				cubeBase[1] = m_fYMin + gy * m_fResolution;

				for (gz=cubeIdxMin[2]; gz <= cubeIdxMax[2]; gz++)
				{
					cubeBase[2] = m_fZMin + gz * m_fResolution;

			
					if (this->CheckOverlapCubeTriangle(cubeBase, cubeSize, p1, p2, p3) == false)
						continue;

					pCube = GetCreateCube(gx, gy, gz);

					pCube->m_iNumTris++;

					vTri2Grid.push_back( GetCubePos(pCube) );
					vNumCubes[i]++;
				} 
			} 
		}
	} 


	count=0;
	for (i=0; i < numTris; i++)
		if (vNumCubes[i] == 0)
			count++;



	m_vCubeTris.resize(vTri2Grid.size());


	if (m_vCubes.size() > 0)
		m_vCubes[0].m_iTriIdx = 0;

	
	for (i=1; i < m_vCubes.size(); i++)
		m_vCubes[i].m_iTriIdx = m_vCubes[i-1].m_iTriIdx + m_vCubes[i-1].m_iNumTris;

	for (i=0; i < m_vCubes.size(); i++)
		m_vCubes[i].m_iNumTris = 0;

	j1=j2=0;

	
	for (i=0; i < numTris; i++)
	{
	
		j2 += vNumCubes[i];

		for (j=j1; j < j2; j++)
		{
	
			pCube = &m_vCubes[ vTri2Grid[j] ];

			posTri = pCube->m_iTriIdx + pCube->m_iNumTris;
	
			m_vCubeTris[posTri] = i;
			
			pCube->m_iNumTris++;
		}
		j1 = j2;
	}
}

void MeshGrid::InsertTrianglesParallel()
{
	int i, j, j1, j2, numTris, cubeIdxMin[3], cubeIdxMax[3]; 
	int gx, gy, gz, posTri, count;
	float cubeBase[3], cubeSize[3], p1[3], p2[3], p3[3];
	Vertex *pPt1, *pPt2, *pPt3;
	CGridCube *pCube;
	vector<int> vNumCubes;

	numTris = m_pMesh->fn;

	vector<int> vTri2Grid;
	vTri2Grid.reserve(1000);

	m_vTableIdx.resize(m_iHashSize, -1);

	m_vCubes.reserve(1000);

	vNumCubes.resize(numTris, 0);

	cubeSize[0] = cubeSize[1] = cubeSize[2] = m_fResolution;

	vector<int> vTri2GridX, vTri2GridY, vTri2GridZ;
	vector<int> vVecPos(numTris);
	vector<int> vXTmp, vYTmp, vZTmp;
	float numCubesPerTri=3.0f;
	int skippedCubes=0;

	int numStored=0;
	int exType = 0;

	vTri2GridX.resize(numCubesPerTri*numTris, -1);
	vTri2GridY.resize(numCubesPerTri*numTris, -1);
	vTri2GridZ.resize(numCubesPerTri*numTris, -1);


#pragma omp parallel for private(pPt1, pPt2, pPt3, p1, p2, p3, cubeIdxMin, cubeIdxMax, gx, gy, gz, cubeBase, vXTmp, vYTmp, vZTmp)
	for (i=0; i < numTris; i++)
	{
#pragma omp flush (exType)
		if (exType != 0)
			continue;

		try
		{
	
			Face * f = m_pMesh->F(i);
			pPt1 = f->V(0);
			pPt2 = f->V(1);
			pPt3 = f->V(2);
			
			p1[0] = pPt1->P()[0];
			p1[1] = pPt1->P()[1];
			p1[2] = pPt1->P()[2];

			p2[0] = pPt2->P()[0];
			p2[1] = pPt2->P()[1];
			p2[2] = pPt2->P()[2];

			p3[0] = pPt3->P()[0];
			p3[1] = pPt3->P()[1];
			p3[2] = pPt3->P()[2];


			GetTriCubeRange(i, cubeIdxMin, cubeIdxMax);

			vXTmp.clear();
			vYTmp.clear();
			vZTmp.clear();

			for (gx=cubeIdxMin[0]; gx <= cubeIdxMax[0]; gx++)
			{
				cubeBase[0] = m_fXMin + gx * m_fResolution;
				
				for (gy=cubeIdxMin[1]; gy <= cubeIdxMax[1]; gy++)
				{
					cubeBase[1] = m_fYMin + gy * m_fResolution;

					for (gz=cubeIdxMin[2]; gz <= cubeIdxMax[2]; gz++)
					{
						cubeBase[2] = m_fZMin + gz * m_fResolution;

						if (this->CheckOverlapCubeTriangle(cubeBase, cubeSize, p1, p2, p3) == false)
							continue;

						vXTmp.push_back(gx);
						vYTmp.push_back(gy);
						vZTmp.push_back(gz);

						vNumCubes[i]++;
					} 
				}
			} 

			int numNewCubes = vXTmp.size();
		

			if (vTri2GridX.size() < numStored+numNewCubes)
			{
				exType = -1;
#pragma omp flush (exType)
				continue;
			}

#pragma omp critical
			{
				vVecPos[i] = numStored;
				numStored += numNewCubes;
			}

		
			int numBytes = numNewCubes*sizeof(float);
			memcpy(&vTri2GridX[vVecPos[i]], &vXTmp[0], numBytes);
			memcpy(&vTri2GridY[vVecPos[i]], &vYTmp[0], numBytes);
			memcpy(&vTri2GridZ[vVecPos[i]], &vZTmp[0], numBytes);
		} 
		catch(...)
		{
			exType = -1;
#pragma omp flush (exType)
		}
	} 
	int storedCubes=0;
	for (i=0; i < vTri2GridX.size(); i++)
	{
		if (vTri2GridX[i] >= 0)
			storedCubes++;
	}


	if (exType != 0)
	{
	
		InsertTriangles();
		return;
	}


	for (i=0; i < numTris; i++)
	{
		int idx = vVecPos[i];
		int idx1 = idx+vNumCubes[i];
		for (j=idx; j < idx1; j++)
		{
			pCube = GetCreateCube(vTri2GridX[j], vTri2GridY[j], vTri2GridZ[j]);
			pCube->m_iNumTris++;
			vTri2Grid.push_back( GetCubePos(pCube) );
		}
	}
	
	count=0;
	for (i=0; i < numTris; i++)
		if (vNumCubes[i] == 0)
			count++;

	m_vCubeTris.resize(vTri2Grid.size());


	if (m_vCubes.size() > 0)
		m_vCubes[0].m_iTriIdx = 0;

	for (i=1; i < m_vCubes.size(); i++)
		m_vCubes[i].m_iTriIdx = m_vCubes[i-1].m_iTriIdx + m_vCubes[i-1].m_iNumTris;


	for (i=0; i < m_vCubes.size(); i++)
		m_vCubes[i].m_iNumTris = 0;


	j1=j2=0;


	for (i=0; i < numTris; i++)
	{
		j2 += vNumCubes[i];

		for (j=j1; j < j2; j++)
		{
			pCube = &m_vCubes[ vTri2Grid[j] ];

			posTri = pCube->m_iTriIdx + pCube->m_iNumTris;
			
			m_vCubeTris[posTri] = i;
			
			pCube->m_iNumTris++;
		}
		j1 = j2;
	}
}
void MeshGrid::GetTriCubeRange(int triIdx, int CubeIdxMin[3], int CubeIdxMax[3])
{
	Vertex *pPt1, *pPt2, *pPt3;
	float p1[3], p2[3], p3[3], pmin[3], pmax[3];


	Face * f = m_pMesh->F(triIdx);
	pPt1 = f->V(0);
	pPt2 = f->V(1);
	pPt3 = f->V(2);

	p1[0] = pPt1->P()[0];
	p1[1] = pPt1->P()[1];
	p1[2] = pPt1->P()[2];

	p2[0] = pPt2->P()[0];
	p2[1] = pPt2->P()[1];
	p2[2] = pPt2->P()[2];

	p3[0] = pPt3->P()[0];
	p3[1] = pPt3->P()[1];
	p3[2] = pPt3->P()[2];

	pmin[0] = min(p1[0],min(p2[0],p3[0]));
	pmin[1] = min(p1[1],min(p2[1],p3[1]));
	pmin[2] = min(p1[2],min(p2[2],p3[2]));
	pmax[0] = max(p1[0],max(p2[0],p3[0]));
	pmax[1] = max(p1[1],max(p2[1],p3[1]));
	pmax[2] = max(p1[2],max(p2[2],p3[2]));

	CubeIdxMin[0] = (int) floor( ( pmin[0] - m_fXMin ) / m_fResolution );
	CubeIdxMax[0] = (int) floor( ( pmax[0] - m_fXMin ) / m_fResolution );
	CubeIdxMin[1] = (int) floor( ( pmin[1] - m_fYMin ) / m_fResolution );
	CubeIdxMax[1] = (int) floor( ( pmax[1] - m_fYMin ) / m_fResolution );
	CubeIdxMin[2] = (int) floor( ( pmin[2] - m_fZMin ) / m_fResolution );
	CubeIdxMax[2] = (int) floor( ( pmax[2] - m_fZMin ) / m_fResolution );
}

int MeshGrid::GetHashIdx(int gx, int gy, int gz)
{
	return ( ((abs(gx) & (m_iHashDimSize-1)) * m_iHashDimSize) + (abs(gy) & (m_iHashDimSize-1))) * m_iHashDimSize+ (abs(gz) & (m_iHashDimSize-1));
}

MeshGrid::CGridCube* MeshGrid::GetCreateCube(int gx, int gy, int gz)
{
	int hashIdx, cubePos;
	bool found;
	CGridCube newCube, *pCube;

	hashIdx = GetHashIdx(gx, gy, gz);

	cubePos = m_vTableIdx[hashIdx];
	pCube = NULL;
	found = false;

	for(;;)
	{
		
		if (cubePos == -1)
			break;

		pCube = &m_vCubes[cubePos];

		if (   pCube->m_iX == gx
				&& pCube->m_iY == gy
				&& pCube->m_iZ == gz )
		{
			found = true;
			break;
		}
		
		cubePos = pCube->m_iNextCube;
	}

	if (found == true)
		return &m_vCubes[cubePos];

	cubePos = m_vCubes.size();
	
	if (pCube != NULL)
	{
		pCube->m_iNextCube = cubePos;
	}
	else
	{

		m_vTableIdx[hashIdx] = cubePos;
	}

	newCube.m_iX = gx;
	newCube.m_iY = gy;
	newCube.m_iZ = gz;
	m_vCubes.push_back(newCube);

	return &m_vCubes[cubePos];
}

MeshGrid::CGridCube* MeshGrid::GetCube(int gx, int gy, int gz)
{
	int hashIdx, cubePos;
	CGridCube *pCube=NULL, *pTmpCube;


	hashIdx = GetHashIdx(gx, gy, gz);


	cubePos = m_vTableIdx[hashIdx];

	while(cubePos != -1)
	{
		pTmpCube = &m_vCubes[cubePos];
		
		if (   pTmpCube->m_iX == gx
				&& pTmpCube->m_iY == gy
				&& pTmpCube->m_iZ == gz )
		{
			pCube = pTmpCube;
			break;
		}

		cubePos = pTmpCube->m_iNextCube;
	}

	return pCube;
}

Face* MeshGrid::PR_ProjectPoint(float q[3], float rad, float &s, float &t, float &dist, int &footCase, bool bSigned)
{
	int i;
	int qgx, qgy, qgz;
	int numsteps;
	int gx, gy, gz, gx1, gx2, gy1, gy2, gz1, gz2;
	float d, qrad, qdistmin=0.0f;
	Face *pMinTri; 


	qgx = (int)floor( ( q[0] - m_fXMin ) / m_fResolution );
	qgy = (int)floor( ( q[1] - m_fYMin ) / m_fResolution );
	qgz = (int)floor( ( q[2] - m_fZMin ) / m_fResolution );
	numsteps = (int)ceil(rad/m_fResolution);

	
	pMinTri = NULL;
	qrad = rad*rad;

	for (i=0; i <= numsteps; i++)
	{
		if (pMinTri != NULL)
		{
			d = (i-1)*m_fResolution;
			if (d*d > qdistmin)
				break;
		}

		gx1 = max(qgx-i, 0);
		gx2 = min(qgx+i, m_iDimX-1);
		gy1 = max(qgy-i, 0);
		gy2 = min(qgy+i, m_iDimY-1);
		gz1 = max(qgz-i, 0);
		gz2 = min(qgz+i, m_iDimZ-1);

		for (gy = gy1; gy <= gy2; gy++)
			for (gz = gz1; gz <= gz2; gz++)
			{
				PR_ProcessCube(q, qrad, gx1, gy, gz, &pMinTri, qdistmin, s, t, footCase);
				if (gx2 > gx1)
					PR_ProcessCube(q, qrad, gx2, gy, gz, &pMinTri, qdistmin, s, t, footCase);
			}

		for (gx = gx1+1; gx < gx2; gx++)
			for (gz = gz1; gz <= gz2; gz++)
			{
				PR_ProcessCube(q, qrad, gx, gy1, gz, &pMinTri, qdistmin, s, t, footCase);
				if (gy2 > gy1)
					PR_ProcessCube(q, qrad, gx, gy2, gz, &pMinTri, qdistmin, s, t, footCase);
			}

	
		for (gx = gx1+1; gx < gx2; gx++)
			for (gy = gy1+1; gy < gy2; gy++)
			{
				PR_ProcessCube(q, qrad, gx, gy, gz1, &pMinTri, qdistmin, s, t, footCase);
				if (gz2 > gz1)
					PR_ProcessCube(q, qrad, gx, gy, gz2, &pMinTri, qdistmin, s, t, footCase);
			}
	} 


	if (pMinTri == NULL) 
		return NULL;

	
	if (qdistmin > qrad)
		return NULL;

	
	dist = sqrt(qdistmin);
	
	if (bSigned == true)
		dist*=PR_CalcSign(q, pMinTri, s, t, footCase);

	return pMinTri;
}

Face* MeshGrid::PR_ProjectPoint(float q[3], float nv[3], float rad, float &s, float &t, float &dist, int &footCase, bool bSigned)
{
	int i;
	int qgx, qgy, qgz;
	int numsteps;
	int gx, gy, gz, gx1, gx2, gy1, gy2, gz1, gz2;
	float d, qrad, qdistmin=0.0f;
	Face *pMinTri; 


	qgx = (int)floor( ( q[0] - m_fXMin ) / m_fResolution );
	qgy = (int)floor( ( q[1] - m_fYMin ) / m_fResolution );
	qgz = (int)floor( ( q[2] - m_fZMin ) / m_fResolution );
	numsteps = (int)ceil(rad/m_fResolution);

	
	pMinTri = NULL;
	qrad = rad*rad;

	for (i=0; i <= numsteps; i++)
	{
	
		if (pMinTri != NULL)
		{
			d = (i-1)*m_fResolution;
			if (d*d > qdistmin)
				break;
		}

	
		gx1 = max(qgx-i, 0);
		gx2 = min(qgx+i, m_iDimX-1);
		gy1 = max(qgy-i, 0);
		gy2 = min(qgy+i, m_iDimY-1);
		gz1 = max(qgz-i, 0);
		gz2 = min(qgz+i, m_iDimZ-1);

		for (gy = gy1; gy <= gy2; gy++)
			for (gz = gz1; gz <= gz2; gz++)
			{
				PR_ProcessCube(q, nv, qrad, gx1, gy, gz, &pMinTri, qdistmin, s, t, footCase);
				if (gx2 > gx1)
					PR_ProcessCube(q, nv, qrad, gx2, gy, gz, &pMinTri, qdistmin, s, t, footCase);
			}

		for (gx = gx1+1; gx < gx2; gx++)
			for (gz = gz1; gz <= gz2; gz++)
			{
				PR_ProcessCube(q, nv, qrad, gx, gy1, gz, &pMinTri, qdistmin, s, t, footCase);
				if (gy2 > gy1)
					PR_ProcessCube(q, nv, qrad, gx, gy2, gz, &pMinTri, qdistmin, s, t, footCase);
			}

	
		for (gx = gx1+1; gx < gx2; gx++)
			for (gy = gy1+1; gy < gy2; gy++)
			{
				PR_ProcessCube(q, nv, qrad, gx, gy, gz1, &pMinTri, qdistmin, s, t, footCase);
				if (gz2 > gz1)
					PR_ProcessCube(q, nv, qrad, gx, gy, gz2, &pMinTri, qdistmin, s, t, footCase);
			}
	} 
	if (pMinTri == NULL) 
		return NULL;

	
	if (qdistmin > qrad)
		return NULL;

	
	dist = sqrt(qdistmin);
	
	
	if (bSigned == true)
		dist*=PR_CalcSign(q, pMinTri, s, t, footCase);

	return pMinTri;
}


void MeshGrid::PR_ProcessCube(float p[3], float qrad, int gx, int gy, int gz, Face **pMinTri, float &qdist, float &s, float &t, int &footCase)
{
	int i, i1, i2, triIdx, footIdx;
	float xmin, xmax, ymin, ymax, zmin, zmax;
	float d2, dx, dy, dz;
	float p1[3], p2[3], p3[3], h1, h2;
	CGridCube *pCube;



	if ( (pCube=GetCube(gx, gy, gz)) == NULL)
		return;


	m_iPR_ProcessedCubes++;

	xmin = m_fXMin+gx*m_fResolution;
	xmax = xmin+m_fResolution;
	ymin = m_fYMin+gy*m_fResolution;
	ymax = ymin+m_fResolution;
	zmin = m_fZMin+gz*m_fResolution;
	zmax = zmin+m_fResolution;
	if (p[0] < xmin)
		dx = xmin-p[0];
	else if (p[0] > xmax)
		dx = p[0]-xmax;
	else
		dx = 0.0f;

	if (p[1] < ymin)
		dy = ymin-p[1];
	else if (p[1] > ymax)
		dy = p[1]-ymax;
	else
		dy = 0.0f;

	if (p[2] < zmin)
		dz = zmin-p[2];
	else if (p[2] > zmax)
		dz = p[2]-zmax;
	else
		dz = 0.0f;

	d2 = dx*dx + dy*dy + dz*dz;
	
	
	if (d2 > qrad)
		return;


	if (*pMinTri != NULL)
		if (d2 > qdist)
			return;

	i1 = pCube->m_iTriIdx;
	i2 = i1 + pCube->m_iNumTris;
	for (i=i1; i < i2; i++)
	{
	
		m_iPR_ProcessedTris++;

	
		triIdx = m_vCubeTris[i];

		Face * f = m_pMesh->F(triIdx);
		Vertex * pPt1 = f->V(0);
		Vertex * pPt2 = f->V(1);
		Vertex * pPt3 = f->V(2);
		

		p1[0] = pPt1->P()[0];
		p1[1] = pPt1->P()[1];
		p1[2] = pPt1->P()[2];

		p2[0] = pPt2->P()[0];
		p2[1] = pPt2->P()[1];
		p2[2] = pPt2->P()[2];

		p3[0] = pPt3->P()[0];
		p3[1] = pPt3->P()[1];
		p3[2] = pPt3->P()[2];

	
		d2 = this->CalcSqrDistPtTri(p, p1, p2, p3, h1, h2, footIdx);

	
		if (*pMinTri == NULL)
		{
			*pMinTri = m_pMesh->F(triIdx);
			qdist = d2;
			s = h1;
			t = h2;
			footCase = footIdx;
			continue;
		}

		if (d2 < qdist)
		{
			*pMinTri = m_pMesh->F(triIdx);
			qdist = d2;
			s = h1;
			t = h2;
			footCase = footIdx;
		}
	
	} 
}


void MeshGrid::PR_ProcessCube(float p[3], float nv[3], float qrad, int gx, int gy, int gz, Face **pMinTri, float &qdist, float &s, float &t, int &footCase)
{
	int i, i1, i2, triIdx, footIdx;
	float xmin, xmax, ymin, ymax, zmin, zmax;
	float d2, dx, dy, dz;
	float p1[3], p2[3], p3[3], h1, h2;
	CGridCube *pCube;

	
	if ( (pCube=GetCube(gx, gy, gz)) == NULL)
		return;


	m_iPR_ProcessedCubes++;

	
	xmin = m_fXMin+gx*m_fResolution;
	xmax = xmin+m_fResolution;
	ymin = m_fYMin+gy*m_fResolution;
	ymax = ymin+m_fResolution;
	zmin = m_fZMin+gz*m_fResolution;
	zmax = zmin+m_fResolution;


	if (p[0] < xmin)
		dx = xmin-p[0];
	else if (p[0] > xmax)
		dx = p[0]-xmax;
	else
		dx = 0.0f;

	if (p[1] < ymin)
		dy = ymin-p[1];
	else if (p[1] > ymax)
		dy = p[1]-ymax;
	else
		dy = 0.0f;

	if (p[2] < zmin)
		dz = zmin-p[2];
	else if (p[2] > zmax)
		dz = p[2]-zmax;
	else
		dz = 0.0f;

	d2 = dx*dx + dy*dy + dz*dz;

	if (d2 > qrad)
		return;

	if (*pMinTri != NULL)
		if (d2 > qdist)
			return;

	i1 = pCube->m_iTriIdx;
	i2 = i1 + pCube->m_iNumTris;
	for (i=i1; i < i2; i++)
	{
	
		m_iPR_ProcessedTris++;

	
		triIdx = m_vCubeTris[i];

		
		p1[0] = m_pMesh->F(triIdx)->N()[0];
		p1[1] = m_pMesh->F(triIdx)->N()[1]; 
		p1[2] = m_pMesh->F(triIdx)->N()[2];
		
		if (this->ScalarProduct(p1, nv) < 0)
			continue;

		


		Face * f = m_pMesh->F(triIdx);
		Vertex * pPt1 = f->V(0);
		Vertex * pPt2 = f->V(1);
		Vertex * pPt3 = f->V(2);
		

		p1[0] = pPt1->P()[0];
		p1[1] = pPt1->P()[1];
		p1[2] = pPt1->P()[2];

		p2[0] = pPt2->P()[0];
		p2[1] = pPt2->P()[1];
		p2[2] = pPt2->P()[2];

		p3[0] = pPt3->P()[0];
		p3[1] = pPt3->P()[1];
		p3[2] = pPt3->P()[2];

		
		d2 = this->CalcSqrDistPtTri(p, p1, p2, p3, h1, h2, footIdx);
		              
		if (*pMinTri == NULL)
		{
			*pMinTri = m_pMesh->F(triIdx);
			qdist = d2;
			s = h1;
			t = h2;
			footCase = footIdx;
			continue;
		}

		if (d2 < qdist)
		{
			*pMinTri = m_pMesh->F(triIdx);
			qdist = d2;
			s = h1;
			t = h2;
			footCase = footIdx;
		}
	
	} 
}


#define FOOT_PT_INNER		0
#define FOOT_PT_EDGE12		1
#define FOOT_PT_EDGE23		2
#define FOOT_PT_EDGE31		3
#define FOOT_PT_VERTEX1		4
#define FOOT_PT_VERTEX2		5
#define FOOT_PT_VERTEX3		6

int MeshGrid::PR_CalcSign(float q[3], Face *pMinTri, float s, float t, int footCase)
{
	int i, vertIdx, triIdx, ptIdx=-1, sign1, sign2;
	float p1[3], p2[3], p3[3], foot[3], n1[3], n2[3], n[3], p[3][3], w;
	Face *pNbTri=NULL, *pTri;

	triIdx = pMinTri->Index();
	
	Face * f = m_pMesh->F(triIdx);
	Vertex * pPt1 = f->V(0);
	Vertex * pPt2 = f->V(1);
	Vertex * pPt3 = f->V(2);
	

	p1[0] = pPt1->P()[0];
	p1[1] = pPt1->P()[1];
	p1[2] = pPt1->P()[2];

	p2[0] = pPt2->P()[0];
	p2[1] = pPt2->P()[1];
	p2[2] = pPt2->P()[2];

	p3[0] = pPt3->P()[0];
	p3[1] = pPt3->P()[1];
	p3[2] = pPt3->P()[2];

	foot[0] = p1[0] + s*(p2[0]-p1[0]) + t*(p3[0]-p1[0]);
	foot[1] = p1[1] + s*(p2[1]-p1[1]) + t*(p3[1]-p1[1]);
	foot[2] = p1[2] + s*(p2[2]-p1[2]) + t*(p3[2]-p1[2]);

	
	switch(footCase)
	{
		case FOOT_PT_INNER:
			return PR_GetSignProjection(q, foot, pMinTri);

		case FOOT_PT_EDGE12: 
			
			pNbTri = pMinTri->FFp(0);
			break;

		case FOOT_PT_EDGE23: 
			pNbTri = pMinTri->FFp(1);
			break;
		
		case FOOT_PT_EDGE31: 
			pNbTri = pMinTri->FFp(2);
			break;
		
		case FOOT_PT_VERTEX1: 
			ptIdx = pMinTri->V(0)->Index();
			break;

		case FOOT_PT_VERTEX2: 
			ptIdx = pMinTri->V(1)->Index();
			break;

		case FOOT_PT_VERTEX3: 
			ptIdx = pMinTri->V(2)->Index();
		
			break;
	} 

	
	if (pNbTri == NULL && ptIdx < 0)
		return PR_GetSignProjection(q, foot, pMinTri);

	
	if (pNbTri != NULL)
	{
		sign1 = PR_GetSignProjection(q, foot, pMinTri);
		sign2 = PR_GetSignProjection(q, foot, pNbTri);

	
		if (sign1 == sign2)
			return sign1;

		n1[0] = pMinTri->N()[0];
		n1[1] = pMinTri->N()[1];
		n1[2] = pMinTri->N()[2];

		n2[0] = pNbTri->N()[0];
		n2[1] = pNbTri->N()[1];
		n2[2] = pNbTri->N()[2];

		
		n[0] = n1[0]+n2[0];
		n[1] = n1[1]+n2[1];
		n[2] = n1[2]+n2[2];

	
		if ( (q[0]-foot[0])*n[0] + (q[1]-foot[1])*n[1] + (q[2]-foot[2])*n[2] > 0)
			return 1;
		else
			return -1;
	}

	
	vector<Face*> vNbTris;
	this->GetTrisOfPt(ptIdx, vNbTris);
	n[0] = n[1] = n[2] = 0;
	
	for (i=0; i < vNbTris.size(); i++)
	{
		pTri = vNbTris[i];
		
		
		if (pTri->V(0)->Index()== ptIdx)
			vertIdx = 0;
	
		else if (pTri->V(1)->Index() == ptIdx)
			vertIdx = 1;
		else
			vertIdx = 2;

	
		triIdx = pTri->Index();

		Face * f = m_pMesh->F(triIdx);
		Vertex * pPt1 = f->V(0);
		Vertex * pPt2 = f->V(1);
		Vertex * pPt3 = f->V(2);
		

		p[0][0] = pPt1->P()[0];
		p[0][1] = pPt1->P()[1];
		p[0][2] = pPt1->P()[2];

		p[1][0] = pPt2->P()[0];
		p[1][1] = pPt2->P()[1];
		p[1][2] = pPt2->P()[2];

		p[2][0] = pPt3->P()[0];
		p[2][1] = pPt3->P()[1];
		p[2][2] = pPt3->P()[2];

	
		w = this->CalcInnerTriAngle(p[(vertIdx+2)%3], p[vertIdx], p[(vertIdx+1)%3]);

	
		n1[0] = pTri->N()[0];
		n1[1] = pTri->N()[1];
		n1[2] = pTri->N()[2];

		n[0] += (w*n1[0]);
		n[1] += (w*n1[1]);
		n[2] += (w*n1[2]);
	}


	if ( (q[0]-foot[0])*n[0] + (q[1]-foot[1])*n[1] + (q[2]-foot[2])*n[2] > 0)
		return 1;

	return -1;
}

int MeshGrid::PR_GetSignProjection(float q[3], float foot[3], Face *pTri)
{
	float nrv[3], dir[3];


	nrv[0] = pTri->N()[0];
	nrv[1] = pTri->N()[1];
	nrv[2] = pTri->N()[2];

	dir[0] = q[0] - foot[0];
	dir[1] = q[1] - foot[1];
	dir[2] = q[2] - foot[2];

	if (dir[0]*nrv[0] + dir[1]*nrv[1] + dir[2]*nrv[2] >= 0)
		return 1;
	else
		return -1;
}

bool MeshGrid::GetCubeIdx(float x, float y, float z, int &gx, int &gy, int &gz)
{
	gx = (int)floor( ( x-m_fXMin ) / m_fResolution );
	if (gx < 0 || gx >= m_iDimX)
		return false;

	gy = (int)floor( ( y-m_fYMin ) / m_fResolution );
	if (gy < 0 || gy >= m_iDimY)
		return false;

	gz = (int)floor( ( z-m_fZMin ) / m_fResolution );
	if (gz < 0 || gz >= m_iDimZ)
		return false;

	return true;
}
void MeshGrid::GetBoxCubes(float x, float y, float z, float rad, int& mingx, int& maxgx, int& mingy, int& maxgy, int& mingz, int& maxgz)
{
	GetPtCube(x-rad, y-rad, z-rad, mingx, mingy, mingz);
	GetPtCube(x+rad, y+rad, z+rad, maxgx, maxgy, maxgz);
	
	mingx = max(mingx, 0);
	maxgx = min(maxgx, m_iDimX-1);
	mingy = max(mingy, 0);
	maxgy = min(maxgy, m_iDimY-1);
	mingz = max(mingz, 0);
	maxgz = min(maxgz, m_iDimZ-1);
}

bool  MeshGrid::CheckOverlapCubeTriangle(float cubePt[3], float boxSize[3], float triPt1[3], float triPt2[3], float triPt3[3])
{
	float boxcenter[3], boxhalfsize[3];
	float v0[3], v1[3], v2[3];
	float e0[3], e1[3], e2[3];
	float fex, fey, fez;
	float p0, p1, p2;
	float mini, maxi;
	float rad;
	float nrv[3];
	float d;

	boxhalfsize[0] = 0.5f*boxSize[0];
	boxhalfsize[1] = 0.5f*boxSize[1];
	boxhalfsize[2] = 0.5f*boxSize[2];

	boxcenter[0] = cubePt[0] + boxhalfsize[0];
	boxcenter[1] = cubePt[1] + boxhalfsize[1];
	boxcenter[2] = cubePt[2] + boxhalfsize[2];

	v0[0] = triPt1[0] - boxcenter[0];
	v0[1] = triPt1[1] - boxcenter[1];
	v0[2] = triPt1[2] - boxcenter[2];
	v1[0] = triPt2[0] - boxcenter[0];
	v1[1] = triPt2[1] - boxcenter[1];
	v1[2] = triPt2[2] - boxcenter[2];
	v2[0] = triPt3[0] - boxcenter[0];
	v2[1] = triPt3[1] - boxcenter[1];
	v2[2] = triPt3[2] - boxcenter[2];

	e0[0] = v1[0] - v0[0];
	e0[1] = v1[1] - v0[1];
	e0[2] = v1[2] - v0[2];
	e1[0] = v2[0] - v1[0];
	e1[1] = v2[1] - v1[1];
	e1[2] = v2[2] - v1[2];
	e2[0] = v0[0] - v2[0];
	e2[1] = v0[1] - v2[1];
	e2[2] = v0[2] - v2[2];

	fex = fabs(e0[0]);
	fey = fabs(e0[1]);
	fez = fabs(e0[2]);

	p0 = e0[2] * v0[1] - e0[1] * v0[2];
	p2 = e0[2] * v2[1] - e0[1] * v2[2];
	mini = min(p0, p2);
	maxi = max(p0, p2);
	rad = fez * boxhalfsize[1] + fey * boxhalfsize[2];
	if (mini > rad || maxi < -rad)
		return false;

	p0 = -e0[2] * v0[0] + e0[0] * v0[2];
	p2 = -e0[2] * v2[0] + e0[0] * v2[2];
	mini = min(p0, p2);
	maxi = max(p0, p2);
	rad = fez * boxhalfsize[0] + fex * boxhalfsize[2];   \
		if (mini > rad || maxi < -rad)
			return false;

	p1 = e0[1] * v1[0] - e0[0] * v1[1];
	p2 = e0[1] * v2[0] - e0[0] * v2[1];
	mini = min(p1, p2);
	maxi = max(p1, p2);
	rad = fey * boxhalfsize[0] + fex * boxhalfsize[1];
	if (mini > rad || maxi < -rad)
		return false;

	fex = fabs(e1[0]);
	fey = fabs(e1[1]);
	fez = fabs(e1[2]);

	p0 = e1[2] * v0[1] - e1[1] * v0[2];
	p2 = e1[2] * v2[1] - e1[1] * v2[2];
	mini = min(p0, p2);
	maxi = max(p0, p2);
	rad = fez * boxhalfsize[1] + fey * boxhalfsize[2];
	if (mini > rad || maxi < -rad)
		return false;

	p0 = -e1[2] * v0[0] + e1[0] * v0[2];
	p2 = -e1[2] * v2[0] + e1[0] * v2[2];
	mini = min(p0, p2);
	maxi = max(p0, p2);
	rad = fez * boxhalfsize[0] + fex * boxhalfsize[2];   \
		if (mini > rad || maxi < -rad)
			return false;

	p0 = e1[1] * v0[0] - e1[0] * v0[1];
	p1 = e1[1] * v1[0] - e1[0] * v1[1];
	mini = min(p0, p1);
	maxi = max(p0, p1);
	rad = fey * boxhalfsize[0] + fex * boxhalfsize[1];
	if (mini > rad || maxi < -rad)
		return false;

	fex = fabs(e2[0]);
	fey = fabs(e2[1]);
	fez = fabs(e2[2]);

	p0 = e2[2] * v0[1] - e2[1] * v0[2];
	p1 = e2[2] * v1[1] - e2[1] * v1[2];
	mini = min(p0, p1);
	maxi = max(p0, p1);
	rad = fez * boxhalfsize[1] + fey * boxhalfsize[2];
	if (mini > rad || maxi < -rad)
		return false;

	p0 = -e2[2] * v0[0] + e2[0] * v0[2];
	p1 = -e2[2] * v1[0] + e2[0] * v1[2];
	mini = min(p0, p1);
	maxi = max(p0, p1);
	rad = fez * boxhalfsize[0] + fex * boxhalfsize[2];
	if (mini > rad || maxi < -rad)
		return false;

	p1 = e2[1] * v1[0] - e2[0] * v1[1];
	p2 = e2[1] * v2[0] - e2[0] * v2[1];
	mini = min(p1, p2);
	maxi = max(p1, p2);
	rad = fey * boxhalfsize[0] + fex * boxhalfsize[1];
	if (mini > rad || maxi < -rad)
		return false;


	mini = min(v0[0], min(v1[0], v2[0]));
	maxi = max(v0[0], max(v1[0], v2[0]));
	if (mini > boxhalfsize[0] || maxi < -boxhalfsize[0])
		return false;

 
	mini = min(v0[1], min(v1[1], v2[1]));
	maxi = max(v0[1], max(v1[1], v2[1]));
	if (mini > boxhalfsize[1] || maxi < -boxhalfsize[1])
		return false;


	mini = min(v0[2], min(v1[2], v2[2]));
	maxi = max(v0[2], max(v1[2], v2[2]));
	if (mini > boxhalfsize[2] || maxi < -boxhalfsize[2])
		return false;

	
	VectorProduct(e0, e1, nrv);
	d = -(nrv[0] * v0[0] + nrv[1] * v0[1] + nrv[2] * v0[2]);
	if (!PlaneBoxOverlap(nrv, d, boxhalfsize))
		return false;


	return true;
}
float MeshGrid::CalcSqrDistPtTri(float q[3], float t1[3], float t2[3], float t3[3], float &s, float &t, int &footCase)
{
	
	int region;
	float E0[3], E1[3], PB[3];
	float a, b, c, d, e, f, det;
	float tmp0, tmp1, numer, denom;
	float invDet, sqrDist = 0.0f;


	E0[0] = t2[0] - t1[0];
	E0[1] = t2[1] - t1[1];
	E0[2] = t2[2] - t1[2];
	E1[0] = t3[0] - t1[0];
	E1[1] = t3[1] - t1[1];
	E1[2] = t3[2] - t1[2];
	PB[0] = t1[0] - q[0];
	PB[1] = t1[1] - q[1];
	PB[2] = t1[2] - q[2];

	a = E0[0] * E0[0] + E0[1] * E0[1] + E0[2] * E0[2];
	b = E0[0] * E1[0] + E0[1] * E1[1] + E0[2] * E1[2];
	c = E1[0] * E1[0] + E1[1] * E1[1] + E1[2] * E1[2];
	d = E0[0] * PB[0] + E0[1] * PB[1] + E0[2] * PB[2];
	e = E1[0] * PB[0] + E1[1] * PB[1] + E1[2] * PB[2];
	f = PB[0] * PB[0] + PB[1] * PB[1] + PB[2] * PB[2];

	det = fabs(a*c - b*b);
	s = b*e - c*d;
	t = b*d - a*e; 

	if (s + t <= det)
	{
		if (s < 0.0f)
		{
			if (t < 0.0f)
				region = 4;
			else
				region = 3;
		}
		else if (t < 0.0f)
			region = 5;
		else
			region = 0;
	}
	else
	{
		if (s < 0.0f)
			region = 2;
		else if (t < 0.0f)
			region = 6;
		else
			region = 1;
	}

	
	switch (region)
	{
	case 0:
	
		invDet = 1.0f / det;
		s *= invDet;
		t *= invDet;
		sqrDist = s*(a*s + b*t + 2.0f*d) + t*(b*s + c*t + 2.0f*e) + f;
		footCase = FOOT_PT_INNER;
		break;

	case 1:
		numer = c + e - b - d;
		if (numer <= 0.0f)
		{
			s = 0.0f;
			t = 1.0f;
			sqrDist = c + 2.0f*e + f;
			footCase = FOOT_PT_VERTEX3;
		}
		else
		{
			denom = a - 2.0f*b + c;
			if (numer >= denom)
			{
				s = 1.0f;
				t = 0.0f;
				sqrDist = a + 2.0f*d + f;
				footCase = FOOT_PT_VERTEX2;
			}
			else
			{
				s = numer / denom;
				t = 1.0f - s;
				sqrDist = s*(a*s + b*t + 2.0f*d) + t*(b*s + c*t + 2.0f*e) + f;
				footCase = FOOT_PT_EDGE23;
			}
		}

		break;

	case 2:
		tmp0 = b + d;
		tmp1 = c + e;
		if (tmp1 > tmp0)
		{
			numer = tmp1 - tmp0;
			denom = a - 2.0f*b + c;
			if (numer >= denom)
			{
				s = 1.0f;
				t = 0.0f;
				sqrDist = a + 2.0f*d + f;
				footCase = FOOT_PT_VERTEX2;
			}
			else
			{
				s = numer / denom;
				t = 1.0f - s;
				sqrDist = s*(a*s + b*t + 2.0f*d) + t*(b*s + c*t + 2.0f*e) + f;
				footCase = FOOT_PT_EDGE23;
			}
		}
		else
		{
			s = 0.0f;
			if (tmp1 <= 0.0f)
			{
				t = 1.0f;
				sqrDist = c + 2.0f*e + f;
				footCase = FOOT_PT_VERTEX3;
			}
			else if (e >= 0.0f)
			{
				t = 0.0f;
				sqrDist = f;
				footCase = FOOT_PT_VERTEX1;
			}
			else
			{
				t = -e / c;
				sqrDist = e*t + f;
				footCase = FOOT_PT_EDGE31;
			}
		}

		break;

	case 3:
		s = 0.0f;
		if (e >= 0.0f)
		{
			t = 0.0f;
			sqrDist = f;
			footCase = FOOT_PT_VERTEX1;
		}
		else if (-e >= c)
		{
			t = 1.0f;
			sqrDist = c + 2.0f*e + f;
			footCase = FOOT_PT_VERTEX3;
		}
		else
		{
			t = -e / c;
			sqrDist = e*t + f;
			footCase = FOOT_PT_EDGE31;
		}

		break;

	case 4:
		if (d < 0.0f)
		{
			t = 0.0f;
			if (-d >= a)
			{
				s = 1.0f;
				sqrDist = a + 2.0f*d + f;
				footCase = FOOT_PT_VERTEX2;
			}
			else
			{
				s = -d / a;
				sqrDist = d*s + f;
				footCase = FOOT_PT_EDGE12;
			}
		}
		else
		{
			s = 0.0f;
			if (e >= 0.0f)
			{
				t = 0.0f;
				sqrDist = f;
				footCase = FOOT_PT_VERTEX1;
			}
			else if (-e >= c)
			{
				t = 1.0f;
				sqrDist = c + 2.0f*e + f;
				footCase = FOOT_PT_VERTEX3;
			}
			else
			{
				t = -e / c;
				sqrDist = e*t + f;
				footCase = FOOT_PT_EDGE31;
			}
		}

		break;

	case 5:
		t = 0.0f;
		if (d >= 0.0f)
		{
			s = 0.0f;
			sqrDist = f;
			footCase = FOOT_PT_VERTEX1;
		}
		else if (-d >= a)
		{
			s = 1.0f;
			sqrDist = a + 2.0f*d + f;
			footCase = FOOT_PT_VERTEX2;
		}
		else
		{
			s = -d / a;
			sqrDist = d*s + f;
			footCase = FOOT_PT_EDGE12;
		}

		break;

	case 6:
		tmp0 = b + e;
		tmp1 = a + d;
		if (tmp1 > tmp0)
		{
			numer = tmp1 - tmp0;
			denom = a - 2.0f*b + c;
			if (numer >= denom)
			{
				t = 1.0f;
				s = 0.0f;
				sqrDist = c + 2.0f*e + f;
				footCase = FOOT_PT_VERTEX3;
			}
			else
			{
				t = numer / denom;
				s = 1.0f - t;
				sqrDist = s*(a*s + b*t + 2.0f*d) + t*(b*s + c*t + 2.0f*e) + f;
				footCase = FOOT_PT_EDGE23;
			}
		}
		else
		{
			t = 0.0f;
			if (tmp1 <= 0.0f)
			{
				s = 1.0f;
				sqrDist = a + 2.0f*d + f;
				footCase = FOOT_PT_VERTEX2;
			}
			else if (d >= 0.0f)
			{
				s = 0.0f;
				sqrDist = f;
				footCase = FOOT_PT_VERTEX1;
			}
			else
			{
				s = -d / a;
				sqrDist = d*s + f;
				footCase = FOOT_PT_EDGE12;
			}
		}

		break;
	} 
	if (_isnan(s) != 0 || _isnan(t) != 0)
	{
		
		float d1 = SqrDist(q, t1);
		float d2 = SqrDist(q, t2);
		float d3 = SqrDist(q, t3);

		if (d1 <= d2 && d1 <= d3)
		{
			s = 0;
			t = 0;
			sqrDist = d1;
			footCase = FOOT_PT_VERTEX1;
		}
		else if (d2 <= d1 && d2 <= d3)
		{
			s = 1;
			t = 0;
			sqrDist = d2;
			footCase = FOOT_PT_VERTEX2;
		}
		else
		{
			s = 0;
			t = 1;
			sqrDist = d3;
			footCase = FOOT_PT_VERTEX3;
		}
	}

	return fabs(sqrDist);
}
float MeshGrid::ScalarProduct(float a[3], float b[3])
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}
float MeshGrid::SqrDist(float a[3], float b[3])
{
	return max(0, (a[0] - b[0])*(a[0] - b[0]) + (a[1] - b[1])*(a[1] - b[1]) + (a[2] - b[2])*(a[2] - b[2]));
}
float MeshGrid::SqrDist(float pax, float pay, float paz, float pbx, float pby, float pbz)
{
	return max(0, (pax - pbx)*(pax - pbx) + (pay - pby)*(pay - pby) + (paz - pbz)*(paz - pbz));
}

float MeshGrid::CalcInnerTriAngle(float q1[3], float q2[3], float q3[3])
{
	float dir1[3], dir2[3];

	dir1[0] = q1[0] - q2[0];
	dir1[1] = q1[1] - q2[1];
	dir1[2] = q1[2] - q2[2];
	dir2[0] = q3[0] - q2[0];
	dir2[1] = q3[1] - q2[1];
	dir2[2] = q3[2] - q2[2];

	Normalize(dir1[0], dir1[1], dir1[2]);
	Normalize(dir2[0], dir2[1], dir2[2]);

	return CalcAngle(dir1, dir2);
}

int MeshGrid::Normalize(float& nx, float& ny, float& nz)
{
	float len = sqrt(nx*nx + ny*ny + nz*nz);

	if (len != 0.0f)
	{
		nx = nx / len;
		ny = ny / len;
		nz = nz / len;
		return 0;
	}

	nx = 0.0f;
	ny = 0.0f;
	nz = 0.0f;
	return -1;
}
void  MeshGrid::VectorProduct(float v1[3], float v2[3], float res[3])
{
	res[0] = v1[1] * v2[2] - v1[2] * v2[1];
	res[1] = v1[2] * v2[0] - v1[0] * v2[2];
	res[2] = v1[0] * v2[1] - v1[1] * v2[0];
}
float MeshGrid::CalcAngle(float dir1[3], float dir2[3])
{
	float sp, c[3];

	sp = ScalarProduct(dir1, dir2);
	if (-0.7 < sp && sp < 0.7f) // approx. +/-45?
		return acos(sp);

	VectorProduct(dir1, dir2, c);

	if (sp > 0)
		return asin(sqrt(c[0] * c[0] + c[1] * c[1] + c[2] * c[2]));
	else
		return PI - asin(sqrt(c[0] * c[0] + c[1] * c[1] + c[2] * c[2]));
}
int   MeshGrid::GetTrisOfPt(int ptIdx, vector<Face*>& vNbTris)
{
	int i;
	VFIterator vfi(m_pMesh->V(ptIdx));
	vNbTris.clear();
	for (;!vfi.End();++vfi)
	{
		vNbTris.push_back(vfi.f);

	}
	return vNbTris.size();
}
float MeshGrid::DistPt2Plane(float pt[3], float planePt[3], float planeNrv[3])
{
	return fabs((pt[0] - planePt[0]) * planeNrv[0]
		+ (pt[1] - planePt[1]) * planeNrv[1]
		+ (pt[2] - planePt[2]) * planeNrv[2]);
}
bool  MeshGrid::PlaneBoxOverlap(float nrv[3], float d, float boxhalfsize[3])
{
	int i;
	float vmin[3], vmax[3];

	for (i = 0; i < 3; i++)
	{
		if (nrv[i] > 0.0f)
		{
			vmin[i] = -boxhalfsize[i];
			vmax[i] = boxhalfsize[i];
		}
		else
		{
			vmin[i] = boxhalfsize[i];
			vmax[i] = -boxhalfsize[i];
		}
	}

	if (nrv[0] * vmin[0] + nrv[1] * vmin[1] + nrv[2] * vmin[2] + d > 0.0f)
		return false;

	if (nrv[0] * vmax[0] + nrv[1] * vmax[1] + nrv[2] * vmax[2] + d >= 0.0f)
		return true;


	return false;
}
float MeshGrid::ProjPt2Plane(float pt[3], float planePt[3], float planeNrv[3], float projPt[3])
{
	float dist;

	dist = (pt[0] - planePt[0]) * planeNrv[0]
		+ (pt[1] - planePt[1]) * planeNrv[1]
		+ (pt[2] - planePt[2]) * planeNrv[2];

	projPt[0] = pt[0] - dist * planeNrv[0];
	projPt[1] = pt[1] - dist * planeNrv[1];
	projPt[2] = pt[2] - dist * planeNrv[2];

	return dist;
}
