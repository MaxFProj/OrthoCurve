#ifndef _EditMarkIcon_
#define _EditMarkIcon_

#include "sn3DSceneGL3D.h"
#include "editInteraction.h"
#include "Common\Point3.h"
#include "Common\Point2.h"
#include "CommonStructures.h"
#include "Graphics\sn3dmeshmodel.h"
#include "vcgmesh.h"
#include "QString"
#include "QCoreApplication"

#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export.h>

using namespace Common;
class EditMarkIcon : public EditInteraction
{
public:
	void   OnButtonDblClk(int X,int Y);
	void   Draw(sn3DSceneGL3D * scene);                     //   场景接口,辅助绘制以及交互时使用  

	EditMarkIcon(sn3DMeshModel * model);
	~EditMarkIcon();

private:
	void DrawPickPoints();
	bool PickSurfacePoint(Point2f & pos,Point3f & point);
	void RemovePickPoint();
	bool  HitPoints(Point2f& curPoint, double* mvMatrix,
		double* projMatrix, int* viewport, unsigned int* selectBuf);

public:
	sn3DMeshModel * m_model;
	int m_pickIndex;				// 当前选中的曲线点索引
	Point2f m_curPoint;
	bool b_pick;
};

//=================================================================
class EditPickAlignPoints : public EditInteraction
{
public:

	void OnButtonDblClk(int X, int Y);
	bool LeftClick(sn3DSceneGL3D*, unsigned int, int, int);
	bool LeftUp(sn3DSceneGL3D*, int, int);
	bool MouseMove(sn3DSceneGL3D* scene, unsigned int nFlags, int pX, int pY);
	void Draw(sn3DSceneGL3D* scene);   //场景接口,辅助绘制以及交互时使用  
	void loadBaseModel(int base_model_id); // 0 with qr; 1, without qr; 2, V3

	EditPickAlignPoints(sn3DMeshModel* model);
	~EditPickAlignPoints();

private:
	void DrawPickPoints();
	void DrawBaseModel();
	bool PickSurfacePoint(Point2f& pos, Point3f& point);
public:
	sn3DMeshModel* m_model;
	CMeshO* demo_base = nullptr;
	Point2f m_curPoint;
	bool isleftdown  = false;
	bool isdbclicked = false;
	int picked_index = -1;
};
#endif