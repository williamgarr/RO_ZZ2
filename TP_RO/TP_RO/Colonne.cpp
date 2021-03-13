#include "Colonne.h"

void Colonne::solve()
{
	int rouleau = 100; // 100dm
	int pattern[5];
	pattern[1] = 45; // 45dm
	pattern[2] = 36; // 36dm
	pattern[3] = 31; // 31dm
	pattern[4] = 14; // 14dm
	int commande[5];
	commande[1] = 97;
	commande[2] = 610;
	commande[3] = 395;
	commande[4] = 211;

	cout << "GLPK version : " << glp_version() << endl;

	glp_prob* lp; //définition nom problème
	int ia[5001], ja[5001];
	double ar[5001], z;
	ostringstream oss;

	//création pb glpk
	lp = glp_create_prob();
	glp_set_prob_name(lp, "Colonne");
	glp_set_obj_dir(lp, GLP_MIN);

	// variables glpk
	string nomcol;
	string nomcontrainte;
	int CompteurCol = 0;            //nombre de colonnes
	int CompteurContrainte = 0;     //nombre de contraintes
	int CompteurIA = 0;             //nombre de ..
	int position;

	// appliquer un algorithme de bin packing pour trouver la meilleur utilisation d'un rouleau (avec le - de pertes)
	// remplir la commande
}
