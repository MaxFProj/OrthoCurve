#ifndef _EditDrawCurve_
#define _EditDrawCurve_

#include "sn3DSceneGL3D.h"
#include "editInteraction.h"
#include "Common\Point3.h"
#include "Common\Point2.h"
#include "CommonStructures.h"
#include"Graphics\sn3dmeshmodel.h"
using namespace Common;
class EditDrawCurve : public EditInteraction
{
public:

	bool   MouseMove(sn3DSceneGL3D *,unsigned int,int,int); //  鼠标移动消息,选择拖动
	bool   LeftClick(sn3DSceneGL3D*,unsigned int,int,int);  //  鼠标左键消息,进入选择拖动状态 
	bool   LeftUp(sn3DSceneGL3D *,int,int);                 //  鼠标左键弹起消息，选择拖动结束
	void   OnButtonDblClk(int X,int Y);

	void   Draw(sn3DSceneGL3D * scene);                     //   场景接口,辅助绘制以及交互时使用  
	
	void   ReconnectCurve();

	//void   SampleCurve(std::vector<Point3f> & oldCurve, std::vector<Point3f> & newCurve, std::vector<Point3f> & newTangent, int nodeNum);
	void  FindTriangleNomral(std::vector<Point3f> & samplePoints, std::vector<Point3f> & smapleTangent, std::vector<Point3f> & sampleNormals, sn3DMeshModel *model);
	EditDrawCurve(sn3DMeshModel * model, float normalZ);
	~EditDrawCurve();

	void RemoveLastPoint();
	void DrawPickPoints();

private:
	bool  PickSurfacePoint(Point2f & pos,Point3f & point);

	bool  HitPoints(Point2f & curPoint, double * mvMatrix,
		double * projMatrix, int * viewport, unsigned int * selectBuf);

public:
	
	sn3DMeshModel * m_model;
	int m_startIndex; // 局部曲线段起点索引
	int m_endIndex;
	int m_pickIndex; // 当前选中的曲线点索引
	std::vector<Point3f> m_localCurve;
	Point2f m_prevPoint;
	Point2f m_curPoint;
	bool b_pick;
	float m_normalZ;

	float m_interval;
};
#endif