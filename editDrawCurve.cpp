#include"EditDrawCurve.h"
#include <windows.h>
#include<GL/gl.h>
#include<GL/GLu.h>
#include<GL/GLut.h>
#include"Common/Quaternion.h"
#include"../OrthoCurve/MeshGrid.h"
#include "ComputeCurve.h"
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

EditDrawCurve::EditDrawCurve(sn3DMeshModel * model, float normalZ) :m_model(model)
{
   m_isDragging = false;
   m_selectBuf = new unsigned int[40000];
   m_startIndex = -1;
   m_endIndex = -1;
   m_pickIndex = -1;
   b_pick = false;
   m_normalZ = normalZ;
   m_interval = 0.05;
}
EditDrawCurve::~EditDrawCurve()
{
	delete m_selectBuf;
}
void  EditDrawCurve::Draw(sn3DSceneGL3D * scene)
{
	//UpdateMatrix(); // 获取场景参数
	//DrawPickPoints();

	//HitPoints(m_curPoint, m_mvMatrix, m_projMatrix, m_viewport, m_selectBuf);
	//Point3f point3;
	//if (b_pick)
	//{
	//	if (PickSurfacePoint(this->m_curPoint, point3))
	//	{
	//		this->m_localCurve.push_back(point3);
	//	}
	//	b_pick = false;
	//}

}
bool  EditDrawCurve::MouseMove(sn3DSceneGL3D * scene,unsigned int nFlags,int pX,int pY)
{
 //   Point2f point = Point2f(pX, pY);
	//
	//m_curPoint = point;
	//Point3f point3;
	
	return true;
}
bool  EditDrawCurve::LeftClick(sn3DSceneGL3D * scene,unsigned int nFlags,int pX,int pY)
{
	//Point2f point2 = Point2f(pX, pY);
	//Point3f point3;
	//if (this->m_startIndex != -1 && this->m_endIndex==-1)
	//{
	//	b_pick = true;
	//}

	//m_curPoint = point2;
		 
	return true;
}
bool  EditDrawCurve::LeftUp(sn3DSceneGL3D * scene,int pX,int pY)
{
 //   Point2f point = Point2f(pX, pY);
	//if (m_isDragging)
	//{
	//	m_isDragging = false;
	//}
	return true;
}
void  EditDrawCurve::OnButtonDblClk(int X,int Y)
{
	//if (m_pickIndex != -1)
	//{
	//	if (m_startIndex == -1)
	//	{
	//		m_startIndex = m_pickIndex;
	//		this->m_localCurve.clear();
	//	}
	//	else if (m_endIndex==-1)
	//	{
	//		m_endIndex = m_pickIndex;
	//		ReconnectCurve();
	//		m_startIndex = -1;
	//		m_endIndex = -1;
	//		this->m_localCurve.clear();
	//	}
	//}
}

