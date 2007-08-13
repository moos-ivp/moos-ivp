/************************************************************/
/*    NAME: Andrew Shafer                                   */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: TestRig.cpp                                     */
/*    DATE: 25 JUL 2007                                     */
/************************************************************/

#include "ArtifactField.h"
#include "ArtifactFieldModel.h"
#include <iostream>

int main(int argc, char *argv[])
{
	int testervalue = 2;
	switch(testervalue){
		case 0:
			{
			// ArtifactField Tester	
			ArtifactField artfArtField;
				
			artfArtField.addArtifact("X=1,Y=2.0,TYPE=MINE");
			artfArtField.addArtifact(2.0, 1.5);
			
			for(int i=0; i < artfArtField.size(); i++){
				std::cout << artfArtField.getArtifact(i) << std::endl;
			}
			}
			break;
	
		case 1:
			{
			// ArtifactFieldModel Tester
			ArtifactFieldModel afmModel(16);
			
			std::cout << afmModel.getCell(0) << std::endl;
			std::cout << afmModel.getCell(15) << std::endl;
			// Next should crash
			// std::cout << afmModel.getCell(16) << std::endl;
			
			afmModel.updateCell(7, .75);
			std::cout << afmModel.getCell(7) << std::endl;
			}
			break;
			
		case 2:
			{
			// SensorModel Tester
			
			break;
			}
			
	}
	
	return 0;
}
