#pragma once

#include "sn3DSceneGL3D.h"
#include "editInteraction.h"
#include "Common\Point3.h"
#include "Common\Point2.h"
#include "CommonStructures.h"
#include "sn3dmeshmodel.h"

//#include <vcg/space/index/octree.h>
#include <vcg/space/index/aabb_binary_tree/aabb_binary_tree.h>
#include <vcg/complex/algorithms/intersection.h>
#include <wrap/utils.h>
#include <vector>

#include "vcgmesh.h"

//typedef vcg::Octree<CFaceO, float> Octree;
typedef vcg::AABBBinaryTreeIndex<CFaceO, float, vcg::EmptyClass> AABBTree;

using namespace Common;
class EditManuallyDrawCurve : public EditInteraction
{
public:

	bool   MouseMove(sn3DSceneGL3D*, unsigned int, int, int);  //  ����ƶ���Ϣ,ѡ���϶�
	bool   LeftClick(sn3DSceneGL3D*, unsigned int, int, int);  //  ��������Ϣ,����ѡ���϶�״̬ 
	bool   LeftUp(sn3DSceneGL3D*, int, int);                   //  ������������Ϣ��ѡ���϶�����
	void   OnButtonDblClk(int X, int Y);

	bool   RightDown(sn3DSceneGL3D*, int, int);              // ����һ���ɾ��ѡ�еĵ�
	bool   RightUp(sn3DSceneGL3D*, int, int);

	void   Draw(sn3DSceneGL3D* scene);                         //   �����ӿ�,���������Լ�����ʱʹ��  

	EditManuallyDrawCurve(sn3DMeshModel* model, float normalZ);
	~EditManuallyDrawCurve();

	void RemoveLastPoint();
	void DrawPickPoints();
	void DoInterpolation(); // ���ÿ��Ƶ���BSpline ��ֵ
	void DrawCurve();
	void setTree(AABBTree* tree)		{ this->tree = tree; }
	void setModel(sn3DMeshModel* model);
	void DoProjection();
	int getCtrlIndex(int pIndex);
	int estimatePointNum();
	void computeDirection();
	Point3f HermiteInterpolation(Point3f y0, Point3f y1, Point3f y2, Point3f y3, double mu, double tension, double bias);
	std::vector<Point3f> interpolationCurve(std::vector<Point3f>& data, std::vector<int>& interpolationNums);
	void adjustDegree();

private:
	bool  PickSurfacePoint(Point2f& pos, Point3f& point);
	bool  HitPoints(Point2f& curPoint, double* mvMatrix, double* projMatrix, int* viewport, unsigned int* selectBuf);

public:
	std::vector<Point3f> m_projDir;        // ͶӰ�ĽǶ�

	Point2f m_curPoint;                    // ��ǰ���Ƶĵ�
	bool isdbclicked;                      // �Ƿ�˫��
	bool isleftdown;                       // �������Ƿ���
	bool isDelete;                         // �Ƿ���ɾ����ť
	bool hasPicked;                        // ������/�Ҽ� ���º��Ѿ��ҵ�����ĵ�
	bool isrightdown;                      
	int pickedIndex;                       // ѡ�е������

	// �и�ǶȵĲ���
	int* degreefb;
	int *cutDegreeOut;
	int *cutDegreeInsideFront;
	int *cutDegreeInsideBack;
	bool* showBur;
	double* cutOffset;
	float* m_interval; // ��ֵ�����ɢ���ߵ�֮��ļ��

	sn3DMeshModel* m_model;
	AABBTree* tree;

	int m_startIndex; // �ֲ����߶��������
	int m_endIndex;
	int m_pickIndex; // ��ǰѡ�е����ߵ�����
	Point2f m_prevPoint;
	bool b_pick;
	float m_normalZ;

	std::vector<int> interpolationNums;      // ÿ�������Ƶ�֮��Ĳ�ֵ��ĸ���

	int lastPointIndex;
};
