#include "raylib.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define mapHeight 15
#define mapWidth 15
#define screenHeight 500
#define screenWidth 500
#define graphRectDim 20

// MAP DEFINES
#define GROUND 0
#define WALL 1
#define DOOR 2
#define KEY 3

#define maxNodes 64
#define maxRuleSize 4
#define maxNumRules 32



typedef enum {M_OBSTACLE,
						M_START, M_GOAL, M_MONSTER, M_TREASURE, M_SECRET,
						M_KEY, M_LOCK, M_ONEWAY, M_ARENA,
						M_BOSS, } MissionType;

typedef struct MissionGraph {
	MissionType type[maxNodes]; // donnée brute des noeuds
	int adj[maxNodes][maxNodes]; // matrice d'adjacence
	int terminal[maxNodes];
	int numNodes;

	int depth[maxNodes]; // distance au départ, pour calculer difficulté d'accès et valeur des trésors.
	int deepestObs; // obstacle le plus profond pour ajout de la fin de niveau.
} MissionGraph;

typedef struct GrammarRule {
	MissionGraph lhs;
	MissionGraph rhs;
	int inNode; // nouvelle node d'entrée
	int outNode; // nouvelle node de sortie
	int numNewNodes; // combien de nodes en plus ?
} GrammarRule;

int addMissionNode(MissionGraph *graph, MissionType type, int terminal){

	assert(graph->numNodes<maxNodes);
	int i = graph->numNodes;
	graph->terminal[i] = terminal;
	graph->type[i] = type;
	graph->numNodes++;
	return i;
}

void addMissionEdge(MissionGraph *graph, int a, int b){
	assert(a<maxNodes && b<maxNodes);
	graph->adj[a][b] = 1;
}

char* getDotLabel(MissionType type){

	char* str = {"xxx"};
	switch (type) {
		case M_OBSTACLE:
			str = "O";
			break;
		case M_ARENA:
			str="a";
			break;
		case M_SECRET:
			str="?";
			break;
		case M_START:
			str="s";
			break;
		case M_GOAL:
			str = "g";
			break;
		case M_MONSTER:
			str="m";
			break;
		case M_TREASURE:
			str ="t";
			break;
		case M_KEY:
			str = "k";
			break;
		case M_LOCK:
			str="l";
			break;
		case M_BOSS:
			str="b";
			break;
		default:
			str="xxx";
			break;
	}
	return str;
}

void getDepths(MissionGraph *g){
	// remplir les profondeurs des noeuds + obtenir l'obstacle le plus profond

	int maxObsDepth = 0;
	// Algortihme : prendre la source du noeud de plus petit indice.
	for (int n=0 ; n < g->numNodes ; n++){ // itération des noeuds
		int ancestor = n;
		int depth = 0;
		while(ancestor!=0){ // tant que l'ancêtre n'est pas le départ
			for (int x=0 ; x < g->numNodes ; x++){ // parcourir les ancêtres potentiels dans l'ordre croissant
				if (g->adj[x][ancestor]){ // lorsqu'on trouve le nouvel ancêtre
					depth++; // profondeur ++
					ancestor = x; // le nouvel ancêtre est x
					break; // on recommence si l'ancêtre n'est pas le départ
				}
			}
		}
		g->depth[n] = depth;

		// mise à jour obstacle le plus profond
		if (g->type[n]==M_OBSTACLE && depth>=maxObsDepth){
			maxObsDepth = depth;
			g->deepestObs = n;
		}
	}
}

void missionGraphToDot(const MissionGraph *g, FILE *fp){
    fprintf(fp, "digraph missiongraph {\n\trankdir=LR;\n");

  // labelisation
    for (int i = 0; i < g->numNodes; i++){
        fprintf(fp, "\t%d [label=\"%s\"%s];\n",
                i, getDotLabel(g->type[i]),
                g->terminal[i] ? "" : ", style=dashed");
    }

    // liens dirigés
    for (int x = 0; x < g->numNodes; x++){
        for (int y = 0; y < g->numNodes; y++){
            if (g->adj[x][y] > 0)
                fprintf(fp, "\t%d -> %d;\n", x, y);
        }
    }

    fprintf(fp, "}\n");
}

void setRuleEndpoints(GrammarRule *rule, int in, int out){	rule->inNode = in; rule->outNode = out;}






