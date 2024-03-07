
#ifndef OPENGLVIEW_H
#define OPENGLVIEW_H

#include <QVector3D>
#include <QGraphicsView>
#include "Camera.h"
#include "GraphicsScene.h"
#include "CustomScene.h"
#include "CustomInteractor.h"
class StageManager;
class OpenGLView: public QGraphicsView
{
    Q_OBJECT
public:

	void SetupMainScene();

	void SetupCalibScene();

    OpenGLView( QWidget* pParent = 0 );
    virtual ~OpenGLView( void );
    void setScene( GraphicsScene* pScene );

	CustomScene * GetMainScene(){return m_scene;}
	
	CustomInteractor* GetCustomInteractor(){return m_interactor;}
	

public slots:

protected:
    void resizeEvent( QResizeEvent* pEvent );
    void mousePressEvent( QMouseEvent* pEvent );
	void mouseDoubleClickEvent( QMouseEvent* pEvent );
    void mouseReleaseEvent( QMouseEvent* pEvent );
    void mouseMoveEvent( QMouseEvent* pEvent );
    void wheelEvent( QWheelEvent* pEvent );
    void drawBackground( QPainter* pPainter, const QRectF& rect );
	void drawForeground(QPainter *painter, const QRectF &rect);

public:
    void InitGL( void );
    void ResizeGL( int width, int height );
    void PaintGL( void );

	CustomScene * m_scene; // Ö÷³¡¾°

	StageManager * m_stageManager;
	CustomInteractor * m_interactor;
	QTimer * m_animationTimer;
};

#endif // OPENGLVIEW_H
