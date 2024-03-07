#include"CustomGLWidget.h"
#include<QOpenGLFunctions.h>
CustomGLWidget::CustomGLWidget(QWidget *parent) :QOpenGLWidget(parent)
{
	this->setMouseTracking(true);
}

void CustomGLWidget::initializeGL()
{
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
}

void CustomGLWidget::resizeGL(int w, int h)
{
}

void CustomGLWidget::paintGL()
{
	this->makeCurrent();
}