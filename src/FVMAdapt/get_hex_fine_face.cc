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
////////////////////////////////////////////////////////////////////////////////////////
//                                get_hex_fine_face.cc                                    //
//                                by: Qiuhan Xue                                      //
//                                                                                    //
// This file generate  fine_faces according refinement plans. The
//original grid is hexcell element. Anisotropic refinement is used. the rules should be
//used After node_offset and cell_offset has been set up.
// FineFaces is defined as c1, c2,followed by face2node.
//
////////////////////////////////////////////////////////////////////////////////////////                             


#include <iostream>
#include <vector>
#include <Loci.h>
#include <Tools/tools.h>
#include "hexcell.h"

using std::cerr;
using std::endl;
using std::vector;
//get  fine_faces of hex cells
class get_hex_cell_faces : public pointwise_rule{
  const_store<std::vector<char> > cellPlan;
  const_store<std::vector<char> > facePlan;
  const_store<std::vector<char> > edgePlan;
  const_multiMap lower;
  const_multiMap upper;
  const_multiMap boundary_map;
   const_store<Array<char,6> > orientCode;
  const_store<Array<char,6> > hex2face;
  const_store<Array<char,8> > hex2node;
  const_multiMap face2node;
  const_multiMap face2edge;
  const_MapVec<2> edge2node;
  const_store<vect3d> pos;
  const_store<int> node_offset;
  const_store<int> cell_offset;
 
  store<Loci::FineFaces> fine_faces;

  const_store<int> face_l2f;
  const_store<int> node_l2f;
public:
  get_hex_cell_faces(){
    name_store("balancedCellPlan", cellPlan);
    name_store("balancedFacePlan", facePlan);
    name_store("balancedEdgePlan", edgePlan);
    name_store("lower", lower);
    name_store("upper", upper);
    name_store("boundary_map", boundary_map);
    name_store("hexOrientCode", orientCode);
    name_store("hex2face", hex2face);
    name_store("hex2node", hex2node);
    
    name_store("face2node", face2node);
    name_store("face2edge", face2edge);
    name_store("edge2node", edge2node);
    name_store("pos", pos);
    name_store("balanced_node_offset", node_offset);
    name_store("balanced_cell_offset", cell_offset);
    
    name_store("fine_faces", fine_faces);
    name_store("fileNumber(face2node)", face_l2f);
    name_store("fileNumber(pos)", node_l2f);
    
    input("balancedCellPlan,balanced_node_offset,balanced_cell_offset");
    input("(hex2face, hex2node, hexOrientCode)");
    input("(lower, upper, boundary_map)->(balancedFacePlan,fileNumber(face2node), balanced_node_offset)");
    input("(lower, upper, boundary_map)->face2edge->(balancedEdgePlan,balanced_node_offset)");
    input("(lower, upper, boundary_map)->face2node->(pos, fileNumber(pos))");
    input("(lower, upper, boundary_map)->face2edge->edge2node->(pos, fileNumber(pos))");
    
    output("fine_faces");
    constraint("hexcells");
  }
  virtual void compute(const sequence &seq){
    if(seq.size()!=0){
     
      do_loop(seq, this);
    }
   
  }
  void calculate(Entity cc){
    
   
    
    if(cellPlan[cc].size() == 0){
      vector<vector<int> >().swap(fine_faces[cc]);
      return;
    }
    
    std::list<Node*> bnode_list; //boundary node
    std::list<Edge*> edge_list;
    std::list<QuadFace*> face_list;
    std::list<Node*> node_list; //inner node
    int c1, c2;

   //build a Cell
      HexCell* aCell =  build_hex_cell(lower[cc].begin(), lower.num_elems(cc),
                                       upper[cc].begin(), upper.num_elems(cc),
                                       boundary_map[cc].begin(), boundary_map.num_elems(cc),
                                       hex2face[cc],
                                       hex2node[cc],
                                       orientCode[cc],
                                       face2node,
                                       face2edge,
                                       edge2node,
                                       pos,
                                       edgePlan,
                                       facePlan,
                                       node_offset,
                                       face_l2f,
                                       node_l2f,
                                       bnode_list,
                                       edge_list,
                                       face_list);
          
      std::vector<HexCell*> cells;
      
      aCell->resplit(cellPlan[cc], 
                     node_list,
                     edge_list,
                     face_list,
                     cells);
          
 
    // and index the nodes
   
    int nindex = 0;
    for(std::list<Node*>::const_iterator np = node_list.begin(); np!= node_list.end(); np++, nindex++){
      
      (*np)->index = node_offset[cc] + nindex;
    }
    //set up inner faces
    std::map<QuadFace*, NeibIndex> inner_faces;
    set_hex_faces(cells,inner_faces);
    
    //put inner faces into fine_faces[cc]
    std::vector<std::vector<int> >(inner_faces.size()).swap(fine_faces[cc]);
    
    int fptr = 0;                    
    for(std::map<QuadFace*, NeibIndex>::const_iterator pf = inner_faces.begin();
        pf != inner_faces.end(); pf++,fptr++){

      // face2node is store in faceVertex 
      std::list<int32> faceVertex;
      //for each face, set up face2node 
      pf->first->set_f2n(faceVertex);
      //transfer c1 , c2 from local(inside a original cell) numbering  to file numbering
      c2 = (pf->second).c2+cell_offset[cc];
      c1 = (pf->second).c1+cell_offset[cc];
      
      //put c1, c2 and faceVertex into one vector
      int vec_size = faceVertex.size() +2;
      std::vector<int>(vec_size).swap(fine_faces[cc][fptr]);
      fine_faces[cc][fptr][0] = c1;
      fine_faces[cc][fptr][1] = c2;

      std::list<int32>::const_iterator nptr = faceVertex.begin();
      for(int i = 2; i < vec_size; i++,nptr++){
        fine_faces[cc][fptr][i] = *nptr;
      }
    } 
    
    //clean up
    if(aCell != 0){
      delete aCell;
      aCell = 0;
    }
    
    //aCell will clean up these
    cleanup_list(bnode_list, edge_list, face_list);
    cleanup_list(node_list);
  }
};
register_rule<get_hex_cell_faces> register_get_hex_cell_faces;  


//get  fine_faces of interior faces
class get_interior_hex_faces : public pointwise_rule{
  const_store<std::vector<char> > facePlan;
  const_store<std::vector<char> > edgePlan;
  const_store<std::vector<char> > cellPlan;
  const_multiMap lower;
  const_multiMap upper;
  const_multiMap boundary_map;
  const_store<Array<char,6> > orientCode;
  const_store<Array<char,6> > hex2face;
  const_store<Array<char,8> > hex2node;
  const_store<char> fl;
  const_store<char> fr;
  const_multiMap face2node;
  const_multiMap face2edge;
  const_MapVec<2> edge2node;
  const_store<vect3d> pos; 
  const_Map cl;
  const_Map cr;
  const_store<int> node_offset;
  const_store<int> cell_offset;
 
    
 
  store<Loci::FineFaces> fine_faces;

  const_store<int> node_l2f;
  
public:
  get_interior_hex_faces(){
    name_store("balancedFacePlan", facePlan);
    name_store("balancedEdgePlan", edgePlan);
    name_store("balancedCellPlan", cellPlan);
    name_store("lower", lower);
    name_store("upper", upper);
    name_store("boundary_map", boundary_map);
     name_store("hexOrientCode", orientCode);
    name_store("hex2face", hex2face);
    name_store("hex2node", hex2node);
    name_store("fl", fl);
    name_store("fr", fr);
    name_store("face2node", face2node);
    name_store("face2edge", face2edge);
     name_store("edge2node", edge2node);
    name_store("pos", pos);
    name_store("cl", cl);
    name_store("cr", cr);
    name_store("balanced_node_offset", node_offset);
    name_store("balanced_cell_offset", cell_offset);
    name_store("fileNumber(pos)", node_l2f);
 
    name_store("fine_faces", fine_faces);
    
    input("balancedFacePlan, balanced_node_offset");
    input("face2edge->(balancedEdgePlan, balanced_node_offset)");
    input("(cl, cr)->(balancedCellPlan, balanced_cell_offset, hex2node, hex2face, hexOrientCode)");
    input("(fl, fr)");
    input("(cl, cr)->(upper, lower, boundary_map)->face2node->pos");
    input("(cl, cr)->(upper, lower, boundary_map)-> face2edge->edge2node->pos");
    input("face2node->(pos, fileNumber(pos))");
   
    output("fine_faces");
    constraint("(cl, cr)->hexcells");
  }
  virtual void compute(const sequence &seq){
    if(seq.size()!=0){
    
      do_loop(seq, this);
    }
  }
  void calculate(Entity f){
   
   
    
    std::list<Node*> bnode_list, node_list;
    std::list<Edge*> edge_list;
    std::list<int32> faceVertex;
    int c1, c2;
      QuadFace* aFace = build_quad_face(face2node[f].begin(),
                                        face2edge[f].begin(),
                                        edge2node,
                                        pos,
                                        edgePlan,
                                        node_offset,
                                        node_l2f,
                                        bnode_list,
                                        edge_list);
      

      if(facePlan[f].size() == 0){
      
      

        //num of fine faces is 1. 
        aFace->set_f2n(faceVertex);
        vector<vector<int> >(1).swap(fine_faces[f]); 
        int vec_size = faceVertex.size()+2;
        vector<int>(vec_size).swap(fine_faces[f][0]);

      //get c1, c2(local numbering start with 1)

        //get CL and CR
        std::vector<int32> CL = get_c1_hex(cellPlan[cl[f]],
                                           facePlan[f],
                                           orientCode[cl[f]][fl[f]],
                                           fl[f]);
        
        if( CL.size()!= 1){
          cerr<<"WARNING: CL has the incorrect size" << endl;
          Loci::Abort();
        }
        
        std::vector<int32>    CR = get_c1_hex(cellPlan[cr[f]],
                                              facePlan[f],
                                              orientCode[cr[f]][fr[f]],
                                              fr[f]);
        
        
        if( CR.size()!= 1){
          cerr<<"WARNING: CR has the incorrect size" << endl;
          Loci::Abort();
        }
        c1 = cell_offset[cl[f]] +CL[0];
        c2 = cell_offset[cr[f]]+CR[0];
        
        fine_faces[f][0][0] = c1;
        fine_faces[f][0][1] = c2;
        
        std::list<int32>::const_iterator nptr = faceVertex.begin();
        for(int i = 2; i < vec_size; i++, nptr++){
          fine_faces[f][0][i] = *nptr;
        }
        //clean up
        if(aFace != 0) delete aFace;
        cleanup_list(bnode_list, edge_list);  
        return;
      }
      
      //split the face
      std::vector<QuadFace*> leaves;
      
      aFace->resplit(facePlan[f],
                     char(0),
                     node_list,
                     edge_list,
                     leaves);
      // and index the node_list
   
    int nindex = 0;
    for(std::list<Node*>::const_iterator np = node_list.begin(); np!= node_list.end(); np++,nindex++){
   
      (*np)->index = node_offset[f] + nindex;
    }
   
  
    //get CL and CR
    std::vector<int32> CL = get_c1_hex(cellPlan[cl[f]],
                                           facePlan[f],
                                           orientCode[cl[f]][fl[f]],
                                           fl[f]);
  
    if(leaves.size() != CL.size()){
      cerr<<"WARNING: CL has the incorrect size" << endl;
      Loci::Abort();
    }
    
    std::vector<int32>    CR = get_c1_hex(cellPlan[cr[f]],
                                          facePlan[f],
                                          orientCode[cr[f]][fr[f]],
                                          fr[f]);
    
    
    if(leaves.size() != CR.size()){
      cerr<<"WARNING: CR has the incorrect size" << endl;
      Loci::Abort();
    }

    //set the size of fine_faces[f]
    vector<std::vector<int> >(leaves.size()).swap(fine_faces[f]);
    
    //set fine_faces[f]
    for( unsigned int  fptr = 0; fptr < leaves.size(); fptr++){
      
      //set c1
      if(cellPlan[cl[f]].size() == 0){
        c1= cell_offset[cl[f]] + 1;
      }
      else{
        c1= cell_offset[cl[f]] + CL[fptr];              
      }
      //set c2
      if(cellPlan[cr[f]].size() == 0){
        c2 = cell_offset[cr[f]] + 1;
      }
      else{
        c2 = cell_offset[cr[f]] + CR[fptr]; 
      }
      
      //set faceVertex
      faceVertex.clear();
      leaves[fptr]->set_f2n(faceVertex);
      
      int vec_size = faceVertex.size() +2;
      vector<int>(vec_size).swap(fine_faces[f][fptr]);
      fine_faces[f][fptr][0] = c1;
      fine_faces[f][fptr][1] = c2;
      std::list<int32>::const_iterator nptr = faceVertex.begin();
      
      for(int i = 2; i < vec_size; i++,nptr++){
        fine_faces[f][fptr][i] = *nptr;
      }
          
    } 
    
    //clean up
    if(aFace != 0){
      delete aFace;
      aFace = 0;
    }
    cleanup_list(node_list);
    cleanup_list(bnode_list, edge_list);
  }
};
register_rule<get_interior_hex_faces> register_get_interior_hex_faces;

//get  fine_faces of boundary faces
class get_boundary_hex_faces : public pointwise_rule{
  const_store<std::vector<char> > facePlan;
  const_store<std::vector<char> > edgePlan;
  const_store<std::vector<char> > cellPlan;
  const_multiMap lower;
  const_multiMap upper;
  const_multiMap boundary_map;
  const_store<Array<char,6> > orientCode;
  const_store<Array<char,6> > hex2face;
  const_store<Array<char,8> > hex2node;
  const_store<char> fl;
 
  const_multiMap face2node;
  const_multiMap face2edge;
  const_MapVec<2> edge2node;
  const_store<vect3d> pos; 
  const_Map cl;
  const_store<int> node_offset;
  const_store<int> cell_offset;
  const_store<std::string> boundary_names;
  const_Map ref;
 
  store<Loci::FineFaces> fine_faces;

  const_store<int> node_l2f;
public:
  get_boundary_hex_faces(){
    name_store("balancedFacePlan", facePlan);
    name_store("balancedEdgePlan", edgePlan);
    name_store("balancedCellPlan", cellPlan);
    name_store("lower", lower);
    name_store("upper", upper);
    name_store("boundary_map", boundary_map);
    name_store("hexOrientCode", orientCode);
    name_store("hex2face", hex2face);
    name_store("hex2node", hex2node);
    name_store("fl", fl);
    name_store("face2node", face2node);
    name_store("face2edge", face2edge);
    name_store("edge2node", edge2node);
    name_store("pos", pos);
    name_store("cl", cl);
    name_store("balanced_node_offset", node_offset);
    name_store("balanced_cell_offset", cell_offset);
    name_store("boundary_tags", boundary_names);
    name_store("ref", ref);
   
    name_store("fine_faces", fine_faces);
    name_store("fileNumber(pos)", node_l2f);
    input("balancedFacePlan, balanced_node_offset");
    input("face2edge->(balancedEdgePlan, balanced_node_offset)");
    input("cl->(balancedCellPlan, balanced_cell_offset, hex2face, hex2node, hexOrientCode)");
    input("fl");
    input("cl->(upper, lower, boundary_map)->face2node->pos");
    input("cl->(upper, lower, boundary_map)-> face2edge->edge2node->pos");
    input("face2node->(pos, fileNumber(pos))");
    input("ref->boundary_tags");
    
    output("fine_faces");
    constraint("cl->hexcells, boundary_faces");
  }
  virtual void compute(const sequence &seq){
    if(seq.size()!=0){
     
      do_loop(seq, this);
    }
  }
  void calculate(Entity f){
   
   
    std::list<Node*> bnode_list, node_list;
    std::list<Edge*> edge_list;
    std::list<int32> faceVertex;
    int c1;

    int c2 = -atoi(&(boundary_names[ref[f]][3]));
    //build a face

    QuadFace* aFace = build_quad_face(face2node[f].begin(),
                                      face2edge[f].begin(),
                                      edge2node,
                                      pos,
                                      edgePlan,
                                      node_offset,
                                      node_l2f,
                                      bnode_list,
                                      edge_list);
      
 
    
    if(facePlan[f].size() == 0){
         
      //num of fine faces is 1. 
      vector< std::vector<int> >(1).swap(fine_faces[f]);
      
      aFace->set_f2n(faceVertex);
      int vec_size = faceVertex.size()+2;
      std::vector<int>(vec_size).swap(fine_faces[f][0]);
      
      //get c1

      //get CL    
      std::vector<int32> CL = get_c1_hex(cellPlan[cl[f]],
                                         facePlan[f],
                                         orientCode[cl[f]][fl[f]],
                                         fl[f]);
  
    
      if( CL.size()!= 1){
        cerr<<"WARNING: CL has the incorrect size" << endl;
        Loci::Abort();
      }
      
      c1 = cell_offset[cl[f]] + CL[0];
      
      fine_faces[f][0][0] = c1;
      fine_faces[f][0][1] = c2;
      
      std::list<int32>::const_iterator nptr = faceVertex.begin();
      for(int i = 2; i < vec_size; i++,nptr++){
        fine_faces[f][0][i] = *nptr;
      }
      

       //clean up
      if(aFace != 0){
        delete aFace;
        aFace = 0;
      }
      cleanup_list(bnode_list, edge_list);  
      return;
    }

    //split the face
    std::vector<QuadFace*> leaves;
    aFace->resplit(facePlan[f],
                     char(0),
                     node_list,
                     edge_list,
                    leaves);
    //and index the node_list
   
    int nindex = 0;
    for(std::list<Node*>::const_iterator np = node_list.begin(); np!= node_list.end(); np++,nindex++){
   
      (*np)->index = node_offset[f] + nindex;
    }
   
        
   
    //get CL    
    std::vector<int32> CL = get_c1_hex(cellPlan[cl[f]],
                                           facePlan[f],
                                           orientCode[cl[f]][fl[f]],
                                           fl[f]);
  
    
    if(leaves.size() != CL.size()){
      cerr<<"WARNING: CL has the incorrect size" << endl;
      Loci::Abort();
    }
    
    
    vector<vector<int> >(leaves.size()).swap(fine_faces[f]);                    
    //set face_vertices
    for( unsigned int  fptr = 0; fptr < leaves.size(); fptr++){
      faceVertex.clear();
      leaves[fptr]->set_f2n(faceVertex);
      
      //set c1
      if(cellPlan[cl[f]].size() == 0){
        c1= cell_offset[cl[f]] + 1;
      }
      else{
        c1= cell_offset[cl[f]] + CL[fptr];              
      }
       
    
      int vec_size = faceVertex.size() +2;
      vector<int>(vec_size).swap(fine_faces[f][fptr]);
      fine_faces[f][fptr][0] = c1;
      fine_faces[f][fptr][1] = c2;

      std::list<int32>::const_iterator nptr = faceVertex.begin();
      for(int i = 2; i < vec_size; i++, nptr++){
        fine_faces[f][fptr][i] = *nptr;
      }
    }
    
  
    //clean up
    if(aFace != 0){
      delete aFace;
      aFace = 0;
    }
    cleanup_list(node_list);
    cleanup_list(bnode_list, edge_list);
  }
};
register_rule<get_boundary_hex_faces> register_get_boundary_hex_faces;



