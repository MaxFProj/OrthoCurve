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

	bool   MouseMove(sn3DSceneGL3D *,unsigned int,int,int); //  ����ƶ���Ϣ,ѡ���϶�
	bool   LeftClick(sn3DSceneGL3D*,unsigned int,int,int);  //  ��������Ϣ,����ѡ���϶�״̬ 
	bool   LeftUp(sn3DSceneGL3D *,int,int);                 //  ������������Ϣ��ѡ���϶�����
	void   OnButtonDblClk(int X,int Y);

	void   Draw(sn3DSceneGL3D * scene);                     //   �����ӿ�,���������Լ�����ʱʹ��  
	
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
	int m_startIndex; // �ֲ����߶��������
	int m_endIndex;
	int m_pickIndex; // ��ǰѡ�е����ߵ�����
	std::vector<Point3f> m_localCurve;
	Point2f m_prevPoint;
	Point2f m_curPoint;
	bool b_pick;
	float m_normalZ;

	float m_interval;
};
#endif