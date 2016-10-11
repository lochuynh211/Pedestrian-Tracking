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