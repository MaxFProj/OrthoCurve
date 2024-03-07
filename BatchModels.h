#ifndef BATCH_MODELS_H
#define BATCH_MODELS_H

#include <vector>
#include <string>
#include <qstring.h>
#include"Graphics\sn3dmeshmodel.h"

class BatchModels
{
	
public:
	BatchModels();
	~BatchModels();

	void SetModels(std::vector<sn3DMeshModel *> &models) { m_models = models; };
	void AddModel(sn3DMeshModel *m)                      { m_models.push_back(m); }
	bool AddModel(std::wstring name);
	int  Count()                                         { return m_models.size(); }
	sn3DMeshModel *GetModel(int index)                   { return m_models[index]; }
	
	void DeleteAll();
	bool SaveAllCurves(QString name,bool withMark, bool saveNormal, bool isManual, bool isStaubli = true);

	static bool SaveCurve(QString name, sn3DMeshModel *model, bool saveNormal);
	static bool SaveCurveAndMark(QString name, sn3DMeshModel *model, bool saveNormal);

	static bool SaveManuallyCurvePts(QString name, sn3DMeshModel* model, bool saveNormal, bool withMark);
	static bool SaveManuallyCurveTxt(QString name, sn3DMeshModel* model, bool saveNormal, bool withMark);
	static sn3DMeshModel* ReadModel(std::wstring name);
private:
	std::vector<sn3DMeshModel *> m_models;
};

#endif // ORTHOCURVE_H
