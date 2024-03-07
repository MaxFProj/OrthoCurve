#include "orthocurve.h"
#include"CustomGLWidget.h"
#include <QFileDialog>
#include <QStyle>
#include <QToolButton>
#include <QToolTip>
#include <QSettings>  //读写ini文件。
#include <QDebug>
#include <QMenu>

#include <qmessagebox.h>
#include "ComputeCurve.h"
#include "SettingWidget.h"
#include "EditDrawCurve.h"
#include "Graphics/EditMarkIcon.h"

//新加的库
#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export.h>

using namespace Common;
QString g_version = QString("V2.1.0");
ComputeCurve g_computeCurve; // 全局数据
OpenGLView* g_pView;
OrthoCurve* g_mainWnd;
int g_offsettep = 8; // 龈缘线offset距离参数

OrthoCurve::OrthoCurve(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	QString appPath = qApp->applicationDirPath();
	QString iconName = "/Resources/OrthoCurve.png";


	iconName = appPath + iconName;

	setWindowIcon(QIcon(iconName));
	ui.mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

	setWindowTitle(tr("正畸曲线"));
	ui.mainToolBar->setStyleSheet("background-color: rgba(255, 255, 255, 255);");
	ui.mainToolBar->setIconSize(QSize(40, 40));

	// add context menu
	QAction* computeIntersection = new QAction(this);
	QMenu pop_menu;
	pop_menu.addAction(computeIntersection);
	//menu.addAction(QIcon("1.png"), QStringLiteral("添加"), this, SLOT(OnBscGroupRightAction()));
	//menu.exec(QCursor::pos());

	//connect(ui.actionImportSingleSTL, &QAction::triggered, this, &OrthoCurve::OnImportSTL);
	connect(ui.actionImportSTL, &QAction::triggered, this, &OrthoCurve::OnImportSTLs);
	connect(ui.actionComputeCurve, &QAction::triggered, this, &OrthoCurve::OnComputeCurve);
	connect(ui.actionSaveCurve, &QAction::triggered, this, &OrthoCurve::OnSaveCurve);
	connect(ui.actionSetting, &QAction::triggered, this, &OrthoCurve::OnSetting);
	connect(ui.actionRemoveLastPoints, &QAction::triggered, this, &OrthoCurve::OnRemoveLastPoint);
	//connect(ui.actionAdjustConnect, &QAction::triggered, this, &OrthoCurve::OnAdjustCurveConnect);
	connect(ui.actionAdjustDegree, &QAction::triggered, this, &OrthoCurve::OnAdjustDegree);
	connect(ui.actionMarkIcon, &QAction::triggered, this, &OrthoCurve::OnMarkIcon);
	connect(computeIntersection, &QAction::triggered, this,&OrthoCurve::OnComputeIntersections);

	ShowMessage("请导入模型");
	LoadSetting();
	m_editCurve = NULL;
	m_editMark  = NULL;
	m_editManuallyCurve = NULL;

	projectCurveProgressDialog = new QProgressDialog(this); 
	projectCurveProgressDialog->setLabelText("正在导入模型...");
	projectCurveProgressDialog->setMinimum(0);
	projectCurveProgressDialog->setMaximum(100);
	projectCurveProgressDialog->setWindowModality(Qt::WindowModal);
	projectCurveProgressDialog->setValue(100);
}