void  EditDrawCurve::DrawPickPoints()
{
	/*
	if (m_model == 0)
		return;

	if (m_startIndex == -1)
		return;

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT|GL_LINE_BIT );
    glEnable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
	glDepthMask(GL_FALSE);
	glEnable(GL_COLOR_MATERIAL);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glLineWidth(4.6);
	Point3f pp = m_model->m_curve[this->m_startIndex];

	//glColor4f(1, 1, 1, 1);
	
	glColor4f(1, 0, 0, 0);// 选中后，显示红色
	glPushMatrix();
	glTranslatef(pp[0], pp[1], pp[2]);
	glutSolidSphere(0.3, 25, 25);
	glPopMatrix();

	glDisable(GL_LIGHTING);
	int size = (int)this->m_localCurve.size();
	
	Point3f prevPoint = pp;
	Point3f curPoint;

	//局部调整线：绿色
	glColor4f(0, 1, 0, 1);
	for (int i = 0; i < size; i++)
	{
		curPoint = m_localCurve[i];
		
		glBegin(GL_LINES);
		glVertex3f(prevPoint[0], prevPoint[1], prevPoint[2]);
		glVertex3f(curPoint[0], curPoint[1], curPoint[2]);
		glEnd();
		
		prevPoint = curPoint;
	}

	//连接点：红色
	glColor4f(1, 0, 0, 1);
	for (int i = 0; i < size; i++)
	{
		curPoint = m_localCurve[i];
		glPushMatrix();
		glTranslatef(curPoint[0], curPoint[1], curPoint[2]);
		glutSolidSphere(0.2, 5, 5);
		glPopMatrix();
	}

	glDepthMask(GL_FALSE);
    glPopAttrib();*/
}
bool  EditDrawCurve::PickSurfacePoint(Point2f & pos,Point3f & point)
{
   GLdouble spacePoint[3];//空间坐标
   float depth;

   if(pos[0]>=0&&pos[0]<m_viewport[2]&&pos[1]>=0&&pos[1]<m_viewport[3])
   {
     glReadPixels(pos[0],pos[1],1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&depth);
   }
   else return false;

   if(depth > 0.99) return false;//如果深度缓存中没有数据返回
  		
   gluUnProject(pos[0],pos[1],depth,m_mvMatrix,m_projMatrix,m_viewport,
			         &spacePoint[0],&spacePoint[1],&spacePoint[2]);

   point = Point3f(spacePoint[0],spacePoint[1],spacePoint[2]);
   return true;
}
bool  EditDrawCurve::HitPoints(Point2f & curPoint, double * mvMatrix,
	double * projMatrix, int * viewport, unsigned int * selectBuf)
{
	if (m_model == 0)
		return false;

	long hits = 0;

	int cnt = (int)m_model->m_curve.size();

	if (cnt == 0) return false;

	m_pickIndex = -1;

	//初始化OpenGL选择模式
	glSelectBuffer(cnt * 4*2, selectBuf);
	glRenderMode(GL_SELECT);//切换选择模式
	glInitNames();//初始化名字栈
	glPushName(-1);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix(curPoint[0], curPoint[1], 10, 10, viewport);
	glMultMatrixd(projMatrix);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	//opengl选择

	glPushAttrib(GL_POINT_BIT);
	glPointSize(12);
	for (int i = 0; i<cnt; i++)
	{
		Point3f & pp = m_model->m_curve[i];

		glLoadName(i);
		glPushMatrix();
		glTranslatef(pp[0], pp[1], pp[2]);
		glutSolidSphere(0.15, 25, 25);
		glPopMatrix();
	}

	glPopAttrib();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	hits = glRenderMode(GL_RENDER);//切换至绘制模式

	if (hits > 0)
	{
		float minz = FLT_MAX;
		int index = -1;
		for (int j = 0; j < hits; j++)
		{
			if (selectBuf[j * 4 + 1] < minz)
			{
				minz = selectBuf[j * 4 + 1];
				index = selectBuf[j * 4 + 3];
			}
		}


		if (index != -1)
		{
			m_pickIndex = index;
		}
	}
	else
	{
		m_pickIndex = -1;
		return false;
	}
	return true;
}
void EditDrawCurve::RemoveLastPoint()
{
	//if (m_localCurve.size())
	//{
	//	this->m_localCurve.pop_back();
	//	b_pick = false;
	//}
	//else
	//{
	//	m_startIndex = -1;
	//	m_endIndex = -1;
	//	this->m_localCurve.clear();
	//}
}

void  EditDrawCurve::ReconnectCurve()
{
	if (m_model == 0)
		return;
	if (m_startIndex == -1)
		return;
	if (m_endIndex == -1)
		return;

	std::vector<Point3f> tempCurve;

	std::vector<Point3f> rawCurve;
	std::vector<Point3f> newCurve;

	int cnt = this->m_localCurve.size();
	rawCurve.push_back(m_model->m_curve[this->m_startIndex]);

	for (int i=0;i<cnt;i++)
	{
		rawCurve.push_back(m_localCurve[i]);
	}

	rawCurve.push_back(m_model->m_curve[this->m_endIndex]);

	std::vector<Point3f> smapleTangent;
	ComputeCurve::SampleCurve(rawCurve, newCurve, smapleTangent, 500);

	int  siz = m_model->m_curve.size();

	int curIndex = this->m_endIndex;

	do {
		tempCurve.push_back(m_model->m_curve[curIndex]);
		curIndex = (curIndex + 1) % siz;
	} while (curIndex !=m_startIndex);

	int num = newCurve.size();
	for (int i=0;i<num;i++)
	{
		tempCurve.push_back(newCurve[i]);
	}
	m_model->m_curve = tempCurve;

	float nodeNum = 800;
	float curLen = ComputeCurve::GetCurveLength(m_model->m_curve);
	nodeNum = curLen / m_interval;
	ComputeCurve::SampleCurve(m_model->m_curve, m_model->m_samplePoints, smapleTangent, nodeNum);
	FindTriangleNomral(m_model->m_samplePoints, smapleTangent, m_model->m_sampleNormals, m_model);

	
	//法相重新更新计算
	ComputeCurve::SetNormalComponetZ(m_model->m_sampleNormals, m_normalZ); // 20210321
	int nomralIter = 5;
	for (int i = 0; i<nomralIter; i++)
		ComputeCurve::SmoothNormal(m_model->m_sampleNormals);
}
void EditDrawCurve::FindTriangleNomral(std::vector<Point3f> & samplePoints, std::vector<Point3f> & smapleTangent, std::vector<Point3f> & sampleNormals, sn3DMeshModel *model)
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
/*
void  EditDrawCurve::SampleCurve(std::vector<Point3f> & oldCurve, std::vector<Point3f> & newCurve, std::vector<Point3f> & newTangent, int nodeNum)
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
*/
