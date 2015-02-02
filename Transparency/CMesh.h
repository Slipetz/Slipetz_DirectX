//Based on Frank Luna's Book
#ifndef _CMesh_HG_
#define _CMesh_HG_


#include "global.h"

class CMesh
{
public:
	CMesh();
	virtual ~CMesh();
	bool CreateGrid(float width, float depth, UINT m, UINT n);
	bool CreateBox(float width, float height, float depth);
	bool CreateSphere(float radius, UINT sliceCount, UINT stackCount);

	std::wstring GetGridName();
	std::wstring GetBoxName();
	std::wstring GetSphereName();

private:
	std::wstring _gridName;
	std::wstring _boxName;
	std::wstring _sphereName;
};

#endif