
#pragma once

/*
 * Knows meaning of scatter.
 */
/*
 * Scatter
 *
 * Alternative designs
 * - unsync all units completely
 *   -- each its own master but not randomizing clocks (will reform cliques quickly)
 *   -- each its own master and randomizing clocks (like starting over from POR)
 * - OR maintain current cliques (stay in sync) and scatter work (using work offset)
 *   -- stay at random work times until a clique is provisioned to a certain time
 *   -- let the master resync work (will happen the next WorkSync time)
 */
class Scatter {
public:
	static void scatter();
};
