#ifndef _MeshClean_
#define _MeshClean_

#include"Graphics\sn3DMeshData.h"
using namespace Common;

class MeshClean
{
public:
  
	void  RemoveNonManifold(sn3DMeshData * model);

  void  RemoveDegenerateFaces(sn3DMeshData * model);    // 退化面

  void  RemoveUnreferencedVertex(sn3DMeshData * data);  // 未被引用顶点

  void  RemoveIsolatedRegion(sn3DMeshData * data);      //  去除孤立元素

};


#endif