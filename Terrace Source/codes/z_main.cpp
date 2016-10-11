//#include "pom.h"
#include "z_video_converter.h"
//#include "z_EPFLdata4p_setup.h"
//#include "z_EPFLdataterrace1_setup.h"
//#include "background subtraction\t_BSmain.h"
#include "z_terrace_tracking.h"
#include "z_Terrace.h"

int main()    
{
	
	//imageFromVideo("Lab data\\original1.avi","C:\\data\\lab_original_1\\","test",7,27,10000,1);
	//videoFromImage("Lab data\\test1.avi","C:\\data\\lab_original_1\\","test",7,0,800,25);
	//videoFromImage("D:\\test\\test.avi","D:\\test\\","test",3,0,300,25);
	
	//get image from EPFL4p data
	//imageFromVideo("EPFL data\\4p-c0.avi","D:\\EPFL4p\\","img",0,0,10000,0);
	//imageFromVideo("EPFL data\\4p-c1.avi","D:\\EPFL4p\\","img",1,0,10000,0);
	//imageFromVideo("EPFL data\\4p-c2.avi","D:\\EPFL4p\\","img",2,0,10000,0);
	//imageFromVideo("EPFL data\\4p-c3.avi","D:\\EPFL4p\\","img",3,0,10000,0);

	//get image from EPFLmatch data
	//imageFromVideo("EPFL data\\terrace1-c0.avi","D:\\EPFLterrace\\","img",0,0,10000,0);
	//imageFromVideo("EPFL data\\terrace1-c1.avi","D:\\EPFLterrace\\","img",1,0,10000,0);
	//imageFromVideo("EPFL data\\terrace1-c2.avi","D:\\EPFLterrace\\","img",2,0,10000,0);
	//imageFromVideo("EPFL data\\terrace1-c3.avi","D:\\EPFLterrace\\","img",3,0,10000,0);
	
	
	//setupEPFLa();
	//test();
	//makeBS("C:\\data\\EPFLterrace", "C:\\data\\EPFLterraceBS3", "img", 3, 0, 5009);
	//makeBS("C:\\data\\EPFLterrace", "C:\\data\\EPFLterraceBS3", "img", 2, 0, 5009);
	//makeBS("C:\\data\\EPFLterrace", "C:\\data\\EPFLterraceBS3", "img", 1, 0, 5009);
	
	//("C:\\data\\EPFLterrace", "C:\\data\\EPFLterraceBS3", "img", 0, 0, 5009);
	//process();

	//z_terrace_tracking* terrace;
	//terrace = new z_terrace_tracking();
	//terrace->process2();
	Terrace* terrace = new Terrace(10, 10000);
	//terrace->process(1);
	terrace->showLogFile();
	return 0;

}