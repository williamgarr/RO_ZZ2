#include "Jobshop.h"


void Jobshop::solve(string filename, bool display)
{
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
	ostringstream oss;
	const int MAX_SIZE = 300;
	const int INF = 100000;
	//string name;
	int n = 0;
	int m = 0;
	int M[MAX_SIZE - 1][MAX_SIZE - 1];
	int P[MAX_SIZE - 1][MAX_SIZE - 1];

	// Lire les données de LaXX
	ifstream file(filename.c_str());
	if (!file.fail()){
		//name = filename;
		file >> n;
		file >> m;

		cout << "Filename = " << filename << endl;
		cout << "n = " << n << ", m = " << m << endl;
		for (int i = 0; i < n; i++){
			for (int j = 0; j < m; j++){
				file >> M[i][j]; //machine
				file >> P[i][j]; //temps ttt
			}
		}
		file.close();
	}

	//Display
	if (display){

		cout << "+++ M +++" << endl;
		for (int i = 0; i < n; i++){
			for (int j = 0; j < m; j++)
				cout << M[i][j] << " ";
			cout << endl;
		}
		cout << "+++ P +++" << endl;
		for (int i = 0; i < n; i++){
			for (int j = 0; j < m; j++)
				cout << P[i][j] << " ";
			cout << endl ;
		}
	}

	cout << "GLPK version : " << glp_version() << endl;

	glp_prob* lp; 

	double  z, v;

	//reduire si crash !
	int* ia = new int[15000];
	int* ja = new int[15000];
	double* ar = new double[15000];

	//pour swv17, mauvaise idée qui marche:
	//int* ia = new int[1000000];
	//int* ja = new int[1000000];
	//double* ar = new double[1000001];

	

	

	//création pb glpk
	lp = glp_create_prob();
	glp_set_prob_name(lp, "JobShop");
	glp_set_obj_dir(lp, GLP_MIN);

	int CompteurContrainte = 0;
	int CompteurCol = 0;
	int CompteurIA = 0;
	int position;

	//encodage ordre de passage:
	//i et k pour n 
	//j et l pour m
	for (int i = 0; i < n; i++){
		for (int j = 0; j < m; j++){
			for (int k = 0; k < n; k++){
				for (int l = 0; l < m; l++){
					//si machine identique pour pc différentes
					if (i != k && M[i][j] == M[k][l]) {
						oss.str(""); //tj vider oss sinon noms incohérents!
						oss << "order_" << i << '_' << j << '_' << k << '_' << l;
						CompteurCol++;
						glp_add_cols(lp, 1);
						glp_set_col_name(lp, CompteurCol, oss.str().c_str());
						glp_set_col_bnds(lp, CompteurCol, GLP_LO, 0, 0);
					}
				}
			}
		}
	}
	// Starting time
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			oss.str("");
			oss << "stime_" << i << "_" << j;
			CompteurCol++;
			glp_add_cols(lp, 1);
			glp_set_col_name(lp, CompteurCol, oss.str().c_str());
			glp_set_col_bnds(lp, CompteurCol, GLP_LO, 0, 0);
		}
	}
	//Temps sur la machine
	for (int i = 0; i < n; i++){
		for (int j = 0; j < m - 1; j++){
			oss.str("");
			oss << "dur_" << i << '_' << j;
			CompteurCol++;
			glp_add_cols(lp, 1);
			glp_set_col_name(lp, CompteurCol, oss.str().c_str());
			glp_set_col_bnds(lp, CompteurCol, GLP_LO, 0, 0);
		}
	}

	glp_create_index(lp);// avant glp_find_col sinon crash

	//contrainte ordre
	for (int i = 0; i < n; i++){
		for (int j = 0; j < m; j++){
			for (int k = 0; k < n; k++){
				for (int l = 0; l < m; l++){
					if (i != k && M[i][j] == M[k][l]) {

						//add row
						CompteurContrainte++; //attention increm!
						oss.str("");
						oss << "c_" << CompteurContrainte;
						v = INF - P[i][j];
						glp_add_rows(lp, 1); //GLP_UP
						glp_set_row_name(lp, CompteurContrainte, oss.str().c_str());
						glp_set_row_bnds(lp, CompteurContrainte, GLP_UP, 0, v);
						
						oss.str("");
						oss << "order_" << i << '_' << j << '_' << k << '_' << l;
						position = glp_find_col(lp, oss.str().c_str());
						CompteurIA++;
						ia[CompteurIA] = CompteurContrainte;
						ja[CompteurIA] = position;
						ar[CompteurIA] = INF;

						oss.str("");
						oss << "stime_" << i << '_' << j;
						position = glp_find_col(lp, oss.str().c_str());
						CompteurIA++;
						ia[CompteurIA] = CompteurContrainte;
						ja[CompteurIA] = position;
						ar[CompteurIA] = 1.0;

						oss.str("");
						oss << "stime_" << k << '_' << l;
						position = glp_find_col(lp, oss.str().c_str());
						CompteurIA++;
						ia[CompteurIA] = CompteurContrainte;
						ja[CompteurIA] = position;
						ar[CompteurIA] = -1.0;

						//add 2nd row
						CompteurContrainte++;
						oss.str("");
						oss << "c_" << CompteurContrainte;
						glp_add_rows(lp, 1);//GLP_LO
						glp_set_row_name(lp, CompteurContrainte, oss.str().c_str());
						glp_set_row_bnds(lp, CompteurContrainte, GLP_LO, P[k][l], 0);

						oss.str("");
						oss << "order_" << i << '_' << j << '_' << k << '_' << l;
						position = glp_find_col(lp, oss.str().c_str());
						CompteurIA++;
						ia[CompteurIA] = CompteurContrainte;
						ja[CompteurIA] = position;
						ar[CompteurIA] = INF;

						oss.str("");
						oss << "stime_" << i << '_' << j;
						position = glp_find_col(lp, oss.str().c_str());
						CompteurIA++;
						ia[CompteurIA] = CompteurContrainte;
						ja[CompteurIA] = position;
						ar[CompteurIA] = 1.0;

						oss.str("");
						oss << "stime_" << k << '_' << l;
						position = glp_find_col(lp, oss.str().c_str());
						CompteurIA++;
						ia[CompteurIA] = CompteurContrainte;
						ja[CompteurIA] = position;
						ar[CompteurIA] = -1.0;
					}
				}
			}
		}
	}

	//contrainte st1 - st0 -dur = 0 -> GLP_FX
	for (int i = 0; i < n; i++){
		for (int j = 0; j < m-1; j++){

			CompteurContrainte++; 
			oss.str("");
			oss << "c_" << CompteurContrainte;
			glp_add_rows(lp, 1);
			glp_set_row_name(lp, CompteurContrainte, oss.str().c_str());
			glp_set_row_bnds(lp, CompteurContrainte, GLP_FX, 0, 0);

			oss.str("");
			oss << "stime_" << i << '_' << j + 1;
			position = glp_find_col(lp, oss.str().c_str());
			CompteurIA++;
			ia[CompteurIA] = CompteurContrainte;
			ja[CompteurIA] = position;
			ar[CompteurIA] = 1;

			oss.str("");
			oss << "stime_" << i << '_' << j;
			position = glp_find_col(lp, oss.str().c_str());
			CompteurIA++;
			ia[CompteurIA] = CompteurContrainte;
			ja[CompteurIA] = position;
			ar[CompteurIA] = -1;

			oss.str("");
			oss << "dur_" << i << '_' << j;
			position = glp_find_col(lp, oss.str().c_str());
			CompteurIA++;
			ia[CompteurIA] = CompteurContrainte;
			ja[CompteurIA] = position;
			ar[CompteurIA] = -1;
		}
	}

	//contrainte : respecter P (temps ttt)
	for (int i = 0; i < n; i++){
		for (int j = 0; j < m - 1; j++){
			
			CompteurContrainte++;
			oss.str("");
			oss << "c_" << CompteurContrainte;
			glp_add_rows(lp, 1);
			glp_set_row_name(lp, CompteurContrainte, oss.str().c_str());
			glp_set_row_bnds(lp, CompteurContrainte, GLP_LO, P[i][j], 0);

			oss.str("");
			oss << "dur_" << i << '_' << j;
			position = glp_find_col(lp, oss.str().c_str());
			CompteurIA++;
			ia[CompteurIA] = CompteurContrainte;
			ja[CompteurIA] = position;
			ar[CompteurIA] = 1.0;
		}
	}

	// ajout col objectif
	CompteurCol++;
	glp_add_cols(lp, 1);
	glp_set_col_name(lp, CompteurCol, "z");
	glp_set_col_bnds(lp, CompteurCol, GLP_LO, 0, 0);
	glp_create_index(lp);//sinon crash


	for (int i = 0; i < n; i++){
		
		CompteurContrainte++;
		oss.str("");
		oss << "c_" << CompteurContrainte;
		glp_add_rows(lp, 1);
		glp_set_row_name(lp, CompteurContrainte, oss.str().c_str());
		glp_set_row_bnds(lp, CompteurContrainte, GLP_LO, P[i][m - 1], 0);//intellisense

		oss.str("");
		oss << "stime_" << i << '_' << m - 1;
		position = glp_find_col(lp, oss.str().c_str());
		CompteurIA++;
		ia[CompteurIA] = CompteurContrainte;
		ja[CompteurIA] = position;
		ar[CompteurIA] = -1;

		position = glp_find_col(lp, "z");
		CompteurIA++;
		ia[CompteurIA] = CompteurContrainte;
		ja[CompteurIA] = position;
		ar[CompteurIA] = 1;
	}

	//charger et lancer solveur
	oss.str("");
	oss << "js" << n << 'x' << m<<".lp";

	position = glp_find_col(lp, "z");
	glp_set_obj_coef(lp, position, 1);
	glp_load_matrix(lp, CompteurIA, ia, ja, ar);
	glp_write_lp(lp, NULL, oss.str().c_str());

	glp_simplex(lp, NULL);
	z = glp_get_obj_val(lp);
	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	cout << "Makespan: " << z << endl;
	long temps = chrono::duration_cast<chrono::milliseconds>(end - begin).count();
	cout << "Temps : " << temps<< " ms" << endl << endl;

}