OrthoCurve::~OrthoCurve()
{

}
void OrthoCurve::Init()
{
	//setWindowIcon(QIcon(":/Resources/OrthoCurve.png"));
	pView = CreateOpenGLView();
	g_pView = pView;

	this->setCentralWidget(pView);

	QString appPath, prevName, nextName;
	appPath = qApp->applicationDirPath();
	prevName = "/Resources/prev.png";
	nextName = "/Resources/next.png";
	prevName = appPath + prevName;
	nextName = appPath + nextName;

	prevButton = createToolButton(tr("前一个模型"), QIcon(prevName), SLOT(OnPreview()));
	nextButton = createToolButton(tr("后一个模型"), QIcon(nextName), SLOT(OnNext()));

	// craete view side buttons
	viewSideButtons.resize(6);
	viewSideButtons[0] = createToolButton(tr("上视图"), QIcon(appPath + "/Resources/up.png"),		SLOT(OnUp()));
	viewSideButtons[1] = createToolButton(tr("下视图"), QIcon(appPath + "/Resources/down.png"),		SLOT(OnDown()));
	viewSideButtons[2] = createToolButton(tr("左视图"), QIcon(appPath + "/Resources/left.png"),		SLOT(OnLeft()));
	viewSideButtons[3] = createToolButton(tr("右视图"), QIcon(appPath + "/Resources/right.png"),		SLOT(OnRight()));
	viewSideButtons[4] = createToolButton(tr("前视图"), QIcon(appPath + "/Resources/front.png"),		SLOT(OnFront()));
	viewSideButtons[5] = createToolButton(tr("后视图"), QIcon(appPath + "/Resources/back.png"),		SLOT(OnBack()));

	showMaximized();
}
OpenGLView* OrthoCurve::CreateOpenGLView()
{
	CustomGLWidget* glWidget = new CustomGLWidget(this);
	pView = new OpenGLView(ui.centralWidget);
	pView->setObjectName(QStringLiteral("graphicsView"));

	pView->setViewport(glWidget);
	pView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

	GraphicsScene* pScene = new GraphicsScene(this);
	pView->setScene(pScene);

	pScene->SetMainScene(pView->GetMainScene());
	pScene->SetInteractor(pView->GetCustomInteractor());
	pScene->SetView(pView);

	return pView;
}
void OrthoCurve::SetSenceModel(sn3DMeshModel* model)
{
	pView->GetMainScene()->ClearScene();
	((CustomGLWidget*)pView->viewport())->makeCurrent();
	pView->GetMainScene()->AddObject(model);

}
void OrthoCurve::ResetSenceEdit()
{
	CustomInteractor* interactor = pView->GetCustomInteractor();
	interactor->SetEdit(NULL);
	if (m_editMark)
	{
		delete m_editMark;
		m_editMark = NULL;
	}
	if (m_editCurve)
	{
		delete m_editCurve;
		m_editCurve = NULL;
	}
}

void OrthoCurve::DoComputeCurve(sn3DMeshModel* model)
{
	g_computeCurve.DoComputeCurve(model, m_thresholdZ, m_normalZ, m_offsetSetp, m_interval, m_smooth);
	return;
	((CustomGLWidget*)pView->viewport())->makeCurrent();
	pView->GetMainScene()->ClearScene();
	int cnt = g_computeCurve.m_modelSet.size();

	for (int i = 0; i < cnt; i++)
	{
		if (!g_computeCurve.m_modelSet[i]->IsS() /*&& !g_computeCurve.m_modelSet[i]->IsM()*/)
			continue;

		pView->GetMainScene()->AddObject(g_computeCurve.m_modelSet[i]);
	}
}

void OrthoCurve::LoadSetting()
{
	QSettings* configIni = new QSettings("config.ini", QSettings::IniFormat);

	m_normalZ		= configIni->value("curve/normalZ", 60).toInt();		//采样点法相与Z轴的夹角
	m_interval		= configIni->value("curve/interval", 0.5).toFloat();	//采样间距 默认0.5
	m_smooth		= configIni->value("curve/smooth", 3).toInt();			//控制曲线的平滑程度，实际上是平滑迭代次数
	m_offsetSetp	= configIni->value("curve/offsetSetp", 8).toInt(); //龈缘线offset距离参数
	m_saveNormal	= configIni->value("curve/saveNormal", true).toBool();
	m_withMark		= configIni->value("curve/withMark", true).toBool();
	m_thresholdZ	= configIni->value("model/thresholdZ", 12).toFloat();
	m_isManual		= configIni->value("model/isManual", false).toBool();
	m_savePath		= configIni->value("path/savePath", false).toString();
	m_importPath	= configIni->value("path/importPath", false).toString();
	m_degreefb      = configIni->value("curve/degreefb", 45).toInt();
	m_cutDegreeOut  = configIni->value("curve/cutDegreeOut", 75).toInt();
	m_cutDegreeInsideFront = configIni->value("curve/cutDegreeInsideFront", 45).toInt();
	m_cutDegreeInsideBack  = configIni->value("curve/cutDegreeInsideBack", 60).toInt();
	m_showBur       = configIni->value("curve/show", false).toBool();
	m_cutOffset     = configIni->value("curve/cutOffset", 0.5).toFloat();

	delete configIni;
}
void OrthoCurve::SaveSetting()
{
	QSettings* configIni = new QSettings("config.ini", QSettings::IniFormat);
	configIni->setValue("curve/normalZ",    m_normalZ);
	configIni->setValue("curve/interval", QString("%2").arg(m_interval));
	configIni->setValue("curve/smooth",     m_smooth);
	configIni->setValue("curve/offsetSetp", m_offsetSetp);
	configIni->setValue("curve/saveNormal", m_saveNormal);
	configIni->setValue("curve/withMark",   m_withMark);
	configIni->setValue("model/thresholdZ", m_thresholdZ);
	configIni->setValue("model/isManual",   m_isManual);
	configIni->setValue("path/importPath",  m_importPath);

	configIni->setValue("curve/degreefb", m_degreefb);
	configIni->setValue("curve/cutDegreeOut", m_cutDegreeOut);
	configIni->setValue("curve/cutDegreeInsideFront", m_cutDegreeInsideFront);
	configIni->setValue("curve/cutDegreeInsideBack", m_cutDegreeInsideBack);

	configIni->setValue("curve/show", m_showBur);
	configIni->setValue("curve/cutOffset", m_cutOffset);

	int start = m_savePath.lastIndexOf('/') + 1;
	int end   = m_savePath.lastIndexOf('.');
	QString savePath = m_savePath.replace(start, end - start, "default");

	configIni->setValue("path/savePath", savePath);

	delete configIni;
}
void OrthoCurve::ShowMessage(QString message)
{
	ui.statusBar->showMessage(message);
}
void OrthoCurve::SetTileFileName(std::string name, int modelCount, int currentIndex)
{
	QString title("正畸曲线   ");
	title += QString().fromStdString(name);
	title += QString().asprintf(" [%d/%d]", currentIndex, modelCount);
	setWindowTitle(title);
}

void OrthoCurve::OnImportSTL()
{
	//m_isManual = true;
	QString filePath = QFileDialog::getOpenFileName(this, tr("STL File"), QDir::currentPath(), tr("STL File (*.stl)"));
	QByteArray ba = filePath.toLocal8Bit();
	const char* c_str2 = ba.data();
	std::string filename = filePath.toUtf8().constData();
	//CMeshO* currentMesh = new CMeshO();

	if (!filePath.isEmpty())
	{
		std::wstring name = filePath.toStdWString();
		sn3DMeshModel* model = BatchModels::ReadModel(name);
		SetSenceModel(model);
		ResetSenceEdit();

		//if (vcg::tri::io::ImporterOFF<CMeshO>::Open(*currentMesh, c_str2) != vcg::tri::io::ImporterOFF<CMeshO>::NoError)
		//{
		//	printf("Error reading file  %s\n", c_str2);
		//	return;
		//}

		//vcg::tri::RequirePerVertexNormal(*currentMesh);
		//vcg::tri::UpdateNormal<CMeshO>::PerVertexNormalized(*currentMesh);
		//printf("Input mesh  vn:%i fn:%i\n", currentMesh->VN(), currentMesh->FN());
		//printf("Mesh has %i vert and %i faces\n", currentMesh->VN(), currentMesh->FN());
	}
}

void OrthoCurve::OnImportSTLs()
{
	//m_models.DeleteAll();
	//ShowMessage("正在导入模型...");
	QFileInfo outputDir(m_importPath);
	//if (!outputDir.exists()) {
	//	m_importPath = QString();
	//}

	QStringList filesPath = QFileDialog::getOpenFileNames(this, tr("STL File"), m_importPath, tr("STL File (*.stl)"));
	if (filesPath.size() > 0) {
		outputDir.setFile(filesPath[0]);
		if (outputDir.exists()) {
			m_importPath = outputDir.dir().path();
			qDebug() <<__LINE__ << "  Model file path " <<  m_importPath;
		}
		SaveSetting();
	}
	else {
		return;
	}

	projectCurveProgressDialog->show();
	projectCurveProgressDialog->setValue(1);
	for (int i = 0; i < filesPath.size(); i++)
	{
		QString str_path = filesPath[i];
		m_models.AddModel(str_path.toStdWString());

		sn3DMeshModel* dataModel = m_models.GetModel(m_models.Count() - 1);
		sn3DMeshData* data = dataModel->GetMeshData();
		dataModel->drawNormal = &m_saveNormal;

		int verticesNum = data->n_vertices();
		int facesNum = data->n_faces();

		// copy them to vcg model
		CMeshO* newMesh = new CMeshO();
		std::vector<vcg::Point3f> points;
		CMeshO::VertexIterator newVertIndex = vcg::tri::Allocator<CMeshO>::AddVertices(*newMesh, verticesNum);
		for (int j = 0; j < verticesNum; ++j, ++newVertIndex) {
			newVertIndex->P()[0] = data->V(j)->P()[0];
			newVertIndex->P()[1] = data->V(j)->P()[1];
			newVertIndex->P()[2] = data->V(j)->P()[2];

			points.push_back(newVertIndex->P());
		}

		CMeshO::FaceIterator newFaceIndex = vcg::tri::Allocator<CMeshO>::AddFaces(*newMesh, facesNum);
		for (int j = 0; j < facesNum; ++j, ++newFaceIndex) {
			newFaceIndex->V(0) = &*(newMesh->vert.begin() + data->F(j)->m_v[0]);
			newFaceIndex->V(1) = &*(newMesh->vert.begin() + data->F(j)->m_v[1]);
			newFaceIndex->V(2) = &*(newMesh->vert.begin() + data->F(j)->m_v[2]);
		}

		//build Octree for each teeth
		AABBTree* tree = new AABBTree();
		tree->Set(newMesh->face.begin(), newMesh->face.end());
		vcg_model_trees.push_back(tree);
	
		// test
		//int result = vcg::tri::io::ExporterSTL<CMeshO>::Save(*newMesh, "D://test.stl", false);
		//if (result != 0) {
		//	std::cout << vcg::tri::io::ExporterSTL<CMeshO>::ErrorMsg(result);
		//}
		projectCurveProgressDialog->setValue((i + 1) * 100.0 / filesPath.size());
	}

	if (m_models.Count() > 0)
	{
		pView->GetMainScene()->ClearScene();
		ResetSenceEdit();

		QMessageBox box;
		box.setText(tr("导入成功"));
		box.exec();

		QString message;
		message = QString("成功导入") + QString::number(m_models.Count()) + QString("个模型");
		ShowMessage(message);
		m_currentID = -1;

		// 手动模式下，导入手动模式的交互器
		if (m_isManual) {
			sn3DMeshModel* model = m_models.GetModel(0);
			if (!m_editManuallyCurve) {
				m_editManuallyCurve = new EditManuallyDrawCurve(model, m_normalZ);
			}
			m_editManuallyCurve->setModel(model);
			m_editManuallyCurve->setTree(vcg_model_trees[0]);

			CustomInteractor* interactor = pView->GetCustomInteractor();
			interactor->SetEdit(m_editManuallyCurve);

			// save cut degree to interactor
			m_editManuallyCurve->cutDegreeOut = &m_cutDegreeOut;
			m_editManuallyCurve->cutDegreeInsideBack = &m_cutDegreeInsideBack;
			m_editManuallyCurve->cutDegreeInsideFront = &m_cutDegreeInsideFront;
			m_editManuallyCurve->showBur = &m_showBur;
			m_editManuallyCurve->cutOffset = &m_cutOffset;
		}

		OnNext();
	}
	else
		ShowMessage("请导入模型");
}

void OrthoCurve::OnComputeCurve()
{
	int num = pView->GetMainScene()->GetObjectCount();
	if (num == 0)
		return;

	sn3DMeshModel* model = (sn3DMeshModel*)pView->GetMainScene()->GetObject(0);
	if (!m_isManual) {
		model->m_curve.clear();
		DoComputeCurve(model);
	}
	else {
		model = m_models.GetModel(m_currentID);
		m_editManuallyCurve->setModel(model);
		m_editManuallyCurve->setTree(vcg_model_trees[m_currentID]);

		CustomInteractor* interactor = pView->GetCustomInteractor();
		interactor->SetEdit(m_editManuallyCurve);
	}
}
void OrthoCurve::OnAdjustDegree()
{
	std::cout << __LINE__ << ":  "<<__FUNCTION__ << " " << "Adjust Degree \n";
	for (int index = 0; index < m_models.Count(); ++index) {
		sn3DMeshModel* model = m_models.GetModel(index);
		AABBTree* tree = vcg_model_trees[index];
		float maxDist = std::numeric_limits<float>::max();
		vcg::EmptyClass objectMarker;
		vcg::RayTriangleIntersectionFunctor<false> rayIntersector;

		if (model->m_curve.size() > 200) {
			// 查找内外侧
			int start = 0;
			int end = 0;
			double minX_Yless0 = std::numeric_limits<double>::max();
			double minX_Ybig0  = std::numeric_limits<double>::max();

			for (int j = 0; j < model->m_curve.size(); ++j) {
				if (model->m_curve[j].Y() < 0) {
					if (model->m_curve[j].X() < minX_Yless0) {
						minX_Yless0 = model->m_curve[j].X();
						end = j;
					}
				}

				if (model->m_curve[j].Y() > 0) {
					if (model->m_curve[j].X() < minX_Ybig0) {
						minX_Ybig0 = model->m_curve[j].X();
						start = j;
					}
				}
			}

			model->m_buccalPointsIndex.clear();
			model->m_lingualPointsBackIndex.clear();
			model->m_lingualPointsFrontIndex.clear();

			int i = start;
			while (i != end) {
				model->m_buccalPointsIndex.insert(i);
				i = (i + 1) % model->m_curve.size();
			}

			i = end;
			while (i != start) {
				// compute the angle between origin-curve[i] direction and X
				Point3f op = model->m_curve[i];
				op = op.Normalize();
				double angle = std::acos(op * Point3f(1.0,0.0,0.0)) / 3.1415926 * 180;
				if (angle >= m_degreefb) {
					model->m_lingualPointsBackIndex.insert(i);
				}
				else {
					model->m_lingualPointsFrontIndex.insert(i);
				}
				i = (i + 1) % model->m_curve.size();
			}

			//调整切割角度
			double cutDegree = 90 - m_cutDegreeOut;
			//std::cout << __LINE__ << " cutDegree " << cutDegree << std::endl;
			for (std::set<int>::iterator it = model->m_buccalPointsIndex.begin(); it != model->m_buccalPointsIndex.end(); ++it) {

				model->m_cutDir[*it].Z() = tan(cutDegree / 180.0 * 3.1415926);
				model->m_cutDir[*it] = model->m_cutDir[*it].Normalize();
			}

			cutDegree = 90 - m_cutDegreeInsideBack;
			//std::cout << __LINE__ << " cutDegree " << cutDegree << std::endl;
			for (std::set<int>::iterator it = model->m_lingualPointsBackIndex.begin(); it != model->m_lingualPointsBackIndex.end(); ++it) {

				model->m_cutDir[*it].Z() = tan(cutDegree / 180.0 * 3.1415926);
				model->m_cutDir[*it] = model->m_cutDir[*it].Normalize();
			}

			cutDegree = 90 - m_cutDegreeInsideFront;
			//std::cout << __LINE__ << " cutDegree " << cutDegree << std::endl;
			for (std::set<int>::iterator it = model->m_lingualPointsFrontIndex.begin(); it != model->m_lingualPointsFrontIndex.end(); ++it) {

				model->m_cutDir[*it].Z() = tan(cutDegree / 180.0 * 3.1415926);
				model->m_cutDir[*it] = model->m_cutDir[*it].Normalize();
			}

			// smooth the cut direction
			int iteration = 2;
			std::vector<Point3f> temp;
			for (int j = 0; j < iteration; ++j) {
				temp = model->m_cutDir;
				for (int i = 0; i < model->m_cutDir.size(); ++i) {
					// smooth i-2、i-1、i、i+1、 i+2
					Point3f res(0.0, 0.0, 0.0);
					for (int k = -1; k <= 1; ++k) {
						int index = i + k;
						if (index < 0) {
							index += model->m_cutDir.size();
						}
						else if (index >= model->m_cutDir.size()) {
							index = index % model->m_cutDir.size();
						}
						res += model->m_cutDir[index];
					}
					temp[i] = res.Normalize();
				}
				model->m_cutDir = temp;
			}

			//update intersection points
			model->m_intersectedPoints.clear();
			for (int i = 0; i < model->m_curve.size(); ++i) {
				vcg::Point3f detectIntersectedDir(model->m_cutDir[i][0], model->m_cutDir[i][1], model->m_cutDir[i][2]);
				vcg::Point3f pointToProj(model->m_curve[i][0], model->m_curve[i][1], model->m_curve[i][2]);
				pointToProj = pointToProj + detectIntersectedDir * 0.5;
				vcg::Ray3<float, false> ray(pointToProj, detectIntersectedDir);
				float rayT = std::numeric_limits<float>::max();
				CFaceO* isectFace = NULL;
				isectFace = tree->DoRay(rayIntersector, objectMarker, ray, maxDist, rayT);
				if (isectFace) {
					std::cout << "have intersection \n";
					vcg::Point3f p = pointToProj + detectIntersectedDir * rayT;
					model->m_intersectedPoints.push_back(Point3f(p.X(), p.Y(), p.Z()));
				}
			}
		}
	}
}
void OrthoCurve::OnSaveCurve()
{
	int num = m_models.Count();
	if (num == 0) {
		QMessageBox box;
		box.setText(tr("当前没有模型"));
		box.exec();
		return;
	}

	//QFileInfo inputDir(m_savePath);
	//if (!inputDir.exists()) {
	//	m_savePath = QString();
	//}
	QString file_path;
	if (m_savePath.mid(m_savePath.lastIndexOf('.')) == QString(".txt")) {
		file_path = QFileDialog::getSaveFileName(this, "请选择切割线保存路径...", m_savePath, tr("text(*.txt);;points(*.pts)"));
	}
	else {
		file_path = QFileDialog::getSaveFileName(this, "请选择切割线保存路径...", m_savePath, tr("points(*.pts);;text(*.txt)"));
	}

	if (file_path.isEmpty()){
		return;
	}
	else
	{
		m_savePath = file_path;
		SaveSetting();
		QMessageBox box;
		if (m_models.SaveAllCurves(file_path, m_withMark, m_saveNormal, m_isManual))
			box.setText(tr("保存成功"));
		else
			box.setText(tr("保存失败"));
		box.exec();
	}
}
void OrthoCurve::OnSetting()
{
	SettingDialog* dialog = new SettingDialog(NULL);
	LoadSetting();//从文件中读取数据
	dialog->SetVersion(g_version);
	dialog->SetValue(m_normalZ, m_offsetSetp, m_interval, m_smooth, m_saveNormal, m_withMark, m_thresholdZ, m_isManual, m_showBur, m_cutOffset);//将文件中数据显示在对话框中
	dialog->SetDegreeValue(m_degreefb, m_cutDegreeOut, m_cutDegreeInsideFront, m_cutDegreeInsideBack);

	//更新界面
	if (m_isManual) {
		dialog->ui.OffsetStepBox->setEnabled(false);
		dialog->ui.intevalBox->setEnabled(false);
		dialog->ui.smoothBox->setEnabled(false);
		dialog->ui.zangleBox->setEnabled(false);
		dialog->ui.zthresholdBox->setEnabled(false);
	}
	else {
		dialog->ui.OffsetStepBox->setEnabled(true);
		dialog->ui.intevalBox->setEnabled(true);
		dialog->ui.smoothBox->setEnabled(true);
		dialog->ui.zangleBox->setEnabled(true);
		dialog->ui.zthresholdBox->setEnabled(true);
	}

	int n = dialog->exec();
	if (n == QDialog::Accepted)
	{
		dialog->GetValue(m_normalZ, m_offsetSetp, m_interval, m_smooth, m_saveNormal, m_withMark, m_thresholdZ, m_isManual, m_showBur, m_cutOffset);
		dialog->GetDegreeValue(m_degreefb, m_cutDegreeOut, m_cutDegreeInsideFront, m_cutDegreeInsideBack);
		SaveSetting();
	}
	delete dialog;
}

