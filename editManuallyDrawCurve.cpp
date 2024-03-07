#include"editManuallyDrawCurve.h"
#include<GL/gl.h>
#include<GL/GLu.h>
#include<GL/GLut.h>
#include"Common/Quaternion.h"
#include"../OrthoCurve/MeshGrid.h"
#include "ComputeCurve.h"

#include <iostream>
#include <limits>
#include "SGSmooth.h"
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

EditManuallyDrawCurve::EditManuallyDrawCurve(sn3DMeshModel* model, float normalZ) :m_model(model)
{
	m_selectBuf		= new unsigned int[40000];
	m_startIndex	= -1;
	m_endIndex		= -1;
	m_pickIndex		= -1;
	b_pick			= false;
	m_normalZ		= normalZ;

	isdbclicked = false;
	isleftdown	= false;
	hasPicked	= false;
	isDelete	= false;

	showBur		= nullptr;
	cutOffset   = nullptr;

	pickedIndex = -1;
	lastPointIndex = 0;
}
EditManuallyDrawCurve::~EditManuallyDrawCurve()
{
	delete m_selectBuf;
}

void  EditManuallyDrawCurve::Draw(sn3DSceneGL3D* scene)
{
	UpdateMatrix(); // 获取场景参数

	if (isleftdown && isrightdown) { // should be impossible...
		return;
	}

	if (isleftdown || isrightdown) {
		Point3f point3;
		if (PickSurfacePoint(m_curPoint, point3)) {
			// 找到与当前点击位置最近的点
			if (!hasPicked) {
				double minDis = 1 << 30;
				int minIndex = -1;
				for (int i = 0; i < m_model->m_ctrlPoints.size(); ++i) {
					double dis = (m_model->m_ctrlPoints[i] - point3).Norm();
					if (dis < minDis) {
						minDis = dis;
						minIndex = i;
					}
				}
				if (minIndex >= 0 && minDis < 0.5) {
					pickedIndex = minIndex;
				}
			}

			if (pickedIndex >= 0) {
				if (isleftdown) {
					m_model->m_ctrlPoints[pickedIndex] = point3;
				}
				if (isrightdown) {
					m_model->m_ctrlPoints.erase(m_model->m_ctrlPoints.begin() + pickedIndex);
					pickedIndex = -1;
				}

				DoInterpolation();
			}
			else if (isleftdown && !hasPicked) {
				int nearestIndex = -1;
				double minDis = 1 << 30;
				for (int j = 0; j < m_model->m_curve.size(); ++j) {
					double dis = (m_model->m_curve[j] - point3).Norm();
					if (dis < minDis && dis < .5) {
						minDis = dis;
						nearestIndex = j;
					}
				}

				if (nearestIndex != -1) {
					int ctrlIndex = getCtrlIndex(nearestIndex);
					if (ctrlIndex != -1) {
						bool isCloseToCtrlPoints = false;
						for (int j = 0; j < m_model->m_ctrlPoints.size(); ++j) {
							double dis = Distance(m_model->m_ctrlPoints[j], point3);
							if (dis < 0.5) {
								isCloseToCtrlPoints = true;
								break;
							}
						}
						if (!isCloseToCtrlPoints) {
							std::vector<Point3f>::iterator it =  m_model->m_ctrlPoints.insert(m_model->m_ctrlPoints.begin() + ctrlIndex + 1, point3);
							lastPointIndex = it - m_model->m_ctrlPoints.begin();
						}
					}
				}
				else {
					m_model->m_ctrlPoints.push_back(point3);
					lastPointIndex = m_model->m_ctrlPoints.size() - 1;
				}

				DoInterpolation();
			}
		}
		hasPicked = true; //只在鼠标左键点下的第一次进行最近点查找工作
	}

	// 如果是双击则重新插值重绘曲线
	if (isdbclicked) {
		//Point3f point3;
		//if (PickSurfacePoint(m_curPoint, point3)) {
		//	int nearestIndex = -1;
		//	double minDis = 1 << 30;
		//	for (int j = 0; j < m_model->m_curve.size(); ++j) {
		//		double dis = (m_model->m_curve[j] - point3).Norm();
		//		if (dis < minDis && dis < 0.5) {
		//			minDis = dis;
		//			nearestIndex = j;
		//		}
		//	}

		//	if (nearestIndex != -1) {
		//		int ctrlIndex = getCtrlIndex(nearestIndex);
		//		if (ctrlIndex != -1) {
		//			bool isCloseToCtrlPoints =  false;
		//			for (int j = 0; j < m_model->m_ctrlPoints.size(); ++j) {
		//				double dis = Distance(m_model->m_ctrlPoints[j], point3);
		//				if (dis < 0.5) {
		//					isCloseToCtrlPoints = true;
		//					break;
		//				}
		//			}
		//			if (!isCloseToCtrlPoints) {
		//				m_model->m_ctrlPoints.insert(m_model->m_ctrlPoints.begin() + ctrlIndex + 1, point3);
		//			}
		//		}
		//	}
		//	else {
		//		m_model->m_ctrlPoints.push_back(point3);
		//	}

		//	DoInterpolation();
		//}
		//isdbclicked = false;
	}

	if (!isleftdown && !isrightdown && !m_model->m_ctrlPoints.empty()) {
		DoInterpolation();
	}

	DrawPickPoints();
	DrawCurve();
}
bool  EditManuallyDrawCurve::MouseMove(sn3DSceneGL3D* scene, unsigned int nFlags, int pX, int pY)
{
	// do nothing...
	if (isleftdown) {
		m_curPoint = Point2f(pX, pY);
	}
	return true;
}
bool  EditManuallyDrawCurve::LeftClick(sn3DSceneGL3D* scene, unsigned int nFlags, int pX, int pY)
{
	Point2f point2 = Point2f(pX, pY);
	m_curPoint = point2;

	isleftdown = true;

	hasPicked = false;
	pickedIndex = -1;

	return true;
}
bool  EditManuallyDrawCurve::LeftUp(sn3DSceneGL3D* scene, int pX, int pY)
{
	isleftdown = false;
	return true;
}
void  EditManuallyDrawCurve::OnButtonDblClk(int X, int Y){
	isdbclicked = true;
}

bool EditManuallyDrawCurve::RightDown(sn3DSceneGL3D* scene, int pX, int pY)
{
	Point2f point2 = Point2f(pX, pY);
	m_curPoint = point2;

	isrightdown = true;

	hasPicked = false;
	pickedIndex = -1;

	return true;
}

bool EditManuallyDrawCurve::RightUp(sn3DSceneGL3D*, int, int)
{
	isrightdown = false;
	return true;
}

void  EditManuallyDrawCurve::DrawPickPoints()
{
	if (m_model == 0)
		return;

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);

	//绘制透明物体（为了使控制点更鲜明，先绘制透明的物体）
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	glColor4f(1, 1, 0, 0.5); //绘制透明刀补区域
	for (int i = 0; i < m_model->m_curve.size(); ++i) {
		glPushMatrix();
		glTranslatef(m_model->m_curve[i][0], m_model->m_curve[i][1], m_model->m_curve[i][2]);
		glutSolidSphere(*cutOffset, 25, 25);
		glPopMatrix();
	}
	
	//绘制切割头示意区域
	if (showBur && *showBur) {
	/*	glColor4f(0, 1, 1, 0.3);
		for (std::set<int>::iterator s = m_model->m_buccalPointsIndex.begin(); s != m_model->m_buccalPointsIndex.end(); ++s) {
			int i = *s;
			Point3f p   = m_model->m_curve[i];
			Point3f dir = m_model->m_cutDir[i];
			Point3f z(0.0, 0.0, 1.0);
			dir = dir.Normalize();
			Point3f rotateDir = (z ^ dir).Normalize();
			double rotateValue = acos(dir * z) / 3.1415926 * 180;

			glPushMatrix();
			glTranslatef(p.X(), p.Y(), p.Z());
			glRotatef(rotateValue, rotateDir.X(), rotateDir.Y(), rotateDir.Z());
			GLUquadricObj* objCylinder = gluNewQuadric();
			gluCylinder(objCylinder, *cutOffset, *cutOffset, 6.0, 32, 5);
			glPopMatrix();
		}

		glColor4f(1, 0, 1, 0.3);
		for (std::set<int>::iterator s = m_model->m_lingualPointsFrontIndex.begin(); s != m_model->m_lingualPointsFrontIndex.end(); ++s) {
			int i = *s;
			Point3f p = m_model->m_curve[i];
			Point3f dir = m_model->m_cutDir[i];
			Point3f z(0.0, 0.0, 1.0);
			dir = dir.Normalize();
			Point3f rotateDir = (z ^ dir).Normalize();
			double rotateValue = acos(dir * z) / 3.1415926 * 180;

			glPushMatrix();
			glTranslatef(p.X(), p.Y(), p.Z());
			glRotatef(rotateValue, rotateDir.X(), rotateDir.Y(), rotateDir.Z());
			GLUquadricObj* objCylinder = gluNewQuadric();
			gluCylinder(objCylinder, *cutOffset, *cutOffset, 6.0, 32, 5);
			glPopMatrix();
		}

		glColor4f(1, 1, 0, 0.3);
		for (std::set<int>::iterator s = m_model->m_lingualPointsBackIndex.begin(); s != m_model->m_lingualPointsBackIndex.end(); ++s) {
			int i = *s;
			Point3f p = m_model->m_curve[i];
			Point3f dir = m_model->m_cutDir[i];
			Point3f z(0.0, 0.0, 1.0);
			dir = dir.Normalize();
			Point3f rotateDir = (z ^ dir).Normalize();
			double rotateValue = acos(dir * z) / 3.1415926 * 180;

			glPushMatrix();
			glTranslatef(p.X(), p.Y(), p.Z());
			glRotatef(rotateValue, rotateDir.X(), rotateDir.Y(), rotateDir.Z());
			GLUquadricObj* objCylinder = gluNewQuadric();
			gluCylinder(objCylinder, *cutOffset, *cutOffset, 6.0, 32, 5);
			glPopMatrix();
		}

		glColor4f(0, 0, 1, 0.3);
		for (std::set<int>::iterator s = m_model->m_lingualPointsBackIndex.begin(); s != m_model->m_lingualPointsBackIndex.end(); ++s) {
			int i = *s;
			Point3f p = m_model->m_curve[i];
			Point3f dir = m_model->m_cutDir[i];
			Point3f z(0.0, 0.0, 1.0);
			dir = dir.Normalize();
			Point3f rotateDir = (z ^ dir).Normalize();
			double rotateValue = acos(dir * z) / 3.1415926 * 180;

			glPushMatrix();
			glTranslatef(p.X(), p.Y(), p.Z());
			glRotatef(rotateValue, rotateDir.X(), rotateDir.Y(), rotateDir.Z());
			GLUquadricObj* objCylinder = gluNewQuadric();
			gluCylinder(objCylinder, *cutOffset, *cutOffset, 6.0, 32, 5);
			glPopMatrix();
		}*/

		if (m_model->m_lingualPointsBackIndex.empty()) {
			glColor4f(0.8, 0.8, 0.8, 0.3); //绘制透明模拟刀头
			for (int i = 0; i < m_model->m_cutDir.size(); ++i) {
				Point3f p = m_model->m_curve[i];
				Point3f dir = m_model->m_cutDir[i];
				Point3f z(0.0, 0.0, 1.0);
				dir = dir.Normalize();
				Point3f rotateDir = (z ^ dir).Normalize();
				double rotateValue = acos(dir * z) / 3.1415926 * 180;

				glPushMatrix();
				glTranslatef(p.X(), p.Y(), p.Z());
				glRotatef(rotateValue, rotateDir.X(), rotateDir.Y(), rotateDir.Z());
				GLUquadricObj* objCylinder = gluNewQuadric();
				gluCylinder(objCylinder, *cutOffset, *cutOffset, 6.0, 32, 5);
				glPopMatrix();
			}
		}
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	// 绘制切割曲线的控制点
	glColor4f(1, 0, 0, 1.0);
	for (int i = 0; i < m_model->m_ctrlPoints.size(); ++i) {
		glPushMatrix();
		glTranslatef(m_model->m_ctrlPoints[i][0], m_model->m_ctrlPoints[i][1], m_model->m_ctrlPoints[i][2]);
		glutSolidSphere(0.3, 25, 25);
		glPopMatrix();
	}

	//绘制曲线
	int siz = m_model->m_curve.size();
	glLineWidth(4.6);
	glColor4f(1, 1, 1, 1);
	glBegin(GL_LINE_LOOP);
	//glBegin(GL_LINE_STRIP);
	for (int k = 0; k < siz; k++)
	{
		Point3f p = m_model->m_curve[k];
		glVertex3f(p[0], p[1], p[2]);
	}
	glEnd();

	glPopAttrib();
}
void EditManuallyDrawCurve::DoInterpolation()
{
	m_model->m_curve.clear();
	m_model->m_cutDir.clear();

	if (m_model->m_ctrlPoints.size() <= 1) {
		return;
	}
	else if (m_model->m_ctrlPoints.size() <= 3) {
		// 做线性插值
		for (int i = 0; i < m_model->m_ctrlPoints.size(); ++i) {
			m_model->m_curve.push_back(m_model->m_ctrlPoints[i]);
		}
		return;
	}

	int curvePointsNumber = estimatePointNum();

	std::vector<Point3f> data;
	data.push_back(m_model->m_ctrlPoints[0] * 1.5 - m_model->m_ctrlPoints[1] * 0.5);
	for (int i = 0; i < m_model->m_ctrlPoints.size(); ++i) {
		data.push_back(m_model->m_ctrlPoints[i]);
	}
	data.push_back(m_model->m_ctrlPoints[m_model->m_ctrlPoints.size() - 1] * 1.5 - m_model->m_ctrlPoints[m_model->m_ctrlPoints.size() - 2] * 0.5);
	m_model->m_curve = interpolationCurve(data, interpolationNums);

	DoProjection();
}
void EditManuallyDrawCurve::DrawCurve()
{
	if (m_model == 0)
		return;

	// draw curve according different position
	//glPushAttrib(GL_ENABLE_BIT);
	//glDisable(GL_LIGHTING);

	// 绘制切割曲线的控制点
	//glColor4f(0, 1, 1, 0);
	//for (std::set<int>::iterator s = m_model->m_buccalPointsIndex.begin(); s != m_model->m_buccalPointsIndex.end(); ++s) {
	//	Point3f p = m_model->m_curve[*s];
	//	glPushMatrix();
	//	glTranslatef(p[0], p[1], p[2]);
	//	glutSolidSphere(0.2, 25, 25);
	//	glPopMatrix();
	//}

	//glColor4f(1, 0, 1, 0);
	//for (std::set<int>::iterator s = m_model->m_lingualPointsFrontIndex.begin(); s != m_model->m_lingualPointsFrontIndex.end(); ++s) {
	//	Point3f p = m_model->m_curve[*s];
	//	glPushMatrix();
	//	glTranslatef(p[0], p[1], p[2]);
	//	glutSolidSphere(0.2, 25, 25);
	//	glPopMatrix();
	//}

	//glColor4f(0, 0, 1, 0);
	//for (std::set<int>::iterator s = m_model->m_lingualPointsBackIndex.begin(); s != m_model->m_lingualPointsBackIndex.end(); ++s) {
	//	Point3f p = m_model->m_curve[*s];
	//	glPushMatrix();
	//	glTranslatef(p[0], p[1], p[2]);
	//	glutSolidSphere(0.2, 25, 25);
	//	glPopMatrix();
	//}

	//glColor4f(0, 0, 0, 0);
	//for (std::set<int>::iterator s = m_model->m_lingualPointsBackIndex.begin(); s != m_model->m_lingualPointsBackIndex.end(); ++s) {
	//	Point3f p = m_model->m_curve[*s];
	//	glPushMatrix();
	//	glTranslatef(p[0], p[1], p[2]);
	//	glutSolidSphere(0.2, 25, 25);
	//	glPopMatrix();
	//}

	//m_intersectedPoints
	glColor4f(128.0/255.0, 0, 128.0 / 255.0, 0);
	for (std::vector<Point3f>::iterator s = m_model->m_intersectedPoints.begin(); s != m_model->m_intersectedPoints.end(); ++s) {
		Point3f p = *s;
		glPushMatrix();
		glTranslatef(p[0], p[1], p[2]);
		glutSolidSphere(0.5, 25, 25);
		glPopMatrix();
	}

	glPopAttrib();
}
void EditManuallyDrawCurve::setModel(sn3DMeshModel* model) // 重新关联模型，并将和之前模型相关数据清空
{
	m_model = model;
	m_projDir.clear();
}
void EditManuallyDrawCurve::DoProjection()
{
	// 计算切割方向
	computeDirection();

	for (int i = 0; i < m_model->m_curve.size(); ++i) {
		float maxDist = std::numeric_limits<float>::max();
		vcg::Point3f projDir(m_projDir[i][0], m_projDir[i][1], m_projDir[i][2]);
		vcg::Point3f pointToProj(m_model->m_curve[i][0], m_model->m_curve[i][1], m_model->m_curve[i][2]);
		vcg::EmptyClass objectMarker;
		vcg::RayTriangleIntersectionFunctor<false> rayIntersector;

		pointToProj = pointToProj + projDir * 10;
		vcg::Ray3<float, false> rayInverse(pointToProj, -projDir);
		float rayTInverse = std::numeric_limits<float>::max();
		CFaceO* isectFaceInverse = NULL;
		isectFaceInverse = tree->DoRay(rayIntersector, objectMarker, rayInverse, maxDist, rayTInverse);

		if (isectFaceInverse) {
			vcg::Point3f p = pointToProj - projDir * rayTInverse;
			int last_index = i > 0 ? i - 1 : i + 1;
			if (Distance(m_model->m_curve[last_index], Point3f(p.X(), p.Y(), p.Z())) < 2.0) {
				m_model->m_curve[i][0] = p[0];
				m_model->m_curve[i][1] = p[1];
				m_model->m_curve[i][2] = p[2];
			}
		}

		//计算当前点是否有干涉
		vcg::Point3f detectIntersectedDir(m_model->m_cutDir[i][0], m_model->m_cutDir[i][1], m_model->m_cutDir[i][2]);
		pointToProj = pointToProj + detectIntersectedDir * 0.5;
		vcg::Ray3<float, false> ray(pointToProj, detectIntersectedDir);
		float rayT = std::numeric_limits<float>::max();
		CFaceO* isectFace = NULL;
		isectFace = tree->DoRay(rayIntersector, objectMarker, ray, maxDist, rayT);
		if (isectFace) {
			std::cout << "have intersection \n";
			vcg::Point3f p = pointToProj + detectIntersectedDir * rayT;
			m_model->m_intersectedPoints.push_back(Point3f(p.X(), p.Y(), p.Z()));
		}
	}
}
int EditManuallyDrawCurve::getCtrlIndex(int pIndex)
{
	int totalNum = 0;
	int index = -1;
	if (pIndex == 0) {
		return -1;
	}
	for (index = 0; index < interpolationNums.size(); ++index) {
		totalNum += interpolationNums[index] + 1;
		if (totalNum > pIndex) {
			return index;
		}
		else if (totalNum == pIndex) {
			return -1;
		}
	}
	return -1;
}
int EditManuallyDrawCurve::estimatePointNum()
{
	int totalNum = 1;
	interpolationNums.clear();
	for (int i = 1; i < m_model->m_ctrlPoints.size(); ++i) {
		double dis = (m_model->m_ctrlPoints[i] - m_model->m_ctrlPoints[i - 1]).Norm();
		int num = floor(dis / *m_interval);
		interpolationNums.push_back(num);
		totalNum += num;
	}
	return totalNum;
}
void EditManuallyDrawCurve::computeDirection()
{
	m_model->m_cutDir.clear();
	m_projDir.clear();
	m_model->m_intersectedPoints.clear();
	Point3f z(0.0, 0.0, 1.0);

	// smooth the cutline 
	std::vector<double> X, Y, Z;
	std::vector<double> Xout, Yout, Zout;
	std::vector<Point3f> pointsAfterSmooth;

	if (m_model->m_curve.size() < 42) { //直接拷贝
		for (int i = 0; i < m_model->m_curve.size(); ++i) {
			pointsAfterSmooth.push_back(m_model->m_curve[i]);
		}
	}
	else {
		for (int i = 0; i < m_model->m_curve.size(); ++i) {
			X.push_back(m_model->m_curve[i].X());
			Y.push_back(m_model->m_curve[i].Y());
			Z.push_back(m_model->m_curve[i].Z());
		}
		Xout = sg_smooth(X, 20, 3);
		Yout = sg_smooth(Y, 20, 3);

		pointsAfterSmooth.resize(m_model->m_curve.size());
		for (int i = 0; i < X.size(); ++i)
		{
			pointsAfterSmooth[i].X() = Xout[i];
			pointsAfterSmooth[i].Y() = Yout[i];
			pointsAfterSmooth[i].Z() = 0.0;
		}
	}

	// compute direction
	for (int i = 0; i < pointsAfterSmooth.size() - 1; ++i) {
		Point3f pNext = (pointsAfterSmooth[i + 1] - pointsAfterSmooth[i]).Normalize();
		Point3f outDir = (z ^ pNext).Normalize();
		
		// 方向
		outDir.Z() = tan(30.0 / 180.0 * 3.1415926);
		outDir = outDir.Normalize();
		m_model->m_cutDir.push_back(outDir);

		// 投影方向
		pNext  = (m_model->m_curve[i + 1] - m_model->m_curve[i]).Normalize();
		outDir = z ^ pNext;
		outDir.Z() = 0.0;
		outDir = outDir.Normalize();
		outDir.Z() = tan(45.0 / 180.0 * 3.1415926);
		outDir = outDir.Normalize();
		m_projDir.push_back(outDir);
	}
	m_model->m_cutDir.push_back(m_model->m_cutDir.back());
	m_projDir.push_back(m_projDir.back());

	// adjust cut degree
	double cutDegree = 90 - *cutDegreeOut;
	//std::cout << __LINE__ << " cutDegree " << cutDegree << std::endl;
	for (std::set<int>::iterator it = m_model->m_buccalPointsIndex.begin(); it != m_model->m_buccalPointsIndex.end(); ++it) {

		m_model->m_cutDir[*it].Z() = tan(cutDegree / 180.0 * 3.1415926);
		m_model->m_cutDir[*it] = m_model->m_cutDir[*it].Normalize();
	}

	cutDegree = 90 - *cutDegreeInsideBack;
	//std::cout << __LINE__ << " cutDegree " << cutDegree << std::endl;
	for (std::set<int>::iterator it = m_model->m_lingualPointsBackIndex.begin(); it != m_model->m_lingualPointsBackIndex.end(); ++it) {

		m_model->m_cutDir[*it].Z() = tan(cutDegree / 180.0 * 3.1415926);
		m_model->m_cutDir[*it] = m_model->m_cutDir[*it].Normalize();
	}

	cutDegree = 90 - *cutDegreeInsideFront;
	//std::cout << __LINE__ << " cutDegree " << cutDegree << std::endl;
	for (std::set<int>::iterator it = m_model->m_lingualPointsFrontIndex.begin(); it != m_model->m_lingualPointsFrontIndex.end(); ++it) {

		m_model->m_cutDir[*it].Z() = tan(cutDegree / 180.0 * 3.1415926);
		m_model->m_cutDir[*it] = m_model->m_cutDir[*it].Normalize();
	}

	// smooth the cut direction
	int iteration = 2;
	std::vector<Point3f> temp;
	for (int j = 0; j < iteration; ++j) {
		temp = m_model->m_cutDir;
		for (int i = 0; i < m_model->m_cutDir.size(); ++i) {
			// smooth i-2、i-1、i、i+1、 i+2
			Point3f res(0.0, 0.0, 0.0);
			for (int k = -1; k <= 1; ++k) {
				int index = i + k;
				if (index < 0) {
					index += m_model->m_cutDir.size();
				}
				else if (index >= m_model->m_cutDir.size()) {
					index = index % m_model->m_cutDir.size();
				}
				res += m_model->m_cutDir[index];
			}
			temp[i] = res.Normalize();
		}
		m_model->m_cutDir = temp;
	}
}