// MISSION RULES
// forks
#define numForkRules 2
#define maxForks 4
void ruleFork0(GrammarRule *rule){
	// fork vers un O et un t
	memset(rule, 0, sizeof(GrammarRule));
	addMissionNode(&rule->lhs, M_OBSTACLE, 0);

	int r0 = addMissionNode(&rule->rhs, M_OBSTACLE, 0);
	int r1 = addMissionNode(&rule->rhs,  M_OBSTACLE, 0);
	int r2 = addMissionNode(&rule->rhs,  M_TREASURE, 1);

	addMissionEdge(&rule->rhs, r0, r1);
	addMissionEdge(&rule->rhs, r1, r2);

	setRuleEndpoints(rule, r0, r0);
	rule->numNewNodes = 2;

} // fork vers O puis t
void ruleFork1(GrammarRule *rule){
	// fork vers un O  et t, puis retour chemin principal
	memset(rule, 0, sizeof(GrammarRule));
	addMissionNode(&rule->lhs, M_OBSTACLE, 0);

	int r0 = addMissionNode(&rule->rhs, M_OBSTACLE, 0);
	int r1 = addMissionNode(&rule->rhs,  M_OBSTACLE, 0);
	int r2 = addMissionNode(&rule->rhs,  M_OBSTACLE, 0);
	int t = addMissionNode(&rule->rhs,  M_TREASURE, 1);

	addMissionEdge(&rule->rhs, r0, r1);
	addMissionEdge(&rule->rhs, r1, r2);
	addMissionEdge(&rule->rhs, r1, t);
	addMissionEdge(&rule->rhs, r0, r2);

	setRuleEndpoints(rule, r0, r2);
	rule->numNewNodes = 3;

} // fork vers O et retour chemin principal

// obstacles
#define numObsRules 4
void ruleObs0(GrammarRule *rule){
	memset(rule, 0, sizeof(GrammarRule));
	addMissionNode(&rule->lhs, M_OBSTACLE, 0);

	int r0 = addMissionNode(&rule->rhs, M_KEY, 1);
	int r1 = addMissionNode(&rule->rhs,  M_OBSTACLE, 0);
	int r2 = addMissionNode(&rule->rhs,  M_LOCK, 1);

	addMissionEdge(&rule->rhs, r0, r1);
	addMissionEdge(&rule->rhs, r1, r2);

	setRuleEndpoints(rule, r0, r2);
	rule->numNewNodes = 2;
} // key, obstacle, lock
void ruleObs1(GrammarRule *rule){
	memset(rule, 0, sizeof(GrammarRule));
	addMissionNode(&rule->lhs, M_OBSTACLE, 0);

	int r0 = addMissionNode(&rule->rhs, M_MONSTER, 1);

	setRuleEndpoints(rule, r0, r0);
	rule->numNewNodes = 0;
}  // simple monster room
void ruleObs2(GrammarRule *rule){
	memset(rule, 0, sizeof(GrammarRule));
	addMissionNode(&rule->lhs, M_OBSTACLE, 0);

	int r0 = addMissionNode(&rule->rhs, M_SECRET, 1);

	setRuleEndpoints(rule, r0, r0);
	rule->numNewNodes = 0;
} // secret room
void ruleObs3(GrammarRule *rule){
	memset(rule, 0, sizeof(GrammarRule));
	addMissionNode(&rule->lhs, M_OBSTACLE, 0);

	int r0 = addMissionNode(&rule->rhs, M_ARENA, 0);

	setRuleEndpoints(rule, r0, r0);
	rule->numNewNodes = 1;
} // arene

void initForkingRules(GrammarRule rules[numForkRules]){
	ruleFork0(&rules[0]);
	ruleFork1(&rules[1]);
}

void initObstacleRules(GrammarRule rules[numObsRules]){
	ruleObs0(&rules[0]);
	ruleObs1(&rules[1]);
	ruleObs2(&rules[2]);
	ruleObs3(&rules[3]);
}

// Solver
int resolveRule(GrammarRule *r, MissionGraph *g){

	if (r->numNewNodes+g->numNodes > maxNodes) return -1;
	// règle : un élément lhs qui se décompose en plusieurs éléments rhs
	MissionType ruleTarget = r->lhs.type[0];
	int candidates[maxNodes]  = {-1};
	int numCand = 0;

	// obtenir les candidats au remplacement
	for (int n=0 ; n < g->numNodes ; n++){
		if (g->type[n] == ruleTarget){
			candidates[numCand] = n;
			numCand++;
		}
	}
	if (numCand==0) return 0; // si pas de candidat, fin de la resolution

	// tirage au sort du candidat
	int ranCand = candidates[rand()%numCand];

	// recolter cible du candidat
	int candidateTarget = -1;
	for (int t=0 ; t< g->numNodes ; t++){
		if (g->adj[ranCand][t]){ // si l'adj donne un lien candidat->target
			candidateTarget = t; // le supprimer
			break;
		}
	}

	// les règles ne doivent pas provoquer l'apparition d'une node non-terminale en bout de chaîne.
	// càd : Les nodes non-terminales ont toujours une source et une cible.
	assert(candidateTarget>=0);

	// remplacer  le candidat
	g->adj[ranCand][candidateTarget] = 0; // supprimer l'arrête vers la cible
	g->type[ranCand]  = r->rhs.type[r->inNode]; // modifier le type
	g->terminal[ranCand]  = r->rhs.terminal[r->inNode]; // et la terminalité de la node


	// ajouter les nouveaux noeuds
	int map[r->rhs.numNodes]; // bijection noeuds de rhs -> noeuds du graphe
	map[r->inNode] = ranCand; // le noeud d'entrée de rhs a remplacé ranCand

	for (int n = 0 ; n < r->rhs.numNodes ; n++){ // pour tous les noeuds de rhs
		if (n==r->inNode)  continue; // si c'est le noeud d'entrée, c'est déjà fait. Sinon,
			int newNode = addMissionNode(g, r->rhs.type[n], r->rhs.terminal[n]); // ajouter ce noeud au graphe
			map[n] = newNode; // mettre à jour la bijection
		}

	// relier les terminaisons
	for (int x=0 ; x < r->rhs.numNodes ; x++){
		for (int y=0 ; y < r->rhs.numNodes ; y++){ // on parcourt la matrice d'adjacence de rhs
			if (r->rhs.adj[x][y])	// si il y a une arrête
				addMissionEdge(g, map[x], map[y]); // en mettre une dans le graphe entre les images de la bijection
	}
	// et  reconnecter la sortie
	addMissionEdge(g, map[r->outNode], candidateTarget);

	}
	return numCand;
}









int main(void){

	srand(time(NULL));
	MissionGraph graph = {0};

	// forking rules
	GrammarRule forkingRules[numForkRules] = {0};
	initForkingRules(forkingRules);
	// obstacle rules
	GrammarRule obstacleRules[numObsRules] = {0};
	initObstacleRules(obstacleRules);


	// générer le graphe de départ :
		int start= addMissionNode(&graph, M_START, 1);
		int O1 = addMissionNode(&graph, M_OBSTACLE, 0);
		int O2 = addMissionNode(&graph, M_OBSTACLE, 0);
		int O3 = addMissionNode(&graph, M_OBSTACLE, 0);
		int t1 = addMissionNode(&graph, M_TREASURE, 1);
		int t2 = addMissionNode(&graph, M_TREASURE, 1);

		addMissionEdge(&graph, start, O1);
		addMissionEdge(&graph, O1, O2);
		addMissionEdge(&graph, O2, O3);
		addMissionEdge(&graph, O3, t1);
		addMissionEdge(&graph, O2, t2);

	/* // export en graphviz dot : etat initial
		FILE *fp = fopen("mg0.dot", "w");
		missionGraphToDot(&graph, fp);
		fclose(fp);
	*/

	// résolution des règles de fork
		int numForks = 0; // nombre de forks effectués

		while (numForks<maxForks){

			int forkNb = rand()%numForkRules; // choix aléatoire de fork
			printf("applying fork %d\n", forkNb);

			resolveRule(&forkingRules[forkNb], &graph); // resolution
			numForks++;
		}

	// résolution profondeurs
		getDepths(&graph);
		printf("deepestObs : %d", graph.deepestObs);
		int b = addMissionNode(&graph, M_BOSS, 1);
		int g = addMissionNode(&graph, M_GOAL, 1);
		addMissionEdge(&graph, graph.deepestObs, b);
		addMissionEdge(&graph, b, g);

	// résolution des règles d'obstacles
		int done=0;
			while (!done) {
				done = 1;
				int ruleNb = rand()%numObsRules; // règle aléatoire
				resolveRule(&obstacleRules[ruleNb], &graph); // appliquer la règle
				for (int n=0 ; n<graph.numNodes ; n++){
					if (graph.type[n]==M_OBSTACLE) done=0;
				}
			}

	// export en graphviz dot : etat post-chaine
		FILE *fp = fopen("mg1.dot", "w");
		missionGraphToDot(&graph, fp);
		fclose(fp);

	// raylib dessin map
	/*

		int map[mapWidth*mapHeight] =
			{
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,1,1,2,1,1,1,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,3,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			};

	InitWindow(screenWidth, screenHeight, "GrammarGen");
	while(!WindowShouldClose()){
		BeginDrawing();
		ClearBackground(GRAY);
		//renderMap(map);
		EndDrawing();
	}

	CloseWindow();
	*/
	return 0;
}


void renderMap(int map[mapWidth*mapHeight]){

	int rectW = (int)(screenWidth/mapWidth);
	int rectH = (int)(screenHeight/mapHeight);
	int drawX = 0;
	int drawY = 0;

	for (int x=0 ; x<mapWidth ; x++){
		for (int y=0 ; y<mapHeight ; y++){
			Color c = BLACK;
			switch (map[x + y*mapWidth]) {
				case WALL:
					c = GRAY;
					break;
				case GROUND:
					c = WHITE;
					break;
				case DOOR:
					c = BLUE;
					break;
				case KEY:
					c = GREEN;
					break;
			}
			DrawRectangle(drawX, drawY, rectW, rectH, c);

			drawY += rectH;
		}
		drawX += rectW;
		drawY = 0;
	}

}
