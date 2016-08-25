typedef enum { Merger, Fisher } RoleType;

// Just the Merger/Fisher role.  Master/Slave role implemented by Clique.
class Role {

private:
	static RoleType role;

public:
	Role() { role = Fisher; }
	static bool isMerger() {return role == Merger;}
	static bool isFisher() {return role == Fisher;}
	static void setFisher() { role = Fisher; }
	static void setMerger() { role = Merger; }
};