Point3f EditManuallyDrawCurve::HermiteInterpolation(Point3f y0, Point3f y1, Point3f y2, Point3f y3, double mu, double tension, double bias)
{
	Point3f m0, m1;
	double a0, a1, a2, a3, mu2, mu3;

	mu2 = mu * mu;
	mu3 = mu2 * mu;
	m0 = (y1 - y0) * (1 + bias) * (1 - tension) / 2;
	m0 += (y2 - y1) * (1 - bias) * (1 - tension) / 2;
	m1 = (y2 - y1) * (1 + bias) * (1 - tension) / 2;
	m1 += (y3 - y2) * (1 - bias) * (1 - tension) / 2;
	a0 = 2 * mu3 - 3 * mu2 + 1;
	a1 = mu3 - 2 * mu2 + mu;
	a2 = mu3 - mu2;
	a3 = -2 * mu3 + 3 * mu2;

	return(y1 * a0 + m0 * a1 + m1 * a2 + y2 * a3);
}

std::vector<Point3f> EditManuallyDrawCurve::interpolationCurve(std::vector<Point3f>& data, std::vector<int>& interpolationNums)
{
	std::vector<Point3f> res;
	double bias = 0;
	if (data.size() <= 3 || data.size() != interpolationNums.size() + 3) {
		return res;
	}
	for (int i = 0; i < data.size() - 3; ++i) {
		int interpolationNum = interpolationNums[i];

		double interval = 1.0 / (interpolationNum + 1);
		std::vector<double> mus;
		for (int i = 0; i < interpolationNum + 1; ++i) {
			mus.push_back(i * interval);
		}

		Point3f y0, y1, y2, y3;
		double mu;
		y0 = data[i];
		y1 = data[i + 1];
		y2 = data[i + 2];
		y3 = data[i + 3];
		for (int j = 0; j < mus.size(); ++j) {
			mu = mus[j];
			res.push_back(HermiteInterpolation(y0, y1, y2, y3, mu, 0, bias));
		}
	}
	return res;
}

