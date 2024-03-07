#include<gl/glew.h>
#include <cmath>
#include <QMatrix4x4>
//#include <QtOpenGL>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPainter>
#include <QMessageBox>

#include "OpenGLView.h"
#include"CustomGLWidget.h"

#include <iostream>

void OpenGLView::SetupMainScene()
{
	((GraphicsScene*)scene())->SetInteractor(this->m_interactor);
	this->setMouseTracking(true);
}
void OpenGLView::SetupCalibScene()
{


}
void OpenGLView::PaintGL(void)
{
	static int count = 0;
	if (m_scene)
	{
		((CustomGLWidget*)viewport())->makeCurrent();

		if (count == 10)
		{
			HWND hwnd = (HWND)(this)->winId();
			HDC hdc = GetDC(hwnd);
			m_scene->m_font.BuildFont(hdc);
		}
		count++;
		m_scene->PaintStatic();
		m_scene->PaintScene();
		if (m_interactor)
			m_interactor->Draw(m_scene, 0);
		m_scene->PaintEnd();
	}
}
OpenGLView::OpenGLView(QWidget* pParent) : QGraphicsView(pParent)
{

	m_scene = new CustomScene;

	m_scene->Initialize(0, 0, Q_NULLPTR);
	m_interactor = new CustomInteractor;
	m_interactor->SetScene(m_scene);
	m_animationTimer = 0;
}
OpenGLView::~OpenGLView(void)
{
	if (m_interactor)
	{
		delete m_interactor;
	}
	if (m_scene)
	{
		delete m_scene;
	}
}
void OpenGLView::setScene(GraphicsScene* pScene)
{
	QGraphicsView::setScene(pScene);
}
void OpenGLView::InitGL(void)
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.6, 0.6, 0.6, 1.0);
}
void OpenGLView::resizeEvent(QResizeEvent* pEvent)
{
	scene()->setSceneRect(0.0, 0.0, pEvent->size().width(), pEvent->size().height());
	if (m_scene != NULL)
	{
		m_scene->Resize(0, 0, pEvent->size().width(), pEvent->size().height());
	}
	//PaintGL();
	pEvent->accept();
}
void OpenGLView::ResizeGL(int w, int h)
{
	if (m_scene != NULL)
	{
		m_scene->Resize(0, 0, w, h);
	}
}
void OpenGLView::drawBackground(QPainter* pPainter, const QRectF& rc)
{
	QGraphicsView::drawBackground(pPainter, rc);

	pPainter->beginNativePainting();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	InitGL();
	ResizeGL(pPainter->device()->width(), pPainter->device()->height());
	//PaintGL();

	if (m_scene)
	{
		((CustomGLWidget*)viewport())->makeCurrent();
		m_scene->PaintStatic();
		m_scene->PaintScene();
		if (m_interactor)
			m_interactor->Draw(m_scene, 0);
		m_scene->PaintEnd();
	}

	glPopAttrib();
	pPainter->endNativePainting();

}
void OpenGLView::drawForeground(QPainter *pPainter, const QRectF &rc)
{
	QGraphicsView::drawForeground(pPainter, rc);

	//»æÖÆ¶þÎ¬Í¼Ïñ
	pPainter->setFont(QFont("Microsoft YaHei", 24));
	pPainter->setPen(QColor(254,185,1));

	if (m_scene && m_scene->m_customObjects.size() >= 1) {
		const QRect rectangle = QRect(10, 50, 1000, 200);
		QString text("Current Model£º");
		text += QString(tr(m_scene->m_customObjects[0]->GetName()));
		pPainter->drawText(rectangle, 0, text);
	}
}
void OpenGLView::mouseDoubleClickEvent(QMouseEvent* pEvent)
{
	QGraphicsView::mouseDoubleClickEvent(pEvent);
}
void OpenGLView::mousePressEvent(QMouseEvent* pEvent)
{
	QGraphicsView::mousePressEvent(pEvent);
	this->scene()->update(this->sceneRect());
}
void OpenGLView::mouseMoveEvent(QMouseEvent* pEvent)
{
	QGraphicsView::mouseMoveEvent(pEvent);
	this->scene()->update(this->sceneRect());

}
void OpenGLView::mouseReleaseEvent(QMouseEvent* pEvent)
{
	QGraphicsView::mouseReleaseEvent(pEvent);
	this->scene()->update(this->sceneRect());

}
void OpenGLView::wheelEvent(QWheelEvent* pEvent)
{
	QGraphicsView::wheelEvent(pEvent);
}
