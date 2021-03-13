#include "Jobshop.h"

void Jobshop::solve(string filename, bool display)
{
	const int MAX_SIZE = 30;
    const int INF = 10000000;
	string name;
	int n;
	int m;

	int M[MAX_SIZE - 1][MAX_SIZE - 1];
	int P[MAX_SIZE - 1][MAX_SIZE - 1];
	//int SommeDuree[MAX_SIZE - 1];
	//int Tab[MAX_SIZE - 1][MAX_SIZE - 1];

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

	/* Somme des durées
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
	}*/
//fin lecture
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
		/*
		cout << "+++ SommeDuree +++" << endl;
		for (int i = 0; i < n; i++)
			cout << SommeDuree[i] << endl;
		cout << "+++ Tab +++" << endl;
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
				cout << Tab[i][j] << " ";
			cout << endl;
		}*/
	}
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
	cout << "GLPK version : " << glp_version() << endl;

	glp_prob* lp; //définition nom problème
	int ia[1000], ja[1000];
	double ar[1000], z, v;
	ostringstream oss;

	//création pb glpk
	lp = glp_create_prob();
	glp_set_prob_name(lp, "JobShop");
	glp_set_obj_dir(lp, GLP_MIN);

	// variables glpk
	string nomcol; 
    string nomcontrainte;
    int CompteurCol = 0;            //nombre de colonnes
    int CompteurContrainte = 0;     //nombre de contraintes
    int CompteurIA = 0;             //nombre de ..
    int position;

	// Contraintes starting time
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			oss.str("");
			oss << "St_" << i << "_" << j;
			nomcol = oss.str();
			glp_add_cols(lp, 1);
			CompteurCol++;
			glp_set_col_name(lp, CompteurCol, nomcol.c_str());
			//glp_set_col_bnds(lp, CompteurCol, GLP_LO, 0.0, 1.0);
			glp_set_col_bnds(lp, CompteurCol, GLP_DB, 0.0, 1.0);
		}
	}

	// ordre des pieces sur les machines
	for (int i = 0; i < n; i++)
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
						glp_set_col_bnds(lp, CompteurCol, GLP_LO, 0.0, 1.0);
					}
				}
			}
		}
	}

	// temps de traitement
	for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m-1; j++)
        {
			oss.str("");
            oss << "q_" << i << '_' << j;
            nomcol = oss.str();
            glp_add_cols(lp, 1);
            CompteurCol++;
            glp_set_col_name(lp, CompteurCol, nomcol.c_str());
            glp_set_col_bnds(lp, CompteurCol, GLP_LO, 0.0, 1.0);
        }
    }

	glp_create_index(lp);



	// Définition des variables :
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m-1; j++)
        {
            glp_add_rows(lp, 1);
            CompteurContrainte++;

            oss.str("");
            oss << "C_" << CompteurContrainte;
            nomcontrainte = oss.str();
            glp_set_row_name(lp, CompteurContrainte, nomcontrainte.c_str());
            glp_set_row_bnds(lp, CompteurContrainte, GLP_FX, 0.0, 0.0);

            oss.str("");
            oss << "q_" << i << '_' << j;
            nomcol = oss.str();
            position = glp_find_col(lp, nomcol.c_str());
            CompteurIA++;
            ia[CompteurIA] = CompteurContrainte;
            ja[CompteurIA] = position;
            ar[CompteurIA] = -1.0;

            oss.str("");
            oss << "St_" << i << '_' << j+1;
            nomcol = oss.str();
            position = glp_find_col(lp, nomcol.c_str());
            CompteurIA++;
            ia[CompteurIA] = CompteurContrainte;
            ja[CompteurIA] = position;
            ar[CompteurIA] = 1.0;

            oss.str("");
            oss << "St_" << i << '_' << j;
            nomcol = oss.str();
            position = glp_find_col(lp, nomcol.c_str());
            CompteurIA++;
            ia[CompteurIA] = CompteurContrainte;
            ja[CompteurIA] = position;
            ar[CompteurIA] = -1.0;
        }
    }

    // 2 - Contraintes q[i][j] >= t[i][j]
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m-1; j++)
        {
            glp_add_rows(lp, 1);
            CompteurContrainte++;

            oss.str("");
            oss << "C_" << CompteurContrainte;
            nomcontrainte = oss.str();
            glp_set_row_name(lp, CompteurContrainte, nomcontrainte.c_str());
            glp_set_row_bnds(lp, CompteurContrainte, GLP_LO, P[i][j], 0);

            oss.str("");
            oss << "q_" << i << '_' << j;
            nomcol = oss.str();
            position = glp_find_col(lp, nomcol.c_str());
            CompteurIA++;
            ia[CompteurIA] = CompteurContrainte;
            ja[CompteurIA] = position;
            ar[CompteurIA] = 1.0;
        }
    }

    // Ordre de passage sur les machines
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            for (int k = 0; k < n; k++)
            {
                for (int l = 0; l < m; l++)
                {
                    if (M[i][j] == M[k][l] && i != k && j != l)
                    {
                        // Première contrainte
                        glp_add_rows(lp, 1);
                        CompteurContrainte++;

                        oss.str("");
                        oss << "C_" << CompteurContrainte;
                        nomcontrainte = oss.str();
                        glp_set_row_name(lp, CompteurContrainte, nomcontrainte.c_str());
                        v = INF - P[i][j];
                        glp_set_row_bnds(lp, CompteurContrainte, GLP_UP, 0.0, v);

                        oss.str("");
                        oss << "St_" << k << '_' << l;
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
                        oss << "St_" << i << '_' << j;
                        nomcol = oss.str();
                        position = glp_find_col(lp, nomcol.c_str());
                        CompteurIA++;
                        ia[CompteurIA] = CompteurContrainte;
                        ja[CompteurIA] = position;
                        ar[CompteurIA] = 1.0;

                        // Deuxième contrainte
                        glp_add_rows(lp, 1);
                        CompteurContrainte++;

                        oss.str("");
                        oss << "C_" << CompteurContrainte;
                        nomcontrainte = oss.str();
                        glp_set_row_name(lp, CompteurContrainte, nomcontrainte.c_str());
                        glp_set_row_bnds(lp, CompteurContrainte, GLP_LO, P[k][l], 0.0);

                        oss.str("");
                        oss << "St_" << k << '_' << l;
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
                        oss << "St_" << i << '_' << j;
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

    // ajout d'une colonne
    oss.str("");
    oss << "Z";
    nomcol = oss.str();
    glp_add_cols(lp, 1);
    CompteurCol++;
    glp_set_col_name(lp, CompteurCol, nomcol.c_str());
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
        oss << "C_" << CompteurContrainte;
        nomcontrainte = oss.str();
        glp_set_row_name(lp, CompteurContrainte, nomcontrainte.c_str());
        glp_set_row_bnds(lp, CompteurContrainte, GLP_LO, P[i][m - 1], 0.0);

        oss.str("");
        oss << "Z";
        nomcol = oss.str();
        position = glp_find_col(lp, nomcol.c_str());
        CompteurIA++;
        ia[CompteurIA] = CompteurContrainte;
        ja[CompteurIA] = position;
        ar[CompteurIA] = 1;

        oss.str("");
        oss << "St_" << i << '_' << m - 1;
        nomcol = oss.str();
        position = glp_find_col(lp, nomcol.c_str());
        CompteurIA++;
        ia[CompteurIA] = CompteurContrainte;
        ja[CompteurIA] = position;
        ar[CompteurIA] = -1;
    }

    // les coefficients de la fonction objectif
    oss.str("");
    oss << "Z";
    nomcol = oss.str();
    position = glp_find_col(lp, nomcol.c_str());
    glp_set_obj_coef(lp, position, 1);

    glp_load_matrix(lp, CompteurIA, ia, ja, ar);
    oss.str("");
    oss << "Jobshop_" << name << ".lp";
    glp_write_lp(lp, NULL, oss.str().c_str());

    glp_simplex(lp, NULL);
    //glp_intopt(lp, NULL);

    // valeur de la fonction objectif
    //glp_set_col_kind(lp, CompteurCol, GLP_IV);
    //z = glp_mip_obj_val(lp);
    z = glp_get_obj_val(lp);
    cout << "Z = " << z << endl;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            oss.str("");
            oss << "St_" << i << '_' << j;
            nomcol = oss.str();
            position = glp_find_col(lp, nomcol.c_str());

            //double x = glp_mip_col_val(lp, position);

            double x = glp_get_col_prim(lp, position);
            cout << "St_" << i << "_" << j << " :" << x << endl;
        }
    }

    for (int i = 0; i < n; i++)
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
                        position = glp_find_col(lp, nomcol.c_str());

                        double x = glp_get_col_prim(lp, position);
                        //double x = glp_mip_col_val(lp, position);
                        cout << "order_" << i << '_' << j << '_' << k << '_' << l << " = " << x << "\n";
                    }
                }
            }
        }
    }


    cout << "fin des calculs" << endl;
    //getchar();

    // liberation mémoire
    glp_delete_prob(lp);
    glp_free_env();
}
