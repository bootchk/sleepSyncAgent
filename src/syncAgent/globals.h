
#pragma once

/*
 * Most objects are singletons, using pure classes without instances.
 *
 * SyncAgent itself is a singleton, the calling app may declare an instance but doesn't need to.
 *
 * Currently, the only instance is of Clique.
 * Don't need to have an instance, but allowing for future use of hops, where gateways may belong to more than one clique.
 */


class Clique;
extern Clique clique;


