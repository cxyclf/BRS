// h2h.cpp : Defines the entry point for the console application.
//
#include "h2h.h"


double file_size(const char* filename)  
{  
    struct stat statbuf;  
    stat(filename,&statbuf);  
    long size=statbuf.st_size;  
  
    return size/1000.0/1000.0;  
}

enum METHOD{
	DegreeOnly = 0,
	LB = 1,
	PotentialHeight = 2,
	NoIncDeg = 3,
	CutVersion = 4,
	COOR = 5,
};

class Tree_Decomposition{
public:
	int g_Samples;
	int g_bRoLCA;
	Tree_Decomposition(ReadFile & rf) :original_pvl(&rf.vertexList), original_pel(&rf.edgeList), pvl(NULL), pel(NULL), g_Samples(1000000),g_bRoLCA(false)
	{
	}

	void run(int method)
	{
		initial();

		switch (method)
		{
		case DegreeOnly:
			treeDecOnlyDegree();
			break;
		default:
			treeDecOnlyDegree();
			break;
		}

		maketree();
	}

	map<char, double> mResult;

	VertexList * pvl;
	EdgeList * pel;

	VertexList * original_pvl;
	EdgeList * original_pel;

	vector<VertexInH> vh;
	int iVNum;
	EdgeList eh;
	vector<TreeNode> TG;
	vector<int> pi;
	int order;
	int root;
	int maxHeight;

	LCA lca;

	vector<int> vGoodBranch;
	vector<double> vBetterBranch;
	vector<double> vProbBranch;

	void initial()
	{
		pvl = original_pvl;
		pel = original_pel;

		iVNum = pvl->size();
		vh.clear();
		vh.resize(0);
		//copy vl to vh
		for (int ii = 0; ii < iVNum; ++ii)
		{
			VertexInH temp(pvl->at(ii));
			vh.push_back(temp);

			//TG[ii].iReal = ii;
		}

		eh.clear();

		eh.copy(*pel);
		//dealTriangle();//iVnum is updated.

		TG.resize(iVNum);
		pi.resize(iVNum, -1);
		order = 0;
		root = 0;

		lca.initial(TG, iVNum);
	}

	void treeDecOnlyDegree()
	{
		//ofstream fout("debug.txt");
		MinHeap minheap;
		minheap.initial(iVNum);

		for (int ii = 0; ii < iVNum; ++ii)
		{
			if (!vh[ii].deleted)
				minheap.Push(vh[ii].edge_list.size(), ii);
		}

		while (minheap.count)
		{

			int vid;
			minheap.Pop(vid);

			//delete the vertex in h
			pi[vid] = order;
			++order;
			TreeNode & tn = TG[vid];

			vh[vid].removeMe(vh, eh, tn, false);

			//update the degree of its neighbors
			for (int jj = 0; jj < tn.neighbors.size(); ++jj)
			{
				int vneighbor = tn.neighbors[jj];
				int degree = vh[vneighbor].edge_list.size();

				int pos = minheap.index[vneighbor];
				minheap.UpdateKey(pos, degree);
			}
		}
	}

	void maketree()
	{
		int width = 0;
		//start from the last deleted vertex, which is the root
		vector<int> ord(iVNum, 0);
		for (int ii = 0; ii < iVNum; ++ii)
		{
			ord[pi[ii]] = ii; //ord[jj] = ii : the vertex ii are the jj=pi[ii]-th deleted vertex
		}
		for (int jj = iVNum - 1; jj >= 0; --jj)
		{
			int v = ord[jj];

			TreeNode & tn = TG[v];

			int iNumNeighbor = tn.neighbors.size();
			if (iNumNeighbor > width)
				width = iNumNeighbor;

			if (iNumNeighbor > 0)
			{
				int least = iVNum;
				int u = 0;
				for (int jj = 0; jj < iNumNeighbor; ++jj)
				{
					int vneighbor = tn.neighbors[jj];
					if (pi[vneighbor] < least)
					{
						least = pi[vneighbor];
						u = vneighbor;
					}
				}
				tn.parent = u;
				TG[u].children.push_back(v);
				TG[v].height = TG[u].height + 1;
			}
			else
			{
				root = v;
				TG[root].height = 0;
#ifdef SHOW_DETAIL
				cout << "root = " << root << endl;
#else
				cout << root << '\t';
#endif
			}
		}

		int leaves = 0;
		int branches=0;
		maxHeight = 0;
		for (int v = 0; v < iVNum; ++v)
			if (TG[v].children.size() == 0)
			{
				++leaves;
				if (TG[v].height > maxHeight)
					maxHeight = TG[v].height;
				TG[v].type=TYPE_D1;
			}
			else if(TG[v].children.size() == 1)
			{
				TG[v].type=TYPE_D1;
			}
			else
			{
				++branches;
			}
#ifdef SHOW_DETAIL
		cout << "leaves = " << leaves << endl;
		cout << "height = " << maxHeight << endl;
		cout << "width = " << width << endl;
#else
		cout << "branches = " << branches << endl;
		cout << leaves << '\t' << maxHeight << '\t' << width << '\t';
#endif
	}

