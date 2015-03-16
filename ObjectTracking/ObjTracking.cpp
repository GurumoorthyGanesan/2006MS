
#include "header.h"

// GLOBAL	////////////////////////////////////
float hranges_arr[] = {0,180};

static CamShift *this_object;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*
 *	constructor and destructor
 */
CamShift::CamShift(){

this_object = this;

capture = 0;
frame = 0; image = 0; hsv = 0; hue = 0; mask = 0; bgbackpro = 0; histimg = 0, distimg = 0, img_gray = 0;

track_object = 0;
select_object = 0;
update_foreground = 0;
bgbackproject_mode = 0;
hdims = MAX_HISTOGRAM;  
hranges = hranges_arr;
pos = 0,start = 0;
source = SOURCE_AVI;

s_person = NULL;
s_person = new Blob[MAX_HUMAN]();
e_kernelType = eKernelType(Normal);


cvNamedWindow( "Demo", 1 );			cvMoveWindow("Demo",0,0);
//cvNamedWindow( "SDM", 1 );			cvMoveWindow("SDM",440,300);
//cvNamedWindow( "Edge", 1 );			cvMoveWindow("Edge",840,0);
//cvNamedWindow( "Result", 1 );		cvMoveWindow("Result",0,300);
//( "MASK", 1 );			cvMoveWindow("MASK",840,300);
//cvNamedWindow( "Histogram", 1 );	cvMoveWindow("Histogram",440,0);

}

CamShift::~CamShift(){
	//cvDestroyWindow("Histogram");
	cvDestroyWindow("Demo");
	//cvDestroyWindow("Result");
	//cvDestroyWindow("MASK");
	//cvDestroyWindow("SDM");
	//cvDestroyWindow("Edge");
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//static  void  on_mouse( int  event,  int  x,  int  y,  int  flags,  void  *param);
/*
 *	mouse control
 */
static void input_static_callback (int event, int x, int y, int flags,  void  *param) {
    this_object->input_mouse_callback (event, x, y);
}

void CamShift::input_mouse_callback( int event, int x, int y )
{
	if( !image )       return;

    if( image->origin )
        y = image->height - y;

    if( select_object ){
		// set the object window
        selection.x = MIN(x,origin.x);
        selection.y = MIN(y,origin.y);
        selection.width = selection.x + CV_IABS(x - origin.x);
        selection.height = selection.y + CV_IABS(y - origin.y);
        
        selection.x = MAX( selection.x, 0 );
        selection.y = MAX( selection.y, 0 );
        selection.width = MIN( selection.width, frame->width );
        selection.height = MIN( selection.height, frame->height );
        selection.width -= selection.x;
        selection.height -= selection.y;

		// record the object window
		s_person[0].Obj.rect = selection;

		start = pos;
    }

    switch( event )
    {
    case CV_EVENT_LBUTTONDOWN:
	{
        origin = cvPoint(x,y);
        selection = cvRect(x,y,0,0);
        select_object = 1;
        break;
	}
    case CV_EVENT_LBUTTONUP:
	{
        select_object = 0;
        if( selection.width > 0 && selection.height > 0 )
            track_object = -1;
#ifdef _DEBUG
    printf("\n mouse area :"); 
    printf("\n   X = %d, Y = %d, Width = %d, Height = %d",
        selection.x, selection.y, selection.width, selection.height);
#endif
        break;
	}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*
 *	show histogram
 */
void CamShift::ShowHist(int i){

	// to switch the display mode
	if( bgbackproject_mode )
         cvCvtColor( hue, frame, CV_GRAY2BGR );
        
	// show histogram image
    float bin_w = histimg->width / hdims;   

    for(int j = 0; j < hdims; j++ ){
        int val = cvRound( cvGetReal1D(s_person[i].hist[ADJ_HIST]->bins,j)*histimg->height/255 );
        CvScalar color = hsv2rgb(j*180.f/hdims);
        cvRectangle( histimg, cvPoint(j*bin_w,histimg->height),
               cvPoint((j+1)*bin_w,histimg->height - val),
         color,1, 8, 0 );
    }
}

/*
 *		the system of information is display on the screen
 */
bool CamShift::ShowInfo(int pindex){
	// frame rate
	int c = cvWaitKey(SPEED_AVI);
    if( c == 27 )
        return false;  // exit from for-loop
    switch( c ){
		case 'b': case 'B':
            bgbackproject_mode ^= 1;
            break;
        case 'c': case 'C':
            track_object = 0;
            cvZero( histimg );
            break;
        default:
            ;
    }

	// show information
	char mess[64];
	sprintf(mess, "Frame: %3d, x: %3d, y: %3d", pos, (int)s_person[pindex].track_obj.center.x, (int)s_person[pindex].track_obj.center.y);

	CvFont font;
	cvInitFont( &font,CV_FONT_VECTOR0,0.5,0.5);
	cvPutText( frame, mess ,cvPoint(10,frame->height-10),&font,CV_RGB(255,255,255));

    cvShowImage( "Demo", frame );
//	cvShowImage( "MASK", distimg );
	//cvShowImage( "Result",  bgbackpro);
	//cvShowImage( "Edge", img_gray);
	//cvShowImage( "SDM", image);
    //cvShowImage( "Histogram", BigMap );
	//cvShowImage( "MASK", mask );
/*
char mess1[64];
sprintf(mess1, "D:\\1\\result\\物轉01_1 result %03d.tif", pos);	
cvSaveImage(mess1,frame);
sprintf(mess1, "D:\\1\\mask\\物轉01_1 mask %03d.tif", pos);	
cvSaveImage(mess1,mask);
sprintf(mess1, "D:\\1\\bp\\物轉01_1 bp %03d.tif", pos);	
cvSaveImage(mess1,image);
sprintf(mess1, "D:\\1\\smooth\\物轉01_1 sm %03d.tif", pos);	
cvSaveImage(mess1,img_gray);
sprintf(mess1, "D:\\1\\hist\\物轉01_1 hist %03d.tif", pos);	
cvSaveImage(mess1,histimg);
sprintf(mess1, "D:\\1\\bpg\\物轉01_1 bgp %03d.tif", pos);	
cvSaveImage(mess1,bgbackpro);*/
	return true;
}

/*
 *		2 norm
 */
double CamShift::Norm2D(CvPoint s,CvPoint d){
	return cvSqrt((float) ( ((s.x - d.x)*(s.x - d.x)) + ((s.y - d.y)*(s.y - d.y)) ));
}

/*
 *		hsv to rgb
 */
CvScalar CamShift::hsv2rgb( float hue )
{
    int rgb[3], p, sector;
    static const int sector_data[][3]=
        {{0,2,1}, {1,2,0}, {1,0,2}, {2,0,1}, {2,1,0}, {0,1,2}};
    hue *= 0.033333333333333333333333333333333f;
    sector = cvFloor(hue);
    p = cvRound(255*(hue - sector));
    p ^= sector & 1 ? 255 : 0;

    rgb[sector_data[sector][0]] = 255;
    rgb[sector_data[sector][1]] = 0;
    rgb[sector_data[sector][2]] = p;

    return cvScalar(rgb[2], rgb[1], rgb[0],0);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*
 *	lookup table
 */
void CamShift::SinCos( int angle, float *cosval, float *sinval )
{
    angle += (angle < 0 ? 360 : 0);
    *sinval = SinTable[angle];
    *cosval = SinTable[450 - angle];
}

/* 
   constructs polygon that represents elliptic arc.
*/
void CamShift::Ellipse2Poly(int pindex, CvPoint center, CvSize axes, int angle,
                 int arc_start, int arc_end, CvPoint* pts, int delta )
{
    float alpha, beta;
    double size_a = axes.width, size_b = axes.height;
    double cx = center.x, cy = center.y;
    CvPoint *pts_origin = pts;
	int tha = angle;
    int i;

    while( angle < 0 )
        angle += 360;
    while( angle > 360 )
        angle -= 360;

    if( arc_start > arc_end ){
        i = arc_start;
        arc_start = arc_end;
        arc_end = i;
    }
    while( arc_start < 0 ){
        arc_start += 360;
        arc_end += 360;
    }
    while( arc_end > 360 ){
        arc_end -= 360;
        arc_start -= 360;
    }
    if( arc_end - arc_start > 360 ){
        arc_start = 0;
        arc_end = 360;
    }
    SinCos( angle, &alpha, &beta );

    for( i = arc_start; i < arc_end; i ++ )
    {
        double x, y;
        angle = i;
	
        if( angle > arc_end )      angle = arc_end;
        if( angle < 0 )            angle += 360;
        
        x = size_a * SinTable[450-angle];
        y = size_b * SinTable[angle];
        pts->x = cvRound( cx + x * alpha - y * beta );
        pts->y = cvRound( cy - x * beta - y * alpha );

		if (pts->x < 0) pts->x =0;
		if (pts->y < 0) pts->y =0;
		if (pts->x > image->width-1)	pts->x =image->width-1;
		if (pts->y > image->height-1)	pts->y =image->height-1;

//cvSet2D(frame, pts->y, pts->x, CV_RGB(255, 255, 255));
	
		s_person[pindex].boundary[angle].x = pts->x;
		s_person[pindex].boundary[angle].y = pts->y;
    }
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*
 *		to garb the frame from the camera or AVI FILE
 */
bool CamShift::GrabFrame()
{
	// grab the frame
    frame = cvQueryFrame( capture );

	// last frame, exit the loop
	if (!frame)
		return false;
	
	// the source come from the AVI files
    if( source == SOURCE_AVI){
       frame->origin = IPL_ORIGIN_TL;
       //cvFlip( frame, frame, 0 );
    }

    if( !image ){
		// store image
		image = cvCreateImage( cvGetSize(frame), 8, 3 );
        image->origin = frame->origin;

		// Convert color to HSV
	    hsv			= cvCreateImage(cvSize( frame->width,frame->height), IPL_DEPTH_8U, 3 );
		hsv->origin	= frame->origin;
        hue			= cvCreateImage(cvSize( frame->width,frame->height), IPL_DEPTH_8U, 1 );
		hue->origin	= frame->origin;
		mask		= cvCreateImage(cvSize( frame->width,frame->height), IPL_DEPTH_8U, 1 );
		mask->origin	= frame->origin;
		bgbackpro = cvCreateImage(cvSize( frame->width,frame->height), IPL_DEPTH_8U, 1 );
		bgbackpro->origin	= frame->origin;
	
		// Convert color to gray
		img_gray = cvCreateImage(cvSize( frame->width,frame->height), IPL_DEPTH_8U, 1 );      
		img_gray->origin = frame->origin;

		distimg = cvCreateImage(cvSize(frame->width,frame->height), IPL_DEPTH_8U, 1 );
		distimg->origin = frame->origin;
		// create the histogram
		histimg = cvCreateImage(cvSize( 360,frame->height), IPL_DEPTH_8U, 3 );
		
		BigMap = cvCreateImage(cvSize( 360*3,frame->height*2), IPL_DEPTH_8U, 3 );
		BigMap->origin = frame->origin;
    }
	// convert the color space
	cvCopy( frame, image, 0 );
	cvCvtColor( frame, hsv, CV_BGR2HSV);


	// split the hsv,*/
	cvSplit( hsv, hue, 0, 0, 0 );

	// clear the images
	cvZero( histimg );cvNot(histimg,histimg);
	cvZero( distimg );

	if(  select_object && selection.width > 0 && selection.height > 0 ){
		cvSetImageROI( frame, selection );
        cvXorS( frame, cvScalarAll(255), frame, 0 );
        cvResetImageROI( frame );
    }

	return true;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/*
 *		To segment the foreground & background 
 */
void CamShift::GrayElement(int pindex){
	
	int k;
	float max_val = 0.f, min_val = 0.f, mid_val = 0.f;
	int max_hid = 0, min_hid =0;
	
	if( track_object ){

// first time tracking, use object window to replace search window
		cvSetImageROI( hue, s_person[pindex].Obj.rect);
        if( track_object < 0  ){
				
			track_object = 1;	// tracking
// push the zero data to the queue
			int gray_num[MAX_HISTOGRAM];
			memset(gray_num,0,sizeof(gray_num));
			for (int k=0; k < TIME_INT; k++)	
				fn_insert( &s_person[0].frontNode, &s_person[0].rearNode, gray_num );
		
// set the search window & object window
			s_person[pindex].search_window.width = s_person[pindex].Obj.rect.width*2;
			s_person[pindex].search_window.height = s_person[pindex].Obj.rect.height*2;
			s_person[pindex].search_window.x = s_person[pindex].Obj.rect.x - s_person[pindex].Obj.rect.width/2;
			s_person[pindex].search_window.y = s_person[pindex].Obj.rect.y - s_person[pindex].Obj.rect.height/2;
			CheckWindow(s_person[pindex].search_window);
			// assign the center
			s_person[pindex].central.x = s_person[pindex].Obj.rect.x + s_person[pindex].Obj.rect.width/2;
			s_person[pindex].central.y = s_person[pindex].Obj.rect.y + s_person[pindex].Obj.rect.height/2;

// update the target model -----------------------------------------------------
			// foreground model, object window
			s_person[pindex].hist[FORE_HIST] = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
			cvCalcHist( &hue, s_person[pindex].hist[FORE_HIST], 0, 0);
			
			// copy foreground data - NO ANY OLD FOREGROUND DATA
			s_person[pindex].hist[FORE_OLD_HIST] = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
			cvCopyHist(s_person[pindex].hist[FORE_HIST], &s_person[pindex].hist[FORE_OLD_HIST]);
		}else{
			// foreground model, object window ----- new
			s_person[pindex].hist[FORE_HIST] = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
			cvCalcHist( &hue, s_person[pindex].hist[FORE_HIST], 0, 0);

	
			cvZero(mask); cvZero(img_gray);
		}
//cvShowImage( "Result",  hue);
		cvResetImageROI( hue );
		
// update the background model -----------------------------------------------
		// background model, search window
		cvSetImageROI( hue, s_person[pindex].search_window);
		s_person[pindex].hist[BACK_HIST] = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
        cvCalcHist( &hue, s_person[pindex].hist[BACK_HIST], 0, 0 );
		cvResetImageROI( hue );
		
	
// sub the foreground model --------------------------------------------------
		float _result;
		float total_pixel[MAX_HISTOGRAM], sum_total = 0;
		for ( k = 0; k <hdims; k++){
			float b = cvQueryHistValue_1D(s_person[pindex].hist[BACK_HIST], k);
			float f = cvQueryHistValue_1D(s_person[pindex].hist[FORE_HIST], k);
			_result = b-f;
			if (_result < 0)
				cvWaitKey(0);

			total_pixel[k] = b;
			sum_total += b;
		
		    // assign to background histogram
			cvSetReal1D( s_person[pindex].hist[BACK_HIST]->bins, k, _result);
		}

					
// kalman filter ----- old -----------------------------------------------------
		if (update_foreground > 0){
			
			double new_data, old_data, result_data;
			double delta = 1-s_person[pindex].confidence;
			for (int j=0 ; j < MAX_HISTOGRAM; j++){
				new_data = cvQueryHistValue_1D(s_person[pindex].hist[FORE_HIST], j);
				old_data = cvQueryHistValue_1D(s_person[pindex].hist[GRAY_HIST], j);

				result_data = new_data + old_data;
cvSetReal1D( s_person[pindex].hist[FORE_HIST]->bins, j, result_data );
			}
	
			update_foreground = 0;
		}
		// create the new gray element histogram
		s_person[pindex].hist[GRAY_HIST] = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
		cvClearHist(s_person[pindex].hist[GRAY_HIST]);


// normalize the histogram(forground,background) -------------------------------
		cvGetMinMaxHistValue( s_person[pindex].hist[FORE_HIST], 0, &max_val, 0, 0 );
        cvConvertScale( s_person[pindex].hist[FORE_HIST]->bins, s_person[pindex].hist[FORE_HIST]->bins, 
			max_val ? 255. / max_val : 0., 0 );
		cvNormalizeHist(s_person[pindex].hist[FORE_HIST],1);

		cvGetMinMaxHistValue( s_person[pindex].hist[BACK_HIST], 0, &max_val, 0, 0 );
        cvConvertScale( s_person[pindex].hist[BACK_HIST]->bins, s_person[pindex].hist[BACK_HIST]->bins, 
			max_val ? 255. / max_val : 0., 0 );
		cvNormalizeHist(s_person[pindex].hist[BACK_HIST],1);

// log likelihood ratio test -------------------------------------------------
		s_person[pindex].hist[LLRT_HIST] = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
	
		for ( k = 0; k < hdims; k++){
			// sub the foreground model
			float b = cvQueryHistValue_1D(s_person[pindex].hist[BACK_HIST], k);
			float f = cvQueryHistValue_1D(s_person[pindex].hist[FORE_HIST], k);

			double p = max(f,0.000000001f); double q= max(b,0.000000001f);

			// calculate the log likelihood ratio
			//wei = (1/(1+exp(-wei)));
			double wei = (total_pixel[k]/sum_total)*log(p / q);

			s_person[pindex].LoglikelihoodRatio[k] = wei;
			cvSetReal1D( s_person[pindex].hist[LLRT_HIST]->bins, k, s_person[pindex].LoglikelihoodRatio[k]);
		}

		double lr_var_up =0, lr_var_low =0;
		double l_counter = 0, u_counter = 0;
		for ( k = 0; k <hdims; k++){
			float lr = cvQueryHistValue_1D(s_person[pindex].hist[LLRT_HIST], k);
			float flr = cvQueryHistValue_1D(s_person[pindex].hist[FORE_HIST], k);
			float slr = cvQueryHistValue_1D(s_person[pindex].hist[BACK_HIST], k);

			double temp=0;
			if (lr < 0){
				temp = (0 - lr)*slr;
				lr_var_low+= (slr);
				l_counter+= temp;
				//	lr_var_low += (temp*temp);
			}
			if (lr > 0){
				temp = (lr - 0)*flr;
				lr_var_up+= (flr);
				u_counter+= temp;
			//	lr_var_up  += (temp*temp);
			}
		}
		lr_var_low /=l_counter;					lr_var_up/= u_counter ;
		s_person[pindex].lG_SD = l_counter;	s_person[pindex].uG_SD = u_counter;
	

// normalize the histogram (log likelihood ratio)-------------------------------
		cvGetMinMaxHistValue( s_person[pindex].hist[LLRT_HIST], &min_val, &max_val, &min_hid, &max_hid );
		mid_val = (float)(max(fabs(min_val),fabs(max_val)));
        cvConvertScale( s_person[pindex].hist[LLRT_HIST]->bins, s_person[pindex].hist[LLRT_HIST]->bins, 
			mid_val ? 255. / mid_val : 0., 0 );

// back-project ----------------------------------------------------------------
		cvCalcBackProject( &hue, bgbackpro, s_person[pindex].hist[LLRT_HIST] );

// sum of the bins--------------------------------------------------------------
		for ( k = 0; k < hdims; k++){
			float lr = cvQueryHistValue_1D(s_person[pindex].hist[LLRT_HIST], k);

			lr+=256;  // shift the value to nonnegative	
			lr /=2;	  // normalized value (0~256) 
			cvSetReal1D( s_person[pindex].hist[LLRT_HIST]->bins, k, lr);	
		}
	

		
		lr_var_up =0, lr_var_low =0;
		l_counter = 0, u_counter = 0;
		for ( k = 0; k <hdims; k++){
			float lr = cvQueryHistValue_1D(s_person[pindex].hist[LLRT_HIST], k);
			float flr = cvQueryHistValue_1D(s_person[pindex].hist[FORE_HIST], k);
			float slr = cvQueryHistValue_1D(s_person[pindex].hist[BACK_HIST], k);

			double temp;
			if (lr < 128){
				temp = (lr - 128)*slr;
				lr_var_low += (temp*temp);
				l_counter+= abs(temp);
			}
			if (lr > 128){
				temp = (lr - 128)*flr;
				lr_var_up  += (temp*temp);
				u_counter+= temp;
			}
		}
		lr_var_low /= l_counter;					lr_var_up /= u_counter;
		s_person[pindex].lG_SD = lr_var_low;	s_person[pindex].uG_SD = lr_var_up;
		
// copy foreground data -------------------------------------------------------
		s_person[pindex].hist[ADJ_HIST] = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
		cvCopyHist(s_person[pindex].hist[LLRT_HIST], &s_person[pindex].hist[ADJ_HIST]);
		
// calculate the background model of distribution ----------------------------
		s_person[pindex].hist[WEI_HIST] = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
		calcProb(bgbackpro, s_person[pindex].Obj.rect, s_person[pindex].hist[WEI_HIST]);

// show the background model --------------------------------------------------
	ShowHist(pindex);
	CvPoint p,q;   //(240-128)
	int varheigh = 120;
	p.x = 0; p.y = varheigh; q.x = histimg->width, q.y = varheigh; cvLine( histimg,p,q,CV_RGB(0,0,0));
	p.x = 0; p.y = varheigh+s_person[pindex].lG_SD; q.x = histimg->width, q.y = p.y; cvLine( histimg,p,q,CV_RGB(0,0,255));
	p.x = 0; p.y = varheigh-s_person[pindex].uG_SD; q.x = histimg->width, q.y = p.y; cvLine( histimg,p,q,CV_RGB(255,0,0));
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void		CamShift::CheckWindow(CvRect &window){

	// adjust the bandwidth -- x
	if(window.x < 0) window.x = 0;
	if(window.x + window.width > frame->width) { 
		window.width = min(frame->width - window.x,	window.width); 
	}	

	// adjust the bandwidth -- y
	if(window.y < 0) window.y = 0;
	if(window.y + window.height > frame->height) { 
		window.height = min(frame->height - window.y, window.height); 
	}
}

CvPoint2D32f	CamShift::GetWinCenter(CvRect &window){
	CvPoint2D32f center;
	center.x = static_cast<float>(window.x + window.width / 2);
	center.y = static_cast<float>(window.y + window.height / 2);
	return center;
}

float	CamShift::calcDistance(CvHistogram *targetProb, CvHistogram *candProb){
    // calculate the color distribution distance between two histograms
    return (static_cast<float>(sqrt(1.0f - calcBhatCoef(targetProb,candProb))));
}

float	CamShift::calcBhatCoef(CvHistogram *targetProb, CvHistogram *candProb){

	// calculates the bhattacharyya metric for measuring
    // similarity of two color distributions
    float bhat=0.0;

	// normalize the histogram
	cvNormalizeHist(targetProb,1);
	cvNormalizeHist(candProb,1);

	for (int k=0; k < MAX_HISTOGRAM; k++){
		float p =	cvQueryHistValue_1D(targetProb, k);
		float q =	cvQueryHistValue_1D(candProb, k);
		bhat +=  (float)(sqrt(p * q));
	}
    return bhat;
}

void	CamShift::calcProb(IplImage* img_src, CvRect &window,CvHistogram *prob){

	double	pixel_color;
	int		nor_vcol;
	float	max_val = 0.f;
	CvPoint2D32f center,pos, radius;
	radius.x = static_cast<float>(window.width / 2);
	radius.y = static_cast<float>(window.height / 2);
	center.x = static_cast<float>(window.x + radius.x);
	center.y = static_cast<float>(window.y + radius.y);

	for(int col = window.y; col < (window.y + window.height); col++){
		for(int row = window.x; row < (window.x + window.width); row++){
			pos.x = (float)row, pos.y = (float)col;

			pixel_color = (unsigned char)img_src->imageData[col * img_src->width + row];
			nor_vcol = (int)(pixel_color / COLOR_RANGE * MAX_HISTOGRAM);

			double kern = kernel(center, pos, radius);
			double tmp = cvQueryHistValue_1D(prob, nor_vcol);
			tmp += kern;

			cvSetReal1D( prob->bins, nor_vcol, tmp);
		}
	}
	
	cvGetMinMaxHistValue( prob, 0, &max_val, 0, 0 );
    cvConvertScale( prob->bins, prob->bins, max_val ? 255. / max_val : 0., 0 );
}

double	CamShift::kernel(CvPoint2D32f center,CvPoint2D32f pos,CvPoint2D32f radius){

    float temp1 = static_cast<float>(center.x-pos.x);
    temp1 = temp1/radius.x;
    temp1 = temp1*temp1;

    float temp2 = static_cast<float>(center.y-pos.y);
    temp2 = temp2/radius.y;
    temp2 = temp2*temp2;

    float ksi = temp1 + temp2;

    // Normal Kernel
    if (e_kernelType == Normal){
      //return 1/(2*Pi)*exp(-0.5*ksi);
      return exp(-0.5f*ksi);
    }
    // Epanechnikov Kernel
    else if(e_kernelType == Epanechnikov){
      if (ksi <=1) {
        //return 1/(2*cd)*4*(1-ksi);
        return (1-ksi);
      }
       else {
         return 0;
       }
    }
    // unknown Kernel
    else return 0;
}

double	CamShift::derivateKernel(CvPoint2D32f center,CvPoint2D32f pos,CvPoint2D32f h){

    float temp1 = static_cast<float>(center.x-pos.x);
    temp1 = temp1/h.x;
    temp1 = temp1*temp1;

    float temp2 = static_cast<float>(center.y-pos.y);
    temp2 = temp2/h.y;
    temp2 = temp2*temp2;

    float ksi = temp1 + temp2;

    // Normal Kernel
    if (e_kernelType == Normal){
      return 0.5f*exp(-0.5f*ksi);
    }
    // Epanechnikov Kernel
    else if(e_kernelType == Epanechnikov){
      if (ksi <=1) {
        return 1;
      }
       else {
         return 0;
       }
    }
    // unknown Kernel
    else return 0;
}

/*
 *		mean shift
 */
void CamShift::MeanShift(int pindex){

// var. -----------------------------------------------------------------------------
	int		k, j, it_counter=0 , s = 0;
// mean-shift
	float	asprat= 0, dist = 0, lastDist = 0, bhat0 = 0;
// monent
	double  m00,m01,m10,m11,m20,m02,u11,u20,u02;
// confidence
	float	all_fore = 0, all_sour = 0, missdetection = 0, falsealarm = 0, hit=0;
// bandwidth energy
	float	sum_bx, sum_by,bandwidth_x,bandwidth_y,bandwidth_s,bandwidth_d;

// radius of distribution -----------------------------------------------------------
	CvPoint2D32f OrgCenter, NewCenter, NowCenter, radius, net;

	// object window
	CvRect	window = s_person[pindex].Obj.rect;
	CvRect	search_window = s_person[pindex].search_window;
	radius.x = static_cast<float>(window.width / 2);
	radius.y = static_cast<float>(window.height / 2);
	OrgCenter.x = static_cast<float>(window.x + radius.x);
	OrgCenter.y = static_cast<float>(window.y + radius.y);
	pf_y1 = OrgCenter;

	// search window
	net.x = static_cast<float>(search_window.width / 2);
	net.y = static_cast<float>(search_window.height / 2);
	
// create the temp histogram
	CvHistogram	*tempHist = cvCreateHist(  1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
	cvClearHist(tempHist);
	CvHistogram	*candProb = cvCreateHist(  1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
	cvClearHist(candProb);

// mean-shift process -------------------------------------------------------------
	do{
		// clear the information(next loop)
		m00=0; m01=0; m10=0; m11=0, m02 = 0, m20 = 0; u11 =0;u20 =0;u02 =0;
		bandwidth_x = 0,bandwidth_y = 0, sum_bx = 0, sum_by = 0, bandwidth_s = 0;
		missdetection = 0, falsealarm = 0, hit = 0;

		// denominator for y1 calculation
		float denominator = 0.0;
		// numerator for y1 calculation
		CvPoint2D32f numerator;
		numerator.x = 0; numerator.y = 0;

		OrgCenter=pf_y1;
			
		// get the HIST_PDF
		calcProb(bgbackpro, window, tempHist);
		bhat0 = calcBhatCoef(s_person[pindex].hist[WEI_HIST], tempHist);

	for(k= search_window.y; k < (search_window.y + search_window.height); k++)
	{	
		for(j= search_window.x; j < (search_window.x + search_window.width); j++)
		{
			// avoid the pixel out of boundary
			if (k < image->height && k > 0 && j < image->width && j > 0){
			
				// read the data
				CvPoint2D32f v_pos;
				v_pos.x = (float)j, v_pos.y = (float)k;
				NowCenter = GetWinCenter(window);

				float	pixel_color = abs((unsigned char)hue->imageData[k*hue->width+j]);
				int		nor_vcol = (int)(pixel_color/COLOR_RANGE*MAX_HISTOGRAM);
								
				float	temp3 = cvQueryHistValue_1D(s_person[pindex].hist[ADJ_HIST], nor_vcol);
				float	pix_wei = temp3 / COLOR_RANGE;

// mean-shift ---------------------------------------------------------------------
			float wi, kern_d, pdf_m,band_wi;
			if (v_pos.y >= window.y && v_pos.y <= (window.y + window.height) &&
				v_pos.x >= window.x && v_pos.x <= (window.x + window.width)){

				// reads the weights in histogram for the color at position (row,col)
				float	temp1, temp2;
				temp1 = cvQueryHistValue_1D(s_person[pindex].hist[WEI_HIST], nor_vcol);
				temp2 = cvQueryHistValue_1D(tempHist, nor_vcol);
	
				// calculate the center
				if (temp1 < 0.0f ) 		temp1=0;
				if (temp2 > 0.0f) {
					wi = static_cast<float>(sqrt( temp1 / temp2));
					kern_d = static_cast<float>(derivateKernel(NowCenter, v_pos, radius));

					pdf_m = wi*kern_d;

					// numerator
					numerator.x += static_cast<float>(pdf_m*v_pos.x);
					numerator.y += static_cast<float>(pdf_m*v_pos.y);

					// denominator
					denominator += static_cast<float>(pdf_m);
					
					// calculate the energy of scanline
					bandwidth_x += (((radius.x - abs(v_pos.x-NowCenter.x))/radius.x)+1)*pdf_m;
					bandwidth_y += (((radius.y - abs(v_pos.y-NowCenter.y))/radius.y)+1)*pdf_m;
					bandwidth_d = denominator;

					// calculate the central moment
pix_wei = pdf_m;
					m00+= pix_wei;
					m10+= (j*pix_wei); //first x moment
					m01+= (k*pix_wei); //first y moment
					m11+= (k*j*pix_wei);
					m20+= (j*j*pix_wei);
					m02+= (k*k*pix_wei);

					u11+= (k-NowCenter.y)*(j-NowCenter.x)*pix_wei;
					u20+= (j-NowCenter.x)*(j-NowCenter.x)*pix_wei;
					u02+= (k-NowCenter.y)*(k-NowCenter.y)*pix_wei;
				}

				if (temp3 < (128 - s_person[pindex].lG_SD))
					falsealarm++;
				else
					hit++;
			}

			// get the weight of the scanline 
			band_wi = static_cast<float>(kernel(NowCenter, v_pos, net));
			pix_wei = band_wi * pix_wei;
			bandwidth_s	+= pix_wei;

			if (NowCenter.x == v_pos.x) sum_by += pix_wei; 
			if (NowCenter.y == v_pos.y) sum_bx += pix_wei;

// calculate the confidence	
			if (temp3 > (128 + s_person[pindex].uG_SD))
					missdetection++;
			}
		}
	}

	// calculate the center position
	if(denominator!=0.0){
		pf_y1.x=numerator.x/denominator;
		pf_y1.y=numerator.y/denominator;
	}else{
		return ;
	}

// update and evaluate at new position y1
	NewCenter.x = pf_y1.x;
	NewCenter.y = pf_y1.y;

// assign the new window
window.x = static_cast<int>(NewCenter.x - radius.x);	
window.y = static_cast<int>(NewCenter.y - radius.y);
CheckWindow(window);

// calculate the distance(criterion)
    calcProb( bgbackpro, window, candProb);
    float bhat1 = calcBhatCoef(s_person[pindex].hist[WEI_HIST], candProb);

    float lastBhat = 0;
    while ((bhat1 < bhat0) && ( fabs(lastBhat - bhat1) > 0.01)) {
      pf_y1.x=static_cast<float>(0.5f*(OrgCenter.x + pf_y1.x));
      pf_y1.y=static_cast<float>(0.5f*(OrgCenter.y + pf_y1.y));
	  
      NewCenter.x=static_cast<float>(pf_y1.x);
      NewCenter.y=static_cast<float>(pf_y1.y);

	  // assign the new window
	  window.x = static_cast<int>(NewCenter.x - radius.x);	
	  window.y = static_cast<int>(NewCenter.y - radius.y);
	  CheckWindow(window);
	
      calcProb(bgbackpro, window ,candProb);
      lastBhat = bhat1;
      bhat1 = calcBhatCoef(s_person[pindex].hist[WEI_HIST], candProb);
    }

     lastDist = dist;
	 dist = static_cast<float>(sqrt(pow((pf_y1.x - OrgCenter.x),2) + pow((pf_y1.y - OrgCenter.y),2)));

	 it_counter++;
	} while((dist >= criterion) && (fabs(lastDist-dist) > 0.3));
	// end of tracking
	

// set new window  -------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// store the information

	// store the center point
	s_person[pindex].central.x = static_cast<int>(NewCenter.x);
	s_person[pindex].central.y = static_cast<int>(NewCenter.y);

/*
fout1 << pos << "  " <<  s_person[pindex].central.x << "  " << s_person[pindex].central.y << endl;
fout3 << pos << "  " <<  it_counter << endl;
fout2 << pos << "  " <<  falsealarm << "  " << missdetection<< " " << hit << endl;
*/

	// Calculating orientation
	int cx = s_person[pindex].central.x; int cy = s_person[pindex].central.y;
	
	double temp1 = 2*((m11/m00)- (cx*cy));
	double temp2 = ((m20/m00) - (cx*cx)) - ((m02/m00) - (cy*cy));
	double theta= atan(temp1/temp2)/2;
	theta += CV_PI/2;
	s_person[pindex].direction = -theta;

	double  aa = (m20/m00) - (cx*cx);
	double  bb = 2*(m11/m00 - cx*cy);
	double  cc = (m02/m00) - (cy*cy);
	double  dd = sqrt(bb*bb + (aa-cc)*(aa-cc));
	double  ll = sqrt(abs(( aa + cc + dd)/2));
	double  ww = sqrt(abs(( aa + cc - dd)/2));

	// store the confidence
	all_fore = static_cast<float>(window.width * window.height);
	all_sour = search_window.width * search_window.height - all_fore;

	s_person[pindex].confidence =  falsealarm / all_sour + missdetection/all_fore;



// enlarge or narrow 10%
	CvRect enlarge_win, narrow_win;
	CvHistogram *ori_hist,*en_hist, *na_hist;

	enlarge_win.width	= window.width *1.1;
	enlarge_win.height	= window.height*1.1;
	enlarge_win.x = window.x - window.width/20;
	enlarge_win.y = window.y - window.height/20;

	narrow_win.width	= window.width *0.9;
	narrow_win.height   = window.height*0.9;
	narrow_win.x = window.x + window.width/20;
	narrow_win.y = window.y + window.height/20;

	cvSetImageROI( hue, enlarge_win);
	en_hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
	cvCalcHist( &hue, en_hist, 0, 0);
	cvResetImageROI( hue );

	double bhat_en = calcBhatCoef(s_person[pindex].hist[FORE_OLD_HIST], en_hist);

	cvSetImageROI( hue, narrow_win);
	na_hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
	cvCalcHist( &hue, na_hist, 0, 0);
	cvResetImageROI( hue );

	double bhat_na = calcBhatCoef(s_person[pindex].hist[FORE_OLD_HIST], na_hist);

	cvSetImageROI( hue, window);
	ori_hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
	cvCalcHist( &hue, ori_hist, 0, 0);
	cvResetImageROI( hue );

	double bhat_ori = calcBhatCoef(s_person[pindex].hist[FORE_OLD_HIST], ori_hist);
	
// store the window
	if (max(bhat_en,bhat_na) >  bhat_ori){
		if (bhat_en > bhat_na)
			s_person[pindex].Obj.rect = enlarge_win;
		else
			s_person[pindex].Obj.rect = narrow_win;
	}else
		s_person[pindex].Obj.rect = window;

	CheckWindow(s_person[pindex].Obj.rect);
	
	// store the search window
	CvBox2D swbox;
	s_person[pindex].search_window.x = cvRound((double)NewCenter.x - s_person[pindex].Obj.rect.width);
	s_person[pindex].search_window.y = cvRound((double)NewCenter.y - s_person[pindex].Obj.rect.height);
	s_person[pindex].search_window.width = s_person[pindex].Obj.rect.width *2;
	s_person[pindex].search_window.height = s_person[pindex].Obj.rect.height *2;

	swbox.size.height = s_person[pindex].search_window.height;
	swbox.size.width  = s_person[pindex].search_window.width;
	swbox.angle = (float)s_person[pindex].direction;
	swbox.center.x = (float)NewCenter.x;
	swbox.center.y = (float)NewCenter.y;

	// store the object ellipse
	s_person[pindex].track_obj.size.height = (float)s_person[pindex].Obj.rect.height;
	s_person[pindex].track_obj.size.width = (float)s_person[pindex].Obj.rect.width;
	s_person[pindex].track_obj.angle = (float)s_person[pindex].direction;
	s_person[pindex].track_obj.center.x = (float)NewCenter.x;
	s_person[pindex].track_obj.center.y = (float)NewCenter.y;

	//////////////////////////////////////////////////////////////////////////
	// show the tracking results

	// to draw the center point
	cvCircle( frame, s_person[pindex].central, 3, CV_RGB(255,255,0));

	// to draw the main axis
	CvPoint p;
	// main axis
	p.x = (int)(ll * cos(theta) + cx); p.y = (int)(ll * sin(theta)+cy);
	cvLine(frame,s_person[pindex].central,p,CV_RGB(0,255,0));
	p.x = (int)(-ll * cos(theta) + cx); p.y = (int)(-ll * sin(theta)+cy);
	cvLine(frame,s_person[pindex].central,p,CV_RGB(0,255,0));

	// second axis
	p.x = (int)(ww * cos(theta-CV_PI/2) + cx); p.y = (int)(ww * sin(theta-CV_PI/2)+cy);
	cvLine(frame,s_person[pindex].central,p,CV_RGB(0,0,255));
	p.x = (int)(-ww * cos(theta-CV_PI/2) + cx); p.y = (int)(-ww * sin(theta-CV_PI/2)+cy);
	cvLine(frame,s_person[pindex].central,p,CV_RGB(0,0,255));


	// to draw the object boundary
    cvEllipseBox( frame, s_person[pindex].track_obj, CV_RGB(0,255,0), 1, CV_AA, 0 );
	cvEllipseBox( frame, swbox, CV_RGB(255,0,0), 2, CV_AA, 0 );


	CvSize axes;
    axes.width = cvRound(0.5*(swbox.size.width));
    axes.height = cvRound(0.5*(swbox.size.height));
	
	CvPoint v[1 << 8];

    int delta = (MAX(axes.width,axes.height)+(XY_ONE>>1))>>XY_SHIFT;
    delta = delta < 3 ? 90 : delta < 10 ? 30 : delta < 15 ? 18 : 5;
	
	int angle = swbox.angle*180;
	Ellipse2Poly(pindex, s_person[pindex].central, axes, angle, 0, 360, v, delta );
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/*
 *		goodness function 
 */
double	CamShift::Cost(int pindex, int _color, int _dist, int _length, int x, int y){

	double Jx,Zx;
	double color = _color;
	double dist = _dist;
	double length = _length;
	
	Jx = ((double)color / COLOR_RANGE);
	Zx = ((double)dist / length);

	int j, k, sum = 0;
	double homogen=1;
	if (x != -1 && y != -1){
	
		for ( j = -MATCH_WINDOW/2; j <= MATCH_WINDOW/2; j++)
		{
			for ( k = -MATCH_WINDOW/2; k <= MATCH_WINDOW/2; k++)
			{
				unsigned char tmpVal= abs((unsigned char)hue->imageData[y+j*hue->width + x+k]);
	
				tmpVal = (int)(((double)tmpVal)/COLOR_RANGE * MAX_HISTOGRAM);
				tmpVal = cvQueryHistValue_1D(s_person[pindex].hist[ADJ_HIST], ((int)tmpVal));
		
				if (tmpVal < 128 - s_person[pindex].lG_SD)
					sum = sum - 1;
				if (tmpVal > 128 + s_person[pindex].uG_SD)
					sum = sum + 1;
			}
		}
		homogen = (1/(1+exp((float)-sum)));
		return ( 0.05*(1-Zx) +  0.9*Jx + 0.05*homogen);
	}
	//return ( 0.1*(1-Zx) +  0.9*Jx);
	return ( 0.05*(1-Zx) +  0.9*Jx + 0.05*homogen);
}

/*
 *		sampling line
 */
void CamShift::sample_line_pixels(int pindex, int angle, CvPoint pt1){

	CvLineIterator iter_pdf;
	int length_pdf = cvInitLineIterator( hue, s_person[pindex].central, pt1, &iter_pdf, 8 );

	CvLineIterator iter_ppp;
	int length_ppp = cvInitLineIterator( bgbackpro, s_person[pindex].central, pt1, &iter_ppp, 8 );

	CvPoint tpos;
	int		i, counter_f = 0, counter_b = 0, offset;
	int		diffcolor = 0, maxdiff = 0;
	uchar	tcolor;

	double fcost_v, tv;
	double fcost_l = Cost(pindex,128 - s_person[pindex].lG_SD, length_pdf/2, length_pdf,-1,-1);
	double fcost_u = Cost(pindex,128 + s_person[pindex].uG_SD, length_pdf/2, length_pdf,-1,-1);

	s_person[pindex].Lmaxdiff[angle] = 0;

	//sampling line //
	for(i = 0; i < length_pdf; i++ ){
		
		/* assume that ROI is not set, otherwise need to take it into account. */
        offset = iter_pdf.ptr - (uchar*)(hue->imageData);
        tpos.y = offset/hue->widthStep;
        tpos.x = (offset - tpos.y*hue->widthStep)/(1*sizeof(uchar) /* size of pixel */);
   
		// query color probability
		fcost_v = (int)(((double)iter_pdf.ptr[0])/COLOR_RANGE * MAX_HISTOGRAM);
		fcost_v = cvQueryHistValue_1D(s_person[pindex].hist[ADJ_HIST], fcost_v);
		tv = fcost_v;

		if (iter_ppp.ptr[0]>20)
			img_gray->imageData[tpos.y * img_gray->width + tpos.x] = iter_ppp.ptr[0];

		// record the color and pos information
		fcost_v = Cost(pindex,fcost_v, i, length_pdf, tpos.x, tpos.y);

		if (fcost_v >= fcost_u)
			mask->imageData[tpos.y * mask->width + tpos.x] = 255;

		if (fcost_v > fcost_l && fcost_v < fcost_u)
			distimg->imageData[tpos.y * distimg->width + tpos.x] = fcost_v*255;
		
		if (counter_f==0)
			diffcolor = 0;
		else
		diffcolor = abs(iter_pdf.ptr[0] - tcolor);
		
		tcolor = iter_pdf.ptr[0];

		if (diffcolor >= maxdiff){
			maxdiff = diffcolor;
			s_person[pindex].Lmaxdiff[angle] = i;
		}
		s_person[pindex].shape[angle][i].g_turn = true;
		s_person[pindex].shape[angle][i].pos = tpos;
		s_person[pindex].shape[angle][i].diff= diffcolor;
		s_person[pindex].shape[angle][i].pdf = fcost_v;
		
		counter_f++;
		

		if (tv > 128 + s_person[pindex].uG_SD)
			img_gray->imageData[tpos.y * img_gray->width + tpos.x] = tv;
	

		CV_NEXT_LINE_POINT(iter_pdf);
		CV_NEXT_LINE_POINT(iter_ppp);
	}
/*
	cvLine( frame,s_person[pindex].shape[angle][s_person[pindex].Lmaxdiff[angle]].pos,\
		s_person[pindex].central,CV_RGB(128,128,0));
*/
}


/*
 *		connected component labeling
 */
int CamShift::ConnectedComponentLableing(int pindex){

	int j;


// closing ------------------------------------------------------------------
	cvErode(mask,mask,NULL,CLOSING);
	cvDilate(mask,mask,NULL,CLOSING);

// get the contour ----------------------------------------------------------
	CvMemStorage	*storage = cvCreateMemStorage(0);
	CvSeq			*ContourSeq;
	CvSeq			*ContourSeq_arr[20];
	int				blob_counter = 0;
	double			vol[20][2] = {0};
	
	// Start to find the contours
	CvContourScanner cs = cvStartFindContours(mask, storage, sizeof(CvContour), CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);

	do{
		// Get the next contour
		ContourSeq = cvFindNextContour(cs);

		if (ContourSeq != NULL){
			// calculates contour area
			double Area = fabs(cvContourArea(ContourSeq, CV_WHOLE_SEQ)); 

			// the Contour are must be bigger than the blob size
			if( Area > LOWER_BOUNDING && Area < UPPER_BOUNDING){
				ContourSeq_arr[blob_counter] = ContourSeq;
				vol[blob_counter][0] = Area;
		
//cvDrawContours(mask, ContourSeq_arr[blob_counter], CV_RGB( 255, 255, 255), CV_RGB( 255, 255, 255), 0, 1, 8);
	
				blob_counter++;
			}
		}
	}while (ContourSeq != NULL);

	// find the max contour
	int flag = 0;
	double temp1=9999999;
	double obj_size = s_person[pindex].Obj.rect.width * s_person[pindex].Obj.rect.height;
	for ( j=0; j < blob_counter; j++){
		vol[j][1] = fabs(vol[j][0]-obj_size);
		if (vol[j][1] < temp1){
			flag = j;
			temp1 = vol[j][1];
		}
	}

	// draw the contour
	if (vol[flag][0] != 0){
cvDrawContours(mask, ContourSeq_arr[flag], CV_RGB( 255, 255, 255), CV_RGB( 255, 255, 255), 0, 1, 8);


		for (j=0; j < ContourSeq_arr[flag]->total; j++){

			CvPoint t =  *(CvPoint*) cvGetSeqElem(ContourSeq_arr[flag], j);
			double	dist = Norm2D(s_person[pindex].central,t);
			float xc1 = (float)(t.x - s_person[pindex].central.x);
			float yc1 = (float)(t.y - s_person[pindex].central.y);

			int deg = cvRound(cvFastArctan(yc1, xc1));

			s_person[pindex].last_shape_inf[deg] = s_person[pindex].shape_inf[deg];
			s_person[pindex].shape_inf[deg] = dist;
		}
/*
CvBox2D box = cvMinAreaRect2(ContourSeq_arr[flag],storage);
s_person[pindex].Obj.rect.x = box.center.x - box.size.width/2;
s_person[pindex].Obj.rect.y = box.center.y - box.size.height/2;
s_person[pindex].Obj.rect.width  = box.size.width;
s_person[pindex].Obj.rect.height = box.size.height;
cvEllipseBox( frame, box, CV_RGB(0,0,255), 1, CV_AA, 0 );
*/
/*
  CvPoint2D32f cen;
  float rad;
cvMinEnclosingCircle(ContourSeq_arr[flag], &cen,&rad);
  cvCircle(frame,cvPointFrom32f(cen),rad,CV_RGB(255,255,255));
*/
	}

	// Release memory allocate
	cvClearMemStorage( storage );

	return blob_counter;
}

/*
 *	check if the object is rotate ------------
 */
bool CamShift::ContourAlignment(int pindex){
	
	int		j;
	float	sum_new=0, sum_old=0, simi=0, energy=0;
	float	s1,s2;
	float	bucket[SCAN_LINE];
	double  avg_energy_diff = 0;

	// normalize
	for (j=0; j < SCAN_LINE; j++){
		s1 = s_person[pindex].shape_inf[j];	s2 = s_person[pindex].last_shape_inf[j];
		sum_new +=s1; sum_old +=s2;
	}

	// calc. the similarly & dissimilarly interval section
	for ( j=0; j < SCAN_LINE; j++){
		s1 = s_person[pindex].shape_inf[j];	s2 = s_person[pindex].last_shape_inf[j];
		s1 = s1/ sum_new; s2 = s2/ sum_old;
		simi += sqrt(s1*s2);

		energy += 2*((s1*s2) / (s1+s2));
		bucket[j] = energy;
	}

//	if (simi < 0.8){
		bool turn = false;
		int rec= 0, st = 0, end = 0, counter = 0;
		const int thr_edge_range = 20;
		double thr_edge = energy / SCAN_LINE; 
		for ( j=1; j < SCAN_LINE; j++){
			bucket[j-1] = bucket[j] - bucket[j-1];

			if (bucket[j-1] > thr_edge/2)
				counter++;
			else{
				if (counter > thr_edge_range){
					s_person[pindex].gray_pair[rec].x = j-counter; 
					s_person[pindex].gray_pair[rec].y = j;
				

	CvPoint p;
	float theta1 = (float)(s_person[pindex].gray_pair[rec].x*CV_PI)/180.0f;
	p.x = (int)(30 * cos(theta1));
	p.y = (int)(30 * sin(theta1));
	p.x += s_person[pindex].central.x;
	p.y += s_person[pindex].central.y;
	cvCircle( image, p, 2, CV_RGB(0,0,255));
	cvLine( image,s_person[pindex].central,p,CV_RGB(0,0,255));


	float theta2 = (float)(s_person[pindex].gray_pair[rec].y*CV_PI)/180.0f;
	p.x = (int)(30 * cos(theta2));
	p.y = (int)(30 * sin(theta2));
	p.x += s_person[pindex].central.x;
	p.y += s_person[pindex].central.y;
	cvCircle( image, p, 2, CV_RGB(0,0,0));
	cvLine(image,s_person[pindex].central,p,CV_RGB(255,0,0));

				rec++;
				counter = 0;
				}else{
					counter = 0;
				}
			}
		}

		if (rec > 0){
			s_person[pindex].numofpair = rec;
			return true;
		}
//	}

	return false;
}
bool CamShift::ShapeMap(int pindex){

	CvPoint p;
	
	for (int cnt = 0 ;cnt < 360; cnt++){
		// convert angle to radian
		float theta = (float)(cnt*CV_PI)/180.0f;

		int	radius = Norm2D(s_person[pindex].central, s_person[pindex].boundary[cnt]);

		p.x = (int)(radius * cos(theta));
		p.y = (int)(radius * sin(theta));

		p.x += s_person[pindex].central.x;
		p.y += s_person[pindex].central.y;

		if (p.x < 0) p.x =0;
		if (p.y < 0) p.y =0;
		if (p.x > image->width-1)	p.x =image->width-1;
		if (p.y > image->height-1)  p.y =image->height-1;

		sample_line_pixels(pindex, cnt , p);
	}

	if (ConnectedComponentLableing(pindex)){
		if (ContourAlignment(pindex))
			return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/*
 *		sampling line to generate the object's shape
 */
void CamShift::GrayRegion(int pindex){

	int angle,j,k;
	int rec = s_person[pindex].numofpair;
	do {
		for ( angle=s_person[pindex].gray_pair[rec].x ; angle < s_person[pindex].gray_pair[rec].y; angle++){

			// convert angle to radian
			float theta = (float)(angle*CV_PI)/180.0f;
			int	radius = Norm2D(s_person[pindex].central, s_person[pindex].boundary[angle]);

			// find the best break point
			int newpos = 0;
			double newpos_v = 0,temp1,temp2,temp3;
			for ( j =0; j< radius; j++){
				if (s_person[pindex].shape[angle][j].flag && 
					s_person[pindex].shape[angle][j].g_turn){
					// 愈小愈好
					temp1 = double(abs(j - s_person[pindex].last_shape_inf[angle]))/radius;
					temp2 = 1.0 - double(s_person[pindex].shape[angle][j].diff)/MAX_HISTOGRAM;;
					temp3 = 0.5*temp1 + 0.5*temp2;
					if (temp3 < newpos_v){
						newpos_v = temp3;
						newpos = j;
					}
				}
			}

			// store the gray element
			for ( k =0; k< newpos /*s_person[pindex].Lmaxdiff[angle]*/; k++){

				if (s_person[pindex].shape[angle][k].g_turn){
				
					CvPoint tpos = s_person[pindex].shape[angle][k].pos;
					if (tpos.x < 0) tpos.x =0;
					if (tpos.y < 0) tpos.y =0;
					if (tpos.x > hue->width-1)	 tpos.x =hue->width-1;
					if (tpos.y > hue->height-1)  tpos.y =hue->height-1;

					double col_pix = hue->imageData[tpos.y * hue->width + tpos.x];
			
					int	   nor_vcol = abs(cvRound((col_pix / COLOR_RANGE * MAX_HISTOGRAM)));

					double tmp1 = cvQueryHistValue_1D(s_person[pindex].hist[GRAY_HIST], nor_vcol);
					cvSetReal1D(s_person[pindex].hist[GRAY_HIST]->bins, nor_vcol, ++tmp1);

				}
			}

cvLine( image,s_person[pindex].shape[angle][newpos].pos,\
	s_person[pindex].central,CV_RGB(128,128,0));

		}
		rec--;
	} while(rec>=0);
}




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int		CamShift::fn_isempty( pSt_queue queue ){
	return ( queue == NULL );
}

void	CamShift::fn_insert( pSt_queue *frontNode, pSt_queue *rearNode, int *num ){
	pSt_queue newNode;
	
	newNode = new St_queue();
	
	for (int i=0 ; i < MAX_HISTOGRAM; i++)
		newNode->gray_var[i] = num[i];

	newNode->NextPtr = NULL;
	
	if( fn_isempty( *frontNode ) )//第一次 insert 時 
		*frontNode = newNode;
	else
		( *rearNode )->NextPtr = newNode;
	
	*rearNode = newNode;
}

int*  CamShift::fn_remove( pSt_queue *frontNode, pSt_queue *rearNode ){
	int num[MAX_HISTOGRAM];
	pSt_queue temp;
	
	if( fn_isempty( *frontNode ) )
	{
		printf( "\nThe queue is empty!\n\n" );
		return 0;
	}
	temp = *frontNode;


	for (int i=0 ; i < MAX_HISTOGRAM; i++)
		num[i] = ( *frontNode )->gray_var[i];
	
	*frontNode = ( *frontNode )->NextPtr;
	free( temp );
	if( fn_isempty( *frontNode ) )//若 queue 中元件已拿完 
		*rearNode = NULL;

	return num;
}

void	CamShift::Estimate_Gray_Element(int pindex){
	
/*
	for( ; !fn_isempty( s_person[pindex].frontNode ) ; s_person[pindex].frontNode = s_person[pindex].frontNode->NextPtr ){
		printf( "NULL\n\n" );
		for (int i=0 ; i < MAX_HISTOGRAM; i++)
			printf( "%-3d ==> ", s_person[pindex].frontNode->gray_var[i] );
	}
	printf( "NULL\n\n" );*/

	int counter, j, k;
/*	
	pSt_queue showNode = s_person[pindex].frontNode;

	counter = 1;
	for( ; !fn_isempty( showNode ) ; showNode = showNode->NextPtr ){
		printf( "\n\n" );
		printf( "%-3d ===>" ,counter);
		printf( "\n\n" );
		for (int i=0 ; i < MAX_HISTOGRAM; i++)
			printf( "%-3d ==> ",  showNode->gray_var[i] );
		counter++;
	}
	printf( "\n\n" );
*/

	

	double grow_var[MAX_HISTOGRAM][TIME_INT];
	memset(grow_var,0,sizeof(grow_var));


	int gary_num[MAX_HISTOGRAM];
	for (k=0; k < MAX_HISTOGRAM; k++)
		gary_num[k] = cvQueryHistValue_1D(s_person[pindex].hist[GRAY_HIST], k);
	
	// remove the old data
	fn_remove(&s_person[pindex].frontNode, &s_person[pindex].rearNode);
	// record the gray data
	fn_insert( &s_person[pindex].frontNode, &s_person[pindex].rearNode, gary_num );
	
	for ( k=0 ; k < MAX_HISTOGRAM; k++){
		pSt_queue runNode = s_person[pindex].frontNode;

		counter = 0;
		for( ; !fn_isempty( runNode ) ; runNode = runNode->NextPtr ){
			pSt_queue tmpNode = runNode->NextPtr;

			if (!fn_isempty( tmpNode ))
				grow_var[k][counter] =  tmpNode->gray_var[k] - runNode->gray_var[k];
			
			counter++;
		}
	
		// 最後一次不用加
		double temp = 0;
		for ( j=TIME_INT-2 ; j >= 0; j--){
			temp += grow_var[k][j];
			grow_var[k][j] = temp;

			grow_var[k][j] /= (TIME_INT - (j+1));
		}	
	}
	
	double grow_ratio[MAX_HISTOGRAM];
	memset(grow_ratio,0,sizeof(grow_ratio));

	for ( k=0 ; k < MAX_HISTOGRAM; k++){
		for ( j=0 ; j < TIME_INT-1; j++){
			if (grow_var[k][j]==0)
				grow_ratio[k] += 0;
			else
				grow_ratio[k] += (grow_var[k][j+1]/grow_var[k][j]);
		}
		grow_ratio[k]/= (TIME_INT-2);

		// the average grow ratio is smaller than 1, to modify the gray element value
		if (grow_ratio[k] <= 1 && grow_ratio[k] >= 0){
			double tmp1 = cvQueryHistValue_1D(s_person[pindex].hist[GRAY_HIST], k);
			tmp1 *= grow_ratio[k];
			cvSetReal1D(s_person[pindex].hist[GRAY_HIST]->bins, k, tmp1);
			gary_num[k] = tmp1 * grow_ratio[k];
		}
	}
//	printf( "NULL\n\n" );

	update_foreground = 1;
}

/*
 *		to tracking the objects
 */
void CamShift::Track(){

	int i=0;
	if( track_object ){
		GrayElement(i);
		MeanShift(i);
		if (ShapeMap(i))
			GrayRegion(i);
		Estimate_Gray_Element(i);
	}
}// end of Track




void CamShift::Run()
{
	cvUseOptimized(1);
	// Get the Device
	//capture = cvCaptureFromCAM(0);
    //capture = cvCaptureFromAVI( "C:\\物轉01_1.avi");
	capture = cvCaptureFromAVI( "C:\\test1.avi");

	//cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 30);
    if( !capture ){
        fprintf(stderr,"Could not initialize capturing...\n");
    }
	printf( "Hot keys: \n"
        "\tESC - quit the program\n"
        "\tc - stop the tracking\n"
        "\tb - switch to/from backprojection view\n");

	cvSetMouseCallback( "Demo", input_static_callback ); // on_mouse 自定?事件
/* 
// file process
fout1.open("D:\\data3\\center_ELLRG.txt", ios_base::app);
if (! fout1.is_open())
	fprintf(stderr,"Error: Could not open file for appending. \n");
// file process
fout2.open("D:\\data3\\misfal_ELLRG.txt", ios_base::app);
if (! fout2.is_open())
	fprintf(stderr,"Error: Could not open file for appending. \n");
// file process
fout3.open("D:\\data3\\it_ELLRG.txt", ios_base::app);
if (! fout3.is_open())
	fprintf(stderr,"Error: Could not open file for appending. \n");
*/

//	CvVideoWriter* camWriter1 = 0;
//	camWriter1 = cvCreateVideoWriter("c:/video1.avi",-1,15.0, cvSize(360*3,240*2));

    for(;;){

		// Grab Frame
		if (!GrabFrame())	break;
		
		// to track the objects
		Track();

		// to show the information
		if (!ShowInfo(0))
			break;

//		cvSaveImage("img_wei1.jpg", img_gray);
//		cvSaveImage("img_hist.jpg", histimg);
//		cvSaveImage("img_bp.jpg", img_gray);
//		cvSaveImage("img_mask.jpg", mask);
//		cvSaveImage("img_frame.jpg", frame);
//		cvSaveImage("img_bgp.jpg", bgbackpro);

		/*

		int k=0;
		IplImage* IMG1,*IMG2,*IMG3;

		IMG1 = cvCreateImage( cvGetSize(frame), 8, 3 );
        IMG1->origin = frame->origin;
		IMG2 = cvCreateImage( cvGetSize(frame), 8, 3 );
        IMG2->origin = frame->origin;
		IMG3 = cvCreateImage( cvGetSize(frame), 8, 3 );
        IMG3->origin = frame->origin;

		cvCvtColor(bgbackpro,IMG1, CV_GRAY2BGR);
		cvCvtColor(mask,IMG2, CV_GRAY2BGR);
		cvCvtColor(img_gray,IMG3, CV_GRAY2BGR);

		for (int i=0; i < frame->height; i++){
			for (int j=0; j < 360; j++){

			
				if (j <= 351){
					cvSet2D(BigMap,i,j,cvGet2D(frame,i,j));
					cvSet2D(BigMap,i,j+360,cvGet2D(IMG1,i,j));
					cvSet2D(BigMap,i+240,j,cvGet2D(image,i,j));
					cvSet2D(BigMap,i+240,j+360,cvGet2D(IMG2,i,j));
					cvSet2D(BigMap,i+240,j+720,cvGet2D(IMG3,i,j));
				}else
					cvSet2D(BigMap,i,j,CV_RGB(0,0,0));
				

				cvSet2D(BigMap,i,j+720,cvGet2D(histimg,i,j));
			}
		}
		*/
//cvWriteFrame(camWriter1, BigMap);
		//if (pos == 1 || pos == 2 || pos == 17 || pos == 32 || pos == 63 || pos == 93 || pos == 104 || pos == 117 || pos == 210 || pos == 240 || pos == 288) // 移動鏡頭
		if (pos ==2 /*|| pos == 5|| pos ==90 || pos ==100 || pos ==30*/)
			cvWaitKey(0);

		pos++;
    }
/*
fout1.close();
fout2.close();
fout3.close();*/
	
//cvReleaseVideoWriter(&camWriter1);

	cvReleaseCapture( &capture );

}

void main( ){
   CamShift* track = new CamShift();
   track->Run();
}
