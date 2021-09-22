
typedef
  struct{
    bddp	node;
    bddp	left;
    int		lnumber;
    short	llevel;
    int		lattrib;
    bddp	right;
    int		rnumber;
    short	rlevel;
    int		rattrib;
  } pack;

extern void	TraverseFunctions();

