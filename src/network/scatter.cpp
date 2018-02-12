
#include "scatter.h"

#include "../globals.h"  // clique
#include "../clique/clique.h"




void Scatter::scatter(){
	// strong alternative
	clique.scatterSync();
}
