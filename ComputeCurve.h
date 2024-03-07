#ifndef COMPUTE_CURVE_H
#define COMPUTE_CURVE_H

#include"Graphics\sn3dmeshmodel.h"

class ComputeCurve
{
public:
	ComputeCurve() {};
	~ComputeCurve() {};

	// thresholdZ 过滤模型，最高点小于阈值是过滤掉
	// normalZ    采样点法相与Z轴的夹角
	// step       龈缘线offset距离参数
	// interval    采样点个数 默认0.5
	// smoothness 控制曲线的平滑程度，实际上是平滑迭代次数
	void DoComputeCurve(sn3DMeshModel * model,float thresholdZ, float normalZ, int step, float interval, int smoothness=3);
public:

	void FindRegion(sn3DMeshData * mesh, std::vector<sn3DMeshModel *> & modelSet);
	void FindGum(std::vector<sn3DMeshModel *> & modelSet,float thresholdZ);
	void ComputeContact(std::vector<sn3DMeshModel *> & modelSet);

	void SelectedRegionProcessing(sn3DMeshData * mesh, int iter=3); // 干涉区域形态联通处理

	void RemoveIsolatedRegion(sn3DMeshData * mesh);
	void SmoothCurve(int it_count, std::vector<Point3f> & nodeSet, sn3DMeshModel * model);
	static void SampleCurve(std::vector<Point3f> & oldCurve, std::vector<Point3f> & newCurve, std::vector<Point3f> & newTangent, int nodeNum);

	void FindTriangleNomral(std::vector<Point3f> & samplePoints, std::vector<Point3f> & smapleTangent, std::vector<Point3f> & sampleNormals, sn3DMeshModel *model);
	void CrossNormal(std::vector<Point3f> & smapleTangent, std::vector<Point3f> & sampleNormals, std::vector<Point3f> &smapleBinormal);
	void CurveOffset(float dis, std::vector<Point3f> & samplePoints, std::vector<Point3f> &sampleNormals, sn3DMeshModel * model);

	static void SmoothNormal(std::vector<Point3f> &sampleNormals);
	static void SetNormalComponetZ(std::vector<Point3f> &sampleNormals, float degreeZ);
	static void SetCutNormal(std::vector<Point3f> & tangent,std::vector<Point3f> &sampleNormals, float degreeZ);

	static float GetCurveLength(std::vector<Point3f> & curve);

	std::vector<sn3DMeshModel *>  m_modelSet;

	

};

#endif // COMPUTE_CURVE_H