//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Author:
//
//    John Burkardt
//

# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <cmath>

using namespace std;

# include "rcm.h"

void RCM_Test01 ( )

//---------------------------------------------------------------------------
//  Purpose:
//
//    RCM_Test01 tests ADJ_SET.
//
//  Modified:
//
//    04 January 2007
//
{
# define NODE_NUM 10
# define ADJ_MAX ( NODE_NUM * ( NODE_NUM - 1 ) )

  int adj[ADJ_MAX];
  int adj_num;
  int adj_row[NODE_NUM+1];
  int i;
  int j;
  int k;
  int n_calls;
  int seed = 123456789;

  cout << "\n";
  cout << "RCM_Test01\n";
  cout << "  ADJ_SET sets up an adjacency matrix incrementally.\n";

  n_calls = i4_uniform ( 1, ADJ_MAX, &seed );

  adj_set ( NODE_NUM, ADJ_MAX, &adj_num, adj_row, adj, -1, -1 );

  cout << "\n";
  cout << "  Creating and recording adjacency information:\n";
  cout << "\n";

  for ( k = 1; k <= n_calls; k++ )
  {
    i = i4_uniform ( 1, NODE_NUM, &seed );
    j = i4_uniform ( 1, NODE_NUM, &seed );

    cout << "  " << setw(8) << i
         << "  " << setw(8) << j << "\n";

    adj_set ( NODE_NUM, ADJ_MAX, &adj_num, adj_row, adj, i, j );
  }

  adj_print ( NODE_NUM, adj_num, adj_row, adj,
    "  Random adjacency matrix:" );

  adj_show ( NODE_NUM, adj_num, adj_row, adj );

  return;
# undef NODE_NUM
# undef ADJ_MAX
}
//---------------------------------------------------------------------------

void RCM_Test02 ( )

//---------------------------------------------------------------------------
//
//  Purpose:
//
//    RCM_Test02 tests GENRCM;
//
//  Modified:
//
//    05 January 2007
//
{
  int *adj;
  int adj_num;
  int *adj_row;
  int bandwidth;
  int i;
  int node_num;
  int *perm;
  int *perm_inv;

  cout << "\n";
  cout << "RCM_Test02\n";
  cout << "  GENRCM reorders the nodes in a graph using\n";
  cout << "  the Reverse Cuthill McKee algorithm.\n";

  graph_01_size ( &node_num, &adj_num );

  adj_row =  new int[node_num+1];
  adj = new int[adj_num];
  perm = new int[node_num];
  perm_inv = new int[node_num];

  graph_01_adj ( node_num, adj_num, adj_row, adj );

  adj_print ( node_num, adj_num, adj_row, adj, "  Adjacency matrix:" );

  adj_show ( node_num, adj_num, adj_row, adj );

  bandwidth = adj_bandwidth ( node_num, adj_num, adj_row, adj );

  cout << "\n";
  cout << "  ADJ bandwidth = " << bandwidth << "\n";

  genrcm ( node_num, adj_num, adj_row, adj, perm );

  perm_inverse3 ( node_num, perm, perm_inv );

  cout << "\n";
  cout << "  The RCM permutation and inverse:\n";
  cout << "\n";

  for ( i = 0; i < node_num; i++ )
  {
    cout << "  " << setw(8) << i + 1
         << "  " << setw(8) << perm[i]
         << "  " << setw(8) << perm_inv[i] << "\n";
  }

  cout << "\n";
  cout << "  Permuted adjacency matrix:\n";
  cout << "\n";

  adj_perm_show ( node_num, adj_num, adj_row, adj, perm, perm_inv );

  bandwidth = adj_perm_bandwidth ( node_num, adj_num, adj_row, adj,
    perm, perm_inv );

  cout << "\n";
  cout << "  ADJ (permuted) bandwidth = " << bandwidth << "\n";

  delete [] adj;
  delete [] adj_row;
  delete [] perm;
  delete [] perm_inv;

  return;
}
//---------------------------------------------------------------------------

