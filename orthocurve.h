#ifndef ORTHOCURVE_H
#define ORTHOCURVE_H

#include <QtWidgets/QMainWindow>
#include <QProgressDialog>
#include "ui_orthocurve.h"
#include <QFile>

#include "GraphicsScene.h"
#include "OpenGLView.h"
#include "Graphics\sn3dmeshmodel.h"
#include "BatchModels.h"
#include "editManuallyDrawCurve.h"

#include "vcgmesh.h"
#include "ui_fileNameDialog.h"
#include "ui_thermobase.h"

#include "qrcode/qrencode.h"

#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export.h>

#define CHINESE_VERSION

class EditDrawCurve;
class EditMarkIcon;
class EditPickAlignPoints;

typedef vcg::AABBBinaryTreeIndex<CFaceO, float, vcg::EmptyClass> AABBTree;

/************************************************************************/
/*  根据一个二维码的二维矩阵生成相应的三维Mesh模型
@param QString information 要生成二维码的内容
@param len double 二维码长和宽
@param height double 二维码高度
*/
/************************************************************************/
static CMeshO* generateQRMesh(QString& information, double length, double height, int level = 0)
{
	//1.先生成二维码对应的灰度矩阵
	std::string information_str = information.toLocal8Bit().data();
	QRcode* qr = QRcode_encodeString(information_str.c_str(), 1, static_cast<QRecLevel>(level), QR_MODE_8, 1);
	int qr_len = qr->width;
	double len = length / qr_len;

	std::vector<std::vector<bool>> code(qr_len); //二维码对应的灰度矩阵
	for (int i = 0; i < qr_len; ++i) {
		code[i].resize(qr_len, false);
	}

	for (int y = 0; y < qr_len; y++) {
		for (int x = 0; x < qr_len; x++) {
			unsigned char b = qr->data[y * qr_len + x];
			if (b & 0x01) {
				code[y][x] = true;
			}
			else {
				code[y][x] = false;
			}
		}
	}
	QRcode_free(qr);

	//2.确定Mesh的点
	std::vector<std::vector<int>> vertices; //点矩阵(len + 1) X (len + 1)
	int VN = 0;
	vertices.resize(qr_len + 1);
	for (int i = 0; i < qr_len + 1; ++i) {
		vertices[i].resize(qr_len + 1, -1);
	}

	for (int i = 0; i < qr_len; ++i) {
		for (int j = 0; j < qr_len; ++j) {
			if (code[i][j]) {
				vertices[i][j] = vertices[i + 1][j] = vertices[i + 1][j + 1] = vertices[i][j + 1] = 1;
			}
		}
	}

	for (int i = 0; i < qr_len + 1; ++i) {
		for (int j = 0; j < qr_len + 1; ++j) {
			if (vertices[i][j] > -1) {
				vertices[i][j] = VN;
				VN += 2;
			}
		}
	}

	//3.再确定Mesh面
	int FN = 0;
	for (int i = 0; i < qr_len; ++i) {
		for (int j = 0; j < qr_len; ++j) {
			if (code[i][j]) {
				FN += 4;//顶面和底面共四个三角面片

				// 侧面的三角面片
				if (j + 1 < qr_len && !code[i][j + 1] || j + 1 == qr_len) {
					FN += 2;
				}

				if (j > 0 && !code[i][j - 1] || j == 0) {
					FN += 2;
				}

				if (i > 0 && !code[i - 1][j] || i == 0) {
					FN += 2;
				}

				if (i + 1 < qr_len && !code[i + 1][j] || i + 1 == qr_len) {
					FN += 2;
				}
			}
		}
	}

	//4.构建QR Mesh
	CMeshO* QRMesh = new CMeshO();
	CMeshO::VertexIterator newVertIndex = vcg::tri::Allocator<CMeshO>::AddVertices(*QRMesh, VN);
	for (int i = 0; i < vertices.size(); ++i) {
		for (int j = 0; j < vertices.size(); ++j) {
			if (vertices[i][j] > -1) {
				newVertIndex->P()[0] = i * len;
				newVertIndex->P()[1] = j * len;
				newVertIndex->P()[2] = 0.0;
				newVertIndex++;

				newVertIndex->P()[0] = i * len;
				newVertIndex->P()[1] = j * len;
				newVertIndex->P()[2] = height;
				newVertIndex++;
			}
		}
	}

	CMeshO::FaceIterator newFaceIndex = vcg::tri::Allocator<CMeshO>::AddFaces(*QRMesh, FN);
	for (int i = 0; i < qr_len; ++i) {
		for (int j = 0; j < qr_len; ++j) {
			if (code[i][j]) {
				//立方体的8个顶点
				CVertexO* p0, * p1, * p2, * p3, * p0_, * p1_, * p2_, * p3_;
				p0 = &*(vertices[i][j] + QRMesh->vert.begin());
				p0_ = &*(vertices[i][j] + 1 + QRMesh->vert.begin());
				p1 = &*(vertices[i + 1][j] + QRMesh->vert.begin());
				p1_ = &*(vertices[i + 1][j] + 1 + QRMesh->vert.begin());
				p2 = &*(vertices[i + 1][j + 1] + QRMesh->vert.begin());
				p2_ = &*(vertices[i + 1][j + 1] + 1 + QRMesh->vert.begin());
				p3 = &*(vertices[i][j + 1] + QRMesh->vert.begin());
				p3_ = &*(vertices[i][j + 1] + 1 + QRMesh->vert.begin());

				newFaceIndex->V(0) = p0;
				newFaceIndex->V(1) = p3;
				newFaceIndex->V(2) = p1;
				newFaceIndex++;

				newFaceIndex->V(0) = p3;
				newFaceIndex->V(1) = p2;
				newFaceIndex->V(2) = p1;
				newFaceIndex++;

				newFaceIndex->V(0) = p0_;
				newFaceIndex->V(1) = p1_;
				newFaceIndex->V(2) = p3_;
				newFaceIndex++;

				newFaceIndex->V(0) = p3_;
				newFaceIndex->V(1) = p1_;
				newFaceIndex->V(2) = p2_;
				newFaceIndex++;


				// 侧面的三角面片
				if (j + 1 == qr_len || !code[i][j + 1]) {
					newFaceIndex->V(0) = p3_;
					newFaceIndex->V(1) = p2;
					newFaceIndex->V(2) = p3;
					newFaceIndex++;

					newFaceIndex->V(0) = p3_;
					newFaceIndex->V(1) = p2_;
					newFaceIndex->V(2) = p2;
					newFaceIndex++;
				}

				if (j == 0 || !code[i][j - 1]) {
					newFaceIndex->V(0) = p0_;
					newFaceIndex->V(1) = p0;
					newFaceIndex->V(2) = p1;
					newFaceIndex++;

					newFaceIndex->V(0) = p0_;
					newFaceIndex->V(1) = p1;
					newFaceIndex->V(2) = p1_;
					newFaceIndex++;
				}

				if (i > 0 && !code[i - 1][j] || i == 0) {
					newFaceIndex->V(0) = p3_;
					newFaceIndex->V(1) = p3;
					newFaceIndex->V(2) = p0;
					newFaceIndex++;

					newFaceIndex->V(0) = p3_;
					newFaceIndex->V(1) = p0;
					newFaceIndex->V(2) = p0_;
					newFaceIndex++;
				}

				if (i + 1 < qr_len && !code[i + 1][j] || i + 1 == qr_len) {
					newFaceIndex->V(0) = p2_;
					newFaceIndex->V(1) = p1;
					newFaceIndex->V(2) = p2;
					newFaceIndex++;

					newFaceIndex->V(0) = p2_;
					newFaceIndex->V(1) = p1_;
					newFaceIndex->V(2) = p1;
					newFaceIndex++;
				}
			}
		}
	}

	return QRMesh;
}

/************************************************************************/
/*  生成底部的数字信息模型
@param information QString 信息字符串（支持26个大小写字母和_-）
*/
/************************************************************************/
static CMeshO* generateInfoMesh(QString& information, bool need_qr = true)
{
	CMeshO* info_mesh = new CMeshO();
	QString dir = QCoreApplication::applicationDirPath();

	QRegularExpression upper_case_re("[A-Z]");
	QRegularExpression lower_case_re("[a-z-_]");
	bool need_rotate = false;

	// 位数必须小于18位
	for (int i = 0; i < information.size() && i <= 17; ++i)
	{
		QString character = information[i];
		need_rotate = false; // 字母和数字的初始位置不一样要做旋转

		//识别字符且区分大小写
		QRegularExpressionMatch match_upper = upper_case_re.match(character);
		QRegularExpressionMatch match_lower = lower_case_re.match(character);
		if (match_upper.hasMatch()) {
			character += '_';
			need_rotate = true;
		}
		else if (match_lower.hasMatch()) {
			need_rotate = true;
		}

		QString character_path = dir + "/models/" + character + ".obj";
		std::string character_path_str = character_path.toLocal8Bit().constData();

		CMeshO* character_mesh = new CMeshO();
		int load_mask_temp;
		int open_result = vcg::tri::io::ImporterOBJ<CMeshO>::Open(*character_mesh, character_path_str.c_str(), load_mask_temp);
		if (open_result != 0) {
			std::cout << vcg::tri::io::ExporterSTL<CMeshO>::ErrorMsg(open_result);
		}

		float scale_z = 1.5;
		vcg::Matrix44f initial_transform_matrix;
		if (need_rotate) {
			vcg::Matrix44f initial_rotate_matrix1, initial_rotate_matrix2;
			vcg::Matrix44f trans_z;
			initial_rotate_matrix1.SetRotateDeg(-90, vcg::Point3f(0.0, 0.0, 1.0));
			initial_rotate_matrix2.SetRotateDeg(180, vcg::Point3f(1.0, 0.0, 0.0));
			trans_z.SetTranslate(0.0, 0.0, 1.0);
			initial_transform_matrix = trans_z * initial_rotate_matrix2 * initial_rotate_matrix1;
		}
		else {
			initial_transform_matrix.SetIdentity();
		}

		vcg::Matrix44f rotate_matrix;
		rotate_matrix.SetRotateDeg(180, vcg::Point3f(1.0, 0.0, 0.0));
		vcg::Matrix44f trans_matrix;
		trans_matrix.SetIdentity();

		double y_pos[6];
		double start = need_qr ? 7.5 : 10;
		double space = need_qr ? 3 : 3.5;
		for (int y_i = 0; y_i <= 5; y_i++) {
			y_pos[y_i] = start - space * y_i;
		}

		double trans_x;
		double trans_z = 2.6;
		if (need_qr) {
			trans_x = 23;
			switch (i) {
			case 0:trans_matrix.SetScale(0.4, 0.48, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[0];	 trans_matrix[2][3] = trans_z; break;
			case 1:trans_matrix.SetScale(0.4, 0.48, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[1];	 trans_matrix[2][3] = trans_z; break;
			case 2:trans_matrix.SetScale(0.4, 0.48, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[2];	 trans_matrix[2][3] = trans_z; break;
			case 3:trans_matrix.SetScale(0.4, 0.48, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[3];	 trans_matrix[2][3] = trans_z; break;
			case 4:trans_matrix.SetScale(0.4, 0.48, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[4];	 trans_matrix[2][3] = trans_z; break;
			case 5:trans_matrix.SetScale(0.4, 0.48, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[5];	 trans_matrix[2][3] = trans_z; break;

			case 6:trans_matrix.SetScale(0.4, 0.48, scale_z);   trans_matrix[0][3] = -13.0 + trans_x; trans_matrix[1][3] = y_pos[0]; trans_matrix[2][3] = trans_z; break;
			case 7:trans_matrix.SetScale(0.4, 0.48, scale_z);   trans_matrix[0][3] = -13.0 + trans_x; trans_matrix[1][3] = y_pos[1]; trans_matrix[2][3] = trans_z; break;
			case 8:trans_matrix.SetScale(0.4, 0.48, scale_z);   trans_matrix[0][3] = -13.0 + trans_x; trans_matrix[1][3] = y_pos[2]; trans_matrix[2][3] = trans_z; break;
			case 9:trans_matrix.SetScale(0.4, 0.48, scale_z);   trans_matrix[0][3] = -13.0 + trans_x; trans_matrix[1][3] = y_pos[3]; trans_matrix[2][3] = trans_z; break;
			case 10:trans_matrix.SetScale(0.4, 0.48, scale_z);  trans_matrix[0][3] = -13.0 + trans_x; trans_matrix[1][3] = y_pos[4]; trans_matrix[2][3] = trans_z; break;
			case 11:trans_matrix.SetScale(0.4, 0.48, scale_z);  trans_matrix[0][3] = -13.0 + trans_x; trans_matrix[1][3] = y_pos[5]; trans_matrix[2][3] = trans_z; break;

			case 12:trans_matrix.SetScale(0.4, 0.48, scale_z); trans_matrix[0][3] = -16.0 + trans_x; trans_matrix[1][3] = y_pos[0]; trans_matrix[2][3] = trans_z; break;
			case 13:trans_matrix.SetScale(0.4, 0.48, scale_z); trans_matrix[0][3] = -16.0 + trans_x; trans_matrix[1][3] = y_pos[1]; trans_matrix[2][3] = trans_z; break;
			case 14:trans_matrix.SetScale(0.4, 0.48, scale_z); trans_matrix[0][3] = -16.0 + trans_x; trans_matrix[1][3] = y_pos[2]; trans_matrix[2][3] = trans_z; break;
			case 15:trans_matrix.SetScale(0.4, 0.48, scale_z); trans_matrix[0][3] = -16.0 + trans_x; trans_matrix[1][3] = y_pos[3]; trans_matrix[2][3] = trans_z; break;
			case 16:trans_matrix.SetScale(0.4, 0.48, scale_z); trans_matrix[0][3] = -16.0 + trans_x; trans_matrix[1][3] = y_pos[4]; trans_matrix[2][3] = trans_z; break;
			case 17:trans_matrix.SetScale(0.4, 0.48, scale_z); trans_matrix[0][3] = -16.0 + trans_x; trans_matrix[1][3] = y_pos[5]; trans_matrix[2][3] = trans_z; break;
			}
		}
		else {
			trans_x = 21;
			trans_z = 2.0;
			scale_z = 1.0;
			switch (i) {
			case 0:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[0];	 trans_matrix[2][3] = trans_z; break;
			case 1:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[1];	 trans_matrix[2][3] = trans_z; break;
			case 2:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[2];	 trans_matrix[2][3] = trans_z; break;
			case 3:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[3];	 trans_matrix[2][3] = trans_z; break;
			case 4:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[4];	 trans_matrix[2][3] = trans_z; break;
			case 5:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[5];	 trans_matrix[2][3] = trans_z; break;

			case 6:trans_matrix.SetScale(0.5, 0.6, scale_z);  trans_matrix[0][3] = -14.0 + trans_x; trans_matrix[1][3] = y_pos[0]; trans_matrix[2][3]  = trans_z; break;
			case 7:trans_matrix.SetScale(0.5, 0.6, scale_z);  trans_matrix[0][3] = -14.0 + trans_x; trans_matrix[1][3] = y_pos[1]; trans_matrix[2][3]  = trans_z; break;
			case 8:trans_matrix.SetScale(0.5, 0.6, scale_z);  trans_matrix[0][3] = -14.0 + trans_x; trans_matrix[1][3] = y_pos[2]; trans_matrix[2][3]  = trans_z; break;
			case 9:trans_matrix.SetScale(0.5, 0.6, scale_z);  trans_matrix[0][3] = -14.0 + trans_x; trans_matrix[1][3] = y_pos[3]; trans_matrix[2][3]  = trans_z; break;
			case 10:trans_matrix.SetScale(0.5, 0.6, scale_z);  trans_matrix[0][3] = -14.0 + trans_x; trans_matrix[1][3] = y_pos[4]; trans_matrix[2][3] = trans_z; break;
			case 11:trans_matrix.SetScale(0.5, 0.6, scale_z);  trans_matrix[0][3] = -14.0 + trans_x; trans_matrix[1][3] = y_pos[5]; trans_matrix[2][3] = trans_z; break;

			case 12:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -18.0 + trans_x; trans_matrix[1][3] = y_pos[0]; trans_matrix[2][3] = trans_z; break;
			case 13:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -18.0 + trans_x; trans_matrix[1][3] = y_pos[1]; trans_matrix[2][3] = trans_z; break;
			case 14:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -18.0 + trans_x; trans_matrix[1][3] = y_pos[2]; trans_matrix[2][3] = trans_z; break;
			case 15:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -18.0 + trans_x; trans_matrix[1][3] = y_pos[3]; trans_matrix[2][3] = trans_z; break;
			case 16:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -18.0 + trans_x; trans_matrix[1][3] = y_pos[4]; trans_matrix[2][3] = trans_z; break;
			case 17:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -18.0 + trans_x; trans_matrix[1][3] = y_pos[5]; trans_matrix[2][3] = trans_z; break;
			}
		}

		vcg::tri::UpdatePosition<CMeshO>::Matrix(*character_mesh, trans_matrix * rotate_matrix * initial_transform_matrix, true);
		vcg::tri::Append<CMeshO, CMeshO>::Mesh(*info_mesh, *character_mesh);

		delete character_mesh;
	}
	return info_mesh;
}

static CMeshO* generateInfoMesh_V3(QString& information) {
	CMeshO* info_mesh = new CMeshO();
	QString dir = QCoreApplication::applicationDirPath();

	QRegularExpression upper_case_re("[A-Z]");
	QRegularExpression lower_case_re("[a-z-_]");
	bool need_rotate = false;

	// 位数必须小于12位
	for (int i = 0; i < information.size() && i <= 11; ++i)
	{
		QString character = information[i];
		need_rotate = false; // 字母和数字的初始位置不一样要做旋转

		//识别字符且区分大小写
		QRegularExpressionMatch match_upper = upper_case_re.match(character);
		QRegularExpressionMatch match_lower = lower_case_re.match(character);
		if (match_upper.hasMatch()) {
			character += '_';
			need_rotate = true;
		}
		else if (match_lower.hasMatch()) {
			need_rotate = true;
		}

		QString character_path = dir + "/models/" + character + ".obj";
		std::string character_path_str = character_path.toLocal8Bit().constData();

		CMeshO* character_mesh = new CMeshO();
		int load_mask_temp;
		int open_result = vcg::tri::io::ImporterOBJ<CMeshO>::Open(*character_mesh, character_path_str.c_str(), load_mask_temp);
		if (open_result != 0) {
			std::cout << vcg::tri::io::ExporterSTL<CMeshO>::ErrorMsg(open_result);
		}

		float scale_z = 1.0;
		vcg::Matrix44f initial_transform_matrix;
		if (need_rotate) {
			vcg::Matrix44f initial_rotate_matrix1, initial_rotate_matrix2;
			vcg::Matrix44f trans_z;
			initial_rotate_matrix1.SetRotateDeg(-90, vcg::Point3f(0.0, 0.0, 1.0));
			initial_rotate_matrix2.SetRotateDeg(180, vcg::Point3f(1.0, 0.0, 0.0));
			trans_z.SetTranslate(0.0, 0.0, 1.0);
			initial_transform_matrix = trans_z * initial_rotate_matrix2 * initial_rotate_matrix1;
		}
		else {
			initial_transform_matrix.SetIdentity();
		}

		vcg::Matrix44f rotate_matrix;
		rotate_matrix.SetRotateDeg(180, vcg::Point3f(1.0, 0.0, 0.0));
		vcg::Matrix44f trans_matrix;
		trans_matrix.SetIdentity();

		double y_pos[6];
		double start = 8.5;
		double space = 3.5;
		for (int y_i = 0; y_i <= 5; y_i++) {
			y_pos[y_i] = start - space * y_i;
		}

		double trans_x = 13.7;
		double trans_z = 2.0;
		switch (i) {
		case 0:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[0];	 trans_matrix[2][3] = trans_z; break;
		case 1:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[1];	 trans_matrix[2][3] = trans_z; break;
		case 2:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[2];	 trans_matrix[2][3] = trans_z; break;
		case 3:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[3];	 trans_matrix[2][3] = trans_z; break;
		case 4:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[4];	 trans_matrix[2][3] = trans_z; break;
		case 5:trans_matrix.SetScale(0.5, 0.6, scale_z); trans_matrix[0][3] = -10 + trans_x; trans_matrix[1][3] = y_pos[5];	 trans_matrix[2][3] = trans_z; break;

		case 6:trans_matrix.SetScale(0.5, 0.6, scale_z);   trans_matrix[0][3] = -14 + trans_x; trans_matrix[1][3] = y_pos[0] - 5.0; trans_matrix[2][3] = trans_z; break;
		case 7:trans_matrix.SetScale(0.5, 0.6, scale_z);   trans_matrix[0][3] = -14 + trans_x; trans_matrix[1][3] = y_pos[1] - 5.0; trans_matrix[2][3] = trans_z; break;
		case 8:trans_matrix.SetScale(0.5, 0.6, scale_z);   trans_matrix[0][3] = -14 + trans_x; trans_matrix[1][3] = y_pos[2] - 5.0; trans_matrix[2][3] = trans_z; break;
		case 9:trans_matrix.SetScale(0.5, 0.6, scale_z);   trans_matrix[0][3] = -14 + trans_x; trans_matrix[1][3] = y_pos[3] - 5.0; trans_matrix[2][3] = trans_z; break;
		case 10:trans_matrix.SetScale(0.5, 0.6, scale_z);  trans_matrix[0][3] = -14 + trans_x; trans_matrix[1][3] = y_pos[4] - 5.0; trans_matrix[2][3] = trans_z; break;
		case 11:trans_matrix.SetScale(0.5, 0.6, scale_z);  trans_matrix[0][3] = -14 + trans_x; trans_matrix[1][3] = y_pos[5] - 5.0; trans_matrix[2][3] = trans_z; break;
		}

		vcg::tri::UpdatePosition<CMeshO>::Matrix(*character_mesh, trans_matrix * rotate_matrix * initial_transform_matrix, true);
		vcg::tri::Append<CMeshO, CMeshO>::Mesh(*info_mesh, *character_mesh);

		delete character_mesh;
	}
	return info_mesh;
}

static void mergeCMeshWithMCUTData(CMeshO* cmesh, std::vector<float>& vertices, std::vector<uint32_t>& faces) {
	if (!cmesh || vertices.empty() || faces.empty()) {
		return;
	}

	int origin_vert_count = cmesh->vert.size();
	CMeshO::VertexIterator vit = vcg::tri::Allocator<CMeshO>::AddVertices(*cmesh, vertices.size() / 3);
	CMeshO::FaceIterator   fit = vcg::tri::Allocator<CMeshO>::AddFaces(*cmesh, faces.size() / 3);

	for (int i = 0; i < vertices.size() / 3; ++i, ++vit) {
		vit->P()[0] = vertices.at(i * 3);
		vit->P()[1] = vertices.at(i * 3 + 1);
		vit->P()[2] = vertices.at(i * 3 + 2);
	}

	for (int i = 0; i < faces.size() / 3; ++i, ++fit) {
		fit->V(0) = &*(cmesh->vert.begin() + origin_vert_count + faces.at(i * 3));
		fit->V(1) = &*(cmesh->vert.begin() + origin_vert_count + faces.at(i * 3 + 1));
		fit->V(2) = &*(cmesh->vert.begin() + origin_vert_count + faces.at(i * 3 + 2));
	}
}

struct _VCGModel {
	CMeshO* vcg_gum_mesh	= nullptr;
	CMeshO* vcg_teeth_mesh  = nullptr;
	AABBTree* vcg_tree      = nullptr;
	QString path;// 存放模型的位置
	int step = -1; // 当前模型所属的步骤

	~_VCGModel() {
		delete vcg_tree;
		vcg_tree = nullptr;

		delete vcg_gum_mesh;
		vcg_gum_mesh = nullptr;

		delete vcg_teeth_mesh;
		vcg_teeth_mesh = nullptr;
	}
};

class ThermoBaseDrawWidget : public QWidget {
public:
	ThermoBaseDrawWidget(QWidget* parent = nullptr) :QWidget(parent) {
		center_A.setX(0.0);
		center_A.setY(0.0);

		center_B.setX(0.0);
		center_B.setY(0.0);

		trans_A.setX(0.0);
		trans_A.setY(0.0);

		trans_B.setX(0.0);
		trans_B.setY(0.0);

		trans_A_origin.setX(0.0);
		trans_A_origin.setY(0.0);

		trans_B_origin.setX(0.0);
		trans_B_origin.setY(0.0);

		rotate_A = 0.0;
		rotate_B = 0.0;
		rotate_A_origin = 0.0;
		rotate_B_origin = 0.0;
	};
	std::vector<QPoint> A, B;
	std::vector<QPoint> fixture_A, fixture_B;

	QPoint trans_A, trans_B;
	QPoint trans_A_origin, trans_B_origin; 
	QPoint center_A, center_B;
	float rotate_A, rotate_B;
	float rotate_A_origin, rotate_B_origin;

protected:
	void paintEvent(QPaintEvent* event);
};

class OrthoCurve : public QMainWindow
{
	Q_OBJECT

public:
	OrthoCurve(QWidget* parent = 0);
	~OrthoCurve();

	void Init();
	OpenGLView* CreateOpenGLView();
	OpenGLView* pView;

	QProgressDialog* projectCurveProgressDialog = nullptr;
	Ui::FileNameDialog* filename_ui = nullptr;
	QDialog* filename_dialog		= nullptr;

	Ui::ThermoBaseDialog twin_model_ui;
	QDialog* twin_model_dialog					= nullptr;
	ThermoBaseDrawWidget* twin_model_widget		= nullptr;

protected:
	void LoadSetting();
	void SaveSetting();
	void ShowMessage(QString message);
	void SetTileFileName(std::string name, int modelCount, int currentIndex);
	void SetSenceModel(sn3DMeshModel* model);
	void DoComputeCurve(sn3DMeshModel* model);
	void ResetSenceEdit();

	float m_thresholdZ;//过滤底板和文字
	int   m_normalZ;
	int   m_offsetSetp;
	float m_interval;
	int   m_smooth;
	bool  m_saveNormal;
	bool  m_withMark;
	QString m_savePath;
	QString m_importPath;
	int m_degreefb;
	int m_cutDegreeOut;
	int m_cutDegreeInsideFront;
	int m_cutDegreeInsideBack;
	bool m_showBur;
	double m_cutOffset;
	bool m_isStaubli;
	int m_curveture;

	bool m_need_qr;
	int m_fixture; //0.robotic 1.V1 2.V2
	int m_qr_correction;
	float m_qr_width;
	 
	bool is_3shape = false;
	int current_jaw = 0; // 0:maxilliary 1:mandibular

	QString _3shape_models_input_path;
	QString patient_id;

	BatchModels m_models;
	std::vector<_VCGModel*> VCG_models; //用于牙模排序和后续查找生成牙龈线的牙龈模型
	CMeshO* base_pin_model = nullptr;

	int  m_currentID = 0;
	bool m_isManual; //记录是自动生成模式，还是手动生成牙龈线模式
	EditDrawCurve* m_editCurve;
	EditMarkIcon*  m_editMark;
	EditPickAlignPoints* m_editPickAlignPoints;
	EditManuallyDrawCurve* m_editManuallyCurve;

protected:
	QToolButton* createToolButton(const QString& toolTip, const QIcon& icon, const char* member);
	int updateButtonGeometry(QToolButton* button, int x, int y);
	int updateViewSideButtonGeometry(QToolButton* button, int x, int y);
	void resizeEvent(QResizeEvent* event) override;

	QToolButton* prevButton;
	QToolButton* nextButton;

	std::vector<QToolButton*> viewSideButtons;

private:
	Ui::OrthoCurveClass ui;
private slots:

	void computeOneCutline3Shape(const int models_id);
	bool computeOneCutline(const int models_id);
	void read3shapeModels();//when import maxilliary jaw = 0 else jaw = 1
	void releaseAllModels();
	void changeFileName(QString& patient_id);
	CMeshO* doSubstract(CMeshO* src_mesh, CMeshO* target_mesh);
	CMeshO* generateBaseModel(int model_id);
	void SaveModels(QString path);

	void OnImportSTL();
	void OnImportSTLs();
	void OnImport3shapeData();

	void OnComputeCurve();
	void OnAdjustDegree();
	void OnSaveCurve();
	void OnSetting();
	void OnPreview();
	void OnNext();

	void OnUp();
	void OnDown();
	void OnLeft();
	void OnRight();
	void OnFront();
	void OnBack();

	void OnShowTwinModel();
	void OnChangeModel();
	void OnTransXorY();
	void OnRotateZ();

	void OnDeleteCurrentCurve();
	void OnMarkIcon();
	void OnComputeIntersections();

	void OnAlignModel();
	void OnPickAlignPoints();
};

#endif // ORTHOCURVE_H