// 当已经完成全部的曲线点选择，在修改曲线点的位置导致点的数量发生变化制，调用这个函数重新分区
void EditManuallyDrawCurve::adjustDegree()
{
	if (m_model->m_curve.size() > 200) { // the points' number of a completed curve should more than 200
		m_model->m_buccalPointsIndex.clear();
		m_model->m_lingualPointsBackIndex.clear();
		m_model->m_lingualPointsFrontIndex.clear();

		// 查找内外侧
		int start = 0;
		int end = 0;
		double minX_Yless0 = std::numeric_limits<double>::max();
		double minX_Ybig0 = std::numeric_limits<double>::max();

		// 查找起始点,并分区
		for (int j = 0; j < m_model->m_curve.size(); ++j) {
			if (m_model->m_curve[j].Y() < 0) {
				if (m_model->m_curve[j].X() < minX_Yless0) {
					minX_Yless0 = m_model->m_curve[j].X();
					end = j;
				}
			}

			if (m_model->m_curve[j].Y() > 0) {
				if (m_model->m_curve[j].X() < minX_Ybig0) {
					minX_Ybig0 = m_model->m_curve[j].X();
					start = j;
				}
			}
		}

		int i = start;
		while (i != end) {
			m_model->m_buccalPointsIndex.insert(i);
			i = (i + 1) % m_model->m_curve.size();
		}

		i = end;
		while (i != start) {
			// compute the angle between origin-curve[i] direction and X
			Point3f op = m_model->m_curve[i];
			op = op.Normalize();
			double angle = std::acos(op * Point3f(1.0, 0.0, 0.0)) / 3.1415926 * 180;
			if (angle >= *degreefb) {
				m_model->m_lingualPointsBackIndex.insert(i);
			}
			else {
				m_model->m_lingualPointsFrontIndex.insert(i);
			}
			i = (i + 1) % m_model->m_curve.size();
		}

		//调整切割角度
		double cutDegree = 90 - *cutDegreeOut;
		//std::cout << __LINE__ << " cutDegree " << cutDegree << std::endl;
		for (std::set<int>::iterator it = m_model->m_buccalPointsIndex.begin(); it != m_model->m_buccalPointsIndex.end(); ++it) {

			m_model->m_cutDir[*it].Z() = tan(cutDegree / 180.0 * 3.1415926);
			m_model->m_cutDir[*it] = m_model->m_cutDir[*it].Normalize();
		}

		cutDegree = 90 - *cutDegreeInsideBack;
		//std::cout << __LINE__ << " cutDegree " << cutDegree << std::endl;
		for (std::set<int>::iterator it = m_model->m_lingualPointsBackIndex.begin(); it != m_model->m_lingualPointsBackIndex.end(); ++it) {

			m_model->m_cutDir[*it].Z() = tan(cutDegree / 180.0 * 3.1415926);
			m_model->m_cutDir[*it] = m_model->m_cutDir[*it].Normalize();
		}

		cutDegree = 90 - *cutDegreeInsideFront;
		//std::cout << __LINE__ << " cutDegree " << cutDegree << std::endl;
		for (std::set<int>::iterator it = m_model->m_lingualPointsFrontIndex.begin(); it != m_model->m_lingualPointsFrontIndex.end(); ++it) {

			m_model->m_cutDir[*it].Z() = tan(cutDegree / 180.0 * 3.1415926);
			m_model->m_cutDir[*it] = m_model->m_cutDir[*it].Normalize();
		}

		// smooth the cut direction
		int iteration = 2;
		std::vector<Point3f> temp;
		for (int j = 0; j < iteration; ++j) {
			temp = m_model->m_cutDir;
			for (int i = 0; i < m_model->m_cutDir.size(); ++i) {
				// smooth i-1、i、i+1
				Point3f res(0.0, 0.0, 0.0);
				for (int k = -1; k <= 1; ++k) {
					int index = i + k;
					if (index < 0) {
						index += m_model->m_cutDir.size();
					}
					else if (index >= m_model->m_cutDir.size()) {
						index = index % m_model->m_cutDir.size();
					}
					res += m_model->m_cutDir[index];
				}
				temp[i] = res.Normalize();
			}
			m_model->m_cutDir = temp;
		}
	}
}