	void calDisAndPos(int iRoot)
	{
		//should not be called before the height is set correctly
		//calculate the distance to each ancestors

		TreeNode & tn = TG[iRoot];

		tn.vToAncestor.resize(tn.height + 1);
		tn.vToAncestor[tn.height] = 0;
		vector<int> & vNeighbor = tn.neighbors;
		vector<LENGTH> & vToNeighbor = tn.VL;
		int ancestor = tn.parent;
		while (ancestor != -1)
		{
			//calculate the distance to the ancestor
			//enumerate all pathes through the neighbors
			LENGTH minDis = INFINITY;
			int posOfAncestor = TG[ancestor].height;

			for (int ii = 0; ii < vNeighbor.size(); ++ii)
			{
				int nvid = vNeighbor[ii];
				LENGTH temp = vToNeighbor[ii];
				if (posOfAncestor < TG[nvid].vToAncestor.size())
					temp += TG[nvid].vToAncestor[posOfAncestor];
				else
				{
					int posOfNvid = TG[nvid].height;
					temp += TG[ancestor].vToAncestor[posOfNvid];
				}
				if (temp < minDis)
					minDis = temp;
			}

			tn.vToAncestor[posOfAncestor] = minDis;

			ancestor = TG[ancestor].parent;
		}

		//set the pos be the height of the neighbors
		vector<int> & vPos = tn.pos;
		vPos.resize(vNeighbor.size()+1);
		for (int ii = 0; ii < vNeighbor.size(); ++ii)
			vPos[ii] = TG[vNeighbor[ii]].height;
		vPos[vNeighbor.size()] = tn.height;
		//recursive on children
		for (int ii = 0; ii < tn.children.size(); ++ii)
			calDisAndPos(tn.children[ii]);
	}

	void RoLCA(int iRoot)
	{
		TreeNode & tn = TG[iRoot];
		vector<int> & vChildren = tn.children;

		int nChildren = vChildren.size();
		if (nChildren == 0)
			return;
		if (nChildren == 1)
			RoLCA(vChildren[0]);
		else
		{			
			if(nChildren == 2)
			{
				int c1 = vChildren[0], c2 = vChildren[1];
				int c = c1;
				if(TG[c1].pos.size() > TG[c2].pos.size())
					c = c2;
				if(TG[c].pos.size() > 0)
				{
					tn.pos.resize(0);
					tn.pos.clear();
					tn.pos = TG[c].pos;

					tn.pos.resize(TG[c].pos.size()-1);
				}
			}
			else
			{
				int iMaxIndex = 0;
				int iMaxSize = 0;
				for (int ii = 0; ii < nChildren; ++ii)
				{
					int nvid = vChildren[ii];
					const int iSize = TG[nvid].neighbors.size();
					if(iSize > iMaxSize)
					{
						iMaxIndex = ii;
						iMaxSize = iSize;
					}
				}
				set<int> sTemp;
				for (int ii = 0; ii < nChildren; ++ii)
				{
					if(ii == iMaxIndex)
						continue;
					int nvid = vChildren[ii];
					const vector<int> & vn = TG[nvid].neighbors;
					const int iSize = vn.size();

					for(int jj=0; jj<iSize; ++jj)
						sTemp.insert(vn[jj]);
				}
				const int iSizeOld = tn.pos.size();
				if(tn.pos.size() > sTemp.size())
				{
					tn.pos.resize(0);
					tn.pos.clear();
					for(set<int>::iterator iter = sTemp.begin(); iter != sTemp.end(); ++iter)
						tn.pos.push_back(TG[*iter].height);
					//	tn.pos.push_back(*iter);
				}
				if(tn.pos.size() > iSizeOld)
				{
					cout << tn.pos.size()<<","<<iSizeOld<<","<<sTemp.size()<<endl;
					int hhh; cin >> hhh;
				}
			}

			//backup the children before recursive called because the children maybe changed.
			vector<int> vBackUpChildren(vChildren);
			for (vector<int>::iterator citer = vBackUpChildren.begin(); citer != vBackUpChildren.end(); ++citer)
			{
				RoLCA(*citer);
			}
		}
	}

	void setCountDescendant()
	{
		//start from the last deleted vertex, which is the root
		vector<int> ord(iVNum, 0);
		for (int ii = 0; ii < iVNum; ++ii)
		{
			ord[pi[ii]] = ii; //ord[jj] = ii : the vertex ii are the jj=pi[ii]-th deleted vertex
		}
		for (int jj = 0; jj < iVNum; ++jj)
		{
			int vid = ord[jj];


			TreeNode & tn = TG[vid];
			if (tn.children.size() == 0)
				continue;

			int sum = 1;//the node itself
			for (vector<int>::iterator iter = tn.children.begin(); iter != tn.children.end(); ++iter)
			{
				sum += TG[*iter].iCountDescendant;
			}
			tn.iCountDescendant = sum;
		}

		//cout << "root has descendants: " << TG[root].iCountDescendant << " |V|=" << iVNum << endl;
	}

	void resetHeight(int nid)
	{
		//nid = TG[nid].iReal;

		int p = TG[nid].parent;
		if (p != -1)
			TG[nid].height = TG[p].height + 1;
		else
			TG[nid].height = 0;

		for (vector<int>::iterator iter = TG[nid].children.begin(); iter != TG[nid].children.end(); ++iter)
			resetHeight(*iter);
	}


	void write(string fileOut)
	{
		FILE * fout;
		fopen_s(&fout, fileOut.c_str(), "wb");
		//fout = fopen(fileOut.c_str(), "wb");
		fwrite(&iVNum, sizeof(int), 1, fout);

		char a = 'a';
		fwrite(&a, sizeof(char), 1, fout);

		for (int ii = 0; ii < iVNum; ++ii)
			TG[ii].printMe(TG, ii, fout);

		fwrite(&a, sizeof(char), 1, fout);

		//write the part for lca
		//fwrite(lca.toRMQ.data(), sizeof(int), iVNum, fout);
		fwrite(lca.toRMQ, sizeof(int), iVNum, fout);

		int iRow = lca.RMQIndex.size();
		fwrite(&iRow, sizeof(int), 1, fout);
		for (int ii = 0; ii < iRow; ++ii)
		{
			int iCol = lca.RMQIndex[ii].size();
			fwrite(&iCol, sizeof(int), 1, fout);
			fwrite(lca.RMQIndex[ii].data(), sizeof(int), iCol, fout);
		}


		fwrite(&a, sizeof(char), 1, fout);
		fclose(fout);
	}

	void setBranchHeight(int bid, int bheight)
	{
		const int iSize = TG[bid].children.size();
		if(iSize > 1)
		{
			TG[bid].branch_height = bheight;
			for(vector<int>::iterator iter = TG[bid].children.begin(); iter != TG[bid].children.end(); ++iter)
			{
				setBranchHeight(*iter, bheight+1);
			}
		}
		else if(iSize == 1)
		{
			setBranchHeight(TG[bid].children[0], bheight);
		}
	}

	void output(string dataset, ofstream & fout)
	{
		fout << dataset << '\t';
		fout << mResult['o'] <<'\t' << mResult['r'] << '\t' << mResult['p'] << '\t';
		fout << mResult['a'] << '\t' << mResult['w'] << '\t' << mResult['t'] << '\t' << mResult['m'] << '\t';
		fout << mResult['L'] << '\t' << mResult['R'] << '\t' << mResult['P'] << endl;
	}
};
double Node::s_outputThredhold = 0.5;

int buildIndex(string city)
{
	int method = 0;//
	bool bRoLCA = true;
	Node::s_outputThredhold = 0;

	string dataname(city + ".road-d");
	string coordinate(city + ".co");

	string filename(dataname);

	clock_t start = clock();

	ReadFile rf(filename);

	rf.readRoadNet(filename);

	char indexName[100];
	sprintf_s(indexName, "%s.index", city.c_str());

	if(method == COOR)
		rf.readCoordinate(coordinate);

	//rf.edgeList.component(47437, rf.vertexList);

	Tree_Decomposition td(rf);
	//td.testD(method);
	
	td.g_bRoLCA = bRoLCA;
	td.run(method);
	td.calDisAndPos(td.root);
	if(bRoLCA)
		td.RoLCA(td.root);
	
	td.setBranchHeight(td.root, 0);

	td.lca.makeRMQ(td.root);

	td.write(indexName);

	return 0;
}
