#ifndef _MeshClean_
#define _MeshClean_

#include"Graphics\sn3DMeshData.h"
using namespace Common;

class MeshClean
{
public:
  
	void  RemoveNonManifold(sn3DMeshData * model);

  void  RemoveDegenerateFaces(sn3DMeshData * model);    // �˻���

  void  RemoveUnreferencedVertex(sn3DMeshData * data);  // δ�����ö���

  void  RemoveIsolatedRegion(sn3DMeshData * data);      //  ȥ������Ԫ��

};


#endif