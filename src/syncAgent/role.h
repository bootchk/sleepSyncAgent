enum { Merger, Fisher };

// Just the Merger/Fisher role.  Master/Slave role implemented by Clique.
class Role {

private:
	static int role;

public:
	Role() { role = Fisher; }
	static bool isMerger() {return role == Merger;}
	static bool isFisher() {return role == Fisher;}
	static void setFisher() { role = Fisher; }
	static void setMerger() { role = Merger; }
};
