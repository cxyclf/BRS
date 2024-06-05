#pragma once
#include "h2h.h"

const LENGTH infinity = 999999999;
class QUERY
{
public:
	int* LOG2, * LOGD, ** RMQIndex;
	int* height;

	LENGTH* DX;
	LENGTH* DY;

	class NODE
	{
	public:
		NODE()
		{
			pos0 = pos1 = pos2 = pos3 = pos4 = pos5 = NULL;
			mToAncestor = NULL;
			pos0Size = pos1Size = pos2Size = pos3Size = pos4Size = pos5Size = 0;
		}

		int  pos0Size, pos1Size, pos2Size, pos3Size, pos4Size, pos5Size;
		int* pos0, * pos1, * pos2, * pos3, * pos4, * pos5;
		LENGTH* mToAncestor;
		int toRMQ;


		~NODE()
		{
			if (pos0)
				free(pos0);
			if (pos1)
				free(pos1);
			if (pos2)
				free(pos2);
			if (pos3)
				free(pos3);
			if (pos4)
				free(pos4);
			if (pos5)
				free(pos5);
			if (mToAncestor)
				free(mToAncestor);
		}

		void set(int size, FILE* fin)
		{
				pos0Size = size;
				pos0 = (int*)malloc(sizeof(int) * size);
				fread(pos0, sizeof(int), size, fin);

		}
	};

	NODE* vtree;
	int iVNum;
	int iRow;
	void readANode(FILE* fin)//refer to Node.printMe
	{
		//read the vertex id, node type
		int vid = 0, nodetype = 0;
		fread(&vid, sizeof(int), 1, fin);

		NODE& tn = vtree[vid];

		fread(&nodetype, sizeof(int), 1, fin);

		bool bFake = false;
		int tn_height;
		switch (nodetype)
		{
		case TYPE_D1://single child
		{
			fread(&tn_height, sizeof(int), 1, fin);
		}
		break;
		case TYPE_C2://lca
		{
			//tn.iReal = vid;
			//read the height, used in the pos to do query if the two query vertices have ancestor relationship
			//fread(&tn_branch_height, sizeof(int), 1, fin);
			fread(&tn_height, sizeof(int), 1, fin);

			int iPosSize = 0;
			fread(&iPosSize, sizeof(int), 1, fin);
			tn.set(iPosSize, fin);
		}
		break;
		default:
			assert(false);
			break;
		}
		char b;
		fread(&b, sizeof(char), 1, fin);
			int iDLSize = 0;
			fread(&iDLSize, sizeof(int), 1, fin);

			tn.mToAncestor = (LENGTH*)malloc(sizeof(LENGTH) * iDLSize);
			fread(tn.mToAncestor, sizeof(LENGTH), iDLSize, fin);

		height[vid] = tn_height;
	}



	void readIndex(string fileIndex)
	{
		FILE* fin;
		//fin = fopen(fileIndex.c_str(), "rb");
		fopen_s(&fin, fileIndex.c_str(), "rb");
		char a;
		//read the number of vertices
		fread(&iVNum, sizeof(int), 1, fin);

		vtree = (NODE*)malloc(sizeof(NODE) * iVNum);

		fread(&a, sizeof(char), 1, fin);
		//cout << a << endl;

		height = (int*)malloc(sizeof(int) * iVNum);
		//sizeBranch = (int *)malloc(sizeof(int)*iVNum);

		//read all nodes
		for (int ii = 0; ii < iVNum; ++ii)
		{
			readANode(fin);
		}
		//cout<<"finish read nodes"<<endl;
		fread(&a, sizeof(char), 1, fin);
		//cout << a << endl;
		//read the part for lca
		int* vtoRMQ = (int*)malloc(sizeof(int) * iVNum);
		fread(vtoRMQ, sizeof(int), iVNum, fin);
		for (int ii = 0; ii < iVNum; ++ii)
			vtree[ii].toRMQ = vtoRMQ[ii];
		free(vtoRMQ);


		fread(&iRow, sizeof(int), 1, fin);
		//cout << iRow <<endl;
		RMQIndex = (int**)malloc(sizeof(int*) * iRow);
		for (int ii = 0; ii < iRow; ++ii)
		{
			int iCol = 0;
			fread(&iCol, sizeof(int), 1, fin);
			RMQIndex[ii] = (int*)malloc(sizeof(int) * iCol);
			fread(RMQIndex[ii], sizeof(int), iCol, fin);
		}
		// cout<<"finish reading index"<<endl;
		fread(&a, sizeof(char), 1, fin);
		//cout << a << endl;
		fclose(fin);

		LOG2 = (int*)malloc(sizeof(int) * (iVNum * 2 + 10));
		LOGD = (int*)malloc(sizeof(int) * (iVNum * 2 + 10));
		int k = 0, j = 1;
		for (int i = 0; i < iVNum * 2 + 10; i++)
		{
			if (i > j * 2)
			{
				j *= 2;
				k++;
			}
			LOG2[i] = k;
			LOGD[i] = j;
		}
	}

	void freeall()
	{
		free(height);
		free(vtree);

		free(LOGD);
		free(LOG2);
		for (int ii = 0; ii < iRow; ++ii)
		{
			free(RMQIndex[ii]);
		}
		free(RMQIndex);
	}


	inline	int LCAQuery(int p, int q) {
		//int p = toRMQ[_p], q = toRMQ[_q];

		if (p > q) {
			int x = p;
			p = q;
			q = x;
		}
		int len = q - p + 1;

		int i = LOGD[len], k = LOG2[len];

		q = q - i + 1;

		if (height[RMQIndex[k][p]] < height[RMQIndex[k][q]])
			return RMQIndex[k][p];
		else return RMQIndex[k][q];
	}

	inline	LENGTH distanceQuery(int x, int y) {
		if (x == y) return 0;

		NODE& tnx = vtree[x];
		NODE& tny = vtree[y];
		int lca = LCAQuery(tnx.toRMQ, tny.toRMQ);

		if (lca == x)
		{

			return tny.mToAncestor[height[x]];
		}
		else if (lca == y)
		{

			return tnx.mToAncestor[height[y]];
		}
		else {
			NODE& tnc = vtree[lca];
			int ps = tnc.pos0Size;
			//cout << ps <<endl;
			int* p2 = tnc.pos0;


			LENGTH* dx = tnx.mToAncestor, * dy = tny.mToAncestor;

			LENGTH res = infinity;

			for (int i = 0; i < ps; i++) {
				LENGTH tmp = dx[p2[i]] + dy[p2[i]];
				if (res > tmp)
					res = tmp;
			}
			return res;
		}
	}
};