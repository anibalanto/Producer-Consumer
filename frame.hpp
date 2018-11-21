struct Frame{
    unsigned int id;
    unsigned int n_objects;
    Frame(){}
    Frame(unsigned int pid, unsigned int pn_objects) : id(pid), n_objects(pn_objects) {}
	friend bool operator< (const Frame & lf, const Frame & rf);
};

struct CompFrameByNObjets
{
	bool operator() (const Frame & lf, const Frame & rf){return lf.n_objects < rf.n_objects;}
};

inline bool operator< (const Frame & lf, const Frame & rf)
{
	if(lf.n_objects == rf.n_objects)
		return lf.id > rf.id;
	return lf.n_objects < rf.n_objects;
}


/*
template<>
struct transporter_priority<Frame>
{
		using type = struct CompFrameByNObjets;
}
*/