void RCM_Test03 ( )

//---------------------------------------------------------------------------
//
//  Purpose:
//
//    RCM_Test03 tests GENRCM
//
//  Modified:
//
//    06 January 2007
//
{
  int *adj;
  int adj_num;
  int *adj_row;
  int bandwidth;
  int hole_num;
  int i;
  int j;
  int node;
  int node_num;
  double *node_xy;
  int *perm;
  int *perm_inv;
  int seed;
  int test;
  int triangle_num;
  int *triangle_neighbor;
  int *triangle_node;

  cout << "\n";
  cout << "RCM_Test03\n";
  cout << "  GENRCM generates the Reverse Cuthill McKee ordering.\n";
  cout << "\n";
  cout << "  Do the test twice.  On the second test, randomly\n";
  cout << "  permute the initial nodes.\n";

  triangulation_order3_example2_size ( &node_num, &triangle_num, &hole_num );

  for ( test = 1; test <= 2; test++ )
  {
    node_xy = new double[2*node_num];
    triangle_node = new int[3*triangle_num];
    triangle_neighbor = new int[3*triangle_num];

    triangulation_order3_example2 ( node_num, triangle_num, node_xy,
      triangle_node, triangle_neighbor );
//
//  Randomly permute the nodes.
//
    if ( test == 2 )
    {
      seed = 123456789;

      perm = perm_uniform ( node_num, &seed );

      i4vec_print ( node_num, perm, "  The random permutation:" );

      for ( i = 0; i < 3; i++ )
      {
        for ( j = 0; j < triangle_num; j++ )
        {
          node = triangle_node[i+j*3];
          triangle_node[i+j*3] = perm[node-1];
        }
      }
      delete [] perm;
    }

    i4mat_transpose_print ( 3, triangle_num, triangle_node,
      "  TRIANGLE_NODE:" );

    adj_row = new int[node_num+1];

    adj_num = triangulation_order3_adj_count ( node_num, triangle_num,
      triangle_node, triangle_neighbor, adj_row );

    adj = triangulation_order3_adj_set ( node_num, triangle_num, triangle_node,
      triangle_neighbor, adj_num, adj_row );

    adj_print ( node_num, adj_num, adj_row, adj, "  ADJ array:" );

    bandwidth = adj_bandwidth ( node_num, adj_num, adj_row, adj );

    cout << "\n";
    cout << "  ADJ bandwidth = " << bandwidth << "\n";

    perm = new int[node_num];

    genrcm ( node_num, adj_num, adj_row, adj, perm );

    i4vec_print ( node_num, perm, "  The RCM permutation:" );

    perm_inv = new int[node_num];

    perm_inverse3 ( node_num, perm, perm_inv );

    bandwidth = adj_perm_bandwidth ( node_num, adj_num, adj_row, adj,
      perm, perm_inv );

    cout << "\n";
    cout << "  Permuted ADJ bandwidth = " << bandwidth << "\n";

    delete [] adj;
    delete [] adj_row;
    delete [] node_xy;
    delete [] perm;
    delete [] perm_inv;
    delete [] triangle_neighbor;
    delete [] triangle_node;
  }
  return;
}
//---------------------------------------------------------------------------

void RCM_Test04 ( )

//---------------------------------------------------------------------------
//
//  Purpose:
//
//    RCM_Test04 tests GENRCM
//
//  Modified:
//
//    06 January 2007
//
{
  int *adj;
  int adj_num;
  int *adj_row;
  int bandwidth;
  int hole_num;
  int i;
  int j;
  int node;
  int node_num;
  double *node_xy;
  int *perm;
  int *perm_inv;
  int seed;
  int triangle_num;
  int *triangle_neighbor;
  int *triangle_node;
  int triangle_order = 6;

  cout << "\n";
  cout << "RCM_Test04\n";
  cout << "  GENRCM generates the Reverse Cuthill McKee ordering.\n";

  triangulation_order6_example2_size ( &node_num, &triangle_num, &hole_num );

  node_xy = new double[2*node_num];
  triangle_node = new int[triangle_order*triangle_num];
  triangle_neighbor = new int[3*triangle_num];

  triangulation_order6_example2 ( node_num, triangle_num, node_xy,
    triangle_node, triangle_neighbor );
//
//  Randomly permute the nodes.
//
  seed = 123456789;

  perm = perm_uniform ( node_num, &seed );

  i4vec_print ( node_num, perm, "  The random permutation:" );

  for ( i = 0; i < triangle_order; i++ )
  {
    for ( j = 0; j < triangle_num; j++ )
    {
      node = triangle_node[i+j*triangle_order];
      triangle_node[i+j*triangle_order] = perm[node-1];
    }
  }

  i4mat_transpose_print ( triangle_order, triangle_num, triangle_node,
    "  Permuted TRIANGLE_NODE" );

  delete [] perm;

  adj_row = new int[node_num+1];

  adj_num = triangulation_order6_adj_count ( node_num, triangle_num,
    triangle_node, triangle_neighbor, adj_row );

  adj = triangulation_order6_adj_set ( node_num, triangle_num, triangle_node,
    triangle_neighbor, adj_num, adj_row );

  adj_print ( node_num, adj_num, adj_row, adj, "  ADJ array:" );

  bandwidth = adj_bandwidth ( node_num, adj_num, adj_row, adj );

  cout << "\n";
  cout << "  ADJ bandwidth = " << bandwidth << "\n";

  perm = new int[node_num];

  genrcm ( node_num, adj_num, adj_row, adj, perm );

  i4vec_print ( node_num, perm, "  The RCM permutation:" );

  perm_inv = new int[node_num];

  perm_inverse3 ( node_num, perm, perm_inv );

  bandwidth = adj_perm_bandwidth ( node_num, adj_num, adj_row, adj,
    perm, perm_inv );

  cout << "\n";
  cout << "  Permuted ADJ bandwidth = " << bandwidth << "\n";

  delete [] adj;
  delete [] adj_row;
  delete [] node_xy;
  delete [] perm;
  delete [] perm_inv;
  delete [] triangle_neighbor;
  delete [] triangle_node;

  return;
}
//---------------------------------------------------------------------------

void RCM_Test05 ( )

//---------------------------------------------------------------------------
//
//  Purpose:
//
//    RCM_Test05 tests GRAPH_01_ADJ and GRAPH_01_SIZE;
//
//  Modified:
//
//    04 January 2007
//
{
  int *adj;
  int adj_num;
  int *adj_row;
  int node_num;

  cout << "\n";
  cout << "RCM_Test05\n";
  cout << "  GRAPH_01_SIZE returns the sizes for graph 1.\n";
  cout << "  GRAPH_01_ADJ returns the adjacency for graph 1.\n";
  cout << "  ADJ_PRINT prints the adjacency information.\n";

  graph_01_size ( &node_num, &adj_num );

  adj_row = new int[node_num+1];
  adj = new int[adj_num];

  graph_01_adj ( node_num, adj_num, adj_row, adj );

  adj_print ( node_num, adj_num, adj_row, adj,
    "  Adjacency for GRAPH_01:" );

  adj_show ( node_num, adj_num, adj_row, adj );

  delete [] adj;
  delete [] adj_row;

  return;
}
//---------------------------------------------------------------------------

void RCM_Test06 ( )

//---------------------------------------------------------------------------
//
//  Purpose:
//
//    RCM_Test06 tests LEVEL_SET;
//
//  Modified:
//
//    05 January 2007
//
{
  int *adj;
  int adj_num;
  int *adj_row;
  int i;
  int j;
  int *level;
  int level_num;
  int *level_row;
  int *mask;
  int node_num;
  int root;
  int seed = 123456789;

  cout << "\n";
  cout << "RCM_Test06\n";
  cout << "  LEVEL_SET computes the level sets of a graph,\n";
  cout << "  given a root node (which defines level 1).\n";

  graph_01_size ( &node_num, &adj_num );

  adj_row = new int[node_num+1];
  adj = new int[adj_num];

  graph_01_adj ( node_num, adj_num, adj_row, adj );

  adj_print ( node_num, adj_num, adj_row, adj, "  Adjacency matrix:" );

  adj_show ( node_num, adj_num, adj_row, adj );
//
//  Choose different roots.
//
  level = new int[node_num];
  level_row = new int[node_num+1];
  mask = new int[node_num];

  for ( i = 1; i <= 3; i++ )
  {
    root = i4_uniform ( 1, node_num, &seed );

    for ( j = 0; j < node_num; j++ )
    {
      mask[j] = 1;
    }

    level_set ( root, adj_num, adj_row, adj, mask, &level_num,
      level_row, level, node_num );

    level_set_print ( node_num, level_num, level_row, level );
  }

  delete [] adj;
  delete [] adj_row;
  delete [] level;
  delete [] level_row;
  delete [] mask;

  return;
}
//---------------------------------------------------------------------------

void RCM_Test07 ( )

//---------------------------------------------------------------------------
//
//  Purpose:
//
//    RCM_Test07 tests ROOT_FIND;
//
//  Modified:
//
//    07 January 2007
//
{
  int *adj;
  int adj_num;
  int *adj_row;
  int i;
  int j;
  int *level;
  int level_num;
  int *level_row;
  int *mask;
  int node_num;
  int root;
  int seed = 123456789;

  cout << "\n";
  cout << "RCM_Test07\n";
  cout << "  ROOT_FIND is given a node in the graph,\n";
  cout << "  and returns a better node to use as a starting\n";
  cout << "  point for reordering.\n";

  graph_01_size ( &node_num, &adj_num );

  adj_row = new int[node_num+1];
  adj = new int[adj_num];

  graph_01_adj ( node_num, adj_num, adj_row, adj );

  adj_print ( node_num, adj_num, adj_row, adj, "  Adjacency matrix:" );

  adj_show ( node_num, adj_num, adj_row, adj );

  level = new int[node_num];
  level_row = new int[node_num+1];
  mask = new int[node_num];

  for ( i = 1; i <= node_num; i++ )
  {
    root = i;

    cout << "\n";
    cout << "  Starting root =    " << root << "\n";

    for ( j = 0; j < node_num; j++ )
    {
      mask[j] = 1;
    }

    root_find ( &root, adj_num, adj_row, adj, mask, &level_num,
      level_row, level, node_num );

    cout << "  Suggested root =   " << root << "\n";
    cout << "  Number of levels = " << level_num << "\n";
  }

  delete [] adj;
  delete [] adj_row;
  delete [] level;
  delete [] level_row;
  delete [] mask;

  return;
}
//---------------------------------------------------------------------------

void RCM_Test08 ( )

//---------------------------------------------------------------------------
//
//  Purpose:
//
//    RCM_Test08 tests TRIANGULATION_ORDER3_ADJ_COUNT
//
//  Modified:
//
//    04 January 2007
//
{
  int adj_num;
  int *adj_row;
  int hole_num;
  int node_num;
  double *node_xy;
  int triangle_num;
  int *triangle_neighbor;
  int *triangle_node;

  cout << "\n";
  cout << "RCM_Test08\n";
  cout << "  TRIANGULATION_ORDER3_ADJ_COUNT counts the (lower)\n";
  cout << "  adjacencies defined by a triangulation.\n";

  triangulation_order3_example2_size ( &node_num, &triangle_num, &hole_num );

  node_xy = new double[2*node_num];
  triangle_node = new int[3*triangle_num];
  triangle_neighbor = new int[3*triangle_num];

  triangulation_order3_example2 ( node_num, triangle_num, node_xy,
    triangle_node, triangle_neighbor );

  i4mat_transpose_print ( 3, triangle_num, triangle_node,
    "  TRIANGLE_NODE" );

  adj_row = new int[node_num+1];

  adj_num = triangulation_order3_adj_count ( node_num, triangle_num,
    triangle_node, triangle_neighbor, adj_row );

  i4vec_print ( node_num+1, adj_row, "  ADJ_ROW" );

  delete [] adj_row;
  delete [] node_xy;
  delete [] triangle_neighbor;
  delete [] triangle_node;

  return;
}
//---------------------------------------------------------------------------

