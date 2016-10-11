
//////////////////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify         //
// it under the terms of the version 3 of the GNU General Public License        //
// as published by the Free Software Foundation.                                //
//                                                                              //
// This program is distributed in the hope that it will be useful, but          //
// WITHOUT ANY WARRANTY; without even the implied warranty of                   //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             //
// General Public License for more details.                                     //
//                                                                              //
// You should have received a copy of the GNU General Public License            //
// along with this program. If not, see <http://www.gnu.org/licenses/>.         //
//                                                                              //
// Written by Francois Fleuret                                                  //
// (C) Ecole Polytechnique Federale de Lausanne                                 //
// Contact <pom@epfl.ch> for comments & bug reports                             //
//////////////////////////////////////////////////////////////////////////////////

#include "pom.h"


double proj[8][3][4]={
	{{ 50.994564, 413.435549, 0.000000, -30329.344729},		{ -11.019785, -17.826192, 169.843469, 465747.318356},	{ 0.457786, 0.740538, 0.000000, -82.130991}	},
	{{ -69.377942, 371.192241, 0.000000, 25388.386686},		{ -2.813986, -26.003427, 111.580079, 307778.817816},	{ 0.093781, 0.866611, 0.000000, -349.761926}},
	{{ -391.511383, 101.928873, 0.000000, 1209498.699178},	{ 19.691287, -15.259245, 157.419207, 366083.237323},	{ -0.689063, 0.533972, 0.000000, 2075.253452}},
	{{ -365.038026, -173.981403, 0.000000, 1573692.871074},	{ 47.877805, -0.404965, 173.180205, 346545.418644},		{ -0.901970, 0.007629, 0.000000, 2790.869736}},
	{{ -119.883321, -448.478451, 0.000000, 2531293.623196},	{ 1.119243, 1.287758, 185.862384, 496098.480391},		{ -0.570097, -0.655932, 0.000000, 4938.289060}},
	{{ 171.384646, -341.254962, 0.000000, 1565241.736941},	{ 0.994393, 13.712393, 196.809905, 467322.386265},		{ -0.062290, -0.858959, 0.000000, 4658.711951}},
	{{ 286.690028, -75.069573, 0.000000, 298630.629360},	{ 14.877665, -25.985516, 172.199611, 578531.640671},	{ 0.418769, -0.731428, 0.000000, 3260.737750}},
	{{ 360.837745, 155.938710, 0.000000, -358377.422080},	{ -41.409351, -0.402728, 153.748733, 423620.688892},	{ 0.894331, 0.008698, 0.000000, 35.012792}}
};


void W2I(double x, double y, double z, double&u, double&v, int icam)
{
	double	x1 = x* proj[icam][0][0] + y * proj[icam][0][1] + z* proj[icam][0][2] + proj[icam][0][3]; 
	double	y1 = x* proj[icam][1][0] + y * proj[icam][1][1] + z* proj[icam][1][2] + proj[icam][1][3]; 
	double	z1 = x* proj[icam][2][0] + y * proj[icam][2][1] + z* proj[icam][2][2] + proj[icam][2][3];
	u=x1/z1;
	v=y1/z1;
}


void check_parameter(char *s, int line_number, char *buffer) {
  if(!s) {
    cerr << "Missing parameter line " << line_number << ":" << endl;
    cerr << buffer << endl;
    exit(1);
  }
}

void setRectangle(Rectangle *r, int X1, int Y1, int X2, int Y2, int X3, int Y3, int X4, int Y4)
{
	int tempX_min = 0;
	int tempX_max = 800;
	int tempY_min = 0;
	int tempY_max = 600;
	if (X1 >= X2) 
	{
		tempX_min = X2;
		tempX_max = X1;
	}
	else
	{
		tempX_min = X1;
		tempX_max = X2;
	}
	if (tempX_min > X3) tempX_min = X3;
	if (tempX_max < X3) tempX_max = X3;
	if (tempX_min > X4) tempX_min = X4;
	if (tempX_max < X4) tempX_max = X4;

	if (Y1 >= Y2) 
	{
		tempY_min = Y2;
		tempY_max = Y1;
	}
	else
	{
		tempY_min = Y1;
		tempY_max = Y2;
	}
	if (tempY_min > Y3) tempY_min = Y3;
	if (tempY_max < Y3) tempY_max = Y3;
	if (tempY_min > Y4) tempY_min = Y4;
	if (tempY_max < Y4) tempY_max = Y4;
	
	
	if (tempX_min < 0) tempX_min = 0;
	if (tempY_min < 0) tempY_min = 0;
	if (tempX_max >= 800) tempX_max = 799;
	if (tempY_max >= 600) tempY_max = 599;

	r->visible = true;
	r->xmax = tempX_max;
	r->xmin = tempX_min;
	r->ymin = tempY_min;
	r->ymax = tempY_max;
}




void process()
{


  int numberPosition = 0;
  ifstream *configuration_file = 0;
  istream *input_stream;
  configuration_file = new ifstream("EPFLtest.pom");
  input_stream = configuration_file;
  char input_view_format[buffer_size] = "";
  char result_format[buffer_size] = "";
  char result_view_format[buffer_size] = "";
  char convergence_view_format[buffer_size] = "";

  char buffer[buffer_size], token[buffer_size];

  int line_number = 0;
  Vectors<ProbaView *> *proba_views = 0;

  Room *room = 0;

  while(!input_stream->eof()) {

    input_stream->getline(buffer, buffer_size);
    line_number++;

    char *s = buffer;
    s = next_word(token, s, buffer_size);

    if(strcmp(token, "ROOM") == 0) {
      int view_width = -1, view_height = -1;
      int nb_positions = -1;
      int nb_cameras = -1;

      check_parameter(s, line_number, buffer);
      s = next_word(token, s, buffer_size);
      view_width = atoi(token);

      check_parameter(s, line_number, buffer);
      s = next_word(token, s, buffer_size);
      view_height = atoi(token);

      check_parameter(s, line_number, buffer);
      s = next_word(token, s, buffer_size);
      nb_cameras = atoi(token);

      check_parameter(s, line_number, buffer);
      s = next_word(token, s, buffer_size);
      nb_positions = atoi(token);
	  numberPosition = nb_positions;

      if(room) {
        cerr << "Room already defined, line" << line_number << "." << endl;
        exit(1);
      }

      room = new Room(view_width, view_height, nb_cameras, nb_positions);
      proba_views = new Vectors<ProbaView *>(nb_cameras);
      for(int c = 0; c < proba_views->length(); c++)
        (*proba_views)[c] = new ProbaView(view_width, view_height);
    }

    else if(strcmp(token, "CONVERGENCE_VIEW_FORMAT") == 0) {
      check_parameter(s, line_number, buffer);
      s = next_word(convergence_view_format, s, buffer_size);
    }

    else if(strcmp(token, "INPUT_VIEW_FORMAT") == 0) {
      check_parameter(s, line_number, buffer);
      s = next_word(input_view_format, s, buffer_size);
    }

    else if(strcmp(token, "RESULT_VIEW_FORMAT") == 0) {
      check_parameter(s, line_number, buffer);
      s = next_word(result_view_format, s, buffer_size);
    }

    else if(strcmp(token, "RESULT_FORMAT") == 0) {
      check_parameter(s, line_number, buffer);
      s = next_word(result_format, s, buffer_size);
    }

    else if(strcmp(token, "PROCESS") == 0) {
      RGBImage tmp;
      int first_frame, nb_frames;

      check_parameter(s, line_number, buffer);
      s = next_word(token, s, buffer_size);
      first_frame = atoi(token);

      check_parameter(s, line_number, buffer);
      s = next_word(token, s, buffer_size);
      nb_frames = atoi(token);

      POMSolver solver(room);

      Vectors<scalar_t> prior(room->nb_positions());
      Vectors<scalar_t> proba_presence(room->nb_positions());
      for(int i = 0; i < room->nb_positions(); i++) prior[i] = global_prior;  

      if(strcmp(input_view_format, "") == 0) {
        cerr << "You must specify the input view format." << endl;
        exit(1);
      }


	  string preName[4];
	  string preOutName[4];
	  for (int iView = 0; iView < 4; iView++)
	  {
		  preName[iView] = createPreName("D:\\EPFLterraceBS", "img", iView);
		  preOutName[iView] = createPreName("D:\\EPFLterraceBSresult", "img", iView);
	  }

	  string fileName;
	  string outFileName;


      for(int f = first_frame; f < first_frame + nb_frames; f++) {

        if(configuration_file)
          cout << "Processing frame " << f << endl;
        char* b = new char[1000];
		char* b1 = new char[1000];
		char* b2 = new char[1000];
        for(int c = 0; c < room->nb_cameras(); c++) {  //so camera
          
          fileName = createFileName(preName[c], f);
		  strcpy(b, fileName.c_str());
		  tmp.read_png(b);
          (*proba_views)[c]->from_image(&tmp);
		  
        }

        if(strcmp(convergence_view_format, "") != 0)
          solver.solve(room, &prior, proba_views, &proba_presence, f, convergence_view_format);
        else
          solver.solve(room, &prior, proba_views, &proba_presence, f, 0);
		cout<<"\n";
		//for (int i = 0; i < numberPosition; i++)
		//{
			//if (proba_presence[i] > 0.9)
			//cout<<i<<"         "<<proba_presence[i]<<"\n";
		//}
		//cout<<"\n";
        if(strcmp(result_view_format, "") != 0)
          for(int c = 0; c < room->nb_cameras(); c++) {
            if(configuration_file)
              cout << "Saving " << buffer << endl;
		  char* temp1 = new char[100];
		  for(int c = 0; c < room->nb_cameras(); c++) 
		  {
			  outFileName = createFileName(preOutName[c], f);
			 strcpy(b1, outFileName.c_str());
            //if (f >= 20) 
			 room->save_stochastic_view(b1, c, (*proba_views)[c], &proba_presence);
          }
		  }
		  if(strcmp(result_format, "") != 0) {
          //pomsprintf(buffer, buffer_size, result_format, 0, f, 0);
            char* temp2 = new char[100];
			string a2 = "D:\\EPFLtmpprob\\";
			a2 += "proba-f";
			itoa(f,temp2,10);
			a2 += temp2;
			a2 += ".txt";
			strcpy(b2, a2.c_str());
          ofstream result(b2);
          if(result.fail()) {
            cerr << "Can not open " << token << " for writing." << endl;
            exit(1);
          }
          if(configuration_file)
            cout << "Saving " << buffer << endl;
          for(int i = 0; i < room->nb_positions(); i++)
            result << i << " " << proba_presence[i] << endl;
          result.flush();
        }

        
      }
    }  

    else if(strcmp(token, "RECTANGLE") == 0) {
      int n_camera, n_position;

      if(!room) {
        cerr << "You must define a room before adding rectangles, line" << line_number << "." << endl;
        exit(1);
      }

      check_parameter(s, line_number, buffer);
      s = next_word(token, s, buffer_size);
      n_camera = atoi(token);

      if(n_camera < 0 || n_camera >= room->nb_cameras()) {
        cerr << "Out of range camera number line " << line_number << "." << endl;
        exit(1);
      }

      check_parameter(s, line_number, buffer);
      s = next_word(token, s, buffer_size);
      n_position = atoi(token);

      if(n_position < 0 || n_camera >= room->nb_positions()) {
        cerr << "Out of range position number line " << line_number << "." << endl;
        exit(1);
      }

      Rectangle *current = room->avatar(n_camera, n_position);

      check_parameter(s, line_number, buffer);
      s = next_word(token, s, buffer_size);
      if(strcmp(token, "notvisible") == 0) {
        current->visible = false;
        current->xmin = -1;
        current->ymin = -1;
        current->xmax = -1;
        current->ymax = -1;
      } else {
        current->visible = true;
        current->xmin = atoi(token);
        check_parameter(s, line_number, buffer);
        s = next_word(token, s, buffer_size);
        current->ymin = atoi(token);
        check_parameter(s, line_number, buffer);
        s = next_word(token, s, buffer_size);
        current->xmax = atoi(token);
        check_parameter(s, line_number, buffer);
        s = next_word(token, s, buffer_size);
        current->ymax = atoi(token);

        if(current->xmin < 0 || current->xmax >= room->view_width() ||
           current->ymin < 0 || current->ymax >= room->view_height()) {
          cerr << "Rectangle out of bounds, line " << line_number << endl;
          exit(1);
        }
      }
    }

    else if(strcmp(token, "PRIOR") == 0) {
      check_parameter(s, line_number, buffer);
      s = next_word(token, s, buffer_size);
      global_prior = atof(token);
    }

    else if(strcmp(token, "SIGMA_IMAGE_DENSITY") == 0) {
      check_parameter(s, line_number, buffer);
      s = next_word(token, s, buffer_size);
      global_sigma_image_density = atof(token);
    }

    else if(strcmp(token, "SMOOTHING_COEFFICIENT") == 0) {
      check_parameter(s, line_number, buffer);
      s = next_word(token, s, buffer_size);
      global_smoothing_coefficient = atof(token);
    }

    else if(strcmp(token, "MAX_NB_SOLVER_ITERATIONS") == 0) {
      check_parameter(s, line_number, buffer);
      s = next_word(token, s, buffer_size);
      global_max_nb_solver_iterations = atoi(token);
    }

    else if(strcmp(token, "ERROR_MAX") == 0) {
      check_parameter(s, line_number, buffer);
      s = next_word(token, s, buffer_size);
      global_error_max = atof(token);
    }

    else if(strcmp(token, "NB_STABLE_ERROR_FOR_CONVERGENCE") == 0) {
      check_parameter(s, line_number, buffer);
      s = next_word(token, s, buffer_size);
      global_nb_stable_error_for_convergence = atoi(token);
    }

    else if(strcmp(token, "PROBA_IGNORED") == 0) {
      check_parameter(s, line_number, buffer);
      s = next_word(token, s, buffer_size);
      global_proba_ignored = atof(token);
      //cout << "global_proba_ignored = " << global_proba_ignored << endl;
    }

    else if(strcmp(buffer, "") == 0 || buffer[0] == '#') { }

    else {
      cerr << "Unknown token " << token << ".";
      exit(1);
    }
  }

  if(proba_views)
    for(int c = 0; c < proba_views->length(); c++) delete (*proba_views)[c];

  delete proba_views;
  delete room;
  delete configuration_file;

}



void setPosition(IplImage* view2, int dotHeight, int dotWidth, int* view2_dotx, int* view2_doty, int down)
{
	int r,g,b;
	r=g=b=0;
	if (down == 0) 
	{
		g = 255;
		b = 255;
	}
	else
	{
		r = 255;
		b = 255;
	}
	for (int i = 0; i < dotHeight; i++)
			for (int j = 0; j <dotWidth; j++)
			{
				int X = view2_dotx[i*dotWidth + j];
				int Y = view2_doty[i*dotWidth + j];
				if (-5 <= X && X < 795 && -5 <= Y && Y < 595) 
				{
					for (int k = -5; k <= 5; k++)
						for (int h = -5; h <= 5; h++)
						{
							view2->imageData[(Y+k)*view2->widthStep + (X + h)*3] = r;
							view2->imageData[(Y+k)*view2->widthStep + (X + h)*3 + 1] = g;
							view2->imageData[(Y+k)*view2->widthStep + (X + h)*3 + 2] = b;
						}
				}
			}
}

void setRectangle(IplImage* view2, int*view2_dotx, int*view2_doty, int dotWidth, int posy, int posx,  int value)
{
	int r,g,b;
	r=g=b=0;
	if (value == 0) 
	{
		g = 255; 
		b = 255;
	}
	else if (value == 1) 
	{
		b = 255;
	}
	else
	{
		r = 255;
		b = 255;
	}
	int X = view2_dotx[posy*dotWidth + posx ];
		int Y = view2_doty[posy*dotWidth + posx ];
				if (-5 <= X && X < 795 && -5 <= Y && Y < 595) 
				{
					for (int k = -5; k <= 5; k++)
						for (int h = -5; h <= 5; h++)
						{
							view2->imageData[(Y+k)*view2->widthStep + (X + h)*3] = r;
							view2->imageData[(Y+k)*view2->widthStep + (X + h)*3 + 1] = g;
							view2->imageData[(Y+k)*view2->widthStep + (X + h)*3 + 2] = b;
						}
				}
}

