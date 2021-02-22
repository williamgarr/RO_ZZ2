#include "Betail.h"

void Betail::solve(int nb_iter)
{
    const int mp = 3;
    const int b = 3;
    const int n = 2;
    const int nb_op = 4;

    string MatPremNom[mp + 1];
    int Qmax[mp + 1];
    double Pr[mp + 1];
    string OperationNom[nb_op + 1];
    double C[nb_op + 1];
    string NutrimentNom[b + 1];
    double BN[b + 1];
    double P[mp + 1][b + 1];
    double D[n + 1];

    // Relaxation de Lagrange
    double lambda[mp + 1];
    lambda[1] = 0.9;
    lambda[2] = 0.9;
    lambda[3] = 0.9;
    int iteration = 0;

    // nom des mat premières
    MatPremNom[1] = "Avoine";
    MatPremNom[2] = "Mais";
    MatPremNom[3] = "Mélasse";
    // qtes des mat premières
    Qmax[1] = 11900;
    Qmax[2] = 23500;
    Qmax[3] = 750;
    // prix des mat premières
    Pr[1] = 0.8;
    Pr[2] = 1.0;
    Pr[3] = 0.75;
    // nom des opérations
    OperationNom[1] = "Broyage";
    OperationNom[2] = "Mélange";
    OperationNom[3] = "Granulation";
    OperationNom[4] = "Tamisage";
    // cout des opérations
    C[1] = 1.50;
    C[2] = 0.30;
    C[3] = 2.50;
    C[4] = 1;
    // noms des nutriments
    NutrimentNom[1] = "Proteines";
    NutrimentNom[2] = "Lipides";
    NutrimentNom[3] = "Fibres";
    // teneurs recommandées
    BN[1] = 9.5;
    BN[2] = 2;
    BN[3] = 6;
    // composition des matières premières dans les différents nutriments
    P[1][1] = 13.6; P[1][2] = 7.1; P[1][3] = 7;
    P[2][1] = 4.1; P[2][2] = 2.4; P[2][3] = 3.7;
    P[3][1] = 5; P[3][2] = 0.3; P[3][3] = 25;
    // Quantités demandées
    D[1] = 9000;
    D[2] = 12000;

    std::cout << "GLPK version : " << glp_version() << std::endl;

    while (iteration < nb_iter) {

        glp_prob* lp;
        int ia[1001], ja[1001];
        double ar[1001], z, x1, x2;

        lp = glp_create_prob();
        glp_set_prob_name(lp, "Aliment pour betail");
        glp_set_obj_dir(lp, GLP_MIN);

        int CompteurContrainte = 0;
        int CompteurCol = 0;
        int CompteurIA = 0;

        string nomcol;
        string nomcontrainte;
        int position;

        // definition des variables
        for (int i = 1; i <= n; i++)
        {
            for (int j = 1; j <= mp; j++)
            {
                ostringstream oss;
                oss << "x_" << i << '_' << j;
                nomcol = oss.str();
                glp_add_cols(lp, 1);
                CompteurCol = CompteurCol + 1;
                glp_set_col_name(lp, CompteurCol, nomcol.c_str());
                glp_set_col_kind(lp, CompteurCol, GLP_IV);
                glp_set_col_bnds(lp, CompteurCol, GLP_LO, 0.0, 0.0);
            }
        }
        for (int i = 1; i <= n; i++)
        {
            std::ostringstream oss;
            oss << "q_" << i;
            nomcol = oss.str();
            glp_add_cols(lp, 1);
            CompteurCol = CompteurCol + 1;
            glp_set_col_name(lp, CompteurCol, nomcol.c_str());
            //glp_set_col_kind(lp, CompteurCol, GLP_IV);
            glp_set_col_bnds(lp, CompteurCol, GLP_LO, 0.0, 0.0);
        }
        // creation Index pour les recherches
        glp_create_index(lp);

        // contrainte 1
        for (int i = 1; i <= n; i++)
        {
            glp_add_rows(lp, 1);
            CompteurContrainte++;
            std::ostringstream oss;
            oss << "C_" << CompteurContrainte;
            nomcontrainte = oss.str();
            glp_set_row_name(lp, CompteurContrainte, nomcontrainte.c_str());
            glp_set_row_bnds(lp, CompteurContrainte, GLP_FX, 0.0, 0.0); // GLP_LO > | GLP_UP < | GLP_FX =

            oss.str("");
            oss << "q_" << i;
            nomcol = oss.str();
            position = glp_find_col(lp, nomcol.c_str());
            CompteurIA = CompteurIA + 1;
            ia[CompteurIA] = CompteurContrainte;
            ja[CompteurIA] = position;
            ar[CompteurIA] = -1.0;

            // Ajout de 𝑞𝑖 dans la contrainte
            for (int j = 1; j <= mp; j++)
            {
                // Ajout des xij
                oss.str("");
                oss << "x_" << i << '_' << j;
                nomcol = oss.str();
                position = glp_find_col(lp, nomcol.c_str());
                CompteurIA = CompteurIA + 1;
                ia[CompteurIA] = CompteurContrainte;
                ja[CompteurIA] = position;
                ar[CompteurIA] = 1.0;
            }
        }

        // contrainte 2
        for (int i = 1; i <= n; i++)
        {
            for (int k = 1; k <= (b - 1); k++)
            {
                glp_add_rows(lp, 1);
                CompteurContrainte++;
                std::ostringstream oss;
                oss << "C_" << CompteurContrainte;
                nomcontrainte = oss.str();
                glp_set_row_name(lp, CompteurContrainte, nomcontrainte.c_str());
                glp_set_row_bnds(lp, CompteurContrainte, GLP_LO, 0.0, 0.0); // GLP_LO > | GLP_UP < | GLP_FX =

                // Ajout de 𝑞𝑖 dans la contrainte
                for (int j = 1; j <= mp; j++)
                {
                    // Ajout des xij
                    oss.str("");
                    oss << "x_" << i << '_' << j;
                    nomcol = oss.str();
                    position = glp_find_col(lp, nomcol.c_str());
                    CompteurIA = CompteurIA + 1;
                    ia[CompteurIA] = CompteurContrainte;
                    ja[CompteurIA] = position;
                    ar[CompteurIA] = (P[j][k] / 100);
                }

                oss.str("");
                oss << "q_" << i;
                nomcol = oss.str();
                position = glp_find_col(lp, nomcol.c_str());
                CompteurIA = CompteurIA + 1;
                ia[CompteurIA] = CompteurContrainte;
                ja[CompteurIA] = position;
                ar[CompteurIA] = -(BN[k] / 100);
            }
        }

        // contrainte 3
        for (int i = 1; i <= n; i++)
        {
            glp_add_rows(lp, 1);
            CompteurContrainte++;
            std::ostringstream oss;
            oss << "C_" << CompteurContrainte;
            nomcontrainte = oss.str();
            glp_set_row_name(lp, CompteurContrainte, nomcontrainte.c_str());
            glp_set_row_bnds(lp, CompteurContrainte, GLP_UP, 0.0, 0.0); // GLP_LO > | GLP_UP < | GLP_FX =

            // Ajout de 𝑞𝑖 dans la contrainte
            for (int j = 1; j <= mp; j++)
            {
                // Ajout des xij
                oss.str("");
                oss << "x_" << i << '_' << j;
                nomcol = oss.str();
                position = glp_find_col(lp, nomcol.c_str());
                CompteurIA = CompteurIA + 1;
                ia[CompteurIA] = CompteurContrainte;
                ja[CompteurIA] = position;
                ar[CompteurIA] = (P[j][b] / 100);
            }

            oss.str("");
            oss << "q_" << i;
            nomcol = oss.str();
            position = glp_find_col(lp, nomcol.c_str());
            CompteurIA = CompteurIA + 1;
            ia[CompteurIA] = CompteurContrainte;
            ja[CompteurIA] = position;
            ar[CompteurIA] = -(BN[b] / 100);
        }

        /* contrainte 4 (supprimé par relaxation)
        for (int j = 1; j <= mp; j++)
        {
            glp_add_rows(lp, 1);
            CompteurContrainte++;
            std::ostringstream oss;
            oss << "C_" << CompteurContrainte;
            nomcontrainte = oss.str();
            glp_set_row_name(lp, CompteurContrainte, nomcontrainte.c_str());
            glp_set_row_bnds(lp, CompteurContrainte, GLP_UP, 0.0, Qmax[j]); // GLP_LO > | GLP_UP < | GLP_FX =

            // Ajout de 𝑞𝑖 dans la contrainte
            for (int i = 1; i <= n; i++)
            {
                // Ajout des xij
                oss.str("");
                oss << "x_" << i << '_' << j;
                nomcol = oss.str();
                position = glp_find_col(lp, nomcol.c_str());
                CompteurIA = CompteurIA + 1;
                ia[CompteurIA] = CompteurContrainte;
                ja[CompteurIA] = position;
                ar[CompteurIA] = 1;
            }
        }*/

        // contrainte 5
        for (int i = 1; i <= n; i++)
        {
            glp_add_rows(lp, 1);
            CompteurContrainte++;
            std::ostringstream oss;
            oss << "C_" << CompteurContrainte;
            nomcontrainte = oss.str();
            glp_set_row_name(lp, CompteurContrainte, nomcontrainte.c_str());
            glp_set_row_bnds(lp, CompteurContrainte, GLP_LO, D[i], 0.0); // GLP_LO > | GLP_UP < | GLP_FX =

            oss.str("");
            oss << "q_" << i;
            nomcol = oss.str();
            position = glp_find_col(lp, nomcol.c_str());
            CompteurIA = CompteurIA + 1;
            ia[CompteurIA] = CompteurContrainte;
            ja[CompteurIA] = position;
            ar[CompteurIA] = 1;
        }

        glp_add_rows(lp, 1);
        CompteurContrainte++;
        std::stringstream oss;
        oss << "C_" << CompteurContrainte;
        nomcontrainte = oss.str();
        glp_set_row_name(lp, CompteurContrainte, nomcontrainte.c_str());
        glp_set_row_bnds(lp, CompteurContrainte, GLP_FX, 0.0, 0.0);

        // ajout col objectif
        oss.str("");
        oss << "Z";
        nomcol = oss.str();
        glp_add_cols(lp, 1);
        CompteurCol++;
        glp_set_col_name(lp, CompteurCol, nomcol.c_str());
        glp_set_col_bnds(lp, CompteurCol, GLP_LO, 0.0, 0.0);

        oss.str("");
        oss << "Z";
        nomcol = oss.str();
        position = glp_find_col(lp, nomcol.c_str());
        glp_set_obj_coef(lp, position, 1);

        // Tweak Relaxation Lagrangienne
        double terme_constant = 0;
        for (int j = 1; j <= mp; j++)
            terme_constant += lambda[j] * Qmax[j];
        glp_set_obj_coef(lp, 0, -terme_constant);

        CompteurIA++;
        ia[CompteurIA] = CompteurContrainte;
        ja[CompteurIA] = position;
        ar[CompteurIA] = 1;
        // fin col obj

        for (int j = 1; j <= mp; j++)
        {
            std::stringstream oss;
            double somme_x_j = 0;
            for (int i = 1; i <= n; i++)
            {
                oss.str("");
                oss << "x_" << i << '_' << j;
                nomcol = oss.str();
                position = glp_find_col(lp, nomcol.c_str());
                double x = glp_get_col_prim(lp, position);
                somme_x_j += x;
            }
            double difference = (somme_x_j - Qmax[j]);
            double delta = (1.0 / (double)iteration) * difference;
            double ancien_lambda = lambda[j];
            lambda[j] = lambda[j] + (delta / 1000);
            if (lambda[j] < 0)
                lambda[j] = 0;
        }

        //std::stringstream oss;
        for (int i = 1; i <= n; i++)
        {
            double resultat = 0.0;

            for (int j = 1; j <= mp; j++)
            {
                oss.str("");
                oss << "x_" << i << '_' << j;
                nomcol = oss.str();
                position = glp_find_col(lp, nomcol.c_str());

                CompteurIA = CompteurIA + 1;
                resultat = lambda[j];

                if (i == 1)
                    resultat += C[2] + Pr[j] + C[3];
                if (i == 2)
                    resultat += C[2] + Pr[j] + C[4];
                if ((i != 1) && (i != 2))
                    resultat += C[2] + Pr[i];
                if (j <= mp - 1)
                    resultat = resultat + C[1];

                ia[CompteurIA] = CompteurContrainte;
                ja[CompteurIA] = position;
                ar[CompteurIA] = -resultat;
            }
        }

        glp_load_matrix(lp, CompteurIA, ia, ja, ar);
        glp_write_lp(lp, NULL, "betail.lp");
        glp_simplex(lp, NULL);

        cout << "ITERATION N°" << iteration << "\n";
        z = glp_get_obj_val(lp);
        cout << "Z = " << z << endl;

        for (int i = 1; i <= n; i++)
        {
            for (int j = 1; j <= mp; j++)
            {
                oss.str("");
                oss << "x_" << i << '_' << j;
                nomcol = oss.str();
                position = glp_find_col(lp, nomcol.c_str());
                double x = glp_get_col_prim(lp, position);
                cout << "x_" << i << '_' << j << " :" << x << "\n";
            }
        }

        for (int i = 1; i <= n; i++)
        {
            oss.str("");
            oss << "q_" << i;
            nomcol = oss.str();
            position = glp_find_col(lp, nomcol.c_str());
            double x = glp_get_col_prim(lp, position);
            cout << "q_" << i << " = " << x << "\n";
        }
        iteration++;
    }
}