bool  EditManuallyDrawCurve::PickSurfacePoint(Point2f& pos, Point3f& point)
{
	UpdateMatrix();

	GLdouble spacePoint[3];//空间坐标
	float depth;

	if (pos[0] >= 0 && pos[0] < m_viewport[2] && pos[1] >= 0 && pos[1] < m_viewport[3])
	{
		glReadPixels(pos[0], pos[1], 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	}
	else return false;

	if (depth > 0.99) return false;//如果深度缓存中没有数据返回

	gluUnProject(pos[0], pos[1], depth, m_mvMatrix, m_projMatrix, m_viewport,
		&spacePoint[0], &spacePoint[1], &spacePoint[2]);

	point = Point3f(spacePoint[0], spacePoint[1], spacePoint[2]);

	return true;
}

bool  EditManuallyDrawCurve::HitPoints(Point2f& curPoint, double* mvMatrix,
	double* projMatrix, int* viewport, unsigned int* selectBuf)
{
	if (m_model == 0)
		return false;

	long hits = 0;

	int cnt = (int)m_model->m_curve.size();

	if (cnt == 0) return false;

	m_pickIndex = -1;

	//初始化OpenGL选择模式
	glSelectBuffer(cnt * 4 * 2, selectBuf);
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
	for (int i = 0; i < cnt; i++)
	{
		Point3f& pp = m_model->m_curve[i];

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
void EditManuallyDrawCurve::RemoveLastPoint()
{
	//m_model->m_ctrlPoints.pop_back();
	if (lastPointIndex >= 0 && lastPointIndex < m_model->m_ctrlPoints.size()) {
		m_model->m_ctrlPoints.erase(m_model->m_ctrlPoints.begin() + lastPointIndex);
		lastPointIndex = -1;
	}
	else {
		m_model->m_ctrlPoints.pop_back();
	}
	DoInterpolation();
}