void setup() {




	
	CvCapture* capture = cvCaptureFromFile("dense-mgd-ut.avi");
	CvCapture* capture2 = cvCaptureFromFile("dense-ut.avi");
	cvNamedWindow("test", 1);
	cvNamedWindow("testview1", 1);
	cvNamedWindow("testview2", 1);
	cvNamedWindow("testview3", 1);
	IplImage* img;
	IplImage* img2;

	int frameRate = (int)(cvGetCaptureProperty(capture, CV_CAP_PROP_FPS));
	int frameWidth = (int)(cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH));
	int frameHeight = (int)(cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT));
	int beginTime = 20; //28
	cout<<"frame rate"<<"      "<<frameRate<<"\n";
	cout<<"frame width"<<"      "<<frameWidth<<"\n";
	cout<<"frame height"<<"      "<<frameHeight<<"\n";
	int countFrame = 0;

	int imageWidth = frameWidth;
	int imageHeight = frameHeight/8;
	int dotSize = 20;
	int dotWidth = 450/25 + 2;  //20
	int dotHeight = 300/25 + 2; //14
	
	IplImage* view1 = cvCreateImage(cvSize(imageWidth,imageHeight),8,3);
	int* view1_dotx = new int[dotHeight*dotWidth];
	int* view1_doty = new int[dotHeight*dotWidth];
	for (int i = 0; i < dotHeight; i++)
		for (int j = 0; j <dotWidth; j++)
		{
			double tempX, tempY;
			W2I(i*250, j*250, 0, tempX, tempY, 0);
			view1_dotx[i*dotWidth + j] = tempX;
			view1_doty[i*dotWidth + j] = tempY;
		}
    int* view1_dotxz = new int[dotHeight*dotWidth];
	int* view1_dotyz = new int[dotHeight*dotWidth];
	for (int i = 0; i < dotHeight; i++)
		for (int j = 0; j <dotWidth; j++)
		{
			double tempX, tempY;
			if (i <= 5 && j <= 5) W2I(i*250, j*250, -1900, tempX, tempY, 0);
			else W2I(i*250, j*250, -1800, tempX, tempY, 0);
			view1_dotxz[i*dotWidth + j] = tempX;
			view1_dotyz[i*dotWidth + j] = tempY;
		}  



	IplImage* view2 = cvCreateImage(cvSize(imageWidth,imageHeight),8,3);
	int* view2_dotx = new int[dotHeight*dotWidth];
	int* view2_doty = new int[dotHeight*dotWidth];
	for (int i = 0; i < dotHeight; i++)
		for (int j = 0; j <dotWidth; j++)
		{
			double tempX, tempY;
			W2I(i*250, j*250, 0, tempX, tempY, 1);
			view2_dotx[i*dotWidth + j] = tempX;
			view2_doty[i*dotWidth + j] = tempY;
		}
    int* view2_dotxz = new int[dotHeight*dotWidth];
	int* view2_dotyz = new int[dotHeight*dotWidth];
	for (int i = 0; i < dotHeight; i++)
		for (int j = 0; j <dotWidth; j++)
		{
			double tempX, tempY;
			if (j <= 4) W2I(i*250, j*250, -1900, tempX, tempY, 1);
			else W2I(i*250, j*250, -1800, tempX, tempY, 1);
			view2_dotxz[i*dotWidth + j] = tempX;
			view2_dotyz[i*dotWidth + j] = tempY;
		}  


	IplImage* view3 = cvCreateImage(cvSize(imageWidth,imageHeight),8,3);
	int* view3_dotx = new int[dotHeight*dotWidth];
	int* view3_doty = new int[dotHeight*dotWidth];
	for (int i = 0; i < dotHeight; i++)
		for (int j = 0; j <dotWidth; j++)
		{
			double tempX, tempY;
			W2I(i*250, j*250, 0, tempX, tempY, 2);
			view3_dotx[i*dotWidth + j] = tempX;
			view3_doty[i*dotWidth + j] = tempY;
		}
    int* view3_dotxz = new int[dotHeight*dotWidth];
	int* view3_dotyz = new int[dotHeight*dotWidth];
	for (int i = 0; i < dotHeight; i++)
		for (int j = 0; j <dotWidth; j++)
		{
			double tempX, tempY;
			if (j < 5 && i >= 8 && i <= 12) W2I(i*250, j*250, -1900, tempX, tempY, 2);
			else W2I(i*250, j*250, -1800, tempX, tempY, 2);
			view3_dotxz[i*dotWidth + j] = tempX;
			view3_dotyz[i*dotWidth + j] = tempY;
		}

	IplImage* view4 = cvCreateImage(cvSize(imageWidth,imageHeight),8,3);
	int* view4_dotx = new int[dotHeight*dotWidth];
	int* view4_doty = new int[dotHeight*dotWidth];
	for (int i = 0; i < dotHeight; i++)
		for (int j = 0; j <dotWidth; j++)
		{
			double tempX, tempY;
			W2I(i*250, j*250, 0, tempX, tempY, 3);
			view4_dotx[i*dotWidth + j] = tempX;
			view4_doty[i*dotWidth + j] = tempY;
		}
    int* view4_dotxz = new int[dotHeight*dotWidth];
	int* view4_dotyz = new int[dotHeight*dotWidth];
	for (int i = 0; i < dotHeight; i++)
		for (int j = 0; j <dotWidth; j++)
		{
			double tempX, tempY;
			if (i > 7) W2I(i*250, j*250, -1900, tempX, tempY, 3);
			else W2I(i*250, j*250, -1800, tempX, tempY, 3);
			view4_dotxz[i*dotWidth + j] = tempX;
			view4_dotyz[i*dotWidth + j] = tempY;
		}

	IplImage* view5 = cvCreateImage(cvSize(imageWidth,imageHeight),8,3);
	int* view5_dotx = new int[dotHeight*dotWidth];
	int* view5_doty = new int[dotHeight*dotWidth];
	for (int i = 0; i < dotHeight; i++)
		for (int j = 0; j <dotWidth; j++)
		{
			double tempX, tempY;
			W2I(i*250, j*250, 0, tempX, tempY, 4);
			view5_dotx[i*dotWidth + j] = tempX;
			view5_doty[i*dotWidth + j] = tempY;
		}
    int* view5_dotxz = new int[dotHeight*dotWidth];
	int* view5_dotyz = new int[dotHeight*dotWidth];
	for (int i = 0; i < dotHeight; i++)
		for (int j = 0; j <dotWidth; j++)
		{
			double tempX, tempY;
			if (i > 7 && j > 13) W2I(i*250, j*250, -1900, tempX, tempY, 4);
			else W2I(i*250, j*250, -1800, tempX, tempY, 4);
			view5_dotxz[i*dotWidth + j] = tempX;
			view5_dotyz[i*dotWidth + j] = tempY;
		}

	IplImage* view6 = cvCreateImage(cvSize(imageWidth,imageHeight),8,3);
	int* view6_dotx = new int[dotHeight*dotWidth];
	int* view6_doty = new int[dotHeight*dotWidth];
	for (int i = 0; i < dotHeight; i++)
		for (int j = 0; j <dotWidth; j++)
		{
			double tempX, tempY;
			W2I(i*250, j*250, 0, tempX, tempY, 5);
			view6_dotx[i*dotWidth + j] = tempX;
			view6_doty[i*dotWidth + j] = tempY;
		}
    int* view6_dotxz = new int[dotHeight*dotWidth];
	int* view6_dotyz = new int[dotHeight*dotWidth];
	for (int i = 0; i < dotHeight; i++)
		for (int j = 0; j <dotWidth; j++)
		{
			double tempX, tempY;
			if (j >= 15 && j <= 18) W2I(i*250, j*250, -1900, tempX, tempY, 5);
			else W2I(i*250, j*250, -1800, tempX, tempY, 5);
			view6_dotxz[i*dotWidth + j] = tempX;
			view6_dotyz[i*dotWidth + j] = tempY;
		}
	
	IplImage* view7 = cvCreateImage(cvSize(imageWidth,imageHeight),8,3);
	int* view7_dotx = new int[dotHeight*dotWidth];
	int* view7_doty = new int[dotHeight*dotWidth];
	for (int i = 0; i < dotHeight; i++)
		for (int j = 0; j <dotWidth; j++)
		{
			double tempX, tempY;
			W2I(i*250, j*250, 0, tempX, tempY, 6);
			view7_dotx[i*dotWidth + j] = tempX;
			view7_doty[i*dotWidth + j] = tempY;
		}
    int* view7_dotxz = new int[dotHeight*dotWidth];
	int* view7_dotyz = new int[dotHeight*dotWidth];
	for (int i = 0; i < dotHeight; i++)
		for (int j = 0; j <dotWidth; j++)
		{
			double tempX, tempY;
			if (j >= 11 && j<= 18) W2I(i*250, j*250, -1900, tempX, tempY, 6);
			else W2I(i*250, j*250, -1800, tempX, tempY, 6);
			view7_dotxz[i*dotWidth + j] = tempX;
			view7_dotyz[i*dotWidth + j] = tempY;
		}

	IplImage* view8 = cvCreateImage(cvSize(imageWidth,imageHeight),8,3);
	int* view8_dotx = new int[dotHeight*dotWidth];
	int* view8_doty = new int[dotHeight*dotWidth];
	for (int i = 0; i < dotHeight; i++)
		for (int j = 0; j <dotWidth; j++)
		{
			double tempX, tempY;
			W2I(i*250, j*250, 0, tempX, tempY, 7);
			view8_dotx[i*dotWidth + j] = tempX;
			view8_doty[i*dotWidth + j] = tempY;
		}
    int* view8_dotxz = new int[dotHeight*dotWidth];
	int* view8_dotyz = new int[dotHeight*dotWidth];
	for (int i = 0; i < dotHeight; i++)
		for (int j = 0; j <dotWidth; j++)
		{
			double tempX, tempY;
			if (i >= 1 && i <= 5) W2I(i*250, j*250, -1900, tempX, tempY, 7);
			else W2I(i*250, j*250, -1800, tempX, tempY, 7);
			view8_dotxz[i*dotWidth + j] = tempX;
			view8_dotyz[i*dotWidth + j] = tempY;
		}



	int posy,posx;
	
	Rectangle* a = new Rectangle(); 
	
	
	
	ofstream myfile;
	myfile.open ("view1 setup.txt");
	for (int j = 1; j <= 12; j++)
		for (int i = 1; i <= 18; i++)
		{
			posy = j; posx = i;
			if ((posx == 1 && posy >= 1 && posy <= 5) || (posx == 2 && posy >= 1 && posy <= 4) ||
				(posx == 3 && posy >= 1 && posy <= 2) || (posx == 3 && posy >= 1 && posy <= 2))
			{
				myfile<<"RECTANGLE "<<0<<" "<<(j-1)*18 + i - 1<<" notvisible"<<"\n";
				continue;
			}
			setRectangle(a, view1_dotx[(posy - 1)*dotWidth + posx + 1], view1_doty[(posy - 1)*dotWidth + posx + 1], 
				view1_dotxz[(posy - 1)*dotWidth + posx + 1], view1_dotyz[(posy - 1)*dotWidth + posx + 1], 
				view1_dotxz[(posy + 1)*dotWidth + posx - 1], view1_dotyz[(posy + 1)*dotWidth + posx - 1], 
				view1_dotx[(posy + 1)*dotWidth + posx - 1], view1_doty[(posy + 1)*dotWidth + posx - 1]);
			myfile<<"RECTANGLE "<<0<<" "<<(j-1)*18 + i - 1<<" "<<a->xmin<<" "<<a->ymin<<" "<<a->xmax<<" "<<a->ymax<<"\n";
		}

	for (int j = 1; j <= 12; j++)
		for (int i = 1; i <= 18; i++)
		{
			posy = j; posx = i;
			if ((posx < 3) || (posx == 3 && posy < 8))
			{
				myfile<<"RECTANGLE "<<1<<" "<<(j-1)*18 + i - 1<<" notvisible"<<"\n";
				continue;
			}
			setRectangle(a, view2_dotx[(posy + 1)*dotWidth + posx], view2_doty[(posy + 1)*dotWidth + posx], 
				view2_dotxz[(posy + 1)*dotWidth + posx], view2_dotyz[(posy + 1)*dotWidth + posx], 
				view2_dotxz[(posy - 1)*dotWidth + posx ], view2_dotyz[(posy - 1)*dotWidth + posx ], 
				view2_dotx[(posy - 1)*dotWidth + posx], view2_doty[(posy - 1)*dotWidth + posx]);
			myfile<<"RECTANGLE "<<1<<" "<<(j-1)*18 + i - 1<<" "<<a->xmin<<" "<<a->ymin<<" "<<a->xmax<<" "<<a->ymax<<"\n";
		}
	for (int j = 1; j <= 12; j++)
		for (int i = 1; i <= 18; i++)
		{
			posy = j; posx = i;
			if ((posx == 1 && posy >= 9 && posy <= 12) || (posx == 2 && posy >= 10 && posy <= 12) ||
				(posx == 3 && posy >= 11 && posy <= 12) || ((posx == 4 || posx == 5) && posy == 12))
			{
				myfile<<"RECTANGLE "<<2<<" "<<(j-1)*18 + i - 1<<" notvisible"<<"\n";
				continue;
			}

			setRectangle(a, view3_dotx[(posy + 1)*dotWidth + posx + 1], view3_doty[(posy + 1)*dotWidth + posx + 1], 
				view3_dotxz[(posy + 1)*dotWidth + posx + 1], view3_dotyz[(posy + 1)*dotWidth + posx + 1], 
				view3_dotxz[(posy - 1)*dotWidth + posx - 1], view3_dotyz[(posy - 1)*dotWidth + posx - 1], 
				view3_dotx[(posy - 1)*dotWidth + posx - 1], view3_doty[(posy - 1)*dotWidth + posx - 1]);
			myfile<<"RECTANGLE "<<2<<" "<<(j-1)*18 + i - 1<<" "<<a->xmin<<" "<<a->ymin<<" "<<a->xmax<<" "<<a->ymax<<"\n";
		}

    for (int j = 1; j <= 12; j++)
		for (int i = 1; i <= 18; i++)
		{
			posy = j; posx = i;
			if ((posy >= 10 && posy <= 12) || (posx == 1 && posy >= 8) || (posx == 2 && posy >= 9))
			{
				myfile<<"RECTANGLE "<<3<<" "<<(j-1)*18 + i - 1<<" notvisible"<<"\n";
				continue;
			}
			setRectangle(a, view4_dotx[(posy)*dotWidth + posx - 1], view4_doty[(posy)*dotWidth + posx - 1], 
				view4_dotxz[(posy)*dotWidth + posx - 1], view4_dotyz[(posy)*dotWidth + posx - 1], 
				view4_dotxz[(posy)*dotWidth + posx + 1], view4_dotyz[(posy)*dotWidth + posx + 1], 
				view4_dotx[(posy)*dotWidth + posx + 1], view4_doty[(posy)*dotWidth + posx + 1]);
			myfile<<"RECTANGLE "<<3<<" "<<(j-1)*18 + i - 1<<" "<<a->xmin<<" "<<a->ymin<<" "<<a->xmax<<" "<<a->ymax<<"\n";
		}
	for (int j = 1; j <= 12; j++)
		for (int i = 1; i <= 18; i++)
		{
			posy = j; posx = i;
			if ((posx == 14 && posy == 12) || (posx == 15 && posy == 11) || (posx == 15 && posy == 12) || 
				(posx == 16 && posy >= 10 && posy <= 12) || ((posx == 17 || posx == 18) && posy >= 8 && posy <= 12))
			{
				myfile<<"RECTANGLE "<<4<<" "<<(j-1)*18 + i - 1<<" notvisible"<<"\n";
				continue;
			}
			setRectangle(a, view5_dotx[(posy - 1)*dotWidth + posx + 1], view5_doty[(posy - 1)*dotWidth + posx + 1], 
				view5_dotxz[(posy - 1)*dotWidth + posx + 1], view5_dotyz[(posy - 1)*dotWidth + posx + 1], 
				view5_dotxz[(posy + 1)*dotWidth + posx - 1], view5_dotyz[(posy + 1)*dotWidth + posx - 1], 
				view5_dotx[(posy + 1)*dotWidth + posx - 1], view5_doty[(posy + 1)*dotWidth + posx - 1]);
			myfile<<"RECTANGLE "<<4<<" "<<(j-1)*18 + i - 1<<" "<<a->xmin<<" "<<a->ymin<<" "<<a->xmax<<" "<<a->ymax<<"\n";
		}


	for (int j = 1; j <= 12; j++)
		for (int i = 1; i <= 18; i++)
		{
			posy = j; posx = i;
			if ((i == 18 && (j >= 8 && j <= 12)))
			{
				myfile<<"RECTANGLE "<<5<<" "<<(j-1)*18 + i - 1<<" notvisible"<<"\n";
				continue;
			}
			setRectangle(a, view6_dotx[(posy - 1)*dotWidth + posx], view6_doty[(posy - 1)*dotWidth + posx], 
				view6_dotxz[(posy - 1)*dotWidth + posx], view6_dotyz[(posy - 1)*dotWidth + posx], 
				view6_dotxz[(posy + 1)*dotWidth + posx], view6_dotyz[(posy + 1)*dotWidth + posx], 
				view6_dotx[(posy + 1)*dotWidth + posx], view6_doty[(posy + 1)*dotWidth + posx]);
			myfile<<"RECTANGLE "<<5<<" "<<(j-1)*18 + i - 1<<" "<<a->xmin<<" "<<a->ymin<<" "<<a->xmax<<" "<<a->ymax<<"\n";
		}


	for (int j = 1; j <= 12; j++)
		for (int i = 1; i <= 18; i++)
		{
			posy = j; posx = i;
			if ((i == 13 && j == 1) || (i == 14 && j >= 1 && j <=2) || (i == 15 && j >= 1 && j <=4) || 
				(i == 16 && j >= 1 && j <=6) || (i == 17 && j >= 1 && j <=8) || (i == 18 && j >= 1 && j <=9))
			{
				myfile<<"RECTANGLE "<<6<<" "<<(j-1)*18 + i - 1<<" notvisible"<<"\n";
				continue;
			}
			setRectangle(a, view7_dotx[(posy - 1)*dotWidth + posx - 1], view7_doty[(posy - 1)*dotWidth + posx - 1], 
				view7_dotxz[(posy - 1)*dotWidth + posx - 1], view7_dotyz[(posy - 1)*dotWidth + posx - 1], 
				view7_dotxz[(posy + 1)*dotWidth + posx + 1], view7_dotyz[(posy + 1)*dotWidth + posx + 1], 
				view7_dotx[(posy + 1)*dotWidth + posx + 1], view7_doty[(posy + 1)*dotWidth + posx + 1]);
			myfile<<"RECTANGLE "<<6<<" "<<(j-1)*18 + i - 1<<" "<<a->xmin<<" "<<a->ymin<<" "<<a->xmax<<" "<<a->ymax<<"\n";
		}

	for (int j = 1; j <= 12; j++)
		for (int i = 1; i <= 18; i++)
		{
			posy = j; posx = i;
			if ((j == 3 && i == 1) || (j == 3 && i == 2) || (j == 3 && i == 18) || j == 1 || j == 2)
			{
				myfile<<"RECTANGLE "<<7<<" "<<(j-1)*18 + i - 1<<" notvisible"<<"\n";
				continue;
			}
			setRectangle(a, view8_dotx[(posy)*dotWidth + posx - 1], view8_doty[(posy)*dotWidth + posx - 1], 
				view8_dotxz[(posy)*dotWidth + posx - 1], view8_dotyz[(posy)*dotWidth + posx - 1], 
				view8_dotxz[(posy)*dotWidth + posx + 1], view8_dotyz[(posy)*dotWidth + posx + 1], 
				view8_dotx[(posy)*dotWidth + posx + 1], view8_doty[(posy)*dotWidth + posx + 1]);
			myfile<<"RECTANGLE "<<7<<" "<<(j-1)*18 + i - 1<<" "<<a->xmin<<" "<<a->ymin<<" "<<a->xmax<<" "<<a->ymax<<"\n";
		}



	myfile.close();

	
	



	posy = 12;  //posx
	posx = 3; //posy 
	
	
	//beginTime = 
	setRectangle(a, view3_dotx[(posy + 1)*dotWidth + posx + 1], view3_doty[(posy + 1)*dotWidth + posx + 1], 
		view3_dotxz[(posy + 1)*dotWidth + posx + 1], view3_dotyz[(posy + 1)*dotWidth + posx + 1], 
		view3_dotxz[(posy - 1)*dotWidth + posx - 1], view3_dotyz[(posy - 1)*dotWidth + posx - 1], 
		view3_dotx[(posy - 1)*dotWidth + posx - 1], view3_doty[(posy - 1)*dotWidth + posx - 1]);

	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, beginTime*frameRate);
	cvSetCaptureProperty(capture2, CV_CAP_PROP_POS_FRAMES, beginTime*frameRate + 1000);
	while (true)
	{
		countFrame++;
		if (cvWaitKey(30) == 27) return;
		cvGrabFrame(capture);
		cvGrabFrame(capture2);
		img = cvRetrieveFrame(capture);  //background image
		img2 = cvRetrieveFrame(capture2);  //color image


		view1->imageData = &img->imageData[2*imageHeight*frameWidth*3];
		view2->imageData = &img2->imageData[1*imageHeight*frameWidth*3];
		view3->imageData = &img2->imageData[1*imageHeight*frameWidth*3];
		
		

		//setPosition(view2, dotHeight, dotWidth, view8_dotx, view8_doty, 0);
		//setPosition(view3, dotHeight, dotWidth, view3_dotxz, view3_dotyz, 1);


		
		if (countFrame == 1) cvSaveImage("frame1.png", view2);

		
		for (int i = a->ymin; i <= a->ymax; i++)
			for (int j = a->xmin; j <= a->xmax; j++)
		{
			view1->imageData[i*view1->widthStep + j*3] = 255;
			view1->imageData[i*view1->widthStep + j*3 + 1] = 255;
			view1->imageData[i*view1->widthStep + j*3 + 2] = 255;
		}
	/*
		setRectangle(view3, view2_dotx, view2_doty, dotWidth, posy + 1, posx, 0);
		setRectangle(view3, view2_dotx, view2_doty, dotWidth, posy - 1, posx, 0);
		setRectangle(view3, view2_dotxz, view2_dotyz, dotWidth, posy + 1, posx, 2);
		setRectangle(view3, view2_dotxz, view2_dotyz, dotWidth, posy - 1, posx, 2);
		setRectangle(view3, view2_dotx, view2_doty, dotWidth, posy, posx, 1);
		*/
		
 
		

		//chep hinh
		
		//char* b = new char[1000];
       // for(int c = 0; c < 8; c++) {
          
         // char* temp = new char[100];
		 // string a = "images\\";
		//  a += "view-f";
		 // itoa(countFrame,temp,10);
		//  a += temp;
		  
		  
		//  a += "-c";
		//  itoa(c,temp,10);
		//  a += temp;
		//  a += ".png";
		//  strcpy(b, a.c_str());
		//  view1->imageData = &img->imageData[c*imageHeight*frameWidth*3];
		//  cvSaveImage(b, view1);
       // }
		



		cvShowImage("test", img);
		cvShowImage("testview1",view1);
		cvShowImage("testview2",view2);
		cvShowImage("testview3",view3);
		if (countFrame/frameRate > 30) return;
	}
	cvReleaseImage(&img);
	cvReleaseImage(&view1);
	cvReleaseImage(&view2);
	cvReleaseImage(&view3);
	cvReleaseImage(&img2);
	cvReleaseCapture(&capture);
	cvReleaseCapture(&capture2);
	cvDestroyAllWindows();
	




  
    

}

