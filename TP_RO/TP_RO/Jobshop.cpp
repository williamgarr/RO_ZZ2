#include "Jobshop.h"

void Jobshop::solve(string filename, bool display)
{
	const int MAX_SIZE = 300;
	const int INF = 100000;
	string name;
	int n;
	int m;

	int M[MAX_SIZE - 1][MAX_SIZE - 1];
	int P[MAX_SIZE - 1][MAX_SIZE - 1];

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
				file >> M[i][j];
				file >> P[i][j];
			}
		}
		file.close();
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
		
	}
//FIN LECTURE
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

	cout << "GLPK version : " << glp_version() << endl;

	glp_prob* lp; 
	int ia[5001], ja[5001];
	double ar[5001], z, v;
	ostringstream oss;

	//création pb glpk
	lp = glp_create_prob();
	glp_set_prob_name(lp, "JobShop");
	glp_set_obj_dir(lp, GLP_MIN);

	int CompteurContrainte = 0;
	int CompteurCol = 0;
	int CompteurIA = 0;

	string nomcol;
	string nomcontrainte;
	int position;


//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////	VARIABLES	////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
	
	//Ordre de passage sur les machines
	for (int i = 0; i < n; i++)////////////	BLOC 2	 ////////////
	{
		for (int j = 0; j < m; j++)
		{
			for (int k = 0; k < n; k++)
			{
				for (int l = 0; l < m; l++)
				{
					if (M[i][j] == M[k][l] && i != k && j != l)
					{
						oss.str("");
						oss << "order_" << i << '_' << j << '_' << k << '_' << l;
						nomcol = oss.str();
						glp_add_cols(lp, 1);
						CompteurCol++;
						glp_set_col_name(lp, CompteurCol, nomcol.c_str());
						glp_set_col_bnds(lp, CompteurCol, GLP_LO, 0, 0);
					}
				}
			}
		}
	}

	//Temps sur la machine
	for (int i = 0; i < n; i++)////////////	BLOC 3	 ////////////
	{
		for (int j = 0; j < m - 1; j++)
		{
			oss.str("");
			oss << "dur_" << i << '_' << j;
			nomcol = oss.str();
			glp_add_cols(lp, 1);
			CompteurCol++;
			glp_set_col_name(lp, CompteurCol, nomcol.c_str());
			glp_set_col_bnds(lp, CompteurCol, GLP_LO, 0, 0);
		}
	}

	// Starting time
	for (int i = 0; i < n; i++)////////////	BLOC 1	 ////////////
	{
		for (int j = 0; j < m; j++)
		{
			oss.str("");
			oss << "stime_" << i << "_" << j;
			nomcol = oss.str();
			glp_add_cols(lp, 1);
			CompteurCol++;
			glp_set_col_name(lp, CompteurCol, nomcol.c_str());
			glp_set_col_bnds(lp, CompteurCol, GLP_LO, 0, 0);
		}
	}


	glp_create_index(lp);


////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////	CONTRAINTES	///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

	// Ordre de passage sur les machines
	for (int i = 0; i < n; i++)////////////	BLOC C6	 ////////////
	{
		for (int j = 0; j < m; j++)
		{
			for (int k = 0; k < n; k++)
			{
				for (int l = 0; l < m; l++)
				{
					if (M[i][j] == M[k][l] && i != k && j != l)
					{
						// Première contrainte //del
						glp_add_rows(lp, 1);
						CompteurContrainte++;

						oss.str("");
						oss << "c_" << CompteurContrainte;
						nomcontrainte = oss.str();
						glp_set_row_name(lp, CompteurContrainte, nomcontrainte.c_str());
						v = INF - P[i][j];
						glp_set_row_bnds(lp, CompteurContrainte, GLP_UP, 0, v);
						oss.str("");
						oss << "stime_" << k << '_' << l;
						nomcol = oss.str();
						position = glp_find_col(lp, nomcol.c_str());
						CompteurIA++;
						ia[CompteurIA] = CompteurContrainte;
						ja[CompteurIA] = position;
						ar[CompteurIA] = -1.0;
						oss.str("");
						oss << "order_" << i << '_' << j << '_' << k << '_' << l;
						nomcol = oss.str();
						position = glp_find_col(lp, nomcol.c_str());
						CompteurIA++;
						ia[CompteurIA] = CompteurContrainte;
						ja[CompteurIA] = position;
						ar[CompteurIA] = INF;
						oss.str("");
						oss << "stime_" << i << '_' << j;
						nomcol = oss.str();
						position = glp_find_col(lp, nomcol.c_str());
						CompteurIA++;
						ia[CompteurIA] = CompteurContrainte;
						ja[CompteurIA] = position;
						ar[CompteurIA] = 1.0;

						// Deuxième contrainte //del
						glp_add_rows(lp, 1);
						CompteurContrainte++;
						oss.str("");
						oss << "c_" << CompteurContrainte;
						nomcontrainte = oss.str();
						glp_set_row_name(lp, CompteurContrainte, nomcontrainte.c_str());
						glp_set_row_bnds(lp, CompteurContrainte, GLP_LO, P[k][l], 0);

						oss.str("");
						oss << "stime_" << k << '_' << l;
						nomcol = oss.str();
						position = glp_find_col(lp, nomcol.c_str());
						CompteurIA++;
						ia[CompteurIA] = CompteurContrainte;
						ja[CompteurIA] = position;
						ar[CompteurIA] = -1.0;

						oss.str("");
						oss << "order_" << i << '_' << j << '_' << k << '_' << l;
						nomcol = oss.str();
						position = glp_find_col(lp, nomcol.c_str());
						CompteurIA++;
						ia[CompteurIA] = CompteurContrainte;
						ja[CompteurIA] = position;
						ar[CompteurIA] = INF;

						oss.str("");
						oss << "stime_" << i << '_' << j;
						nomcol = oss.str();
						position = glp_find_col(lp, nomcol.c_str());
						CompteurIA++;
						ia[CompteurIA] = CompteurContrainte;
						ja[CompteurIA] = position;
						ar[CompteurIA] = 1.0;
					}
				}
			}
		}
	}

	  // 1 - définition des variables qij
	// -q[i][j] + St[i][j+1] - St[i][j] = 0
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m-1; j++)////////////	BLOC C4	 ////////////
		{
			glp_add_rows(lp, 1);
			CompteurContrainte++;

			oss.str("");
			oss << "c_" << CompteurContrainte;
			nomcontrainte = oss.str();
			glp_set_row_name(lp, CompteurContrainte, nomcontrainte.c_str());
			glp_set_row_bnds(lp, CompteurContrainte, GLP_FX, 0.0, 0.0);

			oss.str("");
			oss << "dur_" << i << '_' << j;
			nomcol = oss.str();
			position = glp_find_col(lp, nomcol.c_str());
			CompteurIA++;
			ia[CompteurIA] = CompteurContrainte;
			ja[CompteurIA] = position;
			ar[CompteurIA] = -1.0;


			oss.str("");
			oss << "stime_" << i << '_' << j;
			nomcol = oss.str();
			position = glp_find_col(lp, nomcol.c_str());
			CompteurIA++;
			ia[CompteurIA] = CompteurContrainte;
			ja[CompteurIA] = position;
			ar[CompteurIA] = -1.0;

			oss.str("");
			oss << "stime_" << i << '_' << j + 1;
			nomcol = oss.str();
			position = glp_find_col(lp, nomcol.c_str());
			CompteurIA++;
			ia[CompteurIA] = CompteurContrainte;
			ja[CompteurIA] = position;
			ar[CompteurIA] = 1.0;
		}
	}

	// 2 - Contraintes q[i][j] >= t[i][j]
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m - 1; j++)////////////	BLOC C5	 ////////////
		{
			glp_add_rows(lp, 1);
			CompteurContrainte++;

			oss.str("");
			oss << "c_" << CompteurContrainte;
			nomcontrainte = oss.str();
			glp_set_row_name(lp, CompteurContrainte, nomcontrainte.c_str());
			glp_set_row_bnds(lp, CompteurContrainte, GLP_LO, P[i][j], 0);

			oss.str("");
			oss << "dur_" << i << '_' << j;
			nomcol = oss.str();
			position = glp_find_col(lp, nomcol.c_str());
			CompteurIA++;
			ia[CompteurIA] = CompteurContrainte;
			ja[CompteurIA] = position;
			ar[CompteurIA] = 1.0;
		}
	}


	// obj
	glp_add_cols(lp, 1); ////////////	BLOC C7	 ////////////
	CompteurCol++;
	glp_set_col_name(lp, CompteurCol, "Z");
	glp_set_col_bnds(lp, CompteurCol, GLP_LO, 0.0, 0.0);



	// creation Index pour les recherches
	glp_create_index(lp);

	// fonction objectif
	// définition des contraintes
	for (int i = 0; i < n; i++)
	{
		glp_add_rows(lp, 1);
		CompteurContrainte++;
		oss.str("");
		oss << "c_" << CompteurContrainte;
		nomcontrainte = oss.str();
		glp_set_row_name(lp, CompteurContrainte, nomcontrainte.c_str());
		glp_set_row_bnds(lp, CompteurContrainte, GLP_LO, P[i][m - 1], 0.0);

		position = glp_find_col(lp, "Z");
		CompteurIA++;
		ia[CompteurIA] = CompteurContrainte;
		ja[CompteurIA] = position;
		ar[CompteurIA] = 1;

		oss.str("");
		oss << "stime_" << i << '_' << m - 1;
		nomcol = oss.str();
		position = glp_find_col(lp, nomcol.c_str());
		CompteurIA++;
		ia[CompteurIA] = CompteurContrainte;
		ja[CompteurIA] = position;
		ar[CompteurIA] = -1;
	}

	// les coefficients de la fonction objectif
	////////////	BLOC 8	 ////////////

	position = glp_find_col(lp, "Z");
	glp_set_obj_coef(lp, position, 1);

	glp_load_matrix(lp, CompteurIA, ia, ja, ar);
	oss.str("");
	oss << "Jobshop_" << name << ".lp";
	glp_write_lp(lp, NULL, oss.str().c_str());

	glp_simplex(lp, NULL);

	z = glp_get_obj_val(lp);
	cout << "Makespan: " << z << endl;

	//// liberation mémoire //del
	glp_delete_prob(lp);//del
	glp_free_env();//del
}
