#include"MeshClean.h"
#include"Graphics/sn3DTopology.h"

using namespace Common;

void  MeshClean::RemoveNonManifold(sn3DMeshData * mesh)
{
	Topology::VertexFace(*mesh);
	Topology::FaceFace(*mesh);
	
	int fn = mesh->fn;
	for (int i = 0; i<fn; i++)
	{
		mesh->F(i)->ClearV();
	}

	int regionID = 0;
	std::vector<int> regionScale; // Face联通区域计数

	std::vector<int> faceID; // 面分类
	faceID.resize(fn);

	//========  分析区域联通性
	int startIndex; // 搜索起点 
	do {
		//========== 独立的连通区域
		startIndex = -1;
		for (int i = 0; i<fn; i++)
		{
			if (!mesh->F(i)->IsV())
			{
				startIndex = i;
				break;
			}
		}

		if (startIndex == -1)break;  // 搜索结束

		//======= 搜索联通区域 ==========
		std::vector<Face*> region;
		mesh->F(startIndex)->SetV(); // 防止重复
		region.push_back(mesh->F(startIndex));

		int size = (int)region.size();
		int searchIndex = 0;
		do {
			for (; searchIndex < size; searchIndex++)
			{
				Face * f = region[searchIndex];
				for (int i = 0; i < 3; i++) // 使用面面邻接关系，找出顶点连接的非流形
				{
					if (!f->FFp(i)->IsV())
					{
						region.push_back(f->FFp(i)); // 搜索
						f->FFp(i)->SetV();
					}
				}
			} // front advance

			size = (int)region.size();
		} while (searchIndex<size);

		//====== 记录区域信息
		for (int i = 0; i < region.size(); i++)
		{
			faceID[region[i]->Index()] = regionID;
		}
		regionScale.push_back(region.size());
		regionID++;
	} while (1);

	//=========  保留最大联通区域 regionID  faceID  regionScale
	int cnt = (int)regionScale.size();
	if (cnt <= 1) // 只有一个连通区域
		return;

	int max_num = 0;
	int max_region = -1; //最大联通区域

	for (int i = 0; i<cnt; i++)
	{
		if (regionScale[i]>max_num)
		{
			max_num = regionScale[i];
			max_region = i;
		}
	}

	for (int i = 0; i<fn; i++)
	{
		if (faceID[i] != max_region)
		{
			mesh->F(i)->SetD(); // 删除
		}
	}

	mesh->Compact(); // 只保留一个联通区域
}
void  MeshClean::RemoveDegenerateFaces(sn3DMeshData * data)
{
	bool process = false;

	int fn = data->fn;
	
	for(int i=0;i<fn;i++)
	{
	  Face * f = data->F(i);
	  if (f->V(0) == f->V(1)) // 重复
	  {
		  f->SetD();
		  process = true;
	  }
	  if (f->V(0) == f->V(2))
	  {
		  f->SetD();
		  process = true;
	  }
	  if (f->V(2) == f->V(1))
	  {
		  f->SetD();
		  process = true;
	  }
	}

	if(process)
	data->Compact();
}
void  MeshClean::RemoveIsolatedRegion(sn3DMeshData * mesh)
{
	bool process = false;

	Topology::VertexFace(*mesh);

	int fn = mesh->fn;
	for (int i = 0; i<fn; i++)
	{
		mesh->F(i)->ClearV();
	}

	int regionID = 0;
	std::vector<int> regionScale; // Face联通区域计数

	std::vector<int> faceID; // 面分类
	faceID.resize(fn);

	//========  分析区域联通性
	int startIndex; // 搜索起点 
	do {
		//========== 独立的连通区域
		startIndex = -1;
		for (int i = 0; i<fn; i++)
		{
			if (!mesh->F(i)->IsV())
			{
				startIndex = i;
				break;
			}
		}

		if (startIndex == -1)break;  // 搜索结束
		//======= 搜索联通区域 ==========
		std::vector<Face*> region;
		mesh->F(startIndex)->SetV(); // 控制重复
		region.push_back(mesh->F(startIndex));

		int size = (int)region.size();
		int searchIndex = 0;
		do {
			for (; searchIndex < size; searchIndex++)
			{
				Face * f = region[searchIndex];
				for (int i = 0; i < 3; i++)
				{
					VFIterator iter(f->V(i));
					for (; !iter.End(); ++iter)
					{
						for (int j = 0; j < 3; j++)
							if (!iter.f->IsV())
							{
								region.push_back(iter.f); // 搜索
								iter.f->SetV();
							}
					}
				}
			} // front advance
			size = (int)region.size();
		} while (searchIndex<size);

		//====== 记录区域信息
		for (int i = 0; i < region.size();i++)
		{
			faceID[region[i]->Index()] = regionID;
		}
		regionScale.push_back(region.size());
		regionID++;
	} while (1);

	//=========  保留最大联通区域 regionID  faceID  regionScale
	int cnt = (int)regionScale.size();

	if (cnt <= 1) // 只有一个连通区域
		return;

	int max_num = 0; 
	int max_region = -1; //最大联通区域

	for (int i = 0; i<cnt; i++)
	{
		if (regionScale[i]>max_num)
		{
			max_num = regionScale[i];
			max_region = i;
		}
	}

	for (int i = 0; i<fn; i++)
	{
		if (faceID[i] != max_region)
		{
			mesh->F(i)->SetD(); // 删除
		}
	}

	//---------- 删除孤立顶点 ---------
	int vn = mesh->vn;
	for (int i=0;i<vn;i++)
	{
		Vertex * v = mesh->V(i);
		VFIterator vfi(v);
		int fcnt = 0;
		for (;!vfi.End();++vfi)
		{
			if (!vfi.f->IsD())
				fcnt++;
		}
		if (fcnt == 0)
			v->SetD();
	}

    mesh->Compact(); // 只保留一个联通区域
}
void  MeshClean::RemoveUnreferencedVertex(sn3DMeshData * data)
{
	bool process = false;

	for (int i = 0; i<data->vn; i++)
		data->V(i)->ClearV();

	for (int i = 0; i < data->fn; i++) 
	{
		if (data->F(i)->IsD())continue;
		data->F(i)->V(0)->SetV();
		data->F(i)->V(1)->SetV();
		data->F(i)->V(2)->SetV();
	}

	for (int i = 0; i < data->vn; i++)
	{
		if (!data->V(i)->IsV())  // 顶点未被引用
		{
			data->V(i)->SetD();
			process = true;
		}
	}
	for (int i = 0; i<data->vn; i++)
		data->V(i)->ClearV();

	if (process)
		data->Compact();
}
