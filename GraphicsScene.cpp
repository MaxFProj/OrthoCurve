#include <QDebug>
#include <QApplication>
#include <QMessageBox>
//#include <QtOpenGL>
#include "GraphicsScene.h"
#include "OpenGLView.h"
#include<QGraphicsSceneMouseEvent>
#include<qvector2d>
GraphicsScene::GraphicsScene(QObject* pParent) :
	QGraphicsScene(pParent)
{
}
GraphicsScene::~GraphicsScene()
{
}
void GraphicsScene::SetView(OpenGLView * view)
{
	this->m_view = view;
}
void GraphicsScene::drawBackground(QPainter* pPainter, const QRectF&)
{
	QRectF rect = sceneRect();
	QSize btnSize(48, 48);
	QPointF pos(rect.width() - btnSize.width() - 10, rect.height() / 2 - btnSize.height() * 7);
}
void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* pEvent)
{
	QGraphicsScene::mousePressEvent(pEvent);
	if (pEvent->isAccepted())
		return;

	m_LastPos = pEvent->scenePos();
	switch (pEvent->button())
	{
	case Qt::LeftButton:
		m_interactor->OnLButtonDown(NULL, 0, pEvent->scenePos().x(), pEvent->scenePos().y());
		break;

	case Qt::RightButton:
		m_interactor->OnRButtonDown(NULL, pEvent->modifiers(), pEvent->scenePos().x(), pEvent->scenePos().y());
		break;

	case Qt::MiddleButton:
		m_interactor->OnMButtonDown(NULL, pEvent->modifiers(), pEvent->scenePos().x(), pEvent->scenePos().y());
		break;
	default:
		break;
	}
}
void GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* pEvent)
{
	QGraphicsScene::mouseDoubleClickEvent(pEvent);
	if (pEvent->isAccepted())
		return;
	m_LastPos = pEvent->scenePos();
	switch (pEvent->button())
	{
	case Qt::LeftButton:
		m_interactor->OnLButtonDblDown(NULL, 0, pEvent->scenePos().x(), pEvent->scenePos().y());
		break;
	case Qt::RightButton:
		break;
	case Qt::MiddleButton:
		break;
	default:
		break;
	}
	pEvent->accept();
}
void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* pEvent)
{
	QGraphicsScene::mouseMoveEvent(pEvent);
	if (pEvent->isAccepted())
		return;
	QVector2D delta(pEvent->scenePos() - m_LastPos);
	m_interactor->OnMouseMove(NULL, 0, pEvent->scenePos().x(), pEvent->scenePos().y());

	pEvent->accept();
	this->update(m_view->sceneRect());
}
void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* pEvent)
{
	QGraphicsScene::mouseReleaseEvent(pEvent);
	if (pEvent->isAccepted())
		return;
	QVector2D delta(pEvent->scenePos() - m_LastPos);
	switch (pEvent->button())
	{
	case Qt::LeftButton:
		m_interactor->OnLButtonUp(NULL, 0, pEvent->scenePos().x(), pEvent->scenePos().y());

		break;
	case Qt::RightButton:
		m_interactor->OnRButtonUp(NULL, 0, pEvent->scenePos().x(), pEvent->scenePos().y());
		break;
	case Qt::MiddleButton:
		m_interactor->OnMButtonUp(NULL, 0, pEvent->scenePos().x(), pEvent->scenePos().y());
		break;
	default:
		break;
	}
	pEvent->accept();
	m_view->repaint();
	this->update(m_view->sceneRect());
}
void GraphicsScene::wheelEvent(QGraphicsSceneWheelEvent* pEvent)
{
	QGraphicsScene::wheelEvent(pEvent);
	if (pEvent->isAccepted())
		return;
	m_interactor->OnMouseWheel(NULL, 0, pEvent->delta() / 2, pEvent->scenePos().x(), pEvent->scenePos().y());
	pEvent->accept();
	m_view->repaint();
	this->update(m_view->sceneRect());
}

