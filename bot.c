//// Fichier pour la stratégie ////
#include "bot.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "fonction.h"

//////////////////////////////////////////////////////////
// Dans la librairie
//

typedef enum Etat Etat;
enum Etat
{
    DEPART, TRANSITION, FIN
};


typedef struct {
	Player color;
	Player enemy;
	unsigned int targetScore;
	
} SBotInfo;

Etat etatJeu;
SBotInfo bot;


void InitLibrary(char name[50])
{
	printf("InitLibrary\n");
	strcpy(name,"Ballz");
}

void StartMatch(const unsigned int target_score)
{
	bot.targetScore = target_score;
	printf("StartMatch\n");
}

void StartGame(Player p)
{
	
	bot.color = p;
	if(p == 1) bot.enemy = 0;
	else bot.enemy = 1;
	
	printf("StartGame\n");
}

void EndGame()
{
	printf("EndGame\n");
}

void EndMatch()
{
	printf("EndMatch\n");
}

int DoubleStack(const SGameState * const gameState)
{
	printf("DoubleStack\n");
	return(0);
}

int TakeDouble(const SGameState * const gameState)
{
	printf("TakeDouble\n");
	return(0);
}



int min(int a, int b)
{
	if(a<b) return a;
	else return b;
	
}

int max(int a,int b)
{
	if(a>b) return a;
	else return b;
	
}



// Vérifie que la case sur laquelle le pion va se déplacer est libre, de sa couleur ou n'a qu'un pion dessus
int IsMoveRight(int numCaseDep, int dice, const SGameState * const gameState)
{
	
	if(((numCaseDep + dice) <= 23) && (numCaseDep + dice >=0) && (!IsDiceUsed(dice)))
	{
		if ((gameState->board[(numCaseDep + dice)].owner != bot.enemy) || (gameState->board[(numCaseDep + dice)].nbDames == 1))
		{
			return(1);
		}
		else return 0;
	}
	else return 0;
}


// Vérifie qu'une case est bien de la couleur de l'IA
int IsCaseOurs(int numCase, const SGameState * const gameState)
{
	if (gameState->board[numCase].owner == bot.color)
	{
		return 1;
	}
	else return 0;
}


//Vérifie qu'une case est dans le Jan intérieur
int IsCaseJanInt(int numCase)
{
	if ((numCase>= 18) && (numCase <=23))
	{
		return(1);
	}
	else return (0);
}


//Indique si le dé a été utilisé ou non (1 s'il a déjà été utilisé, 0 sinon)
int IsDiceUsed(int dice)
{
	if (dice < 0)
	{
		return 1;
	}
	else return 0;
}

//Indique si les dés forment un double
int IsDiceDouble(const unsigned char dice[2])
{
	if (dice[0] == dice[1])
	{
		return 1;
	}
	else return 0;
}


//Indique si la case a été vidée de ses pions lors du tour courant
int IsCaseEmpty(int caseDep, int nbMove, SMove moves[4], const SGameState * const gameState)
{
	int i,j=0;
	for(i=0;i<nbMove;i++)
	{
		if(moves[i].src_point == caseDep)
		{
			j++;
		}	
	}
	if(j == gameState->board[caseDep].nbDames)
	{
		return(1);
	}
	else return(0);
	
}


// Effectue la somme des dés non encore utilisés
int SumDice(unsigned int *dice,int sizeDice)
{
	int i;
	int sum = 0;
	for(i=0;i<sizeDice;i++)
	{
		if(!IsDiceUsed(dice[i]))
		{
			sum += dice[i];
		}
	}
	return(sum);
}

// Retourne le nombre de dés non encore utilisés
int NbDiceLeft(unsigned int *dice, int sizeDice)
{
	int i;
	int nb=0;
	for(i=0;i<sizeDice;i++)
	{
		if(!IsDiceUsed(dice[i]))
		{
			nb++;
		}
	}
	return(nb);
}


Etat DefEtat(const SGameState * const gameState)
{
	int i,j;
	int sum;
	for(i=17;i<24;i++)
	{
		if((gameState->board[i].owner == bot.enemy) && (gameState->board[i].nbDames >= 2))
		{
			sum++;
			if(sum > 2)
			{
				return TRANSITION;
			}
		}
	}
	for(j=0;j<6;j++)
	{
		if((gameState->board[j].owner == bot.color) && (gameState->board[j].nbDames == 15))
		{
			return FIN;
		}
	}
	
	return DEPART;
}




void PlayTurn(const SGameState * const gameState, const unsigned char dices[2], SMove moves[4], unsigned int *nbMove, unsigned int tries)
{

	int i,j = 0;
	int casesPionsBot[15];
	int dim;
	unsigned int sizeDice;
	unsigned int sizeBar = sizeof(gameState->bar[bot.color])/sizeof(int);
	int nbMoveInter;
	int numCaseInter;
	
	
	*nbMove = 0;
	
	/*** Transformation du dé en int (prise en compte des doubles) ***/
	
	printf("%d",IsDiceDouble(dices));
	
	if (IsDiceDouble(dices))
	{
		sizeDice = 4;
	}
	else 
	{
		sizeDice = 2;
	}
	
	unsigned int *dice = (unsigned int *)malloc(sizeDice*sizeof(int));
	if(sizeDice == 4)
	{
		dice[0] = (int)dices[0];
		dice[1] = (int)dices[0];
		dice[2] = (int)dices[0];
		dice[3] = (int)dices[0];
	}
	else
	{
		dice[0]=(int)dices[0];
		dice[1]=(int)dices[1];
	}
	
	
	/***Lorsque l'on est dans le bar (prison) ***/
	
	if ((int)gameState->bar[bot.color] != 0)
	{
		/* On parcourt */
		   
		for(i=0;i<sizeBar;i++)
		{
			for(j=0;j<sizeDice;j++)
			{
				if(IsMoveRight(0,dice[j],gameState))
				{
					moves[*nbMove].src_point = 0;
					moves[*nbMove].dest_point = moves[0].src_point + dice[j] + 1;
					*nbMove = *nbMove+1;
					dice[j] = -1;
					break;  //On a utilisé le dé courant et le pion courant donc on sort de la boucle pour passer au pion suivant
				}
			}
		}
		if((*nbMove <= sizeBar) || (*nbMove == sizeDice))
		{
			free(dice);
			return;
		}
		
	}
	
	
	
	
	
	//Remplissage tableau contenant les indices des cases sur lesquelles sont présents les pions du bot
	for(i=0;i<24;i++)
	{
		if (gameState->board[i].owner == bot.color)
		{
			casesPionsBot[j] = i;
			j++;
		}
	}
	dim = j;
	
	
	

	
	
	
	/*** Permet, si on le peut, de faire bouger 2 pions sur la même case ***/
	
	/* Parcours du tableau contenant les indices des cases de nos pions
	   On va comparer tous les indices entre eux */
	   
	if(*nbMove == 0)
	{
		
		for(i=1;i<dim;i++)
		{
			for(j=1;j<dim;j++)
			{
				
				// Vérification : la dist entre les cases = la dist entre les dés + la case d'arrivée ne doit pas être a l'ennemi OU il ne doit y avoir qu'un seul pion dessus.
				if((fabs(casesPionsBot[i]-casesPionsBot[j]) == fabs(dice[0]-dice[1])) && (IsMoveRight(min(casesPionsBot[i],casesPionsBot[j]),max(dice[0],dice[1]),gameState)))
				{
					*nbMove = 2;
					moves[0].src_point = min(casesPionsBot[i],casesPionsBot[j])+1;
					moves[0].dest_point = moves[0].src_point + max(dice[0],dice[1])+1;
					
					moves[1].src_point = max(casesPionsBot[i],casesPionsBot[j])+1;
					moves[1].dest_point = moves[1].src_point + min(dice[0],dice[1])+1;
					free(dice);
					return;
				}
			}
		}
	}
	
	
	
	/*** Quand on a un double : on bouge 2 pions de la même case sur une autre case ***/
	
	if((IsDiceDouble(dices)) && (NbDiceLeft(dice,sizeDice)%2 == 0)) 
	{
		for(i=0;i<dim;i++)
		{
			numCaseInter = 0;
			if(((gameState->board[casesPionsBot[i]].nbDames == 2) || (gameState->board[casesPionsBot[i]].nbDames > 3)) && (IsMoveRight(casesPionsBot[i],SumDice(dice,sizeDice),gameState)) && (!IsCaseEmpty(casesPionsBot[i],*nbMove,moves,gameState)))
			{
				if(NbDiceLeft(dice,sizeDice) == 4)
				{
					moves[*nbMove].src_point = casesPionsBot[i] + 1;
					moves[*nbMove].dest_point = casesPionsBot[i]+dice[j] + 1;
					*nbMove = *nbMove+1;
					dice[j] = -1;
					// A continuer (chainage de mouves)
				}
				// Cas où il n'y a que 2 dés de dispo
				for(j=0;j<sizeDice;j++)
				{

					if((!IsDiceUsed(dice[j]) && (numCaseInter == 0)))
					{
						moves[*nbMove].src_point = casesPionsBot[i] + 1;
						moves[*nbMove].dest_point = casesPionsBot[i]+dice[j] + 1;
						numCaseInter = moves[*nbMove].dest_point;
						*nbMove = *nbMove+1;
						dice[j] = -1;
					}
					else if((!IsDiceUsed(dice[j]) && (numCaseInter != 0)))
					{
						moves[*nbMove].src_point = numCaseInter;
						moves[*nbMove].dest_point = numCaseInter + dice[j];
						numCaseInter = moves[*nbMove].dest_point;
						*nbMove = *nbMove+1;
						dice[j] = -1;
					}
					if (*nbMove == sizeDice)
					{
						free(dice);
						return;
					}
				}
			}
		}
	}
	
	
	/***** A FAIRE : EN BOUGER UN SUR UNE LONGUE DISTANCE, BETTER *****/
	
	for(i=1;i<dim;i++)
	{
		nbMoveInter = 0;
		numCaseInter = 0;
		if((IsCaseOurs(casesPionsBot[i]+SumDice(dice,sizeDice),gameState)) && (!IsCaseEmpty(casesPionsBot[i],*nbMove,moves,gameState)))
		{
			for(j=0;j<sizeDice;j++)
			{
				if(IsMoveRight(casesPionsBot[i],dice[j],gameState))
				{
					nbMoveInter++;
				}
			}
			if(nbMoveInter == NbDiceLeft(dice,sizeDice))
			{
				for(j=0;j<sizeDice;j++)
				{
					if((!IsDiceUsed(dice[j])) && (numCaseInter == 0))
					{
						moves[*nbMove].src_point = casesPionsBot[i] + 1;
						moves[*nbMove].dest_point = casesPionsBot[i]+dice[j] + 1;
						numCaseInter = moves[*nbMove].dest_point;
						*nbMove = *nbMove+1;
						dice[j] = -1;
						
					}
					else if((!IsDiceUsed(dice[j])) && (numCaseInter != 0))
					{
						moves[*nbMove].src_point = numCaseInter;
						moves[*nbMove].dest_point = numCaseInter + dice[j];
						numCaseInter = moves[*nbMove].dest_point;
						*nbMove = *nbMove+1;
						dice[j] = -1;
					}
					if (*nbMove == sizeDice)
					{
						free(dice);
						return;
					}
				}
			}
		}
	}
	
	
	
	/*** Mouvement de pions sur une case alliée si on a pas pu faire le coup d'avant ***/
	
	for(i=1;i<dim;i++)
	{
		for(j=0;j<sizeDice;j++)
		{
			if(*nbMove == sizeDice)
			{
				free(dice);
				return;
			}
			if((IsCaseOurs(casesPionsBot[i]+dice[j],gameState)) && (!IsDiceUsed(dice[j])) && (!IsCaseEmpty(casesPionsBot[i],*nbMove,moves,gameState)))
			{
			moves[*nbMove].src_point = casesPionsBot[i] + 1;
			moves[*nbMove].dest_point = casesPionsBot[i]+dice[j] + 1;
			*nbMove = *nbMove+1;
			dice[j] = -1;
			}
		}
	}
		
	
	
		/*** Si on a réussi à faire aucun des moves d'avant ou s'il reste des moves à faire, on va bouger le ou les pions les plus éloignés 
		(en partant de l'avant derniere case de notre couleur pour garder une ancre) sur une case dispo où une case où il n'y a qu'un pion  ***/
		
		
		
		
	for(i=1;i<dim;i++)
	{
		for(j=0;j<sizeDice;j++)
		{
			if(*nbMove == 2)
			{
				free(dice);
				return;
			}
			if((IsMoveRight(casesPionsBot[i],dice[j],gameState)) && (!IsCaseEmpty(casesPionsBot[i],*nbMove,moves,gameState)))
			{
				moves[*nbMove].src_point = casesPionsBot[i] + 1;
				moves[*nbMove].dest_point = casesPionsBot[i] + dice[j] + 1;
				*nbMove = *nbMove + 1;
				dice[j] = -1;
			}
		}
	}
	
	
	//Mouv de l'ancre si vraiment on ne peut rien faire d'autre
	for(j=0;j<sizeDice;j++)
	{
		if((IsMoveRight(casesPionsBot[0],dice[j],gameState)) && (!IsCaseEmpty(casesPionsBot[0],*nbMove,moves,gameState)))
		{
			moves[*nbMove].src_point = casesPionsBot[0] + 1;
			moves[*nbMove].dest_point = casesPionsBot[0] + dice[j] + 1;
			*nbMove = *nbMove + 1;
			dice[j] = -1;
		}
	}
	
		
		

	free(dice);
	

	
	/* Sinon si possible : déplacement d'un pion de la somme des dés sur une case à nous
	 */
	
	
	/** Maybe faire un enum dans la structure bot pour faire des etats (allié et ennemi, peut être utile pour savoir si on double ou non la mise)
	(debut/milieu de partie + fin de partie avec le ruch de l'ancre, plus besoin de se soucier de se faire prendre des pions) **/
	
	
	printf("max : %d\n",max(5,2));
	
	printf("bot : %d\n",(int)bot.color);
	
	printf("PlayTurn\n");
}






