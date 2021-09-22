
#define N 100

typedef char dummy;

struct _container{
  dummy		*nodes;
  dummy		*tail;
  int		rest;
  struct _container	*next;
};

typedef struct _container container;

typedef
  struct{
    container	*head;
    container	*tail;
    int		size;
    int		bound;
  } train;

void	TrainReset();		/* ( train *root, int size ) */
void	TrainFree();		/* ( train *root ) */
void	TrainLoad();		/* ( train *root, dummy *node ) */
int	TrainCheck();		/* ( train *root, dummy *node ) */
int	TrainComp();		/* ( train *root, dummy *node,
				      (*func)( *a1, *a2 ) ) */
dummy	*TrainIndex();		/* ( train *root, int x ) */
int	TrainBound();		/* ( train *root ) */
