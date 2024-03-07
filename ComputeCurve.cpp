#include "ComputeCurve.h"

#include"Graphics/sn3DTopology.h"
#include"../PQP/include/PQP.h"
#include"MeshBoundaryExtract.h"
#include "../Common/quaternion.h"
#include"MeshGrid.h"
#include"meshclean.h"

#include <iostream>
using namespace Common;



void ComputeCurve::DoComputeCurve(sn3DMeshModel * model, float thresholdZ, float normalZ, int step, float interval, int smoothness)
{
	MeshClean clean;
	clean.RemoveDegenerateFaces(model->GetMeshData());
	/*
	1. Find separate meshes
	2. Find gum
	3. Find boundaries
	4. Smooth curve
	*/

	FindRegion(model->GetMeshData(), m_modelSet);
	FindGum(m_modelSet, thresholdZ);
	ComputeContact(m_modelSet);


	int siz = m_modelSet.size();
	sn3DMeshModel * gum = 0;
	int cnt = 0;
	int *  vlist;
	for (int i = 0; i<siz; i++)
	{
		if (m_modelSet[i]->IsS())
		{
			gum = m_modelSet[i];
			SelectedRegionProcessing(gum->GetMeshData());
			RemoveIsolatedRegion(gum->GetMeshData());

			MeshBoundaryExtract extract;
			cnt = extract.FindLargestBoundary(gum->GetMeshData(), vlist);
			for (int k = 0; k<cnt; k++)
			{
				Point3f p = gum->GetMeshData()->V(vlist[k])->P();
				model->m_curve.push_back(p);
			}
			SmoothCurve(1, model->m_curve, gum);
		}
	}
	if (gum == NULL)
		return;

	gum->UpdateAll();
	std::vector<Point3f>  smapleTangent;
	std::vector<Point3f>  smapleBinormal;

	//int step = g_offsettep;
	for (int k = 0; k < step; k++)
	{
		SampleCurve(model->m_curve, model->m_samplePoints, smapleTangent, 1900); // 等间距采样

		FindTriangleNomral(model->m_samplePoints, smapleTangent, model->m_sampleNormals, gum);
		CrossNormal(smapleTangent, model->m_sampleNormals, smapleBinormal);

		CurveOffset(0.05, model->m_samplePoints, smapleBinormal, gum);
		SmoothCurve(3, model->m_samplePoints, model);
		model->m_curve = model->m_samplePoints;

	}

	for (int i = 0; i < smoothness; i++)
		SmoothCurve(3, model->m_curve, gum);

	float curLen = GetCurveLength(model->m_curve);
	float nodeNum = curLen/interval;
	SampleCurve(model->m_curve, model->m_samplePoints, smapleTangent, nodeNum);
	FindTriangleNomral(model->m_samplePoints, smapleTangent, model->m_sampleNormals, gum);

	SetNormalComponetZ(model->m_sampleNormals, normalZ);

	int nomralIter = 5;
	for (int i = 0; i<nomralIter; i++)
		SmoothNormal(model->m_sampleNormals);

	for (int i = 0; i < m_modelSet.size(); i++)
	{
		delete m_modelSet[i];
	}
	m_modelSet.clear();
}

void  ComputeCurve::FindRegion(sn3DMeshData * mesh,std::vector<sn3DMeshModel *> & modelSet)
{
	bool process = false;

	Topology::VertexFace(*mesh);

	int fn = mesh->fn;
	for (int i = 0; i<fn; i++)
	{
		mesh->F(i)->ClearV();
	}

	int regionID = 0;
	std::vector<int> regionScale; // Face联通区域计数

	std::vector<int> faceID; // 面分类
	faceID.resize(fn);

	//--------  分析区域联通性 --------
	int startIndex; // 搜索起点 
	do {
		startIndex = -1;
		for (int i = 0; i<fn; i++)
		{
			if (!mesh->F(i)->IsV())
			{
				startIndex = i;
				break;
			}
		}

		if (startIndex == -1)break;  // 搜索结束

		//-------- 搜索联通区域 --------
		std::vector<Face*> region;
		mesh->F(startIndex)->SetV(); // 控制重复
		region.push_back(mesh->F(startIndex));

		int size = (int)region.size();
		int searchIndex = 0;
		do {
			for (; searchIndex < size; searchIndex++)
			{
				Face * f = region[searchIndex];
				for (int i = 0; i < 3; i++)
				{
					VFIterator iter(f->V(i));
					for (; !iter.End(); ++iter)
					{
						for (int j = 0; j < 3; j++)
							if (!iter.f->IsV())
							{
								region.push_back(iter.f); // 搜索
								iter.f->SetV();
							}
					}
				}
			} // front advance
			size = (int)region.size();
		} while (searchIndex<size);

		//====== 记录区域信息
		for (int i = 0; i < region.size(); i++)
		{
			faceID[region[i]->Index()] = regionID;
		}
		regionScale.push_back(region.size());
		regionID++;
	} while (1);

	//========= 记录联通区域 regionID  faceID  regionScale

	int cnt = (int)regionScale.size();

	for (int kk=0;kk<cnt;kk++)
	{
		sn3DMeshModel * newModel = new sn3DMeshModel;
		sn3DMeshData * newData = newModel->AddMesh();
		for (int i = 0; i<fn; i++)
		{
			if (faceID[i] != kk)
				continue;

				Face * f = mesh->F(i);
				Point3f p0 = f->V(0)->P();
				Point3f p1 = f->V(1)->P();
				Point3f p2 = f->V(2)->P();
				int vn = newData->vn;
				newData->AddVertex(p0);
				newData->AddVertex(p1);
				newData->AddVertex(p2);
				newData->AddFace(vn,vn+1,vn+2);	
		}
		Topology::RemoveRedundant(newModel->GetMeshData());
		modelSet.push_back(newModel);
	}

	std::cout << "model set number(connected region number): " <<  modelSet.size() << "\n";
}
void  ComputeCurve::FindGum(std::vector<sn3DMeshModel *> & modelSet, float thresholdZ)
{
	int siz = modelSet.size();
	std::vector<float> modelLength;
	for (int i=0;i<siz;i++)
	{
		modelSet[i]->UpdateBox();
		modelSet[i]->ClearS(); //牙龈
		modelSet[i]->ClearM(); //牙齿
		modelLength.push_back(modelSet[i]->BBox.Diag());
	}

	int i, j;
	for (i = 0; i < siz - 1; i++)
	{
		for (j = 0; j < siz - i - 1; j++)
		{
			if (modelLength[j] < modelLength[j + 1])
			{
				int temp = modelLength[j];
				modelLength[j] = modelLength[j + 1];
				modelLength[j + 1] = temp;

				sn3DMeshModel * tempModel = modelSet[j];
				modelSet[j] = modelSet[j + 1];
				modelSet[j + 1] = tempModel;
			}
		}
	}
	//根据条件剔除额外的模型，如附件，文字，底板等
	std::vector<int> indexArray;
	// 1 如果高度小于某个阈值，则认为是底板或文字
	//float thresholdZ = 12;
	for (int i = 0; i < siz; i++)
	{
		float maxZ = modelSet[i]->BBox.max[2];
		if (maxZ > thresholdZ)
			indexArray.push_back(i);
	}
	if (indexArray.size() == 0)
		return;

	modelSet[indexArray[0]]->SetS(); //牙龈

	float lenThresh = modelLength[indexArray[1]]/2.0;
	for (int i = 1; i < indexArray.size(); i++)
	{
		int index = indexArray[i];
		if (modelLength[index] > lenThresh)
		{
			modelSet[index]->SetM();
		}
	}

}
void  ComputeCurve::ComputeContact(std::vector<sn3DMeshModel *> & modelSet)
{
	std::vector<PQP_Model*> PQPModels;
	
	int siz = modelSet.size();

	for (int k = 0; k < siz; k++)
	{
		if (!modelSet[k]->IsS() && !modelSet[k]->IsM())
		{
			PQPModels.push_back(0);
			continue;
		}
		PQP_Model *o = new PQP_Model;

		sn3DMeshData * data = modelSet[k]->GetMeshData();
		int fn = data->fn;
		o->BeginModel();              // begin the model
		for (int i = 0; i < fn; i++)
		{
			Face * f = data->F(i);

			Point3f p1 = f->P(0);
			Point3f p2 = f->P(1);
			Point3f p3 = f->P(2);

			PQP_REAL pp1[3];
			PQP_REAL pp2[3];
			PQP_REAL pp3[3];

			for (int j = 0; j < 3; j++)
			{
				pp1[j] = p1[j];
				pp2[j] = p2[j];
				pp3[j] = p3[j];
			}

			o->AddTri(pp1, pp2, pp3, i);
		}
		o->EndModel();
		PQPModels.push_back(o);
	}

	//----------------------------------------

	int gumIndex = -1;
	sn3DMeshModel * gum = 0;
	
	for (int i=0;i<siz;i++)
	{
		if (modelSet[i]->IsS())
		{
			gumIndex = i;
			gum = modelSet[i];
		}
	}

	for (int i = 0; i<siz; i++)
	{
		if (modelSet[i]->IsS())
			continue;

		if (!modelSet[i]->IsM())
			continue;

		sn3DMeshModel * tooth = modelSet[i];

		PQP_Model * o1 = PQPModels[i];
		PQP_Model * o2 = PQPModels[gumIndex];
		PQP_CollideResult result;

		PQP_REAL R1[3][3], R2[3][3];
		PQP_REAL T1[3], T2[3];
		PQP_REAL rel_err = 0.01;
		PQP_REAL abs_err = 0.01;
		Quaternionf  rot;
		Point3f t = Point3f(0,0,0);
		Matrix33f mat33;
		mat33.SetIdentity();

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				R1[i][j] = mat33[j][i];
			}
			T1[i] = t[i];
		}

	
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				R2[i][j] = mat33[j][i];
			}
			T2[i] = t[i];
		}


		PQP_Collide(&result, R1, T1, o1, R2, T2, o2);

		int num = result.NumPairs();
		
		for (int k=0;k<num;k++)
		{
			int fid1 = result.Id1(k);
			int fid2 = result.Id2(k);

			gum->GetMeshData()->F(fid2)->SetS();
			tooth->GetMeshData()->F(fid1)->SetS();
		}
	}
	for (int i = 0; i < PQPModels.size(); i++)
	{   if(PQPModels[i])
		delete PQPModels[i];
	}
}
void  ComputeCurve::SelectedRegionProcessing(sn3DMeshData * mesh,int iter)
{

	Topology::VertexFace(*mesh);
	Topology::FaceFace(*mesh);
	int fn = mesh->fn;
	for (int i = 0; i<fn; i++)
	{
		mesh->F(i)->ClearV();
	}

	std::vector<Face*> region;

	for (int i=0;i<fn;i++)
	{
		Face * f = mesh->F(i);
		if (!f->IsS())
			continue;
		f->SetV(); // 控制重复
		region.push_back(f);
	}

	int size = (int)region.size();
	int searchIndex = 0;
	for (int index = 0; index < iter;index++) 
	{
		for (; searchIndex < size; searchIndex++)
		{
			Face * f = region[searchIndex];
			for (int i = 0; i < 3; i++)
			{
				if (!f->FFp(i)->IsV() && !f->FFp(i)->IsS())
				{
					region.push_back(f->FFp(i)); // 搜索
					f->FFp(i)->SetV();
					f->FFp(i)->SetS();
				}

			}
		} // front advance

		size = (int)region.size();
	} ;

}
void  ComputeCurve::RemoveIsolatedRegion(sn3DMeshData * mesh)
{
	bool process = false;

	Topology::VertexFace(*mesh);
	Topology::FaceFace(*mesh);
	int fn = mesh->fn;
	for (int i = 0; i<fn; i++)
	{
		mesh->F(i)->ClearV();
	}

	int regionID = 0;
	std::vector<int> regionScale; // Face联通区域计数

	std::vector<int> faceID; // 面分类
	faceID.resize(fn);

	//========  分析区域联通性
	int startIndex; // 搜索起点 
	do {
		//========== 独立的连通区域
		startIndex = -1;
		for (int i = 0; i<fn; i++)
		{
			if (!mesh->F(i)->IsV()&& !mesh->F(i)->IsS())
			{
				startIndex = i;
				break;
			}
		}

		if (startIndex == -1)break;  // 搜索结束

		//======= 搜索联通区域 ==========
		std::vector<Face*> region;
		mesh->F(startIndex)->SetV(); // 控制重复
		region.push_back(mesh->F(startIndex));

		int size = (int)region.size();
		int searchIndex = 0;
		do {
			for (; searchIndex < size; searchIndex++)
			{
				Face * f = region[searchIndex];
				for (int i = 0; i < 3; i++)
				{
				  if(!f->FFp(i)->IsV()&& !f->FFp(i)->IsS())
				  {
					  region.push_back(f->FFp(i)); // 搜索
					  f->FFp(i)->SetV();
				  }

				}
			} // front advance

			size = (int)region.size();
		} while (searchIndex<size);


		for (int i = 0; i < region.size(); i++)
		{
			faceID[region[i]->Index()] = regionID;
		}
		regionScale.push_back(region.size());
		regionID++;
	} while (1);

	int cnt = (int)regionScale.size();

	//if (cnt <= 1) // 只有一个连通区域 //wuzq 20200708
	//	return;

	int max_num = 0;
	int max_region = -1; //最大联通区域

	for (int i = 0; i<cnt; i++)
	{
		if (regionScale[i]>max_num)
		{
			max_num = regionScale[i];
			max_region = i;
		}
	}

	for (int i = 0; i<fn; i++)
	{
		if (mesh->F(i)->IsS())
		{
			mesh->F(i)->SetD(); // 删除
			continue;
		}


		if (faceID[i] != max_region)
		{
			mesh->F(i)->SetD(); // 删除
		}
	}

	//---------- 删除孤立顶点 ---------
	int vn = mesh->vn;
	for (int i = 0; i<vn; i++)
	{
		Vertex * v = mesh->V(i);
		VFIterator vfi(v);
		int fcnt = 0;
		for (; !vfi.End(); ++vfi)
		{
			if (!vfi.f->IsD())
				fcnt++;
		}
		if (fcnt == 0)
			v->SetD();
	}

	mesh->Compact(); // 只保留一个联通区域
}
void  ComputeCurve::SmoothCurve(int it_count, std::vector<Point3f> & nodeSet,sn3DMeshModel * model )
{
	model->UpdateBox();
	float diag = model->BBox.Diag();
	MeshGrid  * meshGrid = new MeshGrid(model->GetMeshData(), diag / 25.0);

	int size = nodeSet.size();
	std::vector<Point3d> points;
	points.resize(size);

	for (int iter = 0; iter < it_count; iter++)
	{

		for (int pIndex = 0; pIndex < size; pIndex++)
		{
			Point3f p0 = nodeSet[(pIndex - 1 + size) % size];
			Point3f p = nodeSet[pIndex];
			Point3f p1 = nodeSet[(pIndex + 1) % size];

			Point3f pp = (p0 + p + p1) / 3.0;

			//========= 采样点投影 ===========
			float rad = 10.0;
			float s;
			float t;
			float dist = 10000.0;
			int footCase;

			float mp[3];
			mp[0] = pp[0];
			mp[1] = pp[1];
			mp[2] = pp[2];
			Face * tri = meshGrid->PR_ProjectPoint(mp, rad, s, t, dist, footCase, false);

			Point3f v0, v1, v2;

			v0 = tri->P(0);
			v1 = tri->P(1);
			v2 = tri->P(2);

			Point3f proj; // 投影点
			proj = v0 + (v1 - v0)*s + (v2 - v0)*t;
			points[pIndex] = Point3d(proj[0], proj[1], proj[2]);
		}

		for (int pIndex = 0; pIndex < size; pIndex++)
		{
			nodeSet[pIndex] = Point3f(points[pIndex][0], points[pIndex][1], points[pIndex][2]);
		}
	}
	delete meshGrid;
}
float  ComputeCurve::GetCurveLength(std::vector<Point3f> & curve)
{
	float sideLen = 0;

	int cnt = curve.size();
	for (int i = 0; i < cnt - 1; i++) // 计算总长度
	{
		Point3f p0 = curve[i];
		Point3f p1 = curve[i + 1];
		float edge = (p1 - p0).Norm();
		sideLen += edge;
	}
	return sideLen;
}
void  ComputeCurve::SampleCurve(std::vector<Point3f> & oldCurve, std::vector<Point3f> & newCurve, std::vector<Point3f> & newTangent, int nodeNum)
{
	float sideLen = 0;
	
	int cnt = oldCurve.size();
	for (int i = 0; i < cnt - 1; i++) // 计算总长度
	{
		Point3f p0 = oldCurve[i];
		Point3f p1 = oldCurve[i + 1];
		float edge = (p1 - p0).Norm();
		sideLen += edge;
	}

	newCurve.clear();
	newTangent.clear();
	float interval = sideLen / ((float)(nodeNum + 1));
	float curLen = 0.0;
	for (int i = 0; i < cnt - 1; i++)
	{
		Point3f p0 = oldCurve[i];
		Point3f p1 = oldCurve[i + 1];
		float edge = (p1 - p0).Norm();

		if (edge < interval) // 原来的密度较小
		{
			curLen += edge;
			if (curLen > interval) // 
			{
				curLen -= interval;
				Point3f dir = (p1 - p0).Normalize();
				Point3f p = p0 + dir*(edge - curLen);
				if (newCurve.size() < nodeNum)
				{
					newCurve.push_back(p);
					newTangent.push_back(dir);
				}
			}
		}
		else
		{
			float remainLen = curLen + edge; // 剩余总长度
			curLen = interval - curLen; // 当前edge内到节点的长度，前面的累计长度一定小于区间长度 
			while (remainLen > interval) // 
			{
				remainLen -= interval;
				Point3f dir = (p1 - p0).Normalize();
				Point3f p = p0 + dir*(curLen);
				if (newCurve.size() < nodeNum)
				{
					newCurve.push_back(p);
					newTangent.push_back(dir);
				}
				curLen += interval;
			}
			curLen = remainLen;
		}
	} // for each line segment

}

void ComputeCurve::FindTriangleNomral(std::vector<Point3f> & samplePoints, std::vector<Point3f> & smapleTangent, std::vector<Point3f> & sampleNormals,sn3DMeshModel *model)
{
	model->UpdateBox();
	float diag = model->BBox.Diag();
	MeshGrid  * meshGrid = new MeshGrid(model->GetMeshData(), diag / 25.0);
	int size = samplePoints.size();
	sampleNormals.resize(size);
	for (int pIndex = 0; pIndex < size; pIndex++)
	{
		Point3f pp = samplePoints[pIndex];
		Point3f tan = smapleTangent[pIndex];

		float rad = 10.0;
		float s;
		float t;
		float dist = 10000.0;
		int footCase;

		float mp[3];
		mp[0] = pp[0];
		mp[1] = pp[1];
		mp[2] = pp[2];

		Face * tri = meshGrid->PR_ProjectPoint(mp, rad, s, t, dist, footCase, false);
		Point3f nn = tri->N();
		sampleNormals[pIndex] = nn;
		//sampleNormals[pIndex] = (tan^nn).Normalize();
	}
	delete meshGrid;
}

void ComputeCurve::CrossNormal(std::vector<Point3f> & smapleTangent, std::vector<Point3f> & facesNormals, std::vector<Point3f> &smapleBinormal)
{
	smapleBinormal.resize(smapleTangent.size());
	for (int i = 0; i < smapleTangent.size(); i++)
	{
		Point3f tan = smapleTangent[i];
		Point3f nn = facesNormals[i];
		smapleBinormal[i] = (tan^nn).Normalize();
	}
}
void ComputeCurve::CurveOffset(float dis,std::vector<Point3f> & samplePoints, std::vector<Point3f> &sampleNormals, sn3DMeshModel * model)
{
	int size = samplePoints.size();
	for (int pIndex = 0; pIndex < size; pIndex++)
	{
		Point3f pp = samplePoints[pIndex];
		Point3f nn = sampleNormals[pIndex];
		pp += nn*dis;
		samplePoints[pIndex] = pp;
	}

}
void ComputeCurve::SmoothNormal(std::vector<Point3f> &sampleNormals)
{
	int size = sampleNormals.size();
	std::vector<Point3f> newNormals;
	newNormals.resize(size);

	for (int pIndex = 0; pIndex < size; pIndex++)
	{
		Point3f n0 = sampleNormals[(pIndex - 1 + size) % size];
		Point3f n = sampleNormals[pIndex];
		Point3f n1 = sampleNormals[(pIndex + 1) % size];

		Point3f nn = (n0 + n + n1) / 3.0;
		newNormals[pIndex] = nn.Normalize();
	}
	sampleNormals.swap(newNormals);
}
void ComputeCurve::SetCutNormal(std::vector<Point3f> & tangent, std::vector<Point3f> &sampleNormals, float degreeZ)
{
	
	for (int i = 0; i < tangent.size(); i++)
	{
		tangent[i] = tangent[i].Normalize();
	}
	sampleNormals.resize(tangent.size());

	Common::Quaternionf quat;
	Matrix33f matrix;
	Point3f dirZ(0, 0, 1);

	float phi = (degreeZ*M_PI) / 180;
	for (int i = 0; i < tangent.size(); i++)
	{
		quat.FromAxis(phi, tangent[i]);
		quat.ToMatrix(matrix);
		sampleNormals[i] = matrix*dirZ;
	}

	for (int i = 0; i < sampleNormals.size(); i++)
	{
		if (dirZ*sampleNormals[i] < 0)
			sampleNormals[i] = sampleNormals[i] * -1.0f;
	}
}
void ComputeCurve::SetNormalComponetZ(std::vector<Point3f> &sampleNormals, float degreeZ)
{
	int size = sampleNormals.size();
	std::vector<Point3f> newNormals;
	newNormals.resize(size);

	float cosN = cos(degreeZ*M_PI / 180.0f);
	
	for (int pIndex = 0; pIndex < size; pIndex++)
	{
		Point3f n = sampleNormals[pIndex];
		float xx_yy = n[0] * n[0] + n[1] * n[1];  
		if (xx_yy) //防止除零
		{
			// 公式：(a*n.x)^2 + (a*n.y)^2 + (cosN)^2 = 1
			float a = sqrt((1.0f - cosN*cosN) / xx_yy);
			n[0] = n[0] * a;
			n[1] = n[1] * a;
			n[2] = cosN;
		}
		newNormals[pIndex] = n;
	}
	sampleNormals.swap(newNormals);
}
