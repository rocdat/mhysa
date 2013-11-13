#define _MAX_STRING_SIZE_ 500
#ifdef debug
  #define IERR ierr= 
  #define _DECLARE_IERR_ int ierr = 0; 
  #define CHECKERR(ierr) { if (ierr) return(ierr); }
#else
  #define IERR
  #define _DECLARE_IERR_
  #define CHECKERR(ierr)
#endif

#define _GetCoordinate_(dir,i,dim,ghosts,x,coord) \
  { \
    int d,offset = 0; \
    for (d = 0; d < dir; d++) offset += (dim[d]+2*ghosts); \
    coord = (x[offset+ghosts+i]); \
  }


