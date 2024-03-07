#include "orthocurve.h"
#include"CustomGLWidget.h"
#include <QFileDialog>
#include <QStyle>
#include <QToolButton>
#include <QToolTip>
#include <QSettings>  //读写ini文件。
#include <QDebug>
#include <QMenu>
#include <QInputDialog>

#include <qmessagebox.h>
#include "ComputeCurve.h"
#include "SettingWidget.h"
#include "EditDrawCurve.h"
#include "EditMarkIcon.h"

#include "mcut/mcut.h"
#include <vcg/complex/algorithms/hole.h>

using namespace Common;
QString g_version = QString("V3.1.0"); //双模版
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

	setWindowTitle(tr("正畸曲线V3.01（双模切割测试版）"));
	ui.mainToolBar->setStyleSheet("background-color: rgba(255, 255, 255, 255);");
	ui.mainToolBar->setIconSize(QSize(40, 40));

	// add context menu
	QAction* computeIntersection = new QAction(this);
	QMenu pop_menu;
	pop_menu.addAction(computeIntersection);
	//menu.addAction(QIcon("1.png"), QStringLiteral("添加"), this, SLOT(OnBscGroupRightAction()));
	//menu.exec(QCursor::pos());

	connect(ui.actionImportSTL, &QAction::triggered, this, &OrthoCurve::OnImportSTLs);
	connect(ui.actionComputeCurve, &QAction::triggered, this, &OrthoCurve::OnComputeCurve);
	connect(ui.actionSaveCurve, &QAction::triggered, this, &OrthoCurve::OnSaveCurve);
	connect(ui.actionSetting, &QAction::triggered, this, &OrthoCurve::OnSetting);
	connect(ui.actionDeleteCurrentCurve, &QAction::triggered, this, &OrthoCurve::OnDeleteCurrentCurve);
	//connect(ui.actionAdjustDegree, &QAction::triggered, this, &OrthoCurve::OnAdjustDegree);
	connect(ui.actionMarkIcon, &QAction::triggered, this, &OrthoCurve::OnMarkIcon);
	connect(computeIntersection, &QAction::triggered, this,&OrthoCurve::OnComputeIntersections);
	connect(ui.action_3shape, &QAction::triggered, this, &OrthoCurve::OnImport3shapeData); 
	connect(ui.action_alignModel, &QAction::triggered, this, &OrthoCurve::OnAlignModel); 
	connect(ui.action_pickAlignPoints, &QAction::triggered, this, &OrthoCurve::OnPickAlignPoints);

	ShowMessage("请导入模型");
	LoadSetting();
	m_editCurve = NULL;
	m_editMark  = NULL;
	m_editManuallyCurve     = NULL;
	m_editPickAlignPoints   = NULL;

	projectCurveProgressDialog = new QProgressDialog(this); 
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
	pView   = CreateOpenGLView();
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
#ifdef IS_TWIN_MODEL
	int button_num = 7;
    #else
	int button_num = 6;
#endif

	viewSideButtons.resize(button_num);
	viewSideButtons[0] = createToolButton(tr("上视图"), QIcon(appPath + "/Resources/up.png"),		SLOT(OnUp()));
	viewSideButtons[1] = createToolButton(tr("下视图"), QIcon(appPath + "/Resources/down.png"),		SLOT(OnDown()));
	viewSideButtons[2] = createToolButton(tr("左视图"), QIcon(appPath + "/Resources/left.png"),		SLOT(OnLeft()));
	viewSideButtons[3] = createToolButton(tr("右视图"), QIcon(appPath + "/Resources/right.png"),		SLOT(OnRight()));
	viewSideButtons[4] = createToolButton(tr("前视图"), QIcon(appPath + "/Resources/front.png"),		SLOT(OnFront()));
	viewSideButtons[5] = createToolButton(tr("后视图"), QIcon(appPath + "/Resources/back.png"),		SLOT(OnBack()));

#ifdef  IS_TWIN_MODEL
		viewSideButtons[6] = createToolButton(tr("显示双牙模压膜俯视图"), QIcon(appPath + "/Resources/thermoBase.png"), SLOT(OnShowTwinModel()));
#endif

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

	//pScene->SetMainScene(pView->GetMainScene());
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
	if (m_editMark){
		delete m_editMark;
		m_editMark = NULL;
	}
	if (m_editCurve){
		delete m_editCurve;
		m_editCurve = NULL;
	}
	if (m_editManuallyCurve) {
		delete m_editManuallyCurve;
		m_editManuallyCurve = nullptr;
	}
	if (m_editPickAlignPoints) {
		delete m_editPickAlignPoints;
		m_editPickAlignPoints = nullptr;
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
	m_isManual      = true; // 不受控制文件影响
	m_savePath		= configIni->value("path/savePath", false).toString();
	m_importPath	= configIni->value("path/importPath", false).toString();
	m_degreefb      = configIni->value("curve/degreefb", 45).toInt();
	m_cutDegreeOut  = configIni->value("curve/cutDegreeOut", 75).toInt();
	m_cutDegreeInsideFront = configIni->value("curve/cutDegreeInsideFront", 45).toInt();
	m_cutDegreeInsideBack  = configIni->value("curve/cutDegreeInsideBack", 60).toInt();
	m_showBur       = configIni->value("curve/show", false).toBool();
	m_cutOffset     = configIni->value("curve/cutOffset", 0.5).toFloat();
	m_curveture     = configIni->value("curve/curveture", 2).toInt();

	m_need_qr       = configIni->value("base/need_qr", true).toBool();
	m_fixture       = configIni->value("base/fixture", 0).toInt();

#ifdef IS_TWIN_MODEL
	if (m_fixture != 3 && m_fixture != 4) {
		m_fixture = 3;
	}	
#endif

	m_qr_correction = configIni->value("base/qr_correction", 0).toInt();
	m_qr_width      = configIni->value("base/qr_width", 12.0).toFloat();

	delete configIni;
}
void OrthoCurve::SaveSetting()
{
	QSettings* configIni = new QSettings("config.ini", QSettings::IniFormat);
	configIni->setValue("curve/normalZ",    m_normalZ);
	configIni->setValue("curve/interval",   m_interval);
	configIni->setValue("curve/smooth",     m_smooth);
	configIni->setValue("curve/offsetSetp", m_offsetSetp);
	configIni->setValue("curve/saveNormal", m_saveNormal);
	configIni->setValue("curve/withMark",   m_withMark);
	configIni->setValue("model/thresholdZ", m_thresholdZ);
	configIni->setValue("model/isManual",   true);
	configIni->setValue("path/importPath",  m_importPath);

	configIni->setValue("curve/degreefb", m_degreefb);
	configIni->setValue("curve/cutDegreeOut", m_cutDegreeOut);
	configIni->setValue("curve/cutDegreeInsideFront", m_cutDegreeInsideFront);
	configIni->setValue("curve/cutDegreeInsideBack", m_cutDegreeInsideBack);

	configIni->setValue("curve/show", m_showBur);
	configIni->setValue("curve/cutOffset", m_cutOffset);
	configIni->setValue("curve/curveture", m_curveture);

	configIni->setValue("base/need_qr", m_need_qr);
	configIni->setValue("base/fixture", m_fixture);
	configIni->setValue("base/qr_correction", m_qr_correction);
	configIni->setValue("base/qr_width", m_qr_width);

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
	QFileInfo outputDir(m_importPath);
	QStringList filesPath = QFileDialog::getOpenFileNames(this, tr("STL File"), m_importPath, tr("STL File (*.stl)"));
	if (filesPath.size() > 0) {
		outputDir.setFile(filesPath[0]);
		if (outputDir.exists()) {
			m_importPath = outputDir.dir().path();
			//qDebug() <<__LINE__ << "  Model file path " <<  m_importPath;
		}
		SaveSetting();
		is_3shape = false;
	}
	else {
		return;
	}

	projectCurveProgressDialog->setLabelText("正在导入模型...");
	projectCurveProgressDialog->show();
	projectCurveProgressDialog->setValue(1);

	pView->GetMainScene()->ClearScene();
	ResetSenceEdit();
	releaseAllModels();

	//读入所有模型
	int load_mask = 0;
	for (int file_index = 0; file_index < filesPath.size(); file_index++)
	{
		QString model_path = filesPath[file_index];
		std::string model_path_str = model_path.toLocal8Bit().constData();

		CMeshO* current_mesh = new CMeshO();
		int result = vcg::tri::io::ImporterSTL<CMeshO>::Open(*current_mesh, model_path_str.c_str(), load_mask);
		vcg::tri::Clean<CMeshO>::RemoveDuplicateVertex(*current_mesh);
		vcg::tri::Allocator<CMeshO>::CompactEveryVector(*current_mesh);

		//save to VCG models
		_VCGModel* one_model = new _VCGModel();
		one_model->vcg_teeth_mesh = current_mesh;
		one_model->path = model_path;
		VCG_models.push_back(one_model);

		projectCurveProgressDialog->setValue((file_index + 1) * 100.0 / filesPath.size());
	}

	if (!VCG_models.empty())
	{
		//对模型按步骤排序
		if (!VCG_models.empty()) {
			std::sort(VCG_models.begin(), VCG_models.end(), [](_VCGModel* a, _VCGModel* b) {
				return a->step < b->step;
				});
		}

		if (filesPath[0].split('/').size() >= 2) {
			patient_id = filesPath[0].split('/').at(filesPath[0].split('/').size() - 2);
		}
		changeFileName(patient_id);

		for (_VCGModel* current_model: VCG_models) {
			CMeshO* current_mesh = current_model->vcg_teeth_mesh;
			QString model_path   = current_model->path;

			//读取模型到sn3里面做显示
			sn3DMeshModel* model = new sn3DMeshModel();
			sn3DMeshData* model_data = model->AddMesh();

			QString model_name = model_path.split('/').back();
			model_name = model_name.mid(0, model_name.lastIndexOf('.'));
			std::string model_name_str = model_name.toLocal8Bit().constData();
			model->SetName(model_name_str);

			for (CMeshO::VertexIterator vit = current_mesh->vert.begin(); vit != current_mesh->vert.end(); ++vit) {
				model_data->AddVertex(Point3f(vit->P().X(), vit->P().Y(), vit->P().Z()));
			}
			CMeshO::VertexIterator first_vit = current_mesh->vert.begin();
			for (CMeshO::FaceIterator fit = current_mesh->face.begin(); fit != current_mesh->face.end(); ++fit) {
				model_data->AddFace(fit->V(0) - &*first_vit, fit->V(1) - &*first_vit, fit->V(2) - &*first_vit);
			}

			m_models.AddModel(model);
		}

		QString message;
		message = QString("成功导入") + QString::number(m_models.Count()) + QString("个模型");
		ShowMessage(message);

		//导入默认交互器
		sn3DMeshModel* model = m_models.GetModel(0);
		SetSenceModel(model);
		if (!m_editCurve) {
			m_editCurve = new EditDrawCurve(model, 0.f);
		}

		CustomInteractor* interactor = pView->GetCustomInteractor();
		interactor->SetEdit(m_editCurve);
	}
	else {
		QMessageBox box;
		box.setText(tr("导入失败"));
		box.exec();
	}		
}
void OrthoCurve::OnImport3shapeData()
{
	_3shape_models_input_path = QFileDialog::getExistingDirectory(nullptr, QString("Select 3Shape Data Directory..."), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (_3shape_models_input_path.isNull() || _3shape_models_input_path.isEmpty()) {
		return;
	}

	is_3shape   = true;
	current_jaw = 0;

    patient_id = _3shape_models_input_path.split('/').back(); //QApplication::translate("Dialog", "\347\227\205\344\276\213id", Q_NULLPTR)
	QString input_patient_id_label = QApplication::translate("Dialog", "\347\227\205\344\276\213id", Q_NULLPTR);
	patient_id = QInputDialog::getText(nullptr, "Input", input_patient_id_label, QLineEdit::Normal, patient_id);

	read3shapeModels(); //读入上颌模型
	
	//显示读入的数据
	if (m_models.Count() > 0)
	{
		QMessageBox box;
		box.setText(tr("导入成功"));
		box.exec();

		QString message;
		message = QString("成功导入") + QString::number(m_models.Count()) + QString("个模型");
		ShowMessage(message);

		//导入默认交互器
		sn3DMeshModel* model = m_models.GetModel(0);
		SetSenceModel(model);
		if (!m_editCurve) {
			m_editCurve = new EditDrawCurve(model, 0.f);
		}

		CustomInteractor* interactor = pView->GetCustomInteractor();
		interactor->SetEdit(m_editCurve);

		pView->viewport()->update();
	}
}

void OrthoCurve::OnComputeCurve()
{
	int num = pView->GetMainScene()->GetObjectCount();
	if (num == 0)
		return;

	if (!m_editManuallyCurve) {
		m_editManuallyCurve = new EditManuallyDrawCurve(nullptr, 0.f);
		m_editManuallyCurve->cutDegreeOut			= &m_cutDegreeOut;
		m_editManuallyCurve->cutDegreeInsideBack	= &m_cutDegreeInsideBack;
		m_editManuallyCurve->cutDegreeInsideFront	= &m_cutDegreeInsideFront;
		m_editManuallyCurve->showBur		= &m_showBur;
		m_editManuallyCurve->cutOffset		= &m_cutOffset;
		m_editManuallyCurve->m_interval		= &m_interval;
	}

	projectCurveProgressDialog->setLabelText("正在计算牙龈线...");
	projectCurveProgressDialog->show();
	projectCurveProgressDialog->setValue(1);
	bool have_show_error = false;
	for (int i = 0; i < VCG_models.size(); ++i) {
		if (!is_3shape) {
			bool result = computeOneCutline(i);
			if (!result && !have_show_error) {
				int ret = QMessageBox::information(nullptr, tr("Information"),
					QApplication::translate("", "\346\227\240\346\263\225\350\207\252\345\212\250\346\217\220\345\217\226\345\210"
						"\207\345\211\262\347\272\277\357\274\214\350\257\267\346\211\213\345\212\250\347\202\271\351\200\211", Q_NULLPTR), QMessageBox::Ok);
				have_show_error = true;

			}
		}
		else {
			computeOneCutline3Shape(i);
		}
		projectCurveProgressDialog->setValue((i + 1) * 100.0 / VCG_models.size());
	}

	//切换交互状态
	CustomInteractor* interactor = pView->GetCustomInteractor();
	interactor->SetEdit(m_editManuallyCurve);

	m_editManuallyCurve->setModel(m_models.GetModel(m_currentID));
	m_editManuallyCurve->setTree(VCG_models.at(m_currentID)->vcg_tree);

	pView->viewport()->update();
}
void OrthoCurve::OnAdjustDegree()
{
	std::cout << __LINE__ << ":  "<<__FUNCTION__ << " " << "Adjust Degree \n";
	for (int index = 0; index < m_models.Count(); ++index) {
		sn3DMeshModel* model = m_models.GetModel(index);
		AABBTree* tree = VCG_models.at(index)->vcg_tree;
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

		QString file_name  = file_path.split('/').back();
		QString model_path = file_path.mid(0, file_path.lastIndexOf('/')) + "/Models-" + patient_id;
		QString curve_path = file_path.mid(0, file_path.lastIndexOf('/')) + "/Curves-" + patient_id + "/" + file_name;

		if (!QDir(model_path).exists()) {
			QDir().mkdir(model_path);
		}
		SaveModels(model_path);

		QMessageBox box;
		if (m_models.SaveAllCurves(curve_path, m_withMark, m_saveNormal, m_isManual))
			box.setText(tr("保存成功"));
		else
			box.setText(tr("保存失败"));
		box.exec();
	}

	if (is_3shape && current_jaw == 0) {
		current_jaw = 1;

		read3shapeModels();// 读入下颌
		//显示读入的数据
		if (m_models.Count() > 0)
		{
			QMessageBox box;
			box.setText(tr("导入成功"));
			box.exec();

			QString message;
			message = QString("成功导入") + QString::number(m_models.Count()) + QString("个模型");
			ShowMessage(message);

			//导入默认交互器
			sn3DMeshModel* model = m_models.GetModel(0);
			SetSenceModel(model);
			if (!m_editCurve) {
				m_editCurve = new EditDrawCurve(model, 0.f);
			}

			CustomInteractor* interactor = pView->GetCustomInteractor();
			interactor->SetEdit(m_editCurve);

			pView->viewport()->update();
		}
	}
	else if(is_3shape && current_jaw == 1) {
		current_jaw = 0;//等待新病例数据的导入
	}
}
void OrthoCurve::OnSetting()
{
	SettingDialog* dialog = new SettingDialog(NULL);
	LoadSetting();//从文件中读取数据

	dialog->SetVersion(g_version);
	dialog->SetValue(m_normalZ, m_offsetSetp, m_interval, m_smooth, m_saveNormal, m_withMark, 
		m_thresholdZ, m_isManual, m_showBur, m_cutOffset, m_curveture);//将文件中数据显示在对话框中
	dialog->SetDegreeValue(m_degreefb, m_cutDegreeOut, m_cutDegreeInsideFront, m_cutDegreeInsideBack);
	dialog->SetFixtureValue(m_need_qr, m_fixture, m_qr_correction, m_qr_width);

	//disable 不需要的界面
	dialog->ui.OffsetStepBox->setEnabled(false);
	dialog->ui.intevalBox->setEnabled(false);
	dialog->ui.smoothBox->setEnabled(false);
	dialog->ui.zangleBox->setEnabled(false);
	dialog->ui.zthresholdBox->setEnabled(false);

#ifdef IS_TWIN_MODEL
	dialog->ui.radioButton_Robotic->setCheckable(false);
	dialog->ui.radioButton_5or4Axis->setCheckable(false);
	dialog->ui.radioButton_5or4Axis_2->setCheckable(false);
#endif // IS_TWIN_MODEL

	int n = dialog->exec();
	if (n == QDialog::Accepted)
	{
		dialog->GetValue(m_normalZ, m_offsetSetp, m_interval, m_smooth, 
			m_saveNormal, m_withMark, m_thresholdZ, 
			m_isManual, m_showBur, m_cutOffset, m_curveture);
		dialog->GetDegreeValue(m_degreefb, m_cutDegreeOut, m_cutDegreeInsideFront, m_cutDegreeInsideBack);
		dialog->GetFixtureValue(m_need_qr, m_fixture, m_qr_correction, m_qr_width);

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

	if (m_editManuallyCurve) {
		m_editManuallyCurve->setModel(model);
		m_editManuallyCurve->setTree(VCG_models.at(m_currentID)->vcg_tree);
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
	//SetTileFileName(model->GetName(), m_models.Count(), m_currentID + 1);

	pView->viewport()->update();
}
void OrthoCurve::OnNext()
{
	if (m_currentID + 1 >= m_models.Count())
		return;

	m_currentID++;
	sn3DMeshModel* model = m_models.GetModel(m_currentID);
	SetSenceModel(model);
	
	//只有在手动模式下，且已经计算AABB tree 的情况下再考虑牙龈线事宜
	if (m_editManuallyCurve) {
		m_editManuallyCurve->setModel(model);
		if (VCG_models.at(m_currentID)->vcg_tree) {
			m_editManuallyCurve->setTree(VCG_models.at(m_currentID)->vcg_tree);

			// copy curve information if last step's model have.
			if (m_currentID >= 1 && model->m_curve.empty()) {
				std::cout << "compute next model curve...\n";

				// project curve of last step to current step
				AABBTree* tree = VCG_models.at(m_currentID)->vcg_tree;
				float maxDist = std::numeric_limits<float>::max();
				vcg::EmptyClass objectMarker;
				vcg::RayTriangleIntersectionFunctor<false> rayIntersector;

				int lastModelIndex = m_currentID - 1;
				sn3DMeshModel* lastModel = m_models.GetModel(lastModelIndex);

				model->m_intersectedPoints.clear();
				for (int i = 0; i < lastModel->m_curve.size(); ++i) {
					Point3f p = lastModel->m_curve[i];
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
				AABBTree* tree = VCG_models.at(m_currentID)->vcg_tree;
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

			//copy cut degree
			if (m_currentID >= 1 && model->m_buccalPointsIndex.empty()) {
				int lastModelIndex = m_currentID - 1;
				sn3DMeshModel* lastModel = m_models.GetModel(lastModelIndex);

				if (!lastModel->m_buccalPointsIndex.empty()) {
					model->m_buccalPointsIndex = lastModel->m_buccalPointsIndex;
					model->m_lingualPointsBackIndex = lastModel->m_lingualPointsBackIndex;
					model->m_lingualPointsFrontIndex = lastModel->m_lingualPointsFrontIndex;
				}
			}
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
	//SetTileFileName(model->GetName(), m_models.Count(), m_currentID + 1);

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

void OrthoCurve::OnShowTwinModel()
{
	if (twin_model_dialog == nullptr) {
		twin_model_dialog = new QDialog();
		twin_model_ui.setupUi(twin_model_dialog);

		twin_model_widget = new ThermoBaseDrawWidget(twin_model_dialog);
		twin_model_widget->setGeometry(QRect(20, 20, 600, 600));

		connect(twin_model_ui.radioButton_left,  &QRadioButton::clicked, this, &OrthoCurve::OnChangeModel);
		connect(twin_model_ui.radioButton_right, &QRadioButton::clicked, this, &OrthoCurve::OnChangeModel);

		connect(twin_model_ui.doubleSpinBox_X, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &OrthoCurve::OnTransXorY);
		connect(twin_model_ui.doubleSpinBox_Y, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &OrthoCurve::OnTransXorY);
		connect(twin_model_ui.doubleSpinBox_Z, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &OrthoCurve::OnRotateZ);

		connect(twin_model_ui.pushButton_save, &QPushButton::clicked, this, [&]() {
			if (twin_model_ui.radioButton_left->isChecked()) {
				twin_model_widget->trans_A_origin.setX(twin_model_widget->trans_A_origin.x() + twin_model_ui.doubleSpinBox_X->value());
				twin_model_widget->trans_A_origin.setY(twin_model_widget->trans_A_origin.y() + twin_model_ui.doubleSpinBox_Y->value());
				twin_model_widget->rotate_A_origin = twin_model_ui.doubleSpinBox_Z->value() + twin_model_widget->rotate_A_origin;
			}
			else if (twin_model_ui.radioButton_right->isChecked()) {
				twin_model_widget->trans_B_origin.setX(twin_model_widget->trans_B_origin.x() + twin_model_ui.doubleSpinBox_X->value());
				twin_model_widget->trans_B_origin.setY(twin_model_widget->trans_B_origin.y() + twin_model_ui.doubleSpinBox_Y->value());
				twin_model_widget->rotate_B_origin = twin_model_ui.doubleSpinBox_Z->value() + twin_model_widget->rotate_B_origin;
			}

			twin_model_ui.doubleSpinBox_X->setValue(0.0);
			twin_model_ui.doubleSpinBox_Y->setValue(0.0);
			twin_model_ui.doubleSpinBox_Z->setValue(0.0);
			});

	}

	//load data
	if (twin_model_widget->A.empty() && !VCG_models.empty()) {
		vcg::Matrix44f transA, transB;
		vcg::Matrix44f trans_mat_1, trans_mat_2, trans_mat_3;
		//trans_mat_1.SetTranslate(vcg::Point3f(-7.9, 0.0, 0.0));
		trans_mat_2.SetRotateDeg(90, vcg::Point3f(0.0, 0.0, 1.0));
		trans_mat_3.SetTranslate(vcg::Point3f(11.99, 27.115, 0.0));
		transA = trans_mat_3 * trans_mat_2;

		trans_mat_2.SetRotateDeg(-90, vcg::Point3f(0.0, 0.0, 1.0));
		trans_mat_3.SetTranslate(vcg::Point3f(-11.99, -27.115, 0.0));
		transB = trans_mat_3 * trans_mat_2;

		_VCGModel* current_model = VCG_models.front();
		if (current_model->vcg_teeth_mesh) {
			for (CMeshO::VertexIterator vit = current_model->vcg_teeth_mesh->vert.begin(); vit != current_model->vcg_teeth_mesh->vert.end(); ++vit) {
				vcg::Point3f pA = transA * vit->P() ;
				twin_model_widget->A.push_back(QPoint(-pA.Y(), -pA.X()));
				twin_model_widget->center_A += twin_model_widget->A.back();

				vcg::Point3f pB = transB * vit->P();
				twin_model_widget->B.push_back(QPoint(-pB.Y(), -pB.X()));
				twin_model_widget->center_B += twin_model_widget->B.back();
			}
		}

		if (current_model->vcg_gum_mesh) {
			for (CMeshO::VertexIterator vit = current_model->vcg_gum_mesh->vert.begin(); vit != current_model->vcg_gum_mesh->vert.end(); ++vit) {
				vcg::Point3f pA = transA * vit->P();
				twin_model_widget->A.push_back(QPoint(-pA.Y(), -pA.X()));
				twin_model_widget->center_A += twin_model_widget->A.back();

				vcg::Point3f pB = transB * vit->P();
				twin_model_widget->B.push_back(QPoint(-pB.Y(), -pB.X()));
				twin_model_widget->center_B += twin_model_widget->B.back();
			}
		}

		//load base
		QString base_pin_path = QCoreApplication::applicationDirPath() + "/models/base_points_V3.stl";
		std::string base_pin_path_str = base_pin_path.toLocal8Bit().constData();
		CMeshO* base_pin_mesh = new CMeshO();
		int load_mask_temp;
		int open_result = vcg::tri::io::ImporterSTL<CMeshO>::Open(*base_pin_mesh, base_pin_path_str.c_str(), load_mask_temp);
		vcg::tri::Clean<CMeshO>::RemoveDuplicateVertex(*base_pin_mesh);
		vcg::tri::Allocator<CMeshO>::CompactEveryVector(*base_pin_mesh);
		for (CMeshO::VertexIterator vit = base_pin_mesh->vert.begin(); vit != base_pin_mesh->vert.end(); ++vit) {
			vcg::Point3f pA = transA * vit->P();
			twin_model_widget->fixture_A.push_back(QPoint(-pA.Y(), -pA.X()));

			vcg::Point3f pB = transB * vit->P();
			twin_model_widget->fixture_B.push_back(QPoint(-pB.Y(), -pB.X()));
		}
		delete base_pin_mesh;

		//set mass center of A
		twin_model_widget->center_A /= twin_model_widget->A.size();
		twin_model_widget->center_B /= twin_model_widget->B.size();
	}
	 
	twin_model_dialog->show();
}

void OrthoCurve::OnChangeModel()
{
	if (twin_model_ui.radioButton_left->isChecked()) {
		twin_model_widget->trans_B_origin.setX(twin_model_widget->trans_B_origin.x() + twin_model_ui.doubleSpinBox_X->value());
		twin_model_widget->trans_B_origin.setY(twin_model_widget->trans_B_origin.y() + twin_model_ui.doubleSpinBox_Y->value());
		twin_model_widget->rotate_B_origin = twin_model_ui.doubleSpinBox_Z->value()  + twin_model_widget->rotate_B_origin;
	}
	else if (twin_model_ui.radioButton_right->isChecked()) {
		twin_model_widget->trans_A_origin.setX(twin_model_widget->trans_A_origin.x() + twin_model_ui.doubleSpinBox_X->value());
		twin_model_widget->trans_A_origin.setY(twin_model_widget->trans_A_origin.y() + twin_model_ui.doubleSpinBox_Y->value());
		twin_model_widget->rotate_A_origin = twin_model_ui.doubleSpinBox_Z->value()  + twin_model_widget->rotate_A_origin;
	}

	twin_model_ui.doubleSpinBox_X->setValue(0.0);
	twin_model_ui.doubleSpinBox_Y->setValue(0.0);
	twin_model_ui.doubleSpinBox_Z->setValue(0.0);
}

void OrthoCurve::OnDeleteCurrentCurve()
{
	sn3DMeshModel* model = (sn3DMeshModel*)pView->GetMainScene()->GetObject(0);
	if (model == 0)
		return;

	model->m_ctrlPoints.clear();
	model->m_curve.clear();
	model->m_cutDir.clear();
	model->m_intersectedPoints.clear();

	pView->viewport()->update();
}

void OrthoCurve::OnTransXorY()
{
	if (twin_model_ui.radioButton_left->isChecked()) {
		twin_model_widget->trans_A.setX(twin_model_ui.doubleSpinBox_X->value() + twin_model_widget->trans_A_origin.x());
		twin_model_widget->trans_A.setY(twin_model_ui.doubleSpinBox_Y->value() + twin_model_widget->trans_A_origin.y());
	}
	else if (twin_model_ui.radioButton_right->isChecked()) {
		twin_model_widget->trans_B.setX(twin_model_ui.doubleSpinBox_X->value() + twin_model_widget->trans_B_origin.x());
		twin_model_widget->trans_B.setY(twin_model_ui.doubleSpinBox_Y->value() + twin_model_widget->trans_B_origin.y());
	}

	twin_model_widget->update();
}

void OrthoCurve::OnRotateZ()
{
	if (twin_model_ui.radioButton_left->isChecked()) {
		twin_model_widget->rotate_A = twin_model_ui.doubleSpinBox_Z->value() + twin_model_widget->rotate_A_origin;
	}
	else if (twin_model_ui.radioButton_right->isChecked()) {
		twin_model_widget->rotate_B = twin_model_ui.doubleSpinBox_Z->value() + twin_model_widget->rotate_B_origin;
	}

	twin_model_widget->update();
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

void OrthoCurve::computeOneCutline3Shape(const int models_id)
{
	CMeshO* gum_mesh			= VCG_models.at(models_id)->vcg_gum_mesh;
	AABBTree* current_tree		= nullptr;

	if (VCG_models.at(models_id)->vcg_tree) {
		current_tree = VCG_models.at(models_id)->vcg_tree;
	}
	else {
		CMeshO* current_mesh = new CMeshO();
		vcg::tri::Append<CMeshO, CMeshO>::Mesh(*current_mesh, *VCG_models.at(models_id)->vcg_teeth_mesh);
		vcg::tri::Append<CMeshO, CMeshO>::Mesh(*current_mesh, *VCG_models.at(models_id)->vcg_gum_mesh);

		current_tree = new AABBTree();
		current_tree->Set(current_mesh->face.begin(), current_mesh->face.end());

		VCG_models.at(models_id)->vcg_tree = current_tree;
	}

	sn3DMeshModel* current_data_model = m_models.GetModel(models_id);
	current_data_model->m_ctrlPoints.clear();
	current_data_model->m_curve.clear();
	current_data_model->m_cutDir.clear();

	//1.0 根据牙龈模型提取牙齿的边界线
	std::vector<CMeshO::VertexPointer> border_points;
	gum_mesh->vert.EnableVFAdjacency();
	gum_mesh->face.EnableVFAdjacency();
	gum_mesh->face.EnableFFAdjacency();
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(*gum_mesh);
	vcg::tri::UpdateTopology<CMeshO>::VertexFace(*gum_mesh);
	vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromNone(*gum_mesh);
	vcg::tri::UpdateFlags<CMeshO>::VertexBorderFromFaceBorder(*gum_mesh);
	for (CMeshO::VertexIterator vit = gum_mesh->vert.begin(); vit != gum_mesh->vert.end(); ++vit) {
		if (!vit->IsD() && vit->IsB()) {
			border_points.push_back(&(*vit));
		}
	}

	//1.1 将每颗牙齿的边界点分开存储
	for (CMeshO::VertexIterator vi = gum_mesh->vert.begin(); vi != gum_mesh->vert.end(); ++vi) {
		if (!(*vi).IsD()) {
			vi->ClearV();
		}
	}

	std::vector<std::vector<CMeshO::VertexPointer>>borders;
	while (!border_points.empty()) {
		CMeshO::VertexPointer current_vert = border_points.back();
		if (!current_vert->IsV()) {
			std::vector<CMeshO::VertexPointer> one_border;
			one_border.push_back(current_vert);
			current_vert->SetV();
			vcg::face::JumpingPos<CFaceO> first_pos(current_vert->VFp(), current_vert);
			vcg::face::JumpingPos<CFaceO> visit_pos(first_pos.F(), first_pos.V());

			CMeshO::VertexPointer visit_vert = first_pos.V();
			do {
				visit_pos.NextB();
				visit_vert = visit_pos.V();
				if (!visit_vert->IsV()) {
					one_border.push_back(visit_vert);
					visit_vert->SetV();
				}
				else {
					break;
				}
			} while (visit_pos != first_pos);

			borders.push_back(one_border);
		}
		else { //remove the vertex that have been visited 
			border_points.pop_back();
		}
	}

	//1.2 计算每条边界的中心点
	std::vector<vcg::Point3f> centers;
	for (std::vector<CMeshO::VertexPointer>& current_border : borders) {
		vcg::Point3f current_center(0.0, 0.0, 0.0);
		for (CMeshO::VertexPointer current_vp : current_border) {
			current_center += current_vp->P();
		}
		current_center /= current_border.size();
		centers.push_back(current_center);
	}

	std::vector<int> ordered_border_index;
	//find center with minimal X when Y > 0
	double min_X = std::numeric_limits<double>::max();
	int min_X_index = 0;
	for (int i = 0; i < centers.size(); ++i) {
		if (centers[i].Y() > 0 && centers[i].X() < min_X) {
			min_X = centers[i].X();
			min_X_index = i;
		}
	}
	ordered_border_index.push_back(min_X_index);

	//将每颗牙齿按照中心点的位置排序
	std::vector<int> unordered_border_index;
	for (int i = 0; i < centers.size(); ++i) {
		if (i != min_X_index) {
			unordered_border_index.push_back(i);
		}
	}

	while (!unordered_border_index.empty()) {
		int last_ordered_index = ordered_border_index.back();
		// find a unordered point nearest to end of ordered points 
		int nearest_index = 0;
		double min_distance = std::numeric_limits<double>::max();
		for (int i = 0; i < unordered_border_index.size(); ++i) {
			int unordered_index = unordered_border_index[i];
			double current_dis = vcg::Distance(centers[last_ordered_index], centers[unordered_index]);
			if (current_dis < min_distance) {
				min_distance = current_dis;
				nearest_index = i;
			}
		}
		ordered_border_index.push_back(unordered_border_index[nearest_index]);
		unordered_border_index.erase(unordered_border_index.begin() + nearest_index);
	}

	std::vector<std::vector<CMeshO::VertexPointer>>sorted_borders;
	std::vector<vcg::Point3f> sorted_centers;

	for (int i = 0; i < ordered_border_index.size(); ++i) {
		sorted_borders.push_back(borders[ordered_border_index[i]]);
		sorted_centers.push_back(centers[ordered_border_index[i]]);
	}
	borders.clear();
	centers.clear();

	// 将牙齿的一周分成36份（10度为一份）
	std::vector<std::vector<vcg::Point3f>> teeth_border_with_gap;//每个牙齿对应的最外面一圈的36个边界点
	std::vector<std::vector<bool>> teeth_border_flag; //标记是否36个点齐全

	teeth_border_with_gap.resize(sorted_borders.size());
	teeth_border_flag.resize(sorted_borders.size());

	//4.clean intersected points  ==> teeth_border_with_gap
	for (int i = 0; i < sorted_borders.size(); ++i) {
		vcg::Point3f center(0, 0, 0);
		for (int j = 0; j < sorted_borders[i].size(); ++j) {
			center += sorted_borders[i][j]->P();
		}
		center /= sorted_borders[i].size();

		teeth_border_with_gap[i].resize(36);
		teeth_border_flag[i].resize(36, false);

		vcg::Point3f x;
		if (i == sorted_borders.size() - 1) {
			x = sorted_centers.at(i) - sorted_centers.at(i - 1);
		}
		else {
			x = sorted_centers.at(i + 1) - sorted_centers.at(i);
		}
		x = x.Normalize();

		vcg::Point3f z(0.0, 0.0, 1.0);
		vcg::Plane3f xoy;
		xoy.Init(vcg::Point3f(0.0, 0.0, 0.0), z);
		vcg::Point3f proj_center = xoy.Projection(center);

		for (int j = 0; j < sorted_borders[i].size(); ++j) {
			//firstly, find the jth point's index in teeth_border_with_gap[i]
			vcg::Point3f proj_p = xoy.Projection(sorted_borders[i][j]->P());
			vcg::Point3f dir = (proj_p - proj_center).Normalize();
			float angle = std::acos(dir * x) / 3.1415926 * 180.0;
			if ((x ^ dir) * z < 0) {
				angle = 360 - angle;
			}

			int index = int(angle / 10) % 36;
			if (!teeth_border_flag[i][index]) {
				teeth_border_with_gap[i][index] = sorted_borders[i][j]->P();
				teeth_border_flag[i][index] = true;
			}
			else {
				vcg::Point3f proj_existed_border = xoy.Projection(teeth_border_with_gap[i][index]);
				if (vcg::Distance(proj_center, proj_p) > vcg::Distance(proj_center, proj_existed_border)) {
					teeth_border_with_gap[i][index] = sorted_borders[i][j]->P();
				}
			}
		}

		//检查是否有未赋值的点
		for (int j = 0; j < 36; ++j) {
			if (!teeth_border_flag[i][j]) {
				int last_index = (j == 0) ? 35 : j - 1;
				while (!teeth_border_flag[i][last_index]) {
					last_index = (last_index == 0) ? 35 : last_index - 1;
				}

				int next_index = (j == 35) ? 0 : j + 1;
				while (!teeth_border_flag[i][next_index]) {
					next_index = (next_index == 35) ? 0 : next_index + 1;
				}

				teeth_border_with_gap[i][j] = (teeth_border_with_gap[i][last_index] + teeth_border_with_gap[i][next_index]) * 0.5;
				teeth_border_flag[i][j] = true;
			}
		}
	}

	//5.remove gap. teeth_border_with_gap ==> teeth_border_lingal, teeth_border_buccal
	std::vector<vcg::Point3f> teeth_border_lingal, teeth_border_buccal;
	//buccal
	for (int i = 0; i < teeth_border_with_gap.size(); ++i) {
		teeth_border_buccal.push_back(teeth_border_with_gap[i][9]);

		if (i < teeth_border_with_gap.size() - 1) {
			vcg::Point3f i_p0 = teeth_border_with_gap[i][0];
			vcg::Point3f i_plus_1_p18 = teeth_border_with_gap[i + 1][18];
			vcg::Point3f mid_up = (i_p0 + i_plus_1_p18) * 0.5;

			vcg::Point3f i_p9 = teeth_border_with_gap[i][9];
			vcg::Point3f i_plus_1_p9 = teeth_border_with_gap[i + 1][9];
			vcg::Point3f mid_bottom = (i_p9 + i_plus_1_p9) * 0.5;

			double height = (mid_up - mid_bottom).Norm();
			vcg::Point3f mid_dir = (mid_up - mid_bottom).Normalize();
			vcg::Point3f p_mid = mid_bottom + mid_dir * height * m_curveture * 0.125;

			teeth_border_buccal.push_back(p_mid);
		}
	}

	//lingual
	for (int i = teeth_border_with_gap.size() - 1; i >= 0; i--) {
		teeth_border_lingal.push_back(teeth_border_with_gap[i][27]);

		if (i > 0) {
			vcg::Point3f i_p18 = teeth_border_with_gap[i][18];
			vcg::Point3f i_minus_1_p0 = teeth_border_with_gap[i - 1][0];
			vcg::Point3f mid_up = (i_p18 + i_minus_1_p0) * 0.5;

			vcg::Point3f i_p27 = teeth_border_with_gap[i][27];
			vcg::Point3f i_minus_1_p27 = teeth_border_with_gap[i - 1][27];
			vcg::Point3f mid_bottom = (i_p27 + i_minus_1_p27) * 0.5;

			double height = (mid_up - mid_bottom).Norm();
			vcg::Point3f mid_dir = (mid_up - mid_bottom).Normalize();
			vcg::Point3f p_mid = mid_bottom + mid_dir * height * m_curveture * 0.075;

			teeth_border_lingal.push_back(p_mid);
		}
	}

	//设置一些求交的参数
	const float maxDist = std::numeric_limits<float>::max();
	vcg::face::PointDistanceBaseFunctor<float> pointFaceFunctor;
	vcg::RayTriangleIntersectionFunctor<false> rayIntersector;
	vcg::EmptyClass objectMarker;

	// 起始端尾部控制点
	Point3f p15(teeth_border_with_gap.front().at(15).X(), teeth_border_with_gap.front().at(15).Y(), teeth_border_with_gap.front().at(15).Z());
	Point3f p17(teeth_border_with_gap.front().at(17).X(), teeth_border_with_gap.front().at(17).Y(), teeth_border_with_gap.front().at(17).Z());
	current_data_model->m_ctrlPoints.push_back(p17);
	current_data_model->m_ctrlPoints.push_back(p15);

	for (int j = 0; j < teeth_border_buccal.size(); ++j) {
		vcg::Point3f p = teeth_border_buccal.at(j) + vcg::Point3f(0.0, 0.0, -1.0) * m_cutOffset;
		//投影操作
		vcg::Point3f dir_front;
		if (j < teeth_border_buccal.size() - 1) {
			dir_front = teeth_border_buccal.at(j + 1) - teeth_border_buccal.at(j);
		}
		else {
			dir_front = teeth_border_buccal.at(j) - teeth_border_buccal.at(j - 1);
		}
		vcg::Point3f dir_proj = dir_front ^ vcg::Point3f(0.0, 0.0, 1.0);
		dir_proj = dir_proj.Normalize();
		vcg::Point3f proj_p = p - dir_proj * 10.0;

		vcg::Ray3<float, false> ray;
		float ray_distance;
		CFaceO* insect_face = nullptr;
		ray.Set(proj_p, dir_proj);
		insect_face = current_tree->DoRay(rayIntersector, objectMarker, ray, maxDist, ray_distance);

		if (insect_face) {
			p = proj_p + dir_proj * ray_distance;
		}

		current_data_model->m_ctrlPoints.push_back(Point3f(p.X(), p.Y(), p.Z()));
	}

	//结束端尾部控制点
	Point3f p2(teeth_border_with_gap.back().at(2).X(), teeth_border_with_gap.back().at(2).Y(), teeth_border_with_gap.back().at(2).Z());
	Point3f p0(teeth_border_with_gap.back().at(0).X(), teeth_border_with_gap.back().at(0).Y(), teeth_border_with_gap.back().at(0).Z());
	Point3f p34(teeth_border_with_gap.back().at(34).X(), teeth_border_with_gap.back().at(34).Y(), teeth_border_with_gap.back().at(34).Z());
	Point3f p32(teeth_border_with_gap.back().at(32).X(), teeth_border_with_gap.back().at(32).Y(), teeth_border_with_gap.back().at(32).Z());

	current_data_model->m_ctrlPoints.push_back(p2);
	current_data_model->m_ctrlPoints.push_back(p0);
	current_data_model->m_ctrlPoints.push_back(p34);
	current_data_model->m_ctrlPoints.push_back(p32);

	for (int j = 0; j < teeth_border_lingal.size(); ++j) {
		vcg::Point3f p = teeth_border_lingal.at(j) + vcg::Point3f(0.0, 0.0, -1.0) * m_cutOffset;
		current_data_model->m_ctrlPoints.push_back(Point3f(p.X(), p.Y(), p.Z()));
	}

	Point3f p21(teeth_border_with_gap.front().at(21).X(), teeth_border_with_gap.front().at(21).Y(), teeth_border_with_gap.front().at(21).Z());
	Point3f p19(teeth_border_with_gap.front().at(19).X(), teeth_border_with_gap.front().at(19).Y(), teeth_border_with_gap.front().at(19).Z());

	current_data_model->m_ctrlPoints.push_back(p21);
	current_data_model->m_ctrlPoints.push_back(p19);

	// 存入交互
	m_editManuallyCurve->setModel(current_data_model);
	m_editManuallyCurve->setTree(current_tree);

	//重新梳理控制点，用和它距离最近的曲线点作为控制点
	m_editManuallyCurve->DoInterpolation();

	for (int i = 0; i < current_data_model->m_ctrlPoints.size(); ++i) {
		int nearestIndex = -1;
		double nearestDis = std::numeric_limits<double>::max();
		for (int j = 0; j < current_data_model->m_curve.size(); ++j) {
			double dis = Distance(current_data_model->m_curve[j], current_data_model->m_ctrlPoints[i]);
			if (dis < nearestDis) {
				nearestDis = dis;
				nearestIndex = j;
			}
		}
		current_data_model->m_ctrlPoints[i] = current_data_model->m_curve[nearestIndex];
	}
}
bool OrthoCurve::computeOneCutline(const int models_id)
{
	//如果这个gum模型存在，则证明不是初次点击重新计算按钮
	if (VCG_models.at(models_id)->vcg_gum_mesh) {
		vcg::tri::Append<CMeshO, CMeshO>::Mesh(*VCG_models.at(models_id)->vcg_teeth_mesh, *VCG_models.at(models_id)->vcg_gum_mesh);

		delete VCG_models.at(models_id)->vcg_gum_mesh;
		VCG_models.at(models_id)->vcg_gum_mesh = nullptr;
	}

	CMeshO* current_mesh = VCG_models.at(models_id)->vcg_teeth_mesh;
	sn3DMeshModel* current_data_model = m_models.GetModel(models_id);
	AABBTree* current_tree = nullptr;
	if (VCG_models.at(models_id)->vcg_tree) {
		current_tree = VCG_models.at(models_id)->vcg_tree;
	}
	else { //模型还没有分开
		current_tree = new AABBTree();
		current_tree->Set(current_mesh->face.begin(), current_mesh->face.end());
		VCG_models.at(models_id)->vcg_tree = current_tree;
	}

	current_data_model->m_ctrlPoints.clear();
	current_data_model->m_curve.clear();
	current_data_model->m_cutDir.clear();

	vcg::tri::Clean<CMeshO>::RemoveDuplicateVertex(*current_mesh);
	vcg::tri::Allocator<CMeshO>::CompactEveryVector(*current_mesh);

	current_mesh->vert.EnableVFAdjacency();
	current_mesh->face.EnableVFAdjacency();
	current_mesh->face.EnableFFAdjacency();
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(*current_mesh);
	vcg::tri::UpdateTopology<CMeshO>::VertexFace(*current_mesh);

	//1, 将牙颌模型分解成牙齿和牙龈以及矫治器特征三部分，并只保留牙齿和牙龈
	std::vector< std::pair<int, CMeshO::FacePointer> > connected_components;
	int num_cc = vcg::tri::Clean<CMeshO>::ConnectedComponents(*current_mesh, connected_components);

	CMeshO* gum_mesh   = nullptr;
	CMeshO* teeth_mesh = new CMeshO();
	vcg::GridStaticPtr<CFaceO, float> gum_tri_grid;
	std::vector<CMeshO*> teeth_meshes, unsorted_teeth_meshes, sorted_teeth_meshes;

	for (size_t i = 0; i < connected_components.size(); ++i)
	{
		vcg::tri::UpdateSelection<CMeshO>::FaceClear(*current_mesh);
		connected_components[i].second->SetS();
		vcg::tri::UpdateSelection<CMeshO>::FaceConnectedFF(*current_mesh);
		vcg::tri::UpdateSelection<CMeshO>::VertexClear(*current_mesh);
		vcg::tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(*current_mesh);

		vcg::Box3f bbox;
		for (CMeshO::VertexIterator vit = current_mesh->vert.begin(); vit != current_mesh->vert.end(); ++vit) {
			if (vit->IsS()) {
				bbox.Add(vit->P());
			}
		}

		if (bbox.Diag() > 50) { // is gum
			gum_mesh = new CMeshO();
			vcg::tri::Append<CMeshO, CMeshO>::Mesh(*gum_mesh, *current_mesh, true);
			gum_mesh->face.EnableMark();
			RequirePerFaceMark(*gum_mesh);
			gum_tri_grid.Set(gum_mesh->face.begin(), gum_mesh->face.end());

			VCG_models.at(models_id)->vcg_gum_mesh = gum_mesh;
		}
		else{// is teeth or undercut or attachments
			CMeshO* splitted_mesh = new CMeshO();
			vcg::tri::Append<CMeshO, CMeshO>::Mesh(*splitted_mesh, *current_mesh, true);
			vcg::tri::Inertia<CMeshO> mesh_inertia(*splitted_mesh);
			Scalarm mesh_volume = mesh_inertia.Mass();

			if (mesh_volume > 100) { // is teeth
				unsorted_teeth_meshes.push_back(splitted_mesh);
			}
			vcg::tri::Append<CMeshO, CMeshO>::Mesh(*teeth_mesh, *current_mesh, true);
		}
	}

	VCG_models.at(models_id)->vcg_teeth_mesh = teeth_mesh;
	if (unsorted_teeth_meshes.size() < 3) {
		//无法计算自动提取牙龈线，请手动点取
		return false;
	}

	std::cout << unsorted_teeth_meshes.size() << std::endl;

	//2. sort teeth meshes
	std::map<CMeshO*, vcg::Point3f> teeth_barycenter_map;
	for (int i = 0; i < unsorted_teeth_meshes.size(); ++i) {
		vcg::Point3f bc = vcg::tri::Stat<CMeshO>::ComputeShellBarycenter(*unsorted_teeth_meshes.at(i));
		teeth_barycenter_map.insert({ unsorted_teeth_meshes.at(i), bc });
	}

	float min_X = std::numeric_limits<float>::max();
	vcg::Point3f min_bc;
	for (int i = 0; i < unsorted_teeth_meshes.size(); ++i) {
		vcg::Point3f bc = teeth_barycenter_map[unsorted_teeth_meshes.at(i)];
		if (bc.Y() > 0 && bc.X() < min_X) {
			min_X = bc.X();
			min_bc = bc;
		}
	}

	while (!unsorted_teeth_meshes.empty()) {
		float nearest_distance = std::numeric_limits<float>::max();
		int nearest_index = -1;
		for (int i = 0; i < unsorted_teeth_meshes.size(); ++i) {
			float distance = vcg::Distance(teeth_barycenter_map[unsorted_teeth_meshes.at(i)], min_bc);
			if (distance < nearest_distance) {
				nearest_distance = distance;
				nearest_index = i;
			}
		}

		sorted_teeth_meshes.push_back(unsorted_teeth_meshes.at(nearest_index));
		min_bc = teeth_barycenter_map[unsorted_teeth_meshes.at(nearest_index)];
		unsorted_teeth_meshes.erase(unsorted_teeth_meshes.begin() + nearest_index);
	}
	
	//检查合并牙模
	time_t now = time(0);
	tm* ltm = localtime(&now);

	int year  = 1900 + ltm->tm_year;
	int month = 1 + ltm->tm_mon;
	int day = ltm->tm_mday;

	if (year < 2024 || (year == 2024 && month == 1 && day < 30)) {
		teeth_meshes.push_back(sorted_teeth_meshes.front());
		for (int i = 1; i < sorted_teeth_meshes.size() - 1; ++i) {
			vcg::Point3f dir_front_current = (teeth_barycenter_map[sorted_teeth_meshes.at(i)] - teeth_barycenter_map[sorted_teeth_meshes.at(i - 1)]).Normalize();
			vcg::Point3f dir_current_back = (teeth_barycenter_map[sorted_teeth_meshes.at(i + 1)] - teeth_barycenter_map[sorted_teeth_meshes.at(i)]).Normalize();

			double angle = std::acos(dir_front_current * dir_current_back);
			angle = angle / 3.1415926 * 180;
			if (angle < 90) {
				teeth_meshes.push_back(sorted_teeth_meshes.at(i));
			}
			else { //如果当前牙齿位置“异常”，则并入到上一颗牙齿合并计算
				vcg::tri::Append<CMeshO, CMeshO>::Mesh(*teeth_meshes.back(), *sorted_teeth_meshes.at(i));
				teeth_barycenter_map[teeth_meshes.back()] = vcg::tri::Stat<CMeshO>::ComputeShellBarycenter(*teeth_meshes.back());
			}
		}
		teeth_meshes.push_back(sorted_teeth_meshes.back());
	}
	else {
		teeth_meshes.assign(sorted_teeth_meshes.begin(), sorted_teeth_meshes.end());
	}

	//3.get intersection(supposed that the models have been aligned)
	std::vector<std::vector<vcg::Point3f>> teeth_intersected_points(teeth_meshes.size());
	for (int i = 0; i < teeth_meshes.size(); ++i) {
		CMeshO* tooth_mesh = teeth_meshes.at(i);
		vcg::tri::UpdateFlags<CMeshO>::FaceClear(*tooth_mesh);
		vcg::tri::UpdateFlags<CMeshO>::VertexClear(*tooth_mesh);

		for (CMeshO::FaceIterator fi_teeth = tooth_mesh->face.begin(); fi_teeth != tooth_mesh->face.end(); ++fi_teeth)
		{
			if (!(*fi_teeth).IsD()) {
				vcg::Box3f teeth_face_bbox;
				fi_teeth->GetBBox(teeth_face_bbox);
				std::vector<CFaceO*> gum_faces_in_bbox;
				vcg::tri::GetInBoxFace(*gum_mesh, gum_tri_grid, teeth_face_bbox, gum_faces_in_bbox);

				for (std::vector<CFaceO*>::iterator fib = gum_faces_in_bbox.begin(); fib != gum_faces_in_bbox.end(); ++fib) {
					if (vcg::tri::Clean<CMeshO>::TestFaceFaceIntersection(&*fi_teeth, *fib)) {
						fi_teeth->SetS();
						break;
					}
				}
			}
		}

		vcg::tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(*tooth_mesh);
		for (CMeshO::VertexIterator vit = tooth_mesh->vert.begin(); vit != tooth_mesh->vert.end(); ++vit) {
			if (vit->IsS()) {
				teeth_intersected_points[i].push_back(vit->P());
			}
		}
	}

	// 将牙齿的一周分成36份（10度为一份）
	std::vector<std::vector<vcg::Point3f>> teeth_border_with_gap;//每个牙齿对应的最外面一圈的36个边界点
	std::vector<std::vector<bool>> teeth_border_flag; //标记是否36个点齐全

	teeth_border_with_gap.resize(teeth_intersected_points.size());
	teeth_border_flag.resize(teeth_intersected_points.size());

	//4.clean intersected points  ==> teeth_border_with_gap
	for (int i = 0; i < teeth_intersected_points.size(); ++i) {
		vcg::Point3f center(0, 0, 0);
		for (int j = 0; j < teeth_intersected_points[i].size(); ++j) {
			center += teeth_intersected_points[i][j];
		}
		center /= teeth_intersected_points[i].size();

		teeth_border_with_gap[i].resize(36);
		teeth_border_flag[i].resize(36, false);

		vcg::Point3f x;
		if (i == teeth_intersected_points.size() - 1) {
			x = teeth_barycenter_map[teeth_meshes.at(i)] - teeth_barycenter_map[teeth_meshes.at(i - 1)];
		}
		else {
			x = teeth_barycenter_map[teeth_meshes.at(i + 1)] - teeth_barycenter_map[teeth_meshes.at(i)];
		}
		x = x.Normalize();

		vcg::Point3f z(0.0, 0.0, 1.0);
		vcg::Plane3f xoy;
		xoy.Init(vcg::Point3f(0.0, 0.0, 0.0), z);
		vcg::Point3f proj_center = xoy.Projection(center);

		for (int j = 0; j < teeth_intersected_points[i].size(); ++j) {
			//firstly, find the jth point's index in teeth_border_with_gap[i]
			vcg::Point3f proj_p = xoy.Projection(teeth_intersected_points[i][j]);
			vcg::Point3f dir = (proj_p - proj_center).Normalize();
			float angle = std::acos(dir * x) / 3.1415926 * 180.0;
			if ((x ^ dir) * z < 0) {
				angle = 360 - angle;
			}

			int index = int(angle / 10) % 36;
			if (!teeth_border_flag[i][index]) {
				teeth_border_with_gap[i][index] = teeth_intersected_points[i][j];
				teeth_border_flag[i][index] = true;
			}
			else {
				vcg::Point3f proj_existed_border = xoy.Projection(teeth_border_with_gap[i][index]);
				if (vcg::Distance(proj_center, proj_p) > vcg::Distance(proj_center, proj_existed_border)) {
					teeth_border_with_gap[i][index] = teeth_intersected_points[i][j];
				}
			}
		}

		//检查是否有未赋值的点
		for (int j = 0; j < 36; ++j) {
			if (!teeth_border_flag[i][j]) {
				int last_index = (j == 0) ? 35 : j - 1;
				while (!teeth_border_flag[i][last_index]) {
					last_index = (last_index == 0) ? 35 : last_index - 1;
				}

				int next_index = (j == 35) ? 0 : j + 1;
				while (!teeth_border_flag[i][next_index]) {
					next_index = (next_index == 35) ? 0 : next_index + 1;
				}

				teeth_border_with_gap[i][j] = (teeth_border_with_gap[i][last_index] + teeth_border_with_gap[i][next_index]) * 0.5;
				teeth_border_flag[i][j] = true;
			}
		}
	}

	//5.remove gap. teeth_border_with_gap ==> teeth_border_lingal, teeth_border_buccal
	std::vector<vcg::Point3f> teeth_border_lingal, teeth_border_buccal;
	//buccal
	for (int i = 0; i < teeth_border_with_gap.size(); ++i) {
		teeth_border_buccal.push_back(teeth_border_with_gap[i][9]);

		if (i < teeth_border_with_gap.size() - 1) {
			vcg::Point3f i_p0 = teeth_border_with_gap[i][0];
			vcg::Point3f i_plus_1_p18 = teeth_border_with_gap[i + 1][18];
			vcg::Point3f mid_up = (i_p0 + i_plus_1_p18) * 0.5;

			vcg::Point3f i_p9 = teeth_border_with_gap[i][9];
			vcg::Point3f i_plus_1_p9 = teeth_border_with_gap[i + 1][9];
			vcg::Point3f mid_bottom = (i_p9 + i_plus_1_p9) * 0.5;

			double height = (mid_up - mid_bottom).Norm();
			vcg::Point3f mid_dir = (mid_up - mid_bottom).Normalize();
			vcg::Point3f p_mid = mid_bottom + mid_dir * height * m_curveture * 0.125;

			teeth_border_buccal.push_back(p_mid);
		}
	}

	//lingual
	for (int i = teeth_border_with_gap.size() - 1; i >= 0; i--) {
		teeth_border_lingal.push_back(teeth_border_with_gap[i][27]);

		if (i > 0) {
			vcg::Point3f i_p18 = teeth_border_with_gap[i][18];
			vcg::Point3f i_minus_1_p0 = teeth_border_with_gap[i - 1][0];
			vcg::Point3f mid_up = (i_p18 + i_minus_1_p0) * 0.5;

			vcg::Point3f i_p27 = teeth_border_with_gap[i][27];
			vcg::Point3f i_minus_1_p27 = teeth_border_with_gap[i - 1][27];
			vcg::Point3f mid_bottom = (i_p27 + i_minus_1_p27) * 0.5;

			double height = (mid_up - mid_bottom).Norm();
			vcg::Point3f mid_dir = (mid_up - mid_bottom).Normalize();
			vcg::Point3f p_mid = mid_bottom + mid_dir * height * m_curveture * 0.075;

			teeth_border_lingal.push_back(p_mid);
		}
	}

	//设置一些求交的参数
	const float maxDist = std::numeric_limits<float>::max();
	vcg::face::PointDistanceBaseFunctor<float> pointFaceFunctor;
	vcg::RayTriangleIntersectionFunctor<false> rayIntersector;
	vcg::EmptyClass objectMarker;

	// 起始端尾部控制点
	Point3f p15(teeth_border_with_gap.front().at(15).X(), teeth_border_with_gap.front().at(15).Y(), teeth_border_with_gap.front().at(15).Z());
	Point3f p17(teeth_border_with_gap.front().at(17).X(), teeth_border_with_gap.front().at(17).Y(), teeth_border_with_gap.front().at(17).Z());
	current_data_model->m_ctrlPoints.push_back(p17);
	current_data_model->m_ctrlPoints.push_back(p15);

	for (int j = 0; j < teeth_border_buccal.size(); ++j) {
		vcg::Point3f p = teeth_border_buccal.at(j) + vcg::Point3f(0.0, 0.0, -1.0) * m_cutOffset;
		//投影操作
		vcg::Point3f dir_front;
		if (j < teeth_border_buccal.size() - 1) {
			dir_front = teeth_border_buccal.at(j + 1) - teeth_border_buccal.at(j);
		}
		else {
			dir_front = teeth_border_buccal.at(j) - teeth_border_buccal.at(j - 1);
		}
		vcg::Point3f dir_proj = dir_front ^ vcg::Point3f(0.0, 0.0, 1.0);
		dir_proj = dir_proj.Normalize();
		vcg::Point3f proj_p = p - dir_proj * 10.0;

		vcg::Ray3<float, false> ray;
		float ray_distance;
		CFaceO* insect_face = nullptr;
		ray.Set(proj_p, dir_proj);
		insect_face = current_tree->DoRay(rayIntersector, objectMarker, ray, maxDist, ray_distance);

		if (insect_face) {
			p = proj_p + dir_proj * ray_distance;
		}

		current_data_model->m_ctrlPoints.push_back(Point3f(p.X(), p.Y(), p.Z()));
	}

	//结束端尾部控制点
	Point3f p2(teeth_border_with_gap.back().at(2).X(), teeth_border_with_gap.back().at(2).Y(), teeth_border_with_gap.back().at(2).Z());
	Point3f p0(teeth_border_with_gap.back().at(0).X(), teeth_border_with_gap.back().at(0).Y(), teeth_border_with_gap.back().at(0).Z());
	Point3f p34(teeth_border_with_gap.back().at(34).X(), teeth_border_with_gap.back().at(34).Y(), teeth_border_with_gap.back().at(34).Z());
	Point3f p32(teeth_border_with_gap.back().at(32).X(), teeth_border_with_gap.back().at(32).Y(), teeth_border_with_gap.back().at(32).Z());

	current_data_model->m_ctrlPoints.push_back(p2);
	current_data_model->m_ctrlPoints.push_back(p0);
	current_data_model->m_ctrlPoints.push_back(p34);
	current_data_model->m_ctrlPoints.push_back(p32);

	for (int j = 0; j < teeth_border_lingal.size(); ++j) {
		vcg::Point3f p = teeth_border_lingal.at(j) + vcg::Point3f(0.0, 0.0, -1.0) * m_cutOffset;
		current_data_model->m_ctrlPoints.push_back(Point3f(p.X(), p.Y(), p.Z()));
	}

	Point3f p21(teeth_border_with_gap.front().at(21).X(), teeth_border_with_gap.front().at(21).Y(), teeth_border_with_gap.front().at(21).Z());
	Point3f p19(teeth_border_with_gap.front().at(19).X(), teeth_border_with_gap.front().at(19).Y(), teeth_border_with_gap.front().at(19).Z());

	current_data_model->m_ctrlPoints.push_back(p21);
	current_data_model->m_ctrlPoints.push_back(p19);

	// 存入交互
	m_editManuallyCurve->setModel(current_data_model);
	m_editManuallyCurve->setTree(current_tree);

	//重新梳理控制点，用和它距离最近的曲线点作为控制点
	m_editManuallyCurve->DoInterpolation();

	for (int i = 0; i < current_data_model->m_ctrlPoints.size(); ++i) {
		int nearestIndex = -1;
		double nearestDis = std::numeric_limits<double>::max();
		for (int j = 0; j < current_data_model->m_curve.size(); ++j) {
			double dis = Distance(current_data_model->m_curve[j], current_data_model->m_ctrlPoints[i]);
			if (dis < nearestDis) {
				nearestDis = dis;
				nearestIndex = j;
			}
		}
		current_data_model->m_ctrlPoints[i] = current_data_model->m_curve[nearestIndex];
	}

	//清空teeth_meshes
	for (CMeshO* mesh_to_releasae : teeth_meshes) {
		delete mesh_to_releasae;
	}
	teeth_meshes.clear();

	return true;
}

void OrthoCurve::read3shapeModels()
{
	//将场景相关的交互器和模型清空清空
	pView->GetMainScene()->ClearScene();
	ResetSenceEdit(); 

	releaseAllModels();

	if (current_jaw == 0) {
		projectCurveProgressDialog->setLabelText("正在导入上颌模型...");
	}
	else {
		projectCurveProgressDialog->setLabelText("正在导入下颌模型...");
	}

	projectCurveProgressDialog->show();
	projectCurveProgressDialog->setValue(1.0);

	QDir input_dir(_3shape_models_input_path);
	QStringList dir_list = input_dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot); //所有的"步骤"文件夹

	//以vcg方式读入分开的模型，将它们合并；然后再转成sn3模型再导入
	for (int d = 0; d < dir_list.size(); ++d) {
		QString dir_name = dir_list.at(d);
		QString step = dir_name.mid(dir_name.lastIndexOf('p') + 1);

		projectCurveProgressDialog->setValue((d + 1) * 90.0 / dir_list.size());

		bool isok;
		step.toInt(&isok);
		if (!isok) {
			continue;
		}

		QDir models_dir(_3shape_models_input_path + '/' + dir_name + "/Models");
		QStringList models_name_list = models_dir.entryList(QStringList() << "*.stl", QDir::Files);//当前步骤下的所有模型文件目录

		CMeshO* teeth_mesh = new CMeshO();
		CMeshO* gum_mesh   = nullptr;
		int load_mask = 0;

		for (int f = 0; f < models_name_list.size(); ++f) {
			QString model_name = models_name_list.at(f);

			QString model_path = _3shape_models_input_path + '/' + dir_name + "/Models/" + model_name;
			std::string model_path_str = model_path.toLocal8Bit().constData();

			model_name = model_name.mid(0, model_name.lastIndexOf('.'));
			if (model_name.split('_').front() != "Tooth") {
				continue;
			}

			QString teeth_id = model_name.split('_').back();
			bool isok;
			int i_teeth_id = teeth_id.toInt(&isok);
			CMeshO* current_mesh = new CMeshO();

			if (isok){ // is teeth
				if ((current_jaw == 0 && i_teeth_id <= 16) || (current_jaw == 1 && i_teeth_id > 16)) {
					load_mask = 0;
					int result = vcg::tri::io::ImporterSTL<CMeshO>::Open(*current_mesh, model_path_str.c_str(), load_mask);
					vcg::tri::Clean<CMeshO>::RemoveDuplicateVertex(*current_mesh);
					vcg::tri::Allocator<CMeshO>::CompactEveryVector(*current_mesh);

					vcg::tri::Append<CMeshO, CMeshO>::Mesh(*teeth_mesh, *current_mesh);
				}
				delete current_mesh;
			}
			else { // is gum
				if ((teeth_id[0] == QChar('U') && current_jaw == 0) || (current_jaw == 1 && teeth_id[0] == QChar('L'))) {
					load_mask = 0;
					int result = vcg::tri::io::ImporterSTL<CMeshO>::Open(*current_mesh, model_path_str.c_str(), load_mask);
					gum_mesh = current_mesh;

					vcg::tri::Clean<CMeshO>::RemoveDuplicateVertex(*gum_mesh);
					vcg::tri::Allocator<CMeshO>::CompactEveryVector(*gum_mesh);
				}
			}
		}

		_VCGModel* _3shape_model = new _VCGModel();
		_3shape_model->vcg_teeth_mesh = teeth_mesh;
		_3shape_model->vcg_gum_mesh   = gum_mesh;
		_3shape_model->step = step.toInt();
		_3shape_model->path = dir_name + "/Models";
		VCG_models.push_back(_3shape_model);
	}

	//对模型按步骤排序
	if (!VCG_models.empty()) {
		std::sort(VCG_models.begin(), VCG_models.end(), [](_VCGModel* a, _VCGModel* b) {
			return a->step < b->step;
			});
	}

	for (_VCGModel* current_model : VCG_models) {
		CMeshO* jaw_mesh = new CMeshO();
		vcg::tri::Append<CMeshO, CMeshO>::Mesh(*jaw_mesh, *current_model->vcg_teeth_mesh);
		vcg::tri::Append<CMeshO, CMeshO>::Mesh(*jaw_mesh, *current_model->vcg_gum_mesh);

		//读取模型到sn3里面做显示
		sn3DMeshModel* model = new sn3DMeshModel();
		sn3DMeshData* model_data = model->AddMesh();

		QString jaw = current_jaw == 0 ? "U" : "L";
		QString model_name = patient_id + "_" + jaw + QString("_%1").arg(current_model->step);
		model_name = model_name.mid(0, model_name.lastIndexOf('.'));
		std::string model_name_str = model_name.toLocal8Bit().constData();
		model->SetName(model_name_str);

		for (CMeshO::VertexIterator vit = jaw_mesh->vert.begin(); vit != jaw_mesh->vert.end(); ++vit) {
			model_data->AddVertex(Point3f(vit->P().X(), vit->P().Y(), vit->P().Z()));
		}
		CMeshO::VertexIterator first_vit = jaw_mesh->vert.begin();
		for (CMeshO::FaceIterator fit = jaw_mesh->face.begin(); fit != jaw_mesh->face.end(); ++fit) {
			model_data->AddFace(fit->V(0) - &*first_vit, fit->V(1) - &*first_vit, fit->V(2) - &*first_vit);
		}

		m_models.AddModel(model);

		delete jaw_mesh;
	}

	projectCurveProgressDialog->setValue(100);
}

void OrthoCurve::releaseAllModels()
{
	for (int i = 0; i < VCG_models.size(); ++i) {
		delete VCG_models[i];
	}
	VCG_models.clear();

	if (m_models.Count() > 0) {
		m_models.DeleteAll();
	}
	m_currentID = 0;

	if (twin_model_widget) {
		twin_model_widget->A.clear();
		twin_model_widget->B.clear();
		twin_model_widget->fixture_A.clear();
		twin_model_widget->fixture_B.clear();
		twin_model_widget->rotate_A = twin_model_widget->rotate_B = 0.0;
		twin_model_widget->rotate_A_origin = twin_model_widget->rotate_B_origin = 0.0;
	}
}

void OrthoCurve::changeFileName(QString& patient_id)
{
	if (filename_ui == nullptr) {
		filename_ui = new Ui::FileNameDialog();
		filename_dialog = new QDialog();
		filename_ui->setupUi(filename_dialog);

		connect(filename_ui->radioButton_max, &QRadioButton::clicked, this, [&]() {
			int row_count = filename_ui->tableWidget->rowCount();
			for (int row = 0; row < row_count; row++) {
				filename_ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString('U')));
			}
			filename_ui->tableWidget->update();
		});

		connect(filename_ui->radioButton_man, &QRadioButton::clicked, this, [&]() {
			int row_count = filename_ui->tableWidget->rowCount();
			for (int row = 0; row < row_count; row++) {
				filename_ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString('L')));
			}
			filename_ui->tableWidget->update();
		});

		connect(filename_dialog, &QDialog::accepted, this, [&]() {
			// rename mesh
			for (int row_count = 0; row_count < filename_ui->tableWidget->rowCount(); ++row_count) {
				QTableWidgetItem* item = filename_ui->tableWidget->item(row_count, 1);
				QString jaw = item ? item->text() : "";

				item = filename_ui->tableWidget->item(row_count, 2);
				QString step = item ? item->text() : "";

				patient_id = filename_ui->lineEdit_patientID->text();
				QString model_name = patient_id + '_' + jaw + '_' + step;
				std::string model_name_str = model_name.toLocal8Bit().data();
				sn3DMeshModel* current_model = m_models.GetModel(row_count);
				current_model->SetName(model_name_str.c_str());

				if (!VCG_models.empty()) {
					VCG_models.at(row_count)->step = step.toInt();
				}
			}
		});
	}
	filename_dialog->show();

	//fill id textline
	filename_ui->lineEdit_patientID->setText(patient_id);

	//update table content.
	QRegularExpression step_re("\\d+");
	QRegularExpression upper_re("(Upper)|(Maxilliary)|([ _-]U)", QRegularExpression::CaseInsensitiveOption);
	QRegularExpression lower_re("(Lower)|(Mandibular)|([ _-]L)", QRegularExpression::CaseInsensitiveOption);

	filename_ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	filename_ui->tableWidget->clearContents();
	filename_ui->tableWidget->setRowCount(VCG_models.size());

	for (int row_count = 0; row_count < VCG_models.size(); ++row_count) {
		QString model_name = VCG_models.at(row_count)->path;
		model_name = model_name.split('/').back();
		model_name = model_name.mid(0, model_name.lastIndexOf('.'));

		// match step
		QRegularExpressionMatchIterator step_match_ite = step_re.globalMatch(model_name);
		QStringList matched_words;
		while (step_match_ite.hasNext()) {
			QRegularExpressionMatch match = step_match_ite.next();
			QString word = match.captured(0);
			if (word.size() <= 3) {
				matched_words << word;
			}
		}

		//match jaw
		QRegularExpressionMatch match_upper = upper_re.match(model_name);
		QRegularExpressionMatch match_lower = lower_re.match(model_name);
		bool upper_matched = match_upper.hasMatch();
		bool lower_matched = match_lower.hasMatch();
		if (upper_matched && !lower_matched) {
			filename_ui->tableWidget->setItem(row_count, 1, new QTableWidgetItem(QString('U')));
			filename_ui->radioButton_max->setChecked(true);
		}
		else if (!upper_matched && lower_matched) {
			filename_ui->tableWidget->setItem(row_count, 1, new QTableWidgetItem(QString('L')));
			filename_ui->radioButton_man->setChecked(true);
		}

		filename_ui->tableWidget->setItem(row_count, 0, new QTableWidgetItem(model_name));
		if (!matched_words.empty()) {
			filename_ui->tableWidget->setItem(row_count, 2, new QTableWidgetItem(matched_words.back()));
		}

	}
}

CMeshO* OrthoCurve::doSubstract(CMeshO* src_mesh, CMeshO* target_mesh)
{
	if (!src_mesh || !target_mesh) {
		return nullptr;
	}
	
	uint32_t num_src_vertices	= src_mesh->vert.size();
	uint32_t num_src_faces		= src_mesh->face.size();

	std::vector<float> src_vertices(num_src_vertices * 3);
	std::vector<uint32_t> src_faces(num_src_faces * 3);

	for (int i = 0; i < src_mesh->vert.size(); ++i) {
		src_vertices[i * 3]		= src_mesh->vert.at(i).P().X();
		src_vertices[i * 3 + 1]	= src_mesh->vert.at(i).P().Y();
		src_vertices[i * 3 + 2]	= src_mesh->vert.at(i).P().Z();
	}

	for (int i = 0; i < src_mesh->face.size(); ++i) {
		src_faces[i * 3]		= src_mesh->face.at(i).V(0) - &*src_mesh->vert.begin();
		src_faces[i * 3 + 1]	= src_mesh->face.at(i).V(1) - &*src_mesh->vert.begin();
		src_faces[i * 3 + 2]	= src_mesh->face.at(i).V(2) - &*src_mesh->vert.begin();
	}

	uint32_t num_target_vertices = target_mesh->vert.size();
	uint32_t num_target_faces    = target_mesh->face.size();

	std::vector<float> target_vertices(num_target_vertices * 3);
	std::vector<uint32_t> target_faces(num_target_faces * 3);

	for (int i = 0; i < target_mesh->vert.size(); ++i) {
		target_vertices[i * 3]     = target_mesh->vert.at(i).P().X();
		target_vertices[i * 3 + 1] = target_mesh->vert.at(i).P().Y();
		target_vertices[i * 3 + 2] = target_mesh->vert.at(i).P().Z();
	}

	for (int i = 0; i < target_mesh->face.size(); ++i) {
		target_faces[i * 3]	    = target_mesh->face.at(i).V(0) - &*target_mesh->vert.begin();
		target_faces[i * 3 + 1] = target_mesh->face.at(i).V(1) - &*target_mesh->vert.begin();
		target_faces[i * 3 + 2] = target_mesh->face.at(i).V(2) - &*target_mesh->vert.begin();
	}

	// 2. create a context
	qDebug() << "create a context";
	McContext context = MC_NULL_HANDLE;
	McResult err = mcCreateContext(&context, MC_NULL_HANDLE);

	qDebug() << "3. do magic";
	//fclose(stdout);
	err = mcDispatch(
		context,
		MC_DISPATCH_VERTEX_ARRAY_FLOAT |
		MC_DISPATCH_FILTER_FRAGMENT_SEALING_INSIDE | MC_DISPATCH_FILTER_FRAGMENT_LOCATION_ABOVE,
		src_vertices.data(),
		src_faces.data(),
		nullptr,
		num_src_vertices,
		num_src_faces,
		target_vertices.data(),
		target_faces.data(),
		nullptr,
		num_target_vertices,
		num_target_faces
	);

	if (err != MC_NO_ERROR)
	{
		fprintf(stderr, "dispatch call failed (err=%d)\n", (int)err);
		return nullptr;
	}

	qDebug() << "4. query the number of available connected component (all types)";
	uint32_t numConnComps;
	std::vector<McConnectedComponent> connComps;

	err = mcGetConnectedComponents(context, MC_CONNECTED_COMPONENT_TYPE_ALL, 0, NULL, &numConnComps);
	if (err != MC_NO_ERROR){
		fprintf(stderr, "1:mcGetConnectedComponents(MC_CONNECTED_COMPONENT_TYPE_ALL) failed (err=%d)\n", (int)err);
		return nullptr;
	}

	if (numConnComps == 0){
		fprintf(stdout, "no connected components found\n");
		return nullptr;
	}

	connComps.resize(numConnComps);
	err = mcGetConnectedComponents(context, MC_CONNECTED_COMPONENT_TYPE_FRAGMENT, (uint32_t)connComps.size(), connComps.data(), NULL);
	if (err != MC_NO_ERROR){
		fprintf(stderr, "2:mcGetConnectedComponents(MC_CONNECTED_COMPONENT_TYPE_FRAGMENT) failed (err=%d)\n", (int)err);
		return nullptr;
	}

	qDebug() << " 5. query the data of each connected component from MCUT";
	CMeshO* result = new CMeshO();
	McConnectedComponent connComp = connComps[0]; // connected compoenent id
	uint64_t numBytes = 0;

	// query the vertices
	numBytes = 0;
	err = mcGetConnectedComponentData(context, connComp, MC_CONNECTED_COMPONENT_DATA_VERTEX_FLOAT, 0, NULL, &numBytes);
	if (err != MC_NO_ERROR) {
		fprintf(stderr, "1:mcGetConnectedComponentData(MC_CONNECTED_COMPONENT_DATA_VERTEX_FLOAT) failed (err=%d)\n", (int)err);
		return nullptr;
	}

	uint32_t numberOfVertices = (uint32_t)(numBytes / (sizeof(float) * 3));
	std::vector<float> vertices(numberOfVertices * 3u);
	err = mcGetConnectedComponentData(context, connComp, MC_CONNECTED_COMPONENT_DATA_VERTEX_FLOAT, numBytes, (void*)vertices.data(), NULL);
	if (err != MC_NO_ERROR){
		fprintf(stderr, "2:mcGetConnectedComponentData(MC_CONNECTED_COMPONENT_DATA_VERTEX_FLOAT) failed (err=%d)\n", (int)err);
		return nullptr;
	}

	// query the faces
	numBytes = 0;
	err = mcGetConnectedComponentData(context, connComp, MC_CONNECTED_COMPONENT_DATA_FACE_TRIANGULATION, 0, NULL, &numBytes);
	if (err != MC_NO_ERROR) {
		fprintf(stderr, "1:mcGetConnectedComponentData(MC_CONNECTED_COMPONENT_DATA_FACE_TRIANGULATION) failed (err=%d)\n", (int)err);
		return nullptr;
	}

	std::vector<uint32_t> faceIndices;
	faceIndices.resize(numBytes / sizeof(uint32_t));
	err = mcGetConnectedComponentData(context, connComp, MC_CONNECTED_COMPONENT_DATA_FACE_TRIANGULATION, numBytes, faceIndices.data(), NULL);
	if (err != MC_NO_ERROR) {
		fprintf(stderr, "2:mcGetConnectedComponentData(MC_CONNECTED_COMPONENT_DATA_FACE_TRIANGULATION) failed (err=%d)\n", (int)err);
		return nullptr;
	}

	//融合到原vcg模型用于输出
	mergeCMeshWithMCUTData(result, vertices, faceIndices);

	qDebug() << "6. free connected component data";
	err = mcReleaseConnectedComponents(context, 0, NULL);

	if (err != MC_NO_ERROR)
	{
		fprintf(stderr, "mcReleaseConnectedComponents failed (err=%d)\n", (int)err);
		return nullptr;
	}

	// 7. destroy context
	// ------------------
	err = mcReleaseContext(context);

	if (err != MC_NO_ERROR)
	{
		fprintf(stderr, "mcReleaseContext failed (err=%d)\n", (int)err);
		return nullptr;
	}

	std::cout << "finish...\n";
	return result;
}

CMeshO* OrthoCurve::generateBaseModel(int model_id)
{
	CMeshO* result_mesh = new CMeshO();

	QString dir = QCoreApplication::applicationDirPath();
	QString model_name(m_models.GetModel(model_id)->GetName());
	QStringList datalist = model_name.split('_');
	if (datalist.size() < 3) {
		return nullptr;
	}
	QString step = QString("%1").arg(datalist[2], 2, QLatin1Char('0'));
	QString currentJaw = datalist[1];
	QString id = datalist[0];

	step = QString("%1").arg(step, 2, QLatin1Char('0'));
	QString information = id + currentJaw + step;

	if (m_fixture == 3 || m_fixture == 4) {
		//1.读入底座模型
		CMeshO* base_mesh = new CMeshO();
		QString base_path =  dir + "/models/base_V3.stl" ;
		std::string base_path_str = base_path.toLocal8Bit().constData();
		int load_mask_temp;
		int open_result = vcg::tri::io::ImporterSTL<CMeshO>::Open(*base_mesh, base_path_str.c_str(), load_mask_temp);
		if (open_result != 0) {
			std::cout << vcg::tri::io::ExporterSTL<CMeshO>::ErrorMsg(open_result);
		}
		vcg::tri::Append<CMeshO, CMeshO>::Mesh(*result_mesh, *base_mesh);
		delete base_mesh;

		//2. 读入info模型
		CMeshO* info_mesh = generateInfoMesh_V3(information);

		vcg::Matrix44f scale_mat, trans_mat, trans_mat_2;
		trans_mat.SetIdentity();
		trans_mat.SetTranslate(0.0, 0.0, -1.2);

		scale_mat.SetIdentity();
		scale_mat.SetScale(1.0, 1.0, 3.0 / 1.5);

		trans_mat_2.SetIdentity();
		trans_mat_2.SetTranslate(0.0, 0.0, 1.0);

		vcg::tri::UpdatePosition<CMeshO>::Matrix(*info_mesh, trans_mat_2 * scale_mat * trans_mat);
		vcg::tri::Append<CMeshO, CMeshO>::Mesh(*result_mesh, *info_mesh);
		delete info_mesh;
	}
	else {
		//1. 生成二维码模型
		if (m_need_qr) {
			CMeshO* qr_mesh = generateQRMesh(information, m_qr_width, 1.4, m_qr_correction);

			//将二维码模型平移到相应位置
			vcg::Matrix44f trans_matrix;
			trans_matrix.SetIdentity();
			trans_matrix.SetTranslate(vcg::Point3f(-m_qr_width / 2.0 - 6, -m_qr_width / 2.0, 1.4));
			vcg::tri::UpdatePosition<CMeshO>::Matrix(*qr_mesh, trans_matrix, true);
			vcg::tri::Append<CMeshO, CMeshO>::Mesh(*result_mesh, *qr_mesh);

			delete qr_mesh;
		}

		//2.读入底座模型
		CMeshO* base_mesh = new CMeshO();
		QString base_path = m_need_qr ? dir + "/models/base.stl" : dir + "/models/base_short.stl";
		std::string base_path_str = base_path.toLocal8Bit().constData();
		int load_mask_temp;
		int open_result = vcg::tri::io::ImporterSTL<CMeshO>::Open(*base_mesh, base_path_str.c_str(), load_mask_temp);
		if (open_result != 0) {
			std::cout << vcg::tri::io::ExporterSTL<CMeshO>::ErrorMsg(open_result);
		}
		vcg::tri::Append<CMeshO, CMeshO>::Mesh(*result_mesh, *base_mesh);
		delete base_mesh;

		//3.生成info模型
		CMeshO* info_mesh = generateInfoMesh(information, m_need_qr);
		vcg::Matrix44f info_mat;
		if (!m_need_qr) {
			vcg::Matrix44f scale_mat, trans_mat, trans_mat_2;
			trans_mat.SetIdentity();
			trans_mat.SetTranslate(0.0, 0.0, -1.2);

			scale_mat.SetIdentity();
			scale_mat.SetScale(1.0, 1.0, 3.0 / 1.5);

			trans_mat_2.SetIdentity();
			trans_mat_2.SetTranslate(0.0, 0.0, 1.0);

			vcg::tri::UpdatePosition<CMeshO>::Matrix(*info_mesh, trans_mat_2 * scale_mat * trans_mat);
		}

		vcg::tri::Append<CMeshO, CMeshO>::Mesh(*result_mesh, *info_mesh);
		delete info_mesh;
	}

	return result_mesh;
}

void OrthoCurve::SaveModels(QString path)
{
	projectCurveProgressDialog->setLabelText("正在导出生产模型和切割线...");
	projectCurveProgressDialog->show();
	projectCurveProgressDialog->setValue(1);

	//读取生成八字模型
	QString base_pin_filename = "/models/5axis_base_pin.stl";

	if (m_fixture == 0) {
		base_pin_filename = "/models/5axis_base_pin.stl";
	}
	else if (m_fixture == 1) {
		base_pin_filename = "/models/robotic_base_pin.stl";
	}
	else if (m_fixture == 2) {
		base_pin_filename = "/models/5axis_base_pin_V2.stl";
	}
	else if (m_fixture == 3 || m_fixture == 4) {
		base_pin_filename = "/models/5axis_base_pin_V3.stl";
	}

	//读取八字模型
	CMeshO* base_pin_mesh = new CMeshO();

	QString base_pin_path = QCoreApplication::applicationDirPath() + base_pin_filename;
	std::string base_pin_path_str = base_pin_path.toLocal8Bit().constData();

	int load_mask_temp;
	int open_result = vcg::tri::io::ImporterSTL<CMeshO>::Open(*base_pin_mesh, base_pin_path_str.c_str(), load_mask_temp);
	vcg::tri::Clean<CMeshO>::RemoveDuplicateVertex(*base_pin_mesh);
	vcg::tri::Allocator<CMeshO>::CompactEveryVector(*base_pin_mesh);

#ifdef IS_TWIN_MODEL   
	//计算矩阵
	//检查是否点击过双膜显示按钮
	vcg::Matrix44f translate_matrix_left, translate_matrix_right;
	vcg::Matrix44f translate_matrix_left_base, translate_matrix_right_base;

	if (twin_model_dialog == nullptr) {
		twin_model_dialog = new QDialog();
		twin_model_ui.setupUi(twin_model_dialog);

		twin_model_widget = new ThermoBaseDrawWidget(twin_model_dialog);
		twin_model_widget->setGeometry(QRect(20, 20, 600, 600));

		twin_model_widget->rotate_A_origin = 0.0;
		twin_model_widget->trans_A_origin.setX(0.0);
		twin_model_widget->trans_A_origin.setY(0.0);

		twin_model_widget->rotate_B_origin = 0.0;
		twin_model_widget->trans_B_origin.setX(0.0);
		twin_model_widget->trans_B_origin.setY(0.0);
	}

	vcg::Matrix44f trans_mat_0, trans_mat_1, trans_mat_2, trans_mat_3, trans_mat_4;
	trans_mat_0.SetRotateDeg(twin_model_widget->rotate_A_origin, vcg::Point3f(0.0, 0.0, 1.0));
	trans_mat_2.SetRotateDeg(90, vcg::Point3f(0.0, 0.0, 1.0));
	trans_mat_3.SetTranslate(vcg::Point3f(11.99, 27.115, 0.0));
	trans_mat_4.SetTranslate(vcg::Point3f(-twin_model_widget->trans_A_origin.y(), -twin_model_widget->trans_A_origin.x(), 0.0));
	translate_matrix_left_base = trans_mat_3 * trans_mat_2;
	translate_matrix_left = trans_mat_4 * translate_matrix_left_base * trans_mat_0;

	trans_mat_0.SetRotateDeg(twin_model_widget->rotate_B_origin, vcg::Point3f(0.0, 0.0, 1.0));
	trans_mat_2.SetRotateDeg(-90, vcg::Point3f(0.0, 0.0, 1.0));
	trans_mat_3.SetTranslate(vcg::Point3f(-11.99, -27.115, 0.0));
	trans_mat_4.SetTranslate(vcg::Point3f(-twin_model_widget->trans_B_origin.y(), -twin_model_widget->trans_B_origin.x(), 0.0));
	translate_matrix_right_base = trans_mat_3 * trans_mat_2;
	translate_matrix_right = trans_mat_4 * translate_matrix_right_base * trans_mat_0;
#endif

	for (int i = 0; i < VCG_models.size(); ++i)
	{
		CMeshO* current_gum = VCG_models.at(i)->vcg_gum_mesh;
		CMeshO* current_teeth = VCG_models.at(i)->vcg_teeth_mesh;
		CMeshO* result_mesh = new CMeshO();

		//生成信息板上的信息
		CMeshO* base_mesh = generateBaseModel(i);

		#ifndef IS_TWIN_MODEL
			if (current_gum) {
				delete result_mesh;
				result_mesh = doSubstract(current_gum, base_pin_mesh);
			}
		#endif

		//按照高度截取牙齿模型
		if (current_teeth && current_gum) {
			current_teeth->vert.EnableQuality();
			current_teeth->vert.EnableVFAdjacency();
			current_teeth->face.EnableVFAdjacency();
			current_teeth->face.EnableFFAdjacency();
			vcg::tri::UpdateTopology<CMeshO>::FaceFace(*current_teeth);
			vcg::tri::UpdateTopology<CMeshO>::VertexFace(*current_teeth);
			vcg::tri::UpdateNormal<CMeshO>::PerFace(*current_teeth);
			vcg::tri::UpdateSelection<CMeshO>::FaceClear(*current_teeth);

			vcg::Plane3f slicing_plane;
			slicing_plane.Init(vcg::Point3f(0.0, 0.0, 3.2), vcg::Point3f(0.0, 0.0, 1.0));

			vcg::tri::QualityMidPointFunctor<CMeshO> slicingfunc(0.0);
			vcg::tri::QualityEdgePredicate<CMeshO> slicingpred(0.0, 0.0);

			vcg::tri::UpdateQuality<CMeshO>::VertexFromPlane(*current_teeth, slicing_plane);
			vcg::tri::RefineE<CMeshO, vcg::tri::QualityMidPointFunctor<CMeshO>, vcg::tri::QualityEdgePredicate<CMeshO> >(*current_teeth, slicingfunc, slicingpred, false);
			vcg::tri::UpdateSelection<CMeshO>::VertexFromQualityRange(*current_teeth, 0, std::numeric_limits<float>::max());
			vcg::tri::UpdateSelection<CMeshO>::FaceFromVertexStrict(*current_teeth);

			CMeshO* up = new CMeshO();
			vcg::tri::Append<CMeshO, CMeshO>::Mesh(*up, *current_teeth, true);
			delete current_teeth;
			VCG_models.at(i)->vcg_teeth_mesh = current_teeth = up;

			#ifndef IS_TWIN_MODEL
				vcg::tri::Append<CMeshO, CMeshO>::Mesh(*result_mesh, *current_teeth);
			#endif
		}

		QString model_name(m_models.GetModel(i)->GetName());
		QStringList model_names = model_name.split('_');
		QString model_name_print = model_names.size() >= 3 ? model_names.at(0) + model_names.at(1) + QString("%1").arg(model_names.at(2), 2, QLatin1Char('0')) : model_name;

		#ifdef IS_TWIN_MODEL
		// 导出双膜切割模型
		CMeshO* result_left, * result_right;
		CMeshO* gum_left = new CMeshO();
		CMeshO* gum_right = new CMeshO();
		CMeshO* teeth_left = new CMeshO();
		CMeshO* teeth_right = new CMeshO();
		CMeshO* base_left = new CMeshO();
		CMeshO* base_right = new CMeshO();
		CMeshO* base_pin_left = new CMeshO();
		CMeshO* base_pin_right = new CMeshO();
		result_right = result_left = nullptr;

		//平移牙龈并且做差
		if (current_gum) {
			if (m_fixture == 4) {
				result_right = new CMeshO();
				result_left = new CMeshO();

				vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(*result_left, *current_gum);
				vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(*result_right, *current_gum);
				vcg::tri::UpdatePosition<CMeshO>::Matrix(*result_left, translate_matrix_left, true);
				vcg::tri::UpdatePosition<CMeshO>::Matrix(*result_right, translate_matrix_right, true);
			}
			else if (m_fixture == 3) {
				vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(*gum_left, *current_gum);
				vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(*gum_right, *current_gum);
				vcg::tri::UpdatePosition<CMeshO>::Matrix(*gum_left, translate_matrix_left, true);
				vcg::tri::UpdatePosition<CMeshO>::Matrix(*gum_right, translate_matrix_right, true);

				vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(*base_pin_left, *base_pin_mesh);
				vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(*base_pin_right, *base_pin_mesh);
				vcg::tri::UpdatePosition<CMeshO>::Matrix(*base_pin_left, translate_matrix_left_base, true);
				vcg::tri::UpdatePosition<CMeshO>::Matrix(*base_pin_right, translate_matrix_right_base, true);

				result_left = doSubstract(gum_left, base_pin_left);
				result_right = doSubstract(gum_right, base_pin_right);
			}
		}

		//平移牙齿
		if (current_teeth) {
			vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(*teeth_left, *current_teeth);
			vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(*teeth_right, *current_teeth);
			vcg::tri::UpdatePosition<CMeshO>::Matrix(*teeth_left, translate_matrix_left, true);
			vcg::tri::UpdatePosition<CMeshO>::Matrix(*teeth_right, translate_matrix_right, true);
		}

		//平移底座
		vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(*base_left, *base_mesh);
		vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(*base_right, *base_mesh);

		vcg::tri::UpdatePosition<CMeshO>::Matrix(*base_left, translate_matrix_left_base, true);
		vcg::tri::UpdatePosition<CMeshO>::Matrix(*base_right, translate_matrix_right_base, true);

		//合并模型
		vcg::tri::Append<CMeshO, CMeshO>::Mesh(*result_left, *base_left);
		vcg::tri::Append<CMeshO, CMeshO>::Mesh(*result_left, *teeth_left);
		vcg::tri::Append<CMeshO, CMeshO>::Mesh(*result_right, *base_right);
		vcg::tri::Append<CMeshO, CMeshO>::Mesh(*result_right, *teeth_right);

		QString model_outpath_left = path + '/' + model_name_print + "L.stl";
		std::string model_outpath_left_str = model_outpath_left.toLocal8Bit().data();
		int result = vcg::tri::io::ExporterSTL<CMeshO>::Save(*result_left, model_outpath_left_str.c_str(), true);
		if (result != 0) {
			std::cout << vcg::tri::io::ExporterSTL<CMeshO>::ErrorMsg(result);
		}

		QString model_outpath_right = path + '/' + model_name_print + "R.stl";
		std::string model_outpath_right_str = model_outpath_right.toLocal8Bit().data();
		result = vcg::tri::io::ExporterSTL<CMeshO>::Save(*result_right, model_outpath_right_str.c_str(), true);
		if (result != 0) {
			std::cout << vcg::tri::io::ExporterSTL<CMeshO>::ErrorMsg(result);
		}

		delete result_left;
		delete result_right;
		delete base_left;
		delete base_right;
		delete gum_left;
		delete gum_right;
		delete base_pin_left;
		delete base_pin_right;
		delete teeth_left;
		delete teeth_right;


		//更新切割线
		sn3DMeshModel* m = m_models.GetModel(i);
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				m->trans_mat_left.ElementAt(j, k) = translate_matrix_left.ElementAt(j, k);
				m->trans_mat_right.ElementAt(j, k) = translate_matrix_right.ElementAt(j, k);
			}
		}
		#else 
			vcg::tri::Append<CMeshO, CMeshO>::Mesh(*result_mesh, *base_mesh);

			QString model_outpath = path + '/' + model_name_print + ".stl";
			std::string model_outpath_str = model_outpath.toLocal8Bit().data();
			int result = vcg::tri::io::ExporterSTL<CMeshO>::Save(*result_mesh, model_outpath_str.c_str(), true);
			if (result != 0) {
				std::cout << vcg::tri::io::ExporterSTL<CMeshO>::ErrorMsg(result);
			}
		#endif

		delete base_mesh;
		delete result_mesh;

		projectCurveProgressDialog->setValue((i + 1) * 100.0 / VCG_models.size());
	}

	delete base_pin_mesh;
}

void OrthoCurve::OnAlignModel()
{
	std::cout << "Align all of models \n";
	auto matrixMoveAlignedModelToXOY = [](CMeshO* mesh) {
		// find minis Z
		double min_Z = std::numeric_limits<double>::max();
		for (CMeshO::VertexType v : mesh->vert) {
			if (v.P().Z() < min_Z) {
				min_Z = v.P().Z();
			}
		}

		// move to xoy plane
		vcg::Matrix44f move_mat;
		move_mat.SetIdentity();
		if (std::fabs(min_Z) > 0.1) {
			move_mat.SetTranslate(vcg::Point3f(0.0, 0.0, -1.0) * min_Z);
		}
		return move_mat;
	};

	//获取摆正点
	sn3DMeshModel* current_model = (sn3DMeshModel*)pView->GetMainScene()->GetObject(0);
	if (current_model->m_alignPoints.size() != 3) {
		return;
	}

	vcg::Point3f p1(current_model->m_alignPoints[0][0], current_model->m_alignPoints[0][1], current_model->m_alignPoints[0][2]);
	vcg::Point3f p2(current_model->m_alignPoints[1][0], current_model->m_alignPoints[1][1], current_model->m_alignPoints[1][2]);
	vcg::Point3f p3(current_model->m_alignPoints[2][0], current_model->m_alignPoints[2][1], current_model->m_alignPoints[2][2]);

	vcg::Point3f F = (p1 - p2).Normalize() + (p1 - p3).Normalize();
	F = F.Normalize();
	vcg::Point3f N = (p3 - p1) ^ (p2 - p1);
	N = N.Normalize();
	vcg::Point3f R = (N ^ F).Normalize();
	vcg::Point3f center = -(p1 + p2 + p3) * 0.333;

	vcg::Matrix44f matrix, transMatrix;
	matrix.SetIdentity();
	matrix[0][0] = F[0];
	matrix[0][1] = F[1];
	matrix[0][2] = F[2];

	matrix[1][0] = R[0];
	matrix[1][1] = R[1];
	matrix[1][2] = R[2];

	matrix[2][0] = N[0];
	matrix[2][1] = N[1];
	matrix[2][2] = N[2];

	transMatrix.SetTranslate(center);
	matrix = matrix * transMatrix;

	//遍历所有mesh
	for (int i = 0; i < VCG_models.size(); ++i) {
		//std::cout << "align " << i << " model \n";
		CMeshO* current_teeth_mesh = VCG_models.at(i)->vcg_teeth_mesh;
		if (current_teeth_mesh) {
			vcg::tri::UpdatePosition<CMeshO>::Matrix(*current_teeth_mesh, matrix, true);
		}
		CMeshO* current_gum_mesh   = VCG_models.at(i)->vcg_gum_mesh;
		if (current_gum_mesh) {
			vcg::tri::UpdatePosition<CMeshO>::Matrix(*current_gum_mesh, matrix, true);
		}
		
		//摆正到XOY平面
		vcg::Matrix44f& align_mat = current_gum_mesh ? matrixMoveAlignedModelToXOY(current_gum_mesh) : matrixMoveAlignedModelToXOY(current_teeth_mesh);
		if (current_gum_mesh) {
			vcg::tri::UpdatePosition<CMeshO>::Matrix(*current_gum_mesh, align_mat, true);
		}
		if (current_teeth_mesh) {
			vcg::tri::UpdatePosition<CMeshO>::Matrix(*current_teeth_mesh, align_mat, true);
		}
			
		//更新vcg tree
		CMeshO* jaw_mesh = new CMeshO();
		if (current_gum_mesh) {
			vcg::tri::Append<CMeshO, CMeshO>::Mesh(*jaw_mesh, *current_gum_mesh);
		}
		if (current_teeth_mesh) {
			vcg::tri::Append<CMeshO, CMeshO>::Mesh(*jaw_mesh, *current_teeth_mesh);
		}
		AABBTree* current_tree = new AABBTree();
		current_tree->Set(jaw_mesh->face.begin(), jaw_mesh->face.end());
		VCG_models.at(i)->vcg_tree = current_tree;

		//摆正要显示的模型
		sn3DMeshModel* data_model = m_models.GetModel(i);
		sn3DMeshData* data = data_model->GetMeshData();
		int verticesNum = data->n_vertices();
		
		for (int j = 0; j < verticesNum; ++j) {
			vcg::Point3f p(data->V(j)->P().X(), data->V(j)->P().Y(), data->V(j)->P().Z());
			p = align_mat * matrix * p;
			data->V(j)->P().X() = p.X();
			data->V(j)->P().Y() = p.Y();
			data->V(j)->P().Z() = p.Z();
		}
	}

	current_model->m_alignPoints.clear();

	SetSenceModel(m_models.GetModel(m_currentID));

	pView->GetMainScene()->SetViewMode(Common::VIEWPOINT_ALIGN);
	pView->viewport()->update();
}

void OrthoCurve::OnPickAlignPoints()
{
	sn3DMeshModel* model = (sn3DMeshModel*)pView->GetMainScene()->GetObject(0);
	if (model == 0)
		return;

	//防止内存泄露
	if (!m_editPickAlignPoints) {
		m_editPickAlignPoints = new EditPickAlignPoints(model);
	}

	if (m_fixture == 3 || m_fixture == 4) {
		m_editPickAlignPoints->loadBaseModel(2);
	}
	else {
		if (m_need_qr) {
			m_editPickAlignPoints->loadBaseModel(0);
		}
		else {
			m_editPickAlignPoints->loadBaseModel(1);
		}
	}
	CustomInteractor* interactor = pView->GetCustomInteractor();
	interactor->SetEdit(m_editPickAlignPoints);

	QString message;
	message = QString("双击选择三个点，用于摆正模型");
	ShowMessage(message);
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

void ThermoBaseDrawWidget::paintEvent(QPaintEvent* event)
{
	float scale_factor = 5.0;
	float radius = scale_factor * 50.0;
	QWidget::paintEvent(event);

	//画坐标系和圆
	//构建坐标系路径
	QPainterPath path_arrow;
	path_arrow.moveTo(20, 10);
	path_arrow.lineTo(60, 10);

	path_arrow.moveTo(60, 10);
	path_arrow.lineTo(56.46, 6.46);

	path_arrow.moveTo(60, 10);
	path_arrow.lineTo(56.45, 13.54);
	
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QPen pen;
	pen.setWidth(2.0);

	pen.setColor(QColor(255, 0, 0));
	painter.setPen(pen);
	painter.drawPath(path_arrow);
	painter.drawText(65, 10, "X");
	painter.drawText(465,10, "直径105mm");

	pen.setColor(QColor(0, 255, 0));
	painter.setPen(pen);
	QTransform transform_arrow;
	transform_arrow.translate(20, 10);
	transform_arrow.rotate(90);
	transform_arrow.translate(-20, -10);
	painter.drawPath(transform_arrow.map(path_arrow));
	painter.drawText(20, 65, "Y");
	painter.drawText(465, 30, "直径110mm");

	pen.setColor(QColor(0, 0, 255));
	painter.setPen(pen);
	painter.drawText(465, 50, "直径115mm");

	painter.translate(width() * 0.5, height() * 0.5); //将画笔移到绘制区域中间
	pen.setColor(QColor(255, 0, 0));
	painter.setPen(pen);
	painter.drawEllipse(QPointF(0, 0), radius, radius);

	pen.setColor(QColor(0, 255, 0));
	painter.setPen(pen);
	painter.drawEllipse(QPointF(0, 0), radius + 2.5 * scale_factor, radius + 2.5 * scale_factor);

	pen.setColor(QColor(0, 0, 255));
	painter.setPen(pen);
	painter.drawEllipse(QPointF(0, 0), radius + 5 * scale_factor, radius + 5 * scale_factor);

	std::vector<QPoint> A_draw(A.size());
	std::vector<QPoint> B_draw(B.size());
	std::vector<QPoint> fixture_A_draw(fixture_A.size());
	std::vector<QPoint> fixture_B_draw(fixture_B.size());

	QTransform transform_A;
	transform_A.translate(center_A.x(), center_A.y());
	transform_A.rotate(rotate_A);
	transform_A.translate(-center_A.x(), -center_A.y());

	QTransform transform_B;
	transform_B.translate(center_B.x(), center_B.y());
	transform_B.rotate(rotate_B);
	transform_B.translate(-center_B.x(), -center_B.y());

	for (int i = 0; i < A.size(); ++i) {
		A_draw[i] = (A[i] * transform_A + trans_A) * scale_factor;
	}
	for (int i = 0; i < B.size(); ++i) {
		B_draw[i] = (B[i] * transform_B + trans_B) * scale_factor;
	}

	for (int i = 0; i < fixture_A.size(); ++i) {
		fixture_A_draw[i] = fixture_A[i] * scale_factor;
	}
	for (int i = 0; i < fixture_B.size(); ++i) {
		fixture_B_draw[i] = fixture_B[i] * scale_factor;
	}

	pen.setColor(QColor(255, 0, 0, 255));
	painter.setPen(pen);
	painter.drawPoints(A_draw.data(), A_draw.size());
	painter.drawPoints(B_draw.data(), B_draw.size());

	painter.drawPoints(fixture_A_draw.data(), fixture_A_draw.size());
	painter.drawPoints(fixture_B_draw.data(), fixture_B_draw.size());

	painter.end();
}