void RCM_Test09 ( )

//---------------------------------------------------------------------------
//
//  Purpose:
//
//    RCM_Test09 tests TRIANGULATION_ORDER3_ADJ_SET
//
//  Modified:
//
//    04 January 2007
//
{
  int *adj;
  int adj_num;
  int *adj_row;
  int bandwidth;
  int hole_num;
  int node_num;
  double *node_xy;
  int triangle_num;
  int *triangle_neighbor;
  int *triangle_node;

  cout << "\n";
  cout << "RCM_Test09\n";
  cout << "  TRIANGULATION_ORDER3_ADJ_SET sets the (lower)\n";
  cout << "  adjacencies defined by a triangulation.\n";

  triangulation_order3_example2_size ( &node_num, &triangle_num, &hole_num );

  node_xy = new double[2*node_num];
  triangle_node = new int[3*triangle_num];
  triangle_neighbor = new int[3*triangle_num];

  triangulation_order3_example2 ( node_num, triangle_num, node_xy,
    triangle_node, triangle_neighbor );

  i4mat_transpose_print ( 3, triangle_num, triangle_node,
    "  TRIANGLE_NODE" );

  adj_row = new int[node_num+1];

  adj_num = triangulation_order3_adj_count ( node_num, triangle_num,
    triangle_node, triangle_neighbor, adj_row );

  adj = triangulation_order3_adj_set ( node_num, triangle_num, triangle_node,
    triangle_neighbor, adj_num, adj_row );

  adj_print ( node_num, adj_num, adj_row, adj, "  ADJ array:" );

  bandwidth = adj_bandwidth ( node_num, adj_num, adj_row, adj );

  cout << "\n";
  cout << "  ADJ bandwidth = " << bandwidth << "\n";

  delete [] adj;
  delete [] adj_row;
  delete [] node_xy;
  delete [] triangle_neighbor;
  delete [] triangle_node;

  return;
}
//---------------------------------------------------------------------------

void RCM_Test10 ( )

//---------------------------------------------------------------------------
//
//  Purpose:
//
//    RCM_Test10 tests TRIANGULATION_NEIGHBOR_TRIANGLES.
//
//  Modified:
//
//    28 September 2009
//
{
# define TRIANGLE_ORDER 3
# define TRIANGLE_NUM 16

  int triangle_node[TRIANGLE_ORDER*TRIANGLE_NUM] = {
     3,   4,   1,
     3,   1,   2,
     3,   2,   8,
     2,   1,   5,
     8,   2,  13,
     8,  13,   9,
     3,   8,   9,
    13,   2,   5,
     9,  13,   7,
     7,  13,   5,
     6,   7,   5,
     9,   7,   6,
    10,   9,   6,
     6,   5,  12,
    11,   6,  12,
    10,   6,  11 };
  int *triangle_neighbor;

  cout << "\n";
  cout << "RCM_Test10\n";
  cout << "  For a triangulation of a set of nodes,\n";
  cout << "  TRIANGULATION_NEIGHBOR_TRIANGLES determines the\n";
  cout << "    adjacency relationships between triangles.\n";

  i4mat_transpose_print ( TRIANGLE_ORDER, TRIANGLE_NUM, triangle_node,
    "  Triangles:" );

  triangle_neighbor = triangulation_neighbor_triangles ( TRIANGLE_ORDER,
    TRIANGLE_NUM, triangle_node );

  i4mat_transpose_print ( 3, TRIANGLE_NUM, triangle_neighbor,
    "  Triangle neighbors:" );

  delete [] triangle_neighbor;

  return;
# undef TRIANGLE_NUM
# undef TRIANGLE_ORDER
}
//---------------------------------------------------------------------------

void RCM_Test11 ( )

//---------------------------------------------------------------------------
//
//  Purpose:
//
//    RCM_Test11 tests TRIANGULATION_ORDER6_ADJ_COUNT
//
//  Modified:
//
//    04 January 2007
//
{
  int adj_num;
  int *adj_row;
  int hole_num;
  int node_num;
  double *node_xy;
  int triangle_num;
  int *triangle_neighbor;
  int *triangle_node;

  cout << "\n";
  cout << "RCM_Test11\n";
  cout << "  TRIANGULATION_ORDER6_ADJ_COUNT counts the (lower)\n";
  cout << "  adjacencies defined by a triangulation.\n";

  triangulation_order6_example2_size ( &node_num, &triangle_num, &hole_num );

  node_xy = new double[2*node_num];
  triangle_node = new int[6*triangle_num];
  triangle_neighbor = new int[3*triangle_num];

  triangulation_order6_example2 ( node_num, triangle_num, node_xy,
    triangle_node, triangle_neighbor );

  adj_row = new int[node_num+1];

  adj_num = triangulation_order6_adj_count ( node_num, triangle_num,
    triangle_node, triangle_neighbor, adj_row );

  i4vec_print ( node_num+1, adj_row, "  ADJ_ROW" );

  delete [] adj_row;
  delete [] node_xy;
  delete [] triangle_neighbor;
  delete [] triangle_node;

  return;
}
//---------------------------------------------------------------------------

void RCM_Test12 ( )

//---------------------------------------------------------------------------
//
//  Purpose:
//
//    RCM_Test12 tests TRIANGULATION_ORDER6_ADJ_SET
//
//  Modified:
//
//    05 January 2007
//
{
  int *adj;
  int adj_num;
  int *adj_row;
  int bandwidth;
  int hole_num;
  int node_num;
  double *node_xy;
  int triangle_num;
  int *triangle_neighbor;
  int *triangle_node;

  cout << "\n";
  cout << "RCM_Test12\n";
  cout << "  TRIANGULATION_ORDER6_ADJ_SET sets the (lower)\n";
  cout << "  adjacencies defined by a triangulation.\n";

  triangulation_order6_example2_size ( &node_num, &triangle_num, &hole_num );

  node_xy = new double[2*node_num];
  triangle_node = new int[6*triangle_num];
  triangle_neighbor = new int[3*triangle_num];

  triangulation_order6_example2 ( node_num, triangle_num, node_xy,
    triangle_node, triangle_neighbor );

  i4mat_transpose_print ( 6, triangle_num, triangle_node,
    "  TRIANGLE_NODE" );

  adj_row = new int[node_num+1];

  adj_num = triangulation_order6_adj_count ( node_num, triangle_num, triangle_node,
    triangle_neighbor, adj_row );

  adj = new int [adj_num];

  adj = triangulation_order6_adj_set ( node_num, triangle_num, triangle_node,
    triangle_neighbor, adj_num, adj_row );

  adj_print ( node_num, adj_num, adj_row, adj, "  ADJ array:" );

  bandwidth = adj_bandwidth ( node_num, adj_num, adj_row, adj );

  cout << "\n";
  cout << "  ADJ bandwidth = " << bandwidth << "\n";

  delete [] adj;
  delete [] adj_row;
  delete [] node_xy;
  delete [] triangle_neighbor;
  delete [] triangle_node;

  return;
}
