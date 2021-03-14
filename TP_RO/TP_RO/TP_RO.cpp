// TP_RO.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.

#include <iostream>
#include <vector>
#include "Betail.h"
#include "Jobshop.h"
#include "Voyageur.h"

int main()
{
    // Exercice 1 : Aliments pour bétail
    // Betail::solve(2);
    // Exercice 2 : Jobshop
    //Jobshop::solve("La01.txt", true);
    //évolution du temps de calcul en fonction de la taille d'instance :
    Jobshop::solve("La01.txt", false);
    Jobshop::solve("La06.txt", false);
    Jobshop::solve("La16.txt", false);
    Jobshop::solve("La22.txt", false);
    //Jobshop::solve("swv17.txt", false); /* Résolu! Makespan: 683 Temps : 407446 ms*/

    // Exercice 3 : Voyageur de commerce
    // Voyageur::solve();

    // Exercice 4 : Génération de colonnes
    // Colonne::solve();

}
