/*
 *	author : Yong-Yuan Line          date : 23/07/2005
 *  title : face-based detector
 *  descript :
 */

#ifndef _ObjectTracking
#define _ObjectTracking

class CamShift
{

public:

		void	GrayElement(int pindex);


		double	Cost(int pindex, int color, int dist, int length, int x, int y);
		void	sample_line_pixels(int pindex,int angle, CvPoint pt1);
		int		ConnectedComponentLableing(int pindex);
		bool    ContourAlignment(int pindex);
		bool	ShapeMap(int pindex);

		void	GrayRegion(int pindex);

		bool	ShowInfo(int pindex);
		void	ShowHist(int pindex);

		CvScalar hsv2rgb( float hue );
		void	SinCos( int angle, float *cosval, float *sinval );
		void	Ellipse2Poly(int pindex, CvPoint center, CvSize axes, int angle,int arc_start, int arc_end, CvPoint* pts, int delta );
		double	Norm2D(CvPoint s,CvPoint d);

		bool	GrabFrame();
		void	Track();
		void	Run();
		void	input_mouse_callback(int event, int x, int y);


		CamShift();
		~CamShift();
private:
    
	// file process
	ofstream fout1;
	ofstream fout2;
	ofstream fout3;
	ofstream fout4;
	// image
	CvCapture	*capture;
    IplImage	*frame;
	IplImage	*image, *hsv, *hue, *mask, *bgbackpro, *histimg, *distimg, *img_gray;

	IplImage	*BigMap;
	// info
	int pos, start;
	int source;
	int bgbackproject_mode;

	// detection
	Blob				*s_person;		// # of bounding box in a frame
	int					numofblob;		// # of people in a frame
	
	//	tracking
	CvPoint				origin;
	CvRect				selection;

	// meanshift
	int					hdims;
	float*				hranges;
	int					track_object;
	int					select_object;
	int					update_foreground;
	int					vmin, vmax, smin;

// mean-shift 
public:
	void			CheckWindow(CvRect &window);
	CvPoint2D32f	GetWinCenter(CvRect &window);
	float			calcBhatCoef(CvHistogram *targetProb, CvHistogram *candProb);
	double			kernel(CvPoint2D32f center,CvPoint2D32f pos, CvPoint2D32f radius);
	double			derivateKernel(CvPoint2D32f center,CvPoint2D32f pos, CvPoint2D32f radius);
	void			calcProb(IplImage *img_src,CvRect &window,CvHistogram *prob);
	float			calcDistance(CvHistogram *targetProb, CvHistogram *candProb);
	void			MeanShift(int pindex);

private:
	CvPoint2D32f	pf_y1;			// final center position y1
	int				e_kernelType;

// update the gray element
public:
		int		fn_isempty( pSt_queue );
		void	fn_insert( pSt_queue *, pSt_queue *, int *);
		int*	fn_remove( pSt_queue *, pSt_queue * );
		void	Estimate_Gray_Element(int pindex);
};

#endif