#pragma once

/*
 * Role self does in normally sleeping part of sync period.
 *
 * Master/Slave role implemented by Clique.
 */
enum class  Role { Merger, Fisher, NoFishing };


class MergerFisherRole {
public:

	static void init();

	static Role role();

	static bool isMerger();
	static bool isFisher();

	static void toFisher();
	static void toMerger();
	static void toNoFishing();

#ifdef FUTURE
	static bool isWorkMerger() {return role == WorkMerger;}

	static void setWorkMerger() {
			log("To role WorkMerger\n");
			role = WorkMerger;
		}
#endif
};
