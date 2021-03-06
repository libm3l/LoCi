//#############################################################################
//#
//# Copyright 2008-2019, Mississippi State University
//#
//# This file is part of the Loci Framework.
//#
//# The Loci Framework is free software: you can redistribute it and/or modify
//# it under the terms of the Lesser GNU General Public License as published by
//# the Free Software Foundation, either version 3 of the License, or
//# (at your option) any later version.
//#
//# The Loci Framework is distributed in the hope that it will be useful,
//# but WITHOUT ANY WARRANTY; without even the implied warranty of
//# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//# Lesser GNU General Public License for more details.
//#
//# You should have received a copy of the Lesser GNU General Public License
//# along with the Loci Framework.  If not, see <http://www.gnu.org/licenses>
//#
//#############################################################################
#include <iostream>
#include <queue>
#include <vector>
#include <map>
#include <set>
#include <Loci.h>
#include <Tools/tools.h>
#include "prism.h"

using std::queue;
using std::cerr;
using std::cout;
using std::endl;
using std::set;
using std::map;
using Loci::storeRepP;
class reset_prism_num_nodes : public pointwise_rule{
  const_store<std::vector<char> > cellPlan;
  const_store<std::vector<char> > facePlan;
  const_store<std::vector<char> > edgePlan;
  const_multiMap lower;
  const_multiMap upper;
  const_multiMap boundary_map;
  const_store<Array<char,5> > orientCode;
  const_store<Array<char,5> > prism2face;
  const_store<Array<char,6> > prism2node;
  const_multiMap face2node;
  const_multiMap face2edge;
  const_MapVec<2> edge2node;
  const_store<vect3d> pos;
  store<int> num_inner_nodes;

  const_store<int> node_l2f;
public:
  reset_prism_num_nodes(){
    name_store("balancedCellPlan", cellPlan);
    name_store("balancedFacePlan", facePlan);
    name_store("balancedEdgePlan", edgePlan);
    name_store("lower", lower);
    name_store("upper", upper);
    name_store("boundary_map", boundary_map);
    name_store("prismOrientCode", orientCode);
    name_store("prism2face", prism2face);
    name_store("prism2node", prism2node);
    name_store("face2node", face2node);
    name_store("face2edge", face2edge);
    name_store("edge2node", edge2node);
    name_store("pos", pos);
   
    name_store("balanced_num_inner_nodes", num_inner_nodes);
    name_store("fileNumber(face2node)", node_l2f);

    input("balancedCellPlan");
    input("(prism2face, prism2node, prismOrientCode)");
    input("(lower, upper, boundary_map)->balancedFacePlan");
    input("(lower, upper, boundary_map)->face2edge->balancedEdgePlan");
    input("(lower, upper, boundary_map)->face2node->pos");
    input("(lower, upper, boundary_map)->face2edge->edge2node");
    input("(lower, upper, boundary_map)->(fileNumber(face2node),face2edge)");
    
    output("balanced_num_inner_nodes");

    constraint("prisms");
  }
  virtual void compute(const sequence &seq){
    if(seq.size()!=0){
    
      do_loop(seq, this);
    }
    
  }
  void calculate(Entity cc){
    
    if(cellPlan[cc].size() == 0){
      num_inner_nodes[cc] = 0;

      return;
    }

    std::list<Node*> bnode_list; //boundary node
    std::list<Node*> node_list; //inner node
    std::list<Edge*> edge_list;
    std::list<QuadFace*> qface_list;
    std::list<Face*> gface_list;
    Prism* aCell = build_prism_cell(lower[cc].begin(), lower.num_elems(cc),
                                    upper[cc].begin(), upper.num_elems(cc),
                                    boundary_map[cc].begin(), boundary_map.num_elems(cc),
                                    prism2face[cc],
                                    prism2node[cc],
                                    orientCode[cc],
                                    face2node,
                                    face2edge,
                                    edge2node,
                                    pos,
                                    edgePlan,
                                    facePlan,
                                    bnode_list,
                                    edge_list,
                                    qface_list,
                                    gface_list,
                                    node_l2f);
    
    std::vector<Prism*> cells;
    aCell->resplit( cellPlan[cc],
                    node_list,
                    edge_list,
                    qface_list,
                    gface_list,
                    cells);
  
  
  
    num_inner_nodes[cc] = node_list.size();

    
    //clean up
    if(aCell != 0){
      delete aCell;
      aCell = 0;
    }
    
    //aCell will clean up these
    cleanup_list(bnode_list, edge_list, gface_list);
    cleanup_list(qface_list);
    cleanup_list(node_list);
  }
};
register_rule<reset_prism_num_nodes> register_reset_prism_num_nodes;  


class reset_prism_num_cells : public pointwise_rule{
  const_store<std::vector<char> > cellPlan;
  store<int> num_fine_cells;
  
public:
  reset_prism_num_cells(){
    name_store("balancedCellPlan", cellPlan);
    name_store("balanced_num_fine_cells", num_fine_cells);
    input("balancedCellPlan");
    output("balanced_num_fine_cells");
    constraint("prisms");
  }
  virtual void compute(const sequence &seq){
    if(seq.size()!=0){
      do_loop(seq, this);
    }
  
  }
  void calculate(Entity cc){
       
    if(cellPlan[cc].size() == 0){
      num_fine_cells[cc] = 1;
      return;
    }
    Prism* aCell = new Prism;
    num_fine_cells[cc] = aCell->empty_resplit(cellPlan[cc]);
    //clean up
    if(aCell != 0){
      delete aCell;
      aCell = 0;
    }
  }
};
register_rule<reset_prism_num_cells> register_reset_prism_num_cells;  


