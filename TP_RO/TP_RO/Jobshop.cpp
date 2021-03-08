#include "Jobshop.h"

void Jobshop::solve(string filename, bool display)
{
	const int MAX_SIZE = 30;
	string name;
	int n;
	int m;

	int M[MAX_SIZE - 1][MAX_SIZE - 1];
	int P[MAX_SIZE - 1][MAX_SIZE - 1];
	int SommeDuree[MAX_SIZE - 1];
	int Tab[MAX_SIZE - 1][MAX_SIZE - 1];

	// Lire les données de LaXX
	ifstream file(filename.c_str());
	if (!file.fail())
	{
		name = filename;
		file >> n;
		file >> m;

		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				string lectureM, lectureP;
				file >> lectureM;
				file >> lectureP;
				M[i][j] = atoi(lectureM.c_str())+1;
				P[i][j] = atoi(lectureP.c_str());
			}
		}
		file.close();
	}

	// Somme des durées
	for (int i = 0; i < n; i++)
	{
		int sum = 0;
		for (int j = 0; j < m; j++)
			sum += P[i][j];
		SommeDuree[i] = sum;
	}

	// Tab
	int t = 0;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			Tab[i][j] = t;
			t++;
		}
	}

	// Affichage de la lecture
	if (display)
	{
		cout << "Filename = " << name << endl;
		cout << "n = " << n << ", m = " << m << endl;
		cout << "+++ M +++" << endl;
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
				cout << M[i][j] << " ";
			cout << endl;
		}
		cout << "+++ P +++" << endl;
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
				cout << P[i][j] << " ";
			cout << endl;
		}
		cout << "+++ SommeDuree +++" << endl;
		for (int i = 0; i < n; i++)
			cout << SommeDuree[i] << endl;
		cout << "+++ Tab +++" << endl;
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
				cout << Tab[i][j] << " ";
			cout << endl;
		}
	}

	cout << "GLPK version : " << glp_version() << endl;
	glp_prob* lp;
	int ia[1000], ja[1000];
	double ar[1000];
	ostringstream oss;

	lp = glp_create_prob();
	glp_set_prob_name(lp, "JobShop");
	glp_set_obj_dir(lp, GLP_MIN);

	// Contraintes starting time
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			int pos = Tab[i][j];
			oss.str("");
			oss << "St_" << i << "_" << j;

		}
	}
}
