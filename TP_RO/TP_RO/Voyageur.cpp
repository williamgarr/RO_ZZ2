#include "Voyageur.h"

void Voyageur::solve()
{
	const int n = 10; // nombre de noeuds (stations)
	// station 0 -> dépot
	int trajet[n][n];
	int x[n][n];
	// création des temps de trajet
	for (int i = 0; i < n; i++){
		int k = 1;
		for (int j = 0; j < n; j++){
			if (i == j)
				trajet[i][j] = 0;
			else
				trajet[i][j] = k*(i+j); // formule au hasard
		}
		k++;
	}

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			x[i][j] = 0;

	// affichage
	cout << "+++++ trajet +++++" << endl;
	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++){
			cout << trajet[i][j] << " ";
		}
		cout << endl;
	}

	// cycle de 2 à n-1 interdits (<=1)

	cout << "GLPK version : " << glp_version() << endl;

	glp_prob* lp; //définition nom problème
	int ia[5001], ja[5001];
	double ar[5001], z;
	ostringstream oss;

	//création pb glpk
	lp = glp_create_prob();
	glp_set_prob_name(lp, "Voyageur");
	glp_set_obj_dir(lp, GLP_MIN);

	// variables glpk
	string nomcol;
	string nomcontrainte;
	int CompteurCol = 0;            //nombre de colonnes
	int CompteurContrainte = 0;     //nombre de contraintes
	int CompteurIA = 0;             //nombre de ..
	int position;

	// Somme de ce qui part de i = 1
	for (int i = 0; i < n; i++){
		for (int j = 1; j < n; j++){
			oss.str("");
			oss << "i_" << i << '_' << j;
			nomcol = oss.str();
			glp_add_cols(lp, 1);
			CompteurCol++;
			glp_set_col_name(lp, CompteurCol, nomcol.c_str());
			glp_set_col_bnds(lp, CompteurCol, GLP_FX, 1.0, 1.0);
		}
	}

	// Somme de ce qui arrive à j = 1
	for (int j = 0; j < n; j++){
		for (int i = 1; i < n; i++){
			oss.str("");
			oss << "j_" << i << '_' << j;
			nomcol = oss.str();
			glp_add_cols(lp, 1);
			CompteurCol++;
			glp_set_col_name(lp, CompteurCol, nomcol.c_str());
			glp_set_col_bnds(lp, CompteurCol, GLP_FX, 1.0, 1.0);
		}
	}

	// x_ii = 0
	for (int i = 0; i < n; i++){
		oss.str("");
		oss << "ii_" << i;
		glp_add_cols(lp, 1);
		CompteurCol++;
		glp_set_col_name(lp, CompteurCol, nomcol.c_str());
		glp_set_col_bnds(lp, CompteurCol, GLP_FX, 0.0, 0.0);
	}
	glp_create_index(lp);
}