#pragma once

#include <QOpenGLWidget>
#include "CustomScene.h"
class CustomGLWidget : public QOpenGLWidget
{
public:

	CustomScene * GetMainScene();
	CustomGLWidget(QWidget *parent);

protected:
	void initializeGL();

	void resizeGL(int w, int h);
	void paintGL();


	CustomScene * m_mainScene; // Ö÷³¡¾°
	

};
