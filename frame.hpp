struct Frame{
    unsigned int id;
    unsigned int n_objects;
    Frame(){}
    Frame(unsigned int pid, unsigned int pn_objects) : id(pid), n_objects(pn_objects) {}
};

struct CompFrameByNObjets
{
	bool operator() (const Frame & lf, const Frame & rf){return lf.n_objects < rf.n_objects;}
};
