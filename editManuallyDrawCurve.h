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

	bool   MouseMove(sn3DSceneGL3D*, unsigned int, int, int);  //  鼠标移动消息,选择拖动
	bool   LeftClick(sn3DSceneGL3D*, unsigned int, int, int);  //  鼠标左键消息,进入选择拖动状态 
	bool   LeftUp(sn3DSceneGL3D*, int, int);                   //  鼠标左键弹起消息，选择拖动结束
	void   OnButtonDblClk(int X, int Y);

	bool   RightDown(sn3DSceneGL3D*, int, int);              // 鼠标右击，删除选中的点
	bool   RightUp(sn3DSceneGL3D*, int, int);

	void   Draw(sn3DSceneGL3D* scene);                         //   场景接口,辅助绘制以及交互时使用  

	EditManuallyDrawCurve(sn3DMeshModel* model, float normalZ);
	~EditManuallyDrawCurve();

	void RemoveLastPoint();
	void DrawPickPoints();
	void DoInterpolation(); // 利用控制点做BSpline 插值
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
	std::vector<Point3f> m_projDir;        // 投影的角度

	Point2f m_curPoint;                    // 当前绘制的点
	bool isdbclicked;                      // 是否双击
	bool isleftdown;                       // 左侧鼠标是否按下
	bool isDelete;                         // 是否点击删除按钮
	bool hasPicked;                        // 鼠标左键/右键 按下后，已经找到最近的点
	bool isrightdown;                      
	int pickedIndex;                       // 选中点的索引

	// 切割角度的参数
	int* degreefb;
	int *cutDegreeOut;
	int *cutDegreeInsideFront;
	int *cutDegreeInsideBack;
	bool* showBur;
	double* cutOffset;
	float* m_interval; // 插值后的离散曲线点之间的间隔

	sn3DMeshModel* m_model;
	AABBTree* tree;

	int m_startIndex; // 局部曲线段起点索引
	int m_endIndex;
	int m_pickIndex; // 当前选中的曲线点索引
	Point2f m_prevPoint;
	bool b_pick;
	float m_normalZ;

	std::vector<int> interpolationNums;      // 每两个控制点之间的插值点的个数

	int lastPointIndex;
};