void getImageFromFile()
{
	CvCapture* capture = cvCaptureFromFile("dense-ut.avi");
	cvNamedWindow("test", 1);
	IplImage* img;

	int frameRate = (int)(cvGetCaptureProperty(capture, CV_CAP_PROP_FPS));
	int frameWidth = (int)(cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH));
	int frameHeight = (int)(cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT));
	int beginTime = 23; //28
	cout<<"frame rate"<<"      "<<frameRate<<"\n";
	cout<<"frame width"<<"      "<<frameWidth<<"\n";
	cout<<"frame height"<<"      "<<frameHeight<<"\n";
	int countFrame = 0;

	IplImage* view = cvCreateImage(cvSize(800, 600), 8,3);

	CvVideoWriter* write = cvCreateVideoWriter("D:\shview3.avi", 0, 30, cvSize(800, 600), 1);
	
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, beginTime*frameRate);
	while (true)
	{
		countFrame++;
		if (cvWaitKey(30) == 27) return;
		cvGrabFrame(capture);
		img = cvRetrieveFrame(capture);
		view->imageData = &img->imageData[2*600*frameWidth*3];
		


		
		

		//setPosition(view2, dotHeight, dotWidth, view8_dotx, view8_doty, 0);
		//setPosition(view3, dotHeight, dotWidth, view3_dotxz, view3_dotyz, 1);


		
		if (countFrame <= 900) 
			{
				//cvSaveImage("frame.png", img);
				cvWriteFrame(write, view);
		}
		else break;

		



		cvShowImage("test", view);
		//if (countFrame/frameRate > 30) return;
	}
	//cvReleaseImage(&img);
	cvReleaseCapture(&capture);
	cvReleaseVideoWriter(&write);
	cvDestroyAllWindows();
}







void getVideo()
{
	CvCapture* capture = cvCaptureFromFile("dense-e.avi");
	cvNamedWindow("test", 1);
	IplImage* img;

	int frameRate = (int)(cvGetCaptureProperty(capture, CV_CAP_PROP_FPS));
	int frameWidth = (int)(cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH));
	int frameHeight = (int)(cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT));
	int beginTime = 23; //28
	cout<<"frame rate"<<"      "<<frameRate<<"\n";
	cout<<"frame width"<<"      "<<frameWidth<<"\n";
	cout<<"frame height"<<"      "<<frameHeight<<"\n";
	int countFrame = 0;

	IplImage* view = cvCreateImage(cvSize(800, 600), 8,3);

	CvVideoWriter* write = cvCreateVideoWriter("D:\original_shview3.avi", 0, 30, cvSize(800, 600), 1);
	
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, beginTime*frameRate);
	while (true)
	{
		countFrame++;
		if (cvWaitKey(30) == 27) return;
		cvGrabFrame(capture);
		img = cvRetrieveFrame(capture);
		view->imageData = &img->imageData[0*600*frameWidth*3];
		


		
		

		//setPosition(view2, dotHeight, dotWidth, view8_dotx, view8_doty, 0);
		//setPosition(view3, dotHeight, dotWidth, view3_dotxz, view3_dotyz, 1);


		
		if (countFrame <= 900) 
			{
				//cvSaveImage("frame.png", img);
				cvWriteFrame(write, view);
		}
		else break;

		



		cvShowImage("test", view);
		//if (countFrame/frameRate > 30) return;
	}
	//cvReleaseImage(&img);
	cvReleaseCapture(&capture);
	cvReleaseVideoWriter(&write);
	cvDestroyAllWindows();
}

//void main()
//{
	//getVideo();
	//process();
	//setup();
	//getImageFromFile();
	//setupEPFL();
//}