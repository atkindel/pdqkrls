#include <Rcpp.h>
using namespace Rcpp;

#include "HODLR_Tree.hpp"
#include "HODLR_Matrix.hpp"
#include <Eigen/Dense>

using std::cout;
using std::vector;
using std::setprecision;
using std::sort;

// RLS_Kernel is just a thin interface to a NumericMatrix in memory
//   TODO: Actually I think I'm making a copy as it sits now...
// This should work with the HODLR API since it yields doubles
// Come back to this if in-memory performance is poor
class RLS_Kernel : public HODLR_Matrix {
  private:
    NumericMatrix _G;
  public:
    RLS_Kernel (NumericMatrix G): _G(G) {};
    double get_Matrix_Entry(const unsigned i, const unsigned j) { return _G(i, j); };
};

// [[Rcpp::export]]
NumericMatrix compute_Ginv(NumericMatrix G, NumericVector resp, int N) {
  unsigned nLeaf	=	100;  // TODO: What goes into this choice? Let user tweak this?
  double tolerance=	1e-15;  // TODO: What's a reasonable default here? Let user tweak this?
  
  // Set up kernel matrix
  RLS_Kernel kern (G);

  // Construct new kernel matrix
  HODLR_Tree<RLS_Kernel> *A = new HODLR_Tree<RLS_Kernel>(&kern, N, nLeaf);
  
  // Assemble matrix in HODLR form
  // TODO: This takes a VectorXd, not a NumericVector
  A->assemble_Matrix(resp, tolerance, 's');
  
  // Factor matrix
  A->compute_Factor();
  
  // Solve system
  // TODO: What is b? Try to decipher examples for this
  MatrixXd x;  // As far as I can tell, this just needs to be initialized?
  A->solve(b, x);
    
  // TODO: How to return inverted matrix?
}
