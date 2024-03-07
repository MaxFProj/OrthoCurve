#include "BatchModels.h"
#include"Graphics/ImporterStl.h"
#include <qfileinfo.h>
#include <qfile.h>
#include <QTextStream.h>
#include <QDir>
#include <iostream>

#include <qDebug>

void rotationMatrixToAngles() {

}

BatchModels::BatchModels()
{
};

BatchModels::~BatchModels()
{
	DeleteAll();
}
void BatchModels::DeleteAll()
{
	for (int i = 0; i < m_models.size(); i++)
		delete m_models[i];
	m_models.clear();
}
bool BatchModels::AddModel(std::wstring name)
{
	sn3DMeshModel * model = BatchModels::ReadModel(name);
	if (model)
	{
		QString qname(QString::fromStdWString(name));
		QFileInfo info(qname);

		//std::string str = info.fileName().toStdString();
		std::string str = info.baseName().toStdString();
		model->SetName(str);
		m_models.push_back(model);
		return true;
	}
	return false;
}
bool BatchModels::SaveAllCurves(QString path, bool withMark, bool saveNormal, bool isManual, bool isStaubli)
{
	bool ret = false;
	QString extension = path.mid(path.lastIndexOf('.'));
	path = path.mid(0, path.lastIndexOf('/'));

	if (!QDir(path).exists()) {
		QDir().mkdir(path);
	}

	for (int i = 0; i < m_models.size(); i++)
	{
		sn3DMeshModel* m = m_models[i];
		QStringList model_names = QString(m->GetName()).split('_');
		QString model_name = model_names.at(0) + model_names.at(1) + QString("%1").arg(model_names.at(2), 2, QLatin1Char('0'));

#ifdef IS_TWIN_MODEL 
		{
			//保存两遍
			std::vector<Point3f> curve;
			std::vector<Point3f> cutDir;
			std::vector<Point3f> markPoints;

			curve.assign(m->m_curve.begin(), m->m_curve.end());
			cutDir.assign(m->m_cutDir.begin(), m->m_cutDir.end());
			markPoints.assign(m->m_markPoints.begin(), m->m_markPoints.end());

			// save left curves
			for (int j = 0; j < curve.size(); ++j) {
				m->m_curve[j] = m->trans_mat_left * curve[j];
			}
			for (int j = 0; j < cutDir.size(); ++j) {
				m->m_cutDir[j] = m->trans_mat_left * cutDir[j];
			}
			for (int j = 0; j < markPoints.size(); ++j) {
				m->m_markPoints[j] = m->trans_mat_left * markPoints[j];
			}

			QString fileName = path + QString("/") + model_name + 'L' + extension;
			if (extension == QString(".txt")) {
				ret = SaveManuallyCurveTxt(fileName, m, saveNormal, withMark);
			}
			else if (extension == QString(".pts")) {
				ret = SaveManuallyCurvePts(fileName, m, saveNormal, withMark);
			}

			// save right curves
			for (int j = 0; j < curve.size(); ++j) {
				m->m_curve[j] = m->trans_mat_right * curve[j];
			}
			for (int j = 0; j < cutDir.size(); ++j) {
				m->m_cutDir[j] = m->trans_mat_right * cutDir[j];
			}
			for (int j = 0; j < markPoints.size(); ++j) {
				m->m_markPoints[j] = m->trans_mat_right * markPoints[j];
			}

			fileName = path + QString("/") + model_name + 'R' + extension;
			if (extension == QString(".txt")) {
				ret = SaveManuallyCurveTxt(fileName, m, saveNormal, withMark);
			}
			else if (extension == QString(".pts")) {
				ret = SaveManuallyCurvePts(fileName, m, saveNormal, withMark);
			}

			m->m_curve.assign(curve.begin(), curve.end());
			m->m_markPoints.assign(markPoints.begin(), markPoints.end());
			m->m_cutDir.assign(cutDir.begin(), cutDir.end());
		}
#else 
		{
			QString fileName = path + QString("/") + model_name + extension;
			if (extension == QString(".txt")) {
				ret = SaveManuallyCurveTxt(fileName, m, saveNormal, withMark);
			}
			else if (extension == QString(".pts")) {
				ret = SaveManuallyCurvePts(fileName, m, saveNormal, withMark);
			}
		}
#endif
	}
	return ret;
}
bool BatchModels::SaveCurve(QString name, sn3DMeshModel *model, bool saveNormal)
{
	QFile file(name);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return false;

	QTextStream out(&file);
	out << QString("BEGIN_0\n");
	int size = model->m_samplePoints.size();

	// 重新梳理起始点
	int start = 0;
	double minX = std::numeric_limits<double>::max();
	for (int i = 0; i < size; ++i) {
		if (model->m_samplePoints[i].Y() > 0 && model->m_samplePoints[i].X() < minX) {
			minX = model->m_samplePoints[i].X();
			start = i;
		}
	}

	int index = start;
	do {
		Point3f p = model->m_samplePoints[index];
		out << QString::number(p[0]) << " " << QString::number(p[1]) << " " << QString::number(p[2]);
		if (saveNormal)
		{
			Point3f n = model->m_sampleNormals[index];
			out << " " << QString::number(n[0]) << " " << QString::number(n[1]) << " " << QString::number(n[2]);
		}
		out << QString("\n");

		index = (index + 1) % model->m_samplePoints.size();
	} while (index != start);

	if (size > 0){
		Point3f p = model->m_samplePoints[start];
		out << QString::number(p[0]) << " " << QString::number(p[1]) << " " << QString::number(p[2]);
		if (saveNormal)
		{
			Point3f n = model->m_sampleNormals[start];
			out << " " << QString::number(n[0]) << " " << QString::number(n[1]) << " " << QString::number(n[2]);
		}
		out << QString("\n");
	}

	out << QString("END_0\n");
	file.close();

	return true;
}
bool BatchModels::SaveCurveAndMark(QString name, sn3DMeshModel *model, bool saveNormal)
{
	QFile file(name);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return false;
	if (name.size()<7)
	{
		return false;
	}
	QString nameInfo = name.right(7);
	nameInfo = nameInfo.left(3);
	QString number = nameInfo.left(2);
	QString word = nameInfo.right(1);
	
	int size = 0;
	QTextStream out(&file);
	out << QString("HEAD\n");
	size = model->m_markPoints.size();
	for (int i = 0; i < size; i++)
	{
		out << QString("logo")<<i+1<< QString("=");
		out << model->m_markPoints[i].x << QString(" ") << model->m_markPoints[i].y << QString(" ") << model->m_markPoints[i].z;
		if (i == 0)
		{
			out << QString("/");
			if(number.size()<2)
				out << QString("0"); //补0
			out << number;

			if(word.size()>0)
				out << word[0];
			out << QString("\n");
		}
			
		else
			out << QString("/ \n");
	}
	out << QString("END\n");
	size = model->m_samplePoints.size();

	// 重新梳理起始点
	int start = 0;
	double minX = std::numeric_limits<double>::max();
	for (int i = 0; i < size; ++i) {
		if(model->m_samplePoints[i].Y() > 0 && model->m_samplePoints[i].X() < minX){
			minX = model->m_samplePoints[i].X();
			start = i;
		}
	}

	int index = start;
	do {
		Point3f p = model->m_samplePoints[index];
		out << QString::number(p[0]) << " " << QString::number(p[1]) << " " << QString::number(p[2]);
		if (saveNormal)
		{
			Point3f n = model->m_sampleNormals[index];
			out << " " << QString::number(n[0]) << " " << QString::number(n[1]) << " " << QString::number(n[2]);
		}
		out << QString("\n");

		index = (index + 1) % model->m_samplePoints.size();
	} while (index != start);

	if (size > 0)
	{
		Point3f p = model->m_samplePoints[start];
		out << QString::number(p[0]) << " " << QString::number(p[1]) << " " << QString::number(p[2]);
		if (saveNormal)
		{
			Point3f n = model->m_sampleNormals[start];
			out << " " << QString::number(n[0]) << " " << QString::number(n[1]) << " " << QString::number(n[2]);
		}
		out << QString("\n");
	}

	out << QString("END\n");
	file.close();

	return true;
}
bool BatchModels::SaveManuallyCurvePts(QString name, sn3DMeshModel* model, bool saveNormal, bool withMark)
{
	QFile file(name);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return false;

	if (name.size() < 7){
		return false;
	}
	QString nameInfo = name.right(7);
	nameInfo = nameInfo.left(3);
	QString number = nameInfo.left(2);
	QString word = nameInfo.right(1);

	QTextStream out(&file);
	int size = 0;

	if (withMark) {
		out << QString("HEAD\n");
		size = model->m_markPoints.size();

		if (size != 2) { // 输出默认值
			out << QString("logo1=0 0 0/ \n");
			out << QString("logo2=0 0 0/ \n");
		}
		else {
			for (int i = 0; i < size; i++)
			{
				out << QString("logo") << i + 1 << QString("=");
				out << model->m_markPoints[i].x << QString(" ") << model->m_markPoints[i].y << QString(" ") << model->m_markPoints[i].z;
				if (i == 0)
				{
					out << QString("/");
					if (number.size() < 2)
						out << QString("0"); //补0
					out << number;

					if (word.size() > 0)
						out << word[0];
					out << QString("\n");
				}
				else
					out << QString("/ \n");
			}
		}

		out << QString("END\n");
	}

	out << QString("BEGIN_0\n");
	size = model->m_curve.size();
	for (int i = 0; i < size; i++)
	{
		Point3f p = model->m_curve[i];
		out << QString::number(p[0]) << " " << QString::number(p[1]) << " " << QString::number(p[2]);
		if (saveNormal)
		{
			Point3f n = model->m_cutDir[i];
			out << " " << QString::number(n[0]) << " " << QString::number(n[1]) << " " << QString::number(n[2]);
		}
		out << QString("\n");
	}
	if (size > 0) // 首尾相连
	{
		Point3f p = model->m_curve[0];
		out << QString::number(p[0]) << " " << QString::number(p[1]) << " " << QString::number(p[2]);
		if (saveNormal)
		{
			Point3f n = model->m_cutDir[0];
			out << " " << QString::number(n[0]) << " " << QString::number(n[1]) << " " << QString::number(n[2]);
		}
		out << QString("\n");
	}
	out << QString("END_0\n");
	file.close();

	return true;
}
bool BatchModels::SaveManuallyCurveTxt(QString name, sn3DMeshModel* model, bool saveNormal, bool withMark)
{
	QFile file(name);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return false;
	if (name.size() < 7)
	{
		return false;
	}
	QString nameInfo = name.right(7);
	nameInfo = nameInfo.left(3);
	QString number = nameInfo.left(2);
	QString word   = nameInfo.right(1);

	int size = 0;
	QTextStream out(&file);

	if (withMark) {
		out << QString("HEAD\n");
		size = model->m_markPoints.size();

		if (size != 2) { // 输出默认值
			out << QString("logo1=0 0 0/ \n");
			out << QString("logo2=0 0 0/ \n");
		}
		else {
			for (int i = 0; i < size; i++)
			{
				Point3f markpoint = model->m_markPoints[i];

				out << QString("logo") << i + 1 << QString("=");
				out << model->m_markPoints[i].x << QString(" ") << model->m_markPoints[i].y << QString(" ") << model->m_markPoints[i].z;
				if (i == 0)
				{
					out << QString("/");
					if (number.size() < 2)
						out << QString("0"); //补0
					out << number;

					if (word.size() > 0)
						out << word[0];
					out << QString("\n");
				}

				else
					out << QString("/ \n");
			}
		}

		out << QString("END\n");
	}

	size = model->m_curve.size();
	for (int i = 0; i < size; i++)
	{
		Point3f p = model->m_curve[i];
		out << QString::number(p[0]) << " " << QString::number(p[1]) << " " << QString::number(p[2]);
		if (saveNormal)
		{	
			Point3f n = model->m_cutDir[i];
			out << " " << QString::number(n[0]) << " " << QString::number(n[1]) << " " << QString::number(n[2]);
		}
		out << QString("\n");
	}

	if (size > 0) //首尾相连
	{
		Point3f p = model->m_curve[0];
		out << QString::number(p[0]) << " " << QString::number(p[1]) << " " << QString::number(p[2]);
		if (saveNormal)
		{
			Point3f n = model->m_cutDir[0];
			out << " " << QString::number(n[0]) << " " << QString::number(n[1]) << " " << QString::number(n[2]);
		}
		out << QString("\n");
	}

	if (saveNormal) {
		out << QString("END\n");
	}

	file.close();

	return true;
}
sn3DMeshModel* BatchModels::ReadModel(std::wstring name)
{
	sn3DMeshModel * model = new sn3DMeshModel;
	model->AddMesh();
	ImporterStl stl;
	if (stl.read(name, *model))
		return model;
	else
	{
		delete model;
		return NULL;
	}
}
