/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2009 Scientific Computing and Imaging Institute,
   University of Utah.

   
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/


//***********************************************************************
//  Cauchy geometry format (.geo) 
//      converting to 
//  SCIRun compatible node(.pts) & mesh(.tet) format
//***********************************************************************

#include <sci_deprecated.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using std::cout;
using std::endl;
using std::cerr;
using std::string;


int
main(int argc, char* argv[])
{

  if(argc !=4) {
    cout << "geoTopts <element type> INPUT OUTPUT " << endl;
    cout << "   INPUT  : input filename having .geo extension" << endl;
    cout << "   OUTPUT : output filename of SCIRun readable node format(.pts)" << endl;
    cout << "            and mesh format (.fac .tet .hex) depending on mesh type" << endl;
    cout << "           (filename extensions(.pts & .fac .tet .hex) are automatically attached to OUTPUT)" << endl;
    cout << "   -s     :  triangular surface element" << endl;
    cout << "   -t     :  tetrahedra volume element" << endl;
    cout << "   -h     :  hexagonal volume element" << endl;    
    exit(1);
  }

  std::ifstream geoFile(argv[2], std::ios::in);
  
  if (!geoFile){
    cerr << "File could not be opened" << endl;
    exit(1);
  }

  string tmp;

  // skipping text lines
  for (int i=0; i<4; ++i){
    getline(geoFile,tmp);
  }

  char c;
  int node, mesh,elem=0;
  double x,y,z;

  geoFile >> tmp >> tmp >> tmp>> c >> node;
  cout << "Number of Node = " << node << endl;

  geoFile >> tmp >> tmp >> tmp >> c >> mesh;
  cout << "Number of Mesh = " << mesh << endl;

  // skipping text lines
  for (int i=0; i<6; ++i){
    getline(geoFile, tmp);
    //  cout << tmp << endl << endl;
  }

  const char *fname=0;
  string ext=".pts";
  string CompleteName=argv[3];
  CompleteName= argv[3]+ext;
  fname=CompleteName.c_str();

  std::ofstream ptsFile(fname,std::ios::out);
        
  // writing number of node
  ptsFile << node << endl;

  //writing node location
  for (int i=0; i<node; ++i){
    geoFile >> x >> y >> z;
    ptsFile << x << " " << y << " " << z << endl;
  }

  ptsFile.close();
        
  // skipping text lines
  for (int i=0; i<5; ++i){
    getline(geoFile, tmp);
    // cout << tmp << endl << endl;
  }
        
  if (strcmp(argv[1],"-t")==0){
    ext=".tet";
    elem=4;
    cout << " !! Tetrahedra element !! " << endl;
  }
  else if (strcmp(argv[1],"-s")==0){
    ext=".fac";
    elem=3;
    cout << " !! Triangular element !! " << endl;
  }
  else if (strcmp(argv[1],"-h")==0){
    ext=".hex";
    elem=8;
    cout << " !! Hexagon element !! " << endl;
  }


  CompleteName=argv[3];
  CompleteName= argv[3]+ext;
  fname=CompleteName.c_str();

  std::ofstream tetFile(fname,std::ios::out);
  tetFile << mesh << endl;
  cout << "reading connectivity:   " << std::flush;

  for (int i=0; i<mesh; ++i){
    if(!((i*20)%mesh))  cout << "+" << std::flush;

    getline(geoFile, tmp);

    int l=0, p;
    for (int j=0; j<elem; ++j){
      p=0;
      for(int k=0; k<6; ++k,++l){
        c=tmp[7+l];  // after skipping 7 characters in the beginning of each line
        if(c !=' ' ){
          p *=10;
          p +=atoi(&c);
        }
      }
      tetFile << p-1 << " ";  // node number begins from 0.
    }
    tetFile << endl;
  }

  cout << endl;
        
  geoFile.close();
  tetFile.close();
        
  return 0;
        
}
