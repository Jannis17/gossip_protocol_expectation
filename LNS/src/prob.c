//~ #include "mainHeader.h"
//~ #include "state.h"
//~ #include "../../nauty26r7/nauty.h"
//~ #include "prob.h"


//~ float calcExpectation (float trMat[MAXSTATES][MAXSTATES+1], int states)
//~ {
	//~ int i, j, k;
	
	//~ float expectationVec[MAXSTATES];
	
	//~ float c, sum;
			
	//~ for (i=0; i<states; i++) {
		//~ expectationVec[i] = 0;
		//~ for (j=0; j<states; j++)
			//~ trMat[i][j] = (i==j) - trMat[i][j];
	//~ }
			
	//~ /* loop for the generation of upper triangular matrix*/
	//~ for(j=0; j<states; j++) 
       //~ for(i=j+1; i<states; i++) 
       //~ {
		   //~ c=trMat[i][j]/trMat[j][j];
           //~ for(k=i; k<states+1; k++)
              //~ trMat[i][k]=trMat[i][k]-c*trMat[j][k];                
        //~ };
          
    //~ expectationVec[states-1]=trMat[states-1][states]/trMat[states-1][states-1];
				
    //~ /* this loop is for backward substitution*/
    //~ for(i=states-2; i>=0; i--)
    //~ {
        //~ sum=(float) 0;
                        
        //~ for(j=i+1; j<states; j++)
            //~ sum = sum + trMat[i][j]*expectationVec[j];
                        
        //~ expectationVec[i]=(trMat[i][states]-sum)/trMat[i][i];             
    //~ }
                          
    //~ return expectationVec[0];
//~ }
