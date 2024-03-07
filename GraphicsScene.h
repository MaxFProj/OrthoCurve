#ifndef _GRAPHICSSCENE_
#define _GRAPHICSSCENE_

#include <QPointF>
#include <QGraphicsScene>
#include "CustomScene.h"
#include "CustomInteractor.h"


#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

class QPushButton;
class QToolButton;

class OpenGLView;
class GraphicsScene: public QGraphicsScene
{
    Q_OBJECT
public:

    GraphicsScene( QObject* pParent = 0 );
	~GraphicsScene();
 
	
	void SetView(OpenGLView * view);
	void SetMainScene(CustomScene* scene) { m_scene = scene; }
	void SetInteractor(CustomInteractor* interactor){m_interactor = interactor;}
	

signals:

protected:
    void mousePressEvent( QGraphicsSceneMouseEvent* pEvent );
	void mouseDoubleClickEvent( QGraphicsSceneMouseEvent* pEvent );
    void mouseMoveEvent( QGraphicsSceneMouseEvent* pEvent );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent* pEvent );
    void wheelEvent( QGraphicsSceneWheelEvent* pEvent );
	void drawBackground(QPainter* pPainter, const QRectF& rect);
private slots:
 
	
private:
    QToolButton* CreateButton(const QString& btnName, QString& pngName,
        const QSize& btnSize, const Qt::ToolButtonStyle& btnStyle);
    
    // 鼠标事件需要
    QPointF         m_LastPos;
	bool            m_bChanged;
	bool            m_bOpened;
	
	OpenGLView *       m_view;
	CustomScene  *     m_scene;
	CustomInteractor * m_interactor;
};

#endif // GRAPHICSSCENE_H
