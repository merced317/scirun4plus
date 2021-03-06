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
//  Vgrid ASCII file format (.gmv) 
//      converting to 
//  SCIRun compatible node(.pts) & mesh(.fac, .tet, .hex) format in ascii
//
//  Date:  2004. 9. 23 
//
//  Written by
//
//  Seok Lew
//  SCI Institute, Univ. of Utah, Salt Lake City, UT, USA
//***********************************************************************

#include <sci_deprecated.h>
 
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using std::cout;
using std::endl;
using std::cerr;
using std::string;
using std::ifstream;
using std::ofstream;

int
main(int argc, char* argv[])
{

  if(argc !=4) {
    cout <<endl<< "gmvTopts <element type> INPUT OUTPUT " << endl << endl;
    cout << "   INPUT  : input filename having .gmv extension" << endl;
    cout << "   OUTPUT : output filename of SCIRun readable ascii node format(.pts)" << endl;
    cout << "            and mesh format (.fac .tet .hex) depending on mesh type" << endl;
    cout << "           (filename extensions(.pts .fac .tet .hex) are automatically attached to OUTPUT)" << endl;
    cout << " <element type> " << endl;
    cout << "   -s     :  triangular surface element" << endl;
    cout << "   -t     :  tetrahedra volume element" << endl;
    cout << "   -h     :  hexagonal volume element" << endl;
    exit(1);
  }

  ifstream gmvFile(argv[2], std::ios::in);
  
  if (!gmvFile){
    cerr << "File could not be opened" << endl;
    exit(1);
  }

  string tmp;

  // skipping text lines
  for (int i=0; i<4; ++i){
    getline(gmvFile,tmp);
    //  cout << tmp << endl << endl;
  }

  //char c;
  long  node, mesh,elem;

  const char *fname=0;
  string ext=".pts";
  string CompleteName=argv[3];
  CompleteName= argv[3]+ext;
  fname=CompleteName.c_str();

  // read in node from .gmv
  gmvFile >> tmp >>  node;
  cout << "Number of Node = " << node << endl;

  std::vector< double > vx,vy,vz;
  double xx, yy, zz;

  for(long i=0; i<node; ++i){
    gmvFile >> xx;
    vx.push_back(xx);
  }

  for(long i=0; i<node; ++i){
    gmvFile >> yy;
    vy.push_back(yy);
  }

  for(long i=0; i<node; ++i){
    gmvFile >> zz;
    vz.push_back(zz);
  }               

  // write node to .pts
  ofstream ptsFile(fname,std::ios::out);
  ptsFile << node << endl;

  for(long i=0; i<node; ++i){
    ptsFile << vx[i] << " " << vy[i] << " " << vz[i] << endl;
  }

  ptsFile.close();

  // read & write mesh
  gmvFile >> tmp >> mesh;
  cout << "Number of Mesh = " << mesh << endl;
        
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
  else {
    cout << "ERROR: Element type not specified!  Use -s, -h, or -t.\n\n";
    exit(1);
  }


  CompleteName=argv[3];
  CompleteName= argv[3]+ext;
  fname=CompleteName.c_str();

  // write mesh
  ofstream tetFile(fname,std::ios::out);
  tetFile << mesh << endl;
  cout << "reading connectivity:   " << std::flush;
  int m;

  for (long i=0; i<mesh; ++i){
    if(!((i*20)%mesh))  cout << "+" << std::flush;

    gmvFile >> tmp >> tmp;
    for (long j=0; j<elem; ++j){
      gmvFile >> m;
      tetFile << m-1 << " ";   // mesh numbering start from 0, not 1
    }
    tetFile << endl;
  }

  cout << endl;
        
  gmvFile.close();
  tetFile.close();

  return 0;
}