void OrthoCurve::OnPreview()
{
	if (m_currentID > 0)
		m_currentID--;
	sn3DMeshModel* model = m_models.GetModel(m_currentID);
	SetSenceModel(model);

	if (m_isManual) {
		m_editManuallyCurve->setModel(model);
		m_editManuallyCurve->setTree(vcg_model_trees[m_currentID]);

		CustomInteractor* interactor = pView->GetCustomInteractor();
		interactor->SetEdit(m_editManuallyCurve);
	}
	else {
		ResetSenceEdit();
	}

	QString message;
	message = QString("总共") + QString::number(m_models.Count()) + QString("个模型，当前是") + QString::number(m_currentID + 1) + QString("个模型。");
	if (m_currentID == 0)
		message += QString("下一个模型向右按钮。");
	else if (m_currentID + 1 >= m_models.Count())
		message += QString("上一个模型向左按钮。");
	else
		message += QString("上一个模型向左按钮，下一个模型向右按钮。");
	if (m_isManual) {
		message += QString("手动选点模式。【左键单击】增加点 【左键单击控制点并拖拽】改变控制点的位置 【右键单击】选择并删除控制点");
	}

	ShowMessage(message);
	SetTileFileName(model->GetName(), m_models.Count(), m_currentID + 1);

	pView->viewport()->update();
}
void OrthoCurve::OnNext()
{
	if (m_currentID + 1 >= m_models.Count())
		return;
	m_currentID++;
	sn3DMeshModel* model = m_models.GetModel(m_currentID);
	SetSenceModel(model);

	if (!m_isManual) {
		ResetSenceEdit();
		if (model->m_curve.size() <= 0) {
			DoComputeCurve(model);
		}
	}
	else {
		m_editManuallyCurve->setModel(model);
		m_editManuallyCurve->setTree(vcg_model_trees[m_currentID]);

		CustomInteractor* interactor = pView->GetCustomInteractor();
		interactor->SetEdit(m_editManuallyCurve);

		// copy curve information if last step's model have.
		if (m_currentID >= 1 && model->m_curve.empty()) {
			std::cout << "compute next model curve...\n";

			// project curve of last step to current step
			AABBTree* tree = vcg_model_trees[m_currentID];
			float maxDist = std::numeric_limits<float>::max();
			vcg::EmptyClass objectMarker;
			vcg::RayTriangleIntersectionFunctor<false> rayIntersector;

			int lastModelIndex = m_currentID - 1;
			sn3DMeshModel* lastModel = m_models.GetModel(lastModelIndex);

			model->m_intersectedPoints.clear();
			for (int i = 0; i < lastModel->m_curve.size(); ++i) {
				Point3f p	= lastModel->m_curve[i];
				Point3f dir = lastModel->m_cutDir[i];
				vcg::Point3f pointToProj(p[0], p[1], p[2]);
				vcg::Point3f projDir(-dir[0], -dir[1], -dir[2]);
				pointToProj = pointToProj - projDir * 10;
				vcg::Ray3<float, false> rayInverse(pointToProj, projDir);
				float rayTInverse = std::numeric_limits<float>::max();
				CFaceO* isectFaceInverse = NULL;
				isectFaceInverse = tree->DoRay(rayIntersector, objectMarker, rayInverse, maxDist, rayTInverse);

				if (isectFaceInverse) {
					pointToProj = pointToProj + projDir * rayTInverse;
				}

				p[0] = pointToProj[0];
				p[1] = pointToProj[1];
				p[2] = pointToProj[2];

				model->m_curve.push_back(p);
				model->m_cutDir.push_back(dir);

				//update intersection points
				vcg::Point3f detectIntersectedDir(model->m_cutDir[i][0], model->m_cutDir[i][1], model->m_cutDir[i][2]);
				pointToProj = pointToProj + detectIntersectedDir * 0.1;
				vcg::Ray3<float, false> ray(pointToProj, detectIntersectedDir);
				float rayT = std::numeric_limits<float>::max();
				CFaceO* isectFace = NULL;
				isectFace = tree->DoRay(rayIntersector, objectMarker, ray, maxDist, rayT);
				if (isectFace) {
					std::cout << "have intersection \n";
					vcg::Point3f intersectedP = pointToProj + detectIntersectedDir * rayT;
					model->m_intersectedPoints.push_back(Point3f(intersectedP.X(), intersectedP.Y(), intersectedP.Z()));
				}
			}

			model->m_ctrlPoints.clear();//just in case that control points vector is not empty
			for (int i = 0; i < lastModel->m_ctrlPoints.size(); ++i) {
				int nearestIndex = -1;
				double nearestDis = std::numeric_limits<double>::max();
				for (int j = 0; j < lastModel->m_curve.size(); ++j) {
					double dis = Distance(lastModel->m_curve[j], lastModel->m_ctrlPoints[i]);
					if (dis < nearestDis) {
						nearestDis = dis;
						nearestIndex = j;
					}
				}
				model->m_ctrlPoints.push_back(model->m_curve[nearestIndex]);
			}
		}

		// project laser points
		if (m_currentID >= 1 && model->m_markPoints.empty()) {
			AABBTree* tree = vcg_model_trees[m_currentID];
			float maxDist = std::numeric_limits<float>::max();
			vcg::EmptyClass objectMarker;
			vcg::RayTriangleIntersectionFunctor<false> rayIntersector;

			int lastModelIndex = m_currentID - 1;
			sn3DMeshModel* lastModel = m_models.GetModel(lastModelIndex);

			for (int i = 0; i < lastModel->m_markPoints.size(); ++i) {
				Point3f p = lastModel->m_markPoints[i];
				Point3f dir(0.0, 0.0, -1.0);
				vcg::Point3f pointToProj(p[0], p[1], p[2]);
				vcg::Point3f projDir(dir[0], dir[1], dir[2]);
				pointToProj = pointToProj - projDir * 10;
				vcg::Ray3<float, false> ray(pointToProj, projDir);
				float rayT = std::numeric_limits<float>::max();
				CFaceO* isectFaceInverse = NULL;
				isectFaceInverse = tree->DoRay(rayIntersector, objectMarker, ray, maxDist, rayT);

				if (isectFaceInverse) {
					pointToProj = pointToProj + projDir * rayT;
				}

				p[0] = pointToProj[0];
				p[1] = pointToProj[1];
				p[2] = pointToProj[2];

				model->m_markPoints.push_back(p);
			}
		}


		//model->m_buccalPointsIndex.clear();
		//model->m_lingualPointsBackIndex.clear();
		//model->m_lingualPointsFrontIndex.clear();

		//copy cut degree
		if ( m_currentID >= 1 && model->m_buccalPointsIndex.empty() ) {
			int lastModelIndex = m_currentID - 1;
			sn3DMeshModel* lastModel = m_models.GetModel(lastModelIndex);

			model->m_buccalPointsIndex			= lastModel->m_buccalPointsIndex;
			model->m_lingualPointsBackIndex		= lastModel->m_lingualPointsBackIndex;
			model->m_lingualPointsFrontIndex	= lastModel->m_lingualPointsFrontIndex;
		}
	}

	QString message;
	message = QString("总共") + QString::number(m_models.Count()) + QString("个模型，当前是") + QString::number(m_currentID + 1) + QString("个模型。");
	if (m_currentID == 0)
		message += QString("下一个模型向右按钮。");
	else if (m_currentID + 1 >= m_models.Count())
		message += QString("上一个模型向左按钮。");
	else
		message += QString("上一个模型向左按钮，下一个模型向右按钮。");

	if (m_isManual) {
		message += QString("手动选点模式。【左键单击】增加点 【左键单击控制点并拖拽】改变控制点的位置 【右键单击】选择并删除控制点");
	}

	ShowMessage(message);
	SetTileFileName(model->GetName(), m_models.Count(), m_currentID + 1);

	pView->viewport()->update();
}
void OrthoCurve::OnUp()
{
	pView->GetMainScene()->SetViewMode(Common::VIEWPOINT_TOP);
	pView->viewport()->update();
}
void OrthoCurve::OnDown()
{
	pView->GetMainScene()->SetViewMode(Common::VIEWPOINT_BOTTOM);
	pView->viewport()->update();
}
void OrthoCurve::OnLeft()
{
	pView->GetMainScene()->SetViewMode(Common::VIEWPOINT_LEFT);
	pView->viewport()->update();
}
void OrthoCurve::OnRight()
{
	pView->GetMainScene()->SetViewMode(Common::VIEWPOINT_RIGHT);
	pView->viewport()->update();
}
void OrthoCurve::OnFront()
{
	pView->GetMainScene()->SetViewMode(Common::VIEWPOINT_FRONT);
	pView->viewport()->update();
}
void OrthoCurve::OnBack()
{
	pView->GetMainScene()->SetViewMode(Common::VIEWPOINT_BACK);
	pView->viewport()->update();
}
void OrthoCurve::OnAdjustCurveConnect()
{
	sn3DMeshModel* model = (sn3DMeshModel*)pView->GetMainScene()->GetObject(0);
	if (model == 0)
		return;

	QString message;
	CustomInteractor* interactor = pView->GetCustomInteractor();
	if (!m_isManual) { // 根据手动还是自动生成牙龈线选择交互模式
			//防止内存泄露
		if (m_editCurve)
			delete m_editCurve;
		m_editCurve = new EditDrawCurve(model, m_normalZ);
		m_editCurve->SetInterval(m_interval);
		interactor->SetEdit(m_editCurve);
		message = QString("在曲线上【左键双击】选择起点，在牙龈周围【左键单击】生成调整曲线，曲线上【左键双击】结束。编辑过程中【右键】旋转模型。");
	}
	ShowMessage(message);
}
void OrthoCurve::OnRemoveLastPoint()
{
	sn3DMeshModel* model = (sn3DMeshModel*)pView->GetMainScene()->GetObject(0);
	if (model == 0)
		return;
	CustomInteractor* interactor = pView->GetCustomInteractor();
	EditInteraction* edit = interactor->GetEdit();

	if (m_isManual) {
		EditManuallyDrawCurve* editManualCurve = dynamic_cast<EditManuallyDrawCurve*>(edit);
		editManualCurve->RemoveLastPoint();
	}
	else {
		EditDrawCurve* editCurve = dynamic_cast<EditDrawCurve*>(edit);
		editCurve->RemoveLastPoint();
	}

	if (edit) {
		pView->viewport()->update();
	}
}
void OrthoCurve::OnMarkIcon()
{
	sn3DMeshModel* model = (sn3DMeshModel*)pView->GetMainScene()->GetObject(0);
	if (model == 0)
		return;

	//防止内存泄露
	if (m_editMark)
		delete m_editMark;
	m_editMark = new EditMarkIcon(model);

	CustomInteractor* interactor = pView->GetCustomInteractor();
	interactor->SetEdit(m_editMark);

	QString message;
	message = QString("在模型上【左键双击】添加标签位置，在标签位置【左键双击】删除该标签位置。");
	ShowMessage(message);
}
void OrthoCurve::OnComputeIntersections()
{
	std::cout << __LINE__ << ":  " << __FUNCTION__ << " " << "Compute Intersections \n";
	for (int index = 0; index < m_models.Count(); ++index) {
		sn3DMeshModel* model = m_models.GetModel(index);
	}
}
QToolButton* OrthoCurve::createToolButton(const QString& toolTip, const QIcon& icon, const char* member)
{
	QToolButton* button = new QToolButton(this);
	button->setToolTip(toolTip);
	button->setIcon(icon);
	button->setIconSize(QSize(32, 32));
	connect(button, SIGNAL(clicked()), this, member);
	return button;
}

int OrthoCurve::updateButtonGeometry(QToolButton* button, int x, int y)
{
	QSize size = button->sizeHint();
	button->setGeometry(x, y,
		size.rwidth(), size.rheight());

	return x + size.rwidth() + style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);
}

int OrthoCurve::updateViewSideButtonGeometry(QToolButton* button, int x, int y)
{
	QSize size = button->sizeHint();
	button->setGeometry(x, y, size.rwidth(), size.rheight());
	return y + size.rheight() + style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);
}

void OrthoCurve::resizeEvent(QResizeEvent* /* event */)
{
	int margin = style()->pixelMetric(QStyle::PM_DefaultTopLevelMargin);
	int x = margin;
	int y = margin + 65;

	x = updateButtonGeometry(prevButton, x, y);
	x = updateButtonGeometry(nextButton, x, y);

	//align view side buttons
	margin = style()->pixelMetric(QStyle::PM_LayoutRightMargin);
	x = this->size().width() - margin - 60;
	for (int i = 0; i < viewSideButtons.size(); ++i) {
		y = updateViewSideButtonGeometry(viewSideButtons[i], x,y);
	}
}
