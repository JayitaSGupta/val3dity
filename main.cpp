/*
Copyright (c) 2011, Hugo Ledoux
All rights reserved.
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgment:
   This product includes software developed by the <organization>.
4. Neither the name of the <organization> nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "input.h"
#include "validate.h"

// -----------------------------------------------------------
// Usage documentation for this method goes here.
//
int main(int argc, char* const argv[])
{

  if (argc < 2)
  {
    cout << "You have to give at least one input POLY file." << endl;
    cout << endl << "Usage: <program> ./src/data/cube.poly ./src/data/py1.poly" << endl;
    cout << "This example would validate a solid with the outer polyhedra from cube.poly and one inner void from py1.poly" << endl;
    return(0);
  }

  vector<polyhedraShell*> polyhedraShells;
  readAllPolyhedraShells((argc-1), argv, polyhedraShells);

  bool isValid(true);
  validatePolyHedra(polyhedraShells, isValid);
  if (isValid == false)
  {
     cout << "polyhedra are not valid." << endl;
     exit(1);
  }


  return(0);
}

