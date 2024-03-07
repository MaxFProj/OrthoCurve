#include"EditMarkIcon.h"
#include <windows.h>
#include<GL/gl.h>
#include<GL/GLu.h>
#include<GL/GLut.h>
#include"Common/Quaternion.h"
#include"../OrthoCurve/MeshGrid.h"
#include <iostream>

EditMarkIcon::EditMarkIcon(sn3DMeshModel * model) :m_model(model)
{
   m_isDragging = false;
   m_selectBuf = new unsigned int[40000];
   m_pickIndex = -1;
   b_pick = false;
}
EditMarkIcon::~EditMarkIcon()
{
	if (m_selectBuf) {
		delete m_selectBuf;
	}
}
void  EditMarkIcon::Draw(sn3DSceneGL3D * scene)
{
	UpdateMatrix(); // 获取场景参数

	if (b_pick)
	{
		m_curPoint.y = m_viewport[3] - m_curPoint.y;
		HitPoints(m_curPoint, m_mvMatrix, m_projMatrix, m_viewport, m_selectBuf);
		if (m_pickIndex >= 0) {
			RemovePickPoint();
		}
		else{
			Point3f point3;
			if (PickSurfacePoint(this->m_curPoint, point3)){
				this->m_model->m_markPoints.push_back(point3);
			}
		}

		b_pick = false;
	}
	DrawPickPoints();
}

void  EditMarkIcon::OnButtonDblClk(int X,int Y)
{
	Point2f point2 = Point2f(X, Y);
	b_pick = true;
	m_curPoint = point2;

}

void  EditMarkIcon::DrawPickPoints()
{
	if (m_model == 0)
		return;
	if (m_model->m_markPoints.size() <= 0)
		return;

	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_LINE_BIT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDepthMask(GL_FALSE);
	glEnable(GL_COLOR_MATERIAL);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);


	glLineWidth(1);
	glColor4f(1, 1, 0, 1);
	for (int k = 0; k < m_model->m_markPoints.size(); k++)
	{
		Point3f pp = m_model->m_markPoints[k];
		glPushMatrix();
		glTranslatef(pp[0], pp[1], pp[2]);
		glutSolidSphere(0.3, 15, 15);
		glPopMatrix();
	}

	glDepthMask(GL_FALSE);
	glPopAttrib();
}

bool  EditMarkIcon::PickSurfacePoint(Point2f & pos,Point3f & point)
{
   GLdouble spacePoint[3];//空间坐标
   float depth;

   if(pos[0]>=0&&pos[0]<m_viewport[2]&&pos[1]>=0&&pos[1]<m_viewport[3])
   {
     glReadPixels(pos[0],pos[1],1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&depth);
   }
   else 
	   return false;

   if(depth > 0.99) 
	   return false;//如果深度缓存中没有数据返回
  		
   gluUnProject(pos[0],pos[1],depth,m_mvMatrix,m_projMatrix,m_viewport,
			         &spacePoint[0],&spacePoint[1],&spacePoint[2]);

   point = Point3f(spacePoint[0],spacePoint[1],spacePoint[2]);

   return true;
}

bool  EditMarkIcon::HitPoints(Point2f & curPoint, double * mvMatrix,
	double * projMatrix, int * viewport, unsigned int * selectBuf)
{
	if (m_model == 0)
		return false;

	long hits = 0;

	int cnt = (int)m_model->m_markPoints.size();

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
		Point3f & pp = m_model->m_markPoints[i];

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

		if (index != -1){
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

void EditMarkIcon::RemovePickPoint()
{
	if (m_pickIndex < 0)
		return;
	std::vector<Point3f>::iterator it = m_model->m_markPoints.begin();
	it = it + m_pickIndex;
	m_model->m_markPoints.erase(it);
	m_pickIndex = -1;
}

///////////////////////////////////////////////////////////
EditPickAlignPoints::EditPickAlignPoints(sn3DMeshModel* model) :m_model(model)
{
	m_isDragging	= false;
}
EditPickAlignPoints::~EditPickAlignPoints()
{
	if (m_selectBuf) {
		delete m_selectBuf;
	}

	if (demo_base) {
		delete demo_base;
		demo_base = nullptr;
	}
}
void  EditPickAlignPoints::Draw(sn3DSceneGL3D* scene)
{
	UpdateMatrix(); // 获取场景参数

	if (isleftdown) {
		//std::cout << "left down \n";
		Point3f point3;
		if (PickSurfacePoint(this->m_curPoint, point3))
		{
			//pick a points
			double min_dis = std::numeric_limits<double>::max();
			for (int i = 0; i < m_model->m_alignPoints.size(); ++i) {
				double current_dis = Distance(m_model->m_alignPoints.at(i), point3);

				if (current_dis < 2.0) {
					if (current_dis < min_dis) {
						min_dis = current_dis;
						picked_index = i;
					}
				}
			}

			if (picked_index != -1) {
				//std::cout << "picked points \n";
				m_model->m_alignPoints.at(picked_index) = point3;
			}
		}
	}

	if (isdbclicked) {
		Point3f point3;
		if (PickSurfacePoint(this->m_curPoint, point3)) {
			if (m_model->m_alignPoints.size() < 3) {
				m_model->m_alignPoints.push_back(point3);
			}
		}
		isdbclicked = false;
	}

	DrawPickPoints();
	DrawBaseModel();
}

void EditPickAlignPoints::loadBaseModel(int base_model_id)
{
	QString demo_base_name;
	if (base_model_id == 0) {
		demo_base_name = "/models/demo_base.stl";
	}
	else if (base_model_id == 1) {
		demo_base_name = "/models/info.stl";
	}
	else if (base_model_id == 2) {
		demo_base_name = "/models/info_V3.stl";
	}

	// read demo model
	QString base_path = QCoreApplication::applicationDirPath() + demo_base_name;
	std::string base_path_str = base_path.toLocal8Bit().constData();

	if (demo_base) {
		delete demo_base;
	}
	demo_base = new CMeshO();
	int load_mask_temp;
	int open_result = vcg::tri::io::ImporterSTL<CMeshO>::Open(*demo_base, base_path_str.c_str(), load_mask_temp);
}

void  EditPickAlignPoints::OnButtonDblClk(int X, int Y)
{
	m_curPoint = Point2f(X, Y);
	m_curPoint.y = m_viewport[3] - m_curPoint.y;
	isdbclicked = true;
}

bool EditPickAlignPoints::LeftClick(sn3DSceneGL3D* scene, unsigned int nFlags, int pX, int pY)
{
	m_curPoint = Point2f(pX, pY);
	//m_curPoint.y = m_viewport[3] - m_curPoint.y;
	isleftdown = true;
	picked_index = -1;
	return true;
}

bool EditPickAlignPoints::LeftUp(sn3DSceneGL3D*, int, int)
{
	isleftdown   = false;
	picked_index = -1;
	return true;
}

bool EditPickAlignPoints::MouseMove(sn3DSceneGL3D* scene, unsigned int nFlags, int pX, int pY)
{
	if (isleftdown) {
		m_curPoint = Point2f(pX, pY);
		//m_curPoint.y = m_viewport[3] - m_curPoint.y;
	}
	return true;
}

void  EditPickAlignPoints::DrawPickPoints()
{
	if (m_model == 0)
		return;
	if (m_model->m_alignPoints.size() <= 0)
		return;

	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_LINE_BIT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDepthMask(GL_FALSE);
	glEnable(GL_COLOR_MATERIAL);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glLineWidth(1);
	if (m_model->m_alignPoints.size() > 0) {
		glColor4f(1, 0, 0, 1);
		glPushMatrix();
		glTranslatef(m_model->m_alignPoints[0][0], m_model->m_alignPoints[0][1], m_model->m_alignPoints[0][2]);
		glutSolidSphere(1.0, 15, 15);
		glPopMatrix();
	}

	if (m_model->m_alignPoints.size() > 1) {
		glColor4f(0, 1, 0, 1);
		glPushMatrix();
		glTranslatef(m_model->m_alignPoints[1][0], m_model->m_alignPoints[1][1], m_model->m_alignPoints[1][2]);
		glutSolidSphere(1.0, 15, 15);
		glPopMatrix();
	}

	if (m_model->m_alignPoints.size() > 2) {
		glColor4f(0, 0, 1, 1);
		glPushMatrix();
		glTranslatef(m_model->m_alignPoints[2][0], m_model->m_alignPoints[2][1], m_model->m_alignPoints[2][2]);
		glutSolidSphere(1.0, 15, 15);
		glPopMatrix();
	}

	glDepthMask(GL_FALSE);
	glPopAttrib();
}
void EditPickAlignPoints::DrawBaseModel()
{
	//绘制二维码的示意位置
	if (m_model->m_alignPoints.size() == 3) {
		vcg::Point3f p1(m_model->m_alignPoints[0].X(), m_model->m_alignPoints[0].Y(), m_model->m_alignPoints[0].Z());
		vcg::Point3f p2(m_model->m_alignPoints[1].X(), m_model->m_alignPoints[1].Y(), m_model->m_alignPoints[1].Z());
		vcg::Point3f p3(m_model->m_alignPoints[2].X(), m_model->m_alignPoints[2].Y(), m_model->m_alignPoints[2].Z());

		vcg::Point3f F = (p1 - p2).Normalize() + (p1 - p3).Normalize();
		F = F.Normalize();
		vcg::Point3f N = (p3 - p1) ^ (p2 - p1);
		N = N.Normalize();
		vcg::Point3f R = (N ^ F).Normalize();
		vcg::Point3f center = (p1 + p2 + p3) * 0.333;

		vcg::Matrix44f matrix, transMatrix;
		matrix.SetIdentity();
		matrix[0][0] = F[0];
		matrix[1][0] = F[1];
		matrix[2][0] = F[2];

		matrix[0][1] = R[0];
		matrix[1][1] = R[1];
		matrix[2][1] = R[2];

		matrix[0][2] = N[0];
		matrix[1][2] = N[1];
		matrix[2][2] = N[2];

		transMatrix.SetTranslate(center);
		matrix = transMatrix * matrix;

		//draw base
		if (demo_base) {
			glColor4f(0.6, 0.6, 0.0, 1);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glBegin(GL_TRIANGLES);
			for (CMeshO::FaceIterator fit = demo_base->face.begin(); fit != demo_base->face.end(); ++fit) {
				vcg::Point3f p0, p1, p2;
				p0 = matrix * fit->V(0)->P();
				p1 = matrix * fit->V(1)->P();
				p2 = matrix * fit->V(2)->P();
				glVertex3f(p0.X(), p0.Y(), p0.Z());
				glVertex3f(p1.X(), p1.Y(), p1.Z());
				glVertex3f(p2.X(), p2.Y(), p2.Z());
			}
			glEnd();
		}

		// draw circle around base
		double cx, cy, cz;
		cx = cy = cz = 0;
		float r = 50;

		int num_segments = 50;
		float theta = 2 * 3.1415926 / float(num_segments);
		float x = r;//we start at angle = 0 
		float y = 0;

		glLineWidth(3.0f);
		glColor4f(0.0, 0.0, 0.0, 1.0);
		glBegin(GL_LINE_LOOP);
		for (int ii = 0; ii < num_segments; ii++)
		{
			vcg::Point3f p(cx + r * cosf(theta * ii), cy + r * sinf(theta * ii), 0);
			p = matrix * p;
			glVertex3f(p.X(), p.Y(), p.Z());//output vertex 
		}
		glEnd();
	}
}
bool  EditPickAlignPoints::PickSurfacePoint(Point2f& pos, Point3f& point)
{
	GLdouble spacePoint[3];//空间坐标
	float depth;

	if (pos[0] >= 0 && pos[0] < m_viewport[2] && pos[1] >= 0 && pos[1] < m_viewport[3])
	{
		glReadPixels(pos[0], pos[1], 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	}
	else
		return false;

	if (depth > 0.99)
		return false;//如果深度缓存中没有数据返回

	gluUnProject(pos[0], pos[1], depth, m_mvMatrix, m_projMatrix, m_viewport,
		&spacePoint[0], &spacePoint[1], &spacePoint[2]);

	point = Point3f(spacePoint[0], spacePoint[1], spacePoint[2]);

	return true;
}



