

#include "stdafx.h"
#define CV_NO_BACKWARD_COMPATIBILITY

#ifndef _EiC
#include "cv.h"
#include "highgui.h"
#include "cvaux.h"
#include <stdio.h>
#include <ctype.h>
#endif

IplImage *image = 0, *grey = 0, *prev_grey = 0, *pyramid = 0, *prev_pyramid = 0, *swap_temp, *mask;

int win_size = 3;
const int MAX_COUNT = 10000;
CvPoint2D32f* points[2] = {0,0}, *swap_points;
char* status = 0;
int count = 0;
int need_to_init = 1;
int night_mode = 0;
int flags = 0;
int add_remove_pt = 0;
CvPoint pt,start;
CvPoint2D32f* path[MAX_COUNT]={0};
CvPoint2D32f* pointsnew[1]={0};
CvPoint2D32f* vj[1]={0};
CvPoint2D32f* mj[1]={0};
CvPoint2D32f* dj[1]={0};


void on_mouse( int event, int x, int y, int flags, void* param )
{
    if( !image )
        return;

    if( image->origin )
        y = image->height - y;

    if( event == CV_EVENT_LBUTTONDOWN )
    {
        pt = cvPoint(x,y);
        add_remove_pt = 1;
    }
}


int main( int argc, char** argv )
{
    CvCapture* capture = 0;
	int x=0;
	int v=0;
	FILE *out;
	int fcount=0;
	int i,j,k,c,z=0;
	int vel=0,m=0,d=0;
	int fno=0;
	
    /*if( argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))
        capture = cvCaptureFromCAM( argc == 2 ? argv[1][0] - '0' : 0 );
    else if( argc == 2 )
        capture = cvCaptureFromAVI( argv[1] );*/
	//capture = cvCaptureFromAVI( "test1.avi" );
	//capture = cvCaptureFromFile( "flow1.mov" );
	//capture = cvCaptureFromFile( "Crowdan.mp4" );
	capture = cvCaptureFromFile( "v1_n.avi" );
    if( !capture )
    {
        fprintf(stderr,"Could not initialize capturing...\n");
        return -1;
    }

    /* print a welcome message, and the OpenCV version */
    printf ("Welcome to lkdemo, using OpenCV version %s (%d.%d.%d)\n",
	    CV_VERSION,
	    CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION);

    printf( "Hot keys: \n"
            "\tESC - quit the program\n"
            "\tr - auto-initialize tracking\n"
            "\tc - delete all the points\n"
            "\tn - switch the \"night\" mode on/off\n"
            "To add/remove a feature point click it\n" );

    cvNamedWindow( "LkDemo", 0 );
    cvSetMouseCallback( "LkDemo", on_mouse, 0 );
	/*int y=0;*/
	IplImage* frame = 0;
        

        frame = cvQueryFrame( capture );
		//printf("%d\n",fno++);
		if( !image )
        {
            /* allocate all the buffers */
            image = cvCreateImage( cvGetSize(frame), 8, 3 );
            image->origin = frame->origin;
            grey = cvCreateImage( cvGetSize(frame), 8, 1 );
			mask = cvCreateImage( cvGetSize(frame), 8, 1 );    //mask
            prev_grey = cvCreateImage( cvGetSize(frame), 8, 1 );
            pyramid = cvCreateImage( cvGetSize(frame), 8, 1 );
            prev_pyramid = cvCreateImage( cvGetSize(frame), 8, 1 );
            points[0] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));
            points[1] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));
			pointsnew[0] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(pointsnew[0][0]));
			vj[0] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(vj[0][0]));
			mj[0] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(mj[0][0]));
			dj[0] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(dj[0][0]));
			for(int i=0;i<1000;i++)
			{
				path[i] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(path[0][0]));
				

			}
	        status = (char*)cvAlloc(MAX_COUNT);
            flags = 0;
        }
///Initializing vectors to zero
			for(j=0;j<MAX_COUNT;j++)
			{
				vj[0][j].x=0,vj[0][j].y=0;
				mj[0][j].x=0,mj[0][j].y=0;
				dj[0][j].x=0,dj[0][j].y=0;
			}
		cvCopy( frame, image, 0 );
        cvCvtColor( image, grey, CV_BGR2GRAY );	
		cvCopy(grey,prev_grey,0);
		IplImage* eig = cvCreateImage( cvGetSize(grey), 32, 1 );
            IplImage* temp = cvCreateImage( cvGetSize(grey), 32, 1 );
            double quality = 0.01;
            double min_distance = 10;

            count = MAX_COUNT;
		
            cvGoodFeaturesToTrack( grey, eig, temp, points[0], &count,quality, min_distance, 0, 3, 0, 0.04 );
            cvFindCornerSubPix( grey, points[0], count,cvSize(win_size,win_size), cvSize(-1,-1),cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
            cvReleaseImage( &eig );
            cvReleaseImage( &temp );
	
///////////////storing points of the first frame//////////////////////////
			for(i=0;i<count;i++)
			{
			path[v][i].x=points[0][i].x;
			path[v][i].y=points[0][i].y;
			}
			v++;

//mask

			for(i=0;i<mask->height;i++)
			for(int j=0;j<mask->width;j++)
				mask->imageData[i*mask->widthStep+j]=0;
//top
		for(j=0;j<mask->width;j++)
			mask->imageData[j]=255;
		i=1;						
		while(i<10)
		{
		for(j=0;j<mask->width;j++)
			mask->imageData[i*mask->widthStep+j]=255;
		i++;
		}
//left
		for(j=0;j<mask->height;j++)
			mask->imageData[j*mask->widthStep]=255;
		i=1;
		while(i<10)	
		{
		for(j=0;j<mask->height;j++)
			mask->imageData[j*mask->widthStep+i]=255;
		i++;
		}
//right
		for(i=0;i<mask->height;i++)
			mask->imageData[i*mask->widthStep+mask->widthStep-1]=255;
																		
		j=10;
		while(j>0)	
		{
		for(i=0;i<mask->height;i++)
			mask->imageData[i*mask->widthStep+mask->widthStep-j]=255;
		j--;
		}
//bottom
		for(j=0;j<mask->width;j++)
			mask->imageData[mask->widthStep*(mask->height-1)+j]=255;
		i=1;
		while(i<10)	
		{
		for(j=0;j<mask->width;j++)
			mask->imageData[mask->widthStep*(mask->height-i)+j]=255;
		i++;
		}


	int tempcount=count;
    for(;;)
	{
        //IplImage* frame = 0;
        
		fcount=30;
		if( !frame )
            break;
		while(fcount!=0)
		{
        frame = cvQueryFrame( capture );
		
        if( !frame )
            break;

	
        cvCopy( frame, image, 0 );
        cvCvtColor( image, grey, CV_BGR2GRAY );
	     
        if( night_mode )
            cvZero( image );

        if( fcount==1 )
        {
            /* automatic initialization */
            IplImage* eig = cvCreateImage( cvGetSize(grey), 32, 1 );
            IplImage* temp = cvCreateImage( cvGetSize(grey), 32, 1 );
            double quality = 0.01;
            double min_distance = 10;

            count = MAX_COUNT;
            cvGoodFeaturesToTrack( grey, eig, temp, pointsnew[0], &count,
                                   quality, min_distance, mask, 3, 0, 0.04 );
            cvFindCornerSubPix( grey, pointsnew[0], count,
                cvSize(win_size,win_size), cvSize(-1,-1),
                cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
            cvReleaseImage( &eig );
            cvReleaseImage( &temp );

            add_remove_pt = 0;
       }
      
            cvCalcOpticalFlowPyrLK( prev_grey, grey, prev_pyramid, pyramid,
                points[0], points[1], tempcount, cvSize(win_size,win_size), 3, status, 0,
                cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.01), flags );
            flags |= CV_LKFLOW_PYR_A_READY;
            for( i = k = 0; i < tempcount; i++ )
            {
                if( add_remove_pt )
                {
                    double dx = pt.x - points[1][i].x;
                    double dy = pt.y - points[1][i].y;

                    if( dx*dx + dy*dy <= 25 )
                    {
                        add_remove_pt = 0;
                        continue;
                    }
                }

                if( !status[i] )
                    continue;

                points[1][k++] = points[1][i];
                cvCircle( image, cvPointFrom32f(points[1][i]), 3, CV_RGB(0,255,0), -1, 8,0);
				//cvLine(image,cvPointFrom32f(points[0][i]),cvPointFrom32f(points[1][i]),CV_RGB(255,0,0),1,8,0);
            }
            tempcount = k;
       /* }*/

		
//storing the path 		
		
		for(z=0;z<tempcount;z++)
		{
			if(status[z])
			{
				path[v][z].x=points[1][z].x;
				path[v][z].y=points[1][z].y;
		
			}
			else
			{
				path[v][z].x=999;
				path[v][z].y=999;
				
			
			}
		}
		v++;


			
///////////////Drawing path////////////////////////
		for(i=0;i<tempcount;i++)
			for(j=0;j<v-1;j++)
			{

				CvPoint p0 = cvPoint(cvRound(path[j][i].x),cvRound(path[j][i].y));
				
				cvCircle( image, p0, 0.8, CV_RGB(255,0,0), -1, 8,0);
			}
	
        if( add_remove_pt && tempcount < MAX_COUNT )
        {
            points[1][tempcount++] = cvPointTo32f(pt);
            cvFindCornerSubPix( grey, points[1] + tempcount - 1, 1,
                cvSize(win_size,win_size), cvSize(-1,-1),
                cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
            add_remove_pt = 0;
        }

//Removing stationary points		
		if(fcount==1)
		{
		for(z=0;z<count;z++)
		{
			int flag=0,flag1=0;
			//cvCircle( image, cvPointFrom32f(pointsnew[0][z]), 3, CV_RGB(0,255,0), -1, 8,0);
			for(j=0;j<tempcount;j++)
			{
				
					
				for(i=0;i<=5;i++)
					if((int)pointsnew[0][z].x==(int)points[0][j].x+i)
					{
						flag=1;
						break;
					}
				if(flag)
					for(i=5;i>=0;i--)
						if((int)pointsnew[0][z].y==(int)points[0][j].y-i)
						{
							flag1=1;
							break;
						}
			}
			
			if(!flag1)
			{
			points[1][tempcount].x=pointsnew[0][z].x;
			points[1][tempcount++].y=pointsnew[0][z].y;
			//cvLine(image,cvPointFrom32f(points[0][i]),cvPointFrom32f(points[1][i]),CV_RGB(255,0,0),1,8,0);
			}
		
			
		}
		}
	
	
        CV_SWAP( prev_grey, grey, swap_temp );
        CV_SWAP( prev_pyramid, pyramid, swap_temp );
        CV_SWAP( points[0], points[1], swap_points );
        need_to_init = 0;
        cvShowImage( "LkDemo", image );
	
        
		c = cvWaitKey(10);
        if( (char)c == 27 )
            break;
        switch( (char) c )
        {
        case 'r':
            need_to_init = 1;
            break;
        case 'c':
            count = 0;
            break;
        case 'n':
            night_mode ^= 1;
            break;
        default:
            ;
        }
	fcount--;

      }
						
	}
	
	    cvReleaseCapture( &capture );
    cvDestroyWindow("LkDemo");

////////average target velocity//////////
	int flagv=0;	
	vel=0;
			
		for(i=0;i<tempcount;i++)
		{
			j=0;
			while(j<v)
			{
				flagv=0;
				vj[0][vel].x=0;
				vj[0][vel].y=0;
				while((path[j][i].x!=999 && path[j][i].y!=999) && (path[j+1][i].x!=999 && path[j+1][i].y!=999) && j<v)
				{
					vj[0][vel].x+=path[j+1][i].x-path[j][i].x;
					vj[0][vel].y+=path[j+1][i].y-path[j][i].y;
					j++;
					flagv=1;
				}
				if(flagv)
				{
					vj[0][vel].x/=j;
					vj[0][vel].y/=j;
					vel++;					
				}
				j++;
				
			}
			
		}
		
/////////////target trajectory mean/////////////////
		m=0;
		flagv=0;
		for(i=0;i<tempcount;i++)
		{
			j=0;
			while(j<v)
			{
				flagv=0;
				mj[0][m].x=0;
				mj[0][m].y=0;
				while((path[j][i].x!=999 && path[j][i].y!=999) && (path[j+1][i].x!=999 && path[j+1][i].y!=999) && j<v)
				{
					mj[0][m].x+=path[j][i].x;
					mj[0][m].y+=path[j][i].y;
					j++;
					flagv=1;
				}
				if(flagv)
				{
					mj[0][m].x/=j+1;
					mj[0][m].y/=j+1;
					m++;					
				}
				j++;
				
			}
			
		}
		
/////////////////directional distance/////////////
		d=0;
		
		for(i=0;i<tempcount;i++)
		{
			j=0;
			while(j<v)
			{

				start.x=path[j][i].x;
				start.y=path[j][i].y;
				while(path[j][i].x!=999 && path[j][i].y!=999 && j<v)
					j++;
				
				dj[0][d].x=start.x-path[j-1][i-1].x;
				dj[0][d].y=start.y-path[j-1][i-1].y;
				d++;
				
				j++;
			}
							
		}
	
////////////////////////////writing to file//////////////////////////////////////////////////////////////////////////////////////////////////////
		
			FILE *fp;
			fp=fopen("avg_vel_v1n.txt", "w" );
	
			for(int j=0;j<vel;j++)
				fprintf(fp,"%f %f ",vj[0][j].x,vj[0][j].y);
	
			fclose(fp);
		

		
		
			fp=fopen("trajectory_mean_v1n.txt", "w" );
	
			for(int j=0;j<m;j++)
				fprintf(fp,"%f %f ",mj[0][j].x,mj[0][j].y);
	
	
			fclose(fp);

		
			fp=fopen("directional_dist_v1n.txt", "w" );
	
			for(int j=0;j<m;j++)
				fprintf(fp,"%f %f ",dj[0][j].x,dj[0][j].y);
			
	
			fclose(fp);
		


	out=fopen("path.txt", "w" );

	for(int i=0;i<tempcount;i++)
	{
			for(int j=0;j<v;j++)
				if(!(path[j][i].x==999 && path[j][i].y==999))
					fprintf(out,"%f,%f->",path[j][i].x,path[j][i].y);
				else
					fprintf(out,"%s","END");
			fprintf(out,"%s","END");
	}
			fclose(out);
	
	
	
	//cvDestroyWindow("mask");
	cvWaitKey();
    return 0;
}





