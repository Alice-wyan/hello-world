#include"lib.h"
#include"findcorner.h"
#include"detectline.h"
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
using namespace std;


//!  a  Getcorner class
class Getcorner
{
//!  a constructor of Getcorner
public:Getcorner(){}




///@brief   the setup function
/*! the setup function is to  transform IplImage* frame to    CSixSword  class 
* to get the contour of CEdgeHitChain chain
*/
///@param   IplImage* frame input IplImage* 
void  setup(IplImage* frame)///����ԭͼ
{  
	if(!frame){cout<<"no frame !";}/// if  no frame   will cout  no frame
      ss.setup(frame);///  setup frame to   CSixSword  class
	  fr.set(frame);///transform frame to CFrame class  for read and draw contour in picture
	  ss.transform();/// invoke transform function of CSixSword  class  to get the contour of CEdgeHitChain chain
	 
}

///@brief   the getcornerpoint function
/*! the getcornerpoint function's  object to  get coernerpoint  */
///@param   IplImage* frame input IplImage* 
void getcornerpoint(IplImage* frame, IplImage *dst,int T1,int T2,int T3,int T4)
{
	 IplImage *src=cvCloneImage(frame);///  frame clone to src  for  CSixSword ss to setup
	 //IplImage *dst=cvCloneImage(frame);///  frame clone to dst for mark the cornerpoint 

	 setup(src);///CSixSword ss setup  IplImage *src  to get CEdgeHitChain chain(contour)

	 CvSeq* temp;///define the  CvSeq* temp (contour)
	 FindCorner fc;///define object  fc of FindCorner class
	 DetectLine dl;///define object dl  of DetectLine class

	 vector<CvPoint> point;///define vector to store the all points of contour

	  point.clear();/// clear the vector
	  cornerpoints.clear();///clear the vector
	  corneracos.clear();///clear the vector

	    cout<<"size=="<<ss.v.size()<<endl;///cout the size of contour 

		/**
		* circulate the   size of contour  to get every contour's  cornerpoint
		* print  the every contour in   IplImage *src  and  IplImage *dst
		* invoke the findcorner function of FindCorner class
		*/
	 	  	for(unsigned int k=0;k<ss.v.size();k++)
			{
			  temp=transform(ss.v[k]);/// transform the contour of CEdgeHitChain chain  to contour of CvSeq* 
			  ss.v[k].print(fr);///draw the contour in IplImage *src 

			  /**  invoke the findcorner function of FindCorner class
			  * input  the contour of CvSeq* 
			  * input  int T1
			  *   T1 �� ����double L ����С��Χ��ֵ  ��  L>T1
			  *input  int T2
			  *   T2  ������double L�����Χ��ֵ   ��L<=T2
			  *input   int T3
			  *   T3 �����ø���λ����֮����нǵ��ɸѡ  ����������֮��Ľǵ���бȽϣ�acosֵС��ɾ��.
			  */
			  fc.findcorner(temp,T1,T2,T3);


			  for(int i=0;i<fc.corner.size();i++)///  circulate the   size of corner of  object fc
			  {
				  cornerpoints.push_back(fc.corner[i]);///  using the circulation to put every corner in the vector of cornerpoints
				  corneracos.push_back(fc.accos[i]);/// using the circulation to put every corner's the value of acos in the vector of cornerpoints
			  }

			  //! invoke setup function of  FindCorner class
			  /** input the contour of CvSeq* 
			  *  invoke setup function  to get the contour of all points
			  */
			  point=fc.setup(temp);  

			  //! the for circulation
			  /**  
			  *circulate the size of all points to print these points in IplImage *dst
			  * the  goal is to draw the contour
			  */
			  for(int i=0;i<point.size()-1;i++){cvLine( dst,point[i], point[i+1], CV_RGB(0,255,0),1, 8, 0 );}/// draw the contour			 
		    }

        //! invoke the filtercorner function
		/** input the threshold value T4  
		* int T4
		*  T4����ֵ  �������յõ�ÿ�������Ľǵ��������Ҫ�������������Ľǵ��������
		*�ٴ�ɸѡ ������T4��λ����֮��  ɸѡ��acosֵ�����Ϊ�ǵ㱣��
		*/
		filtercorner(T4);

		///  ̽��ǵ�  ����������----�޸�
		vector<double> XY; 
	   	for(int i=0;i<points.size();i++)///for  circulation to draw the corenerpoint in picture 
	{
		cvCircle(dst,points[i], 5, CV_RGB(0,0,255), 1,8,0);/// draw every cornerpoint in   dst picture
		//vector<double> xy=dl.detectlinedegree(points[i],-accos[i]/2,10,frame,dst);//vector<double > detectlinedegree(CvPoint x,double degree,int distance,IplImage *src,IplImage *dst)
		//XY.push_back(xy[0]);
  //       XY.push_back(xy[1]);
		//cout<<accos[i]<<"  "<<points[i].x<<"  "<<points[i].y<<endl;
	}   


		cvNamedWindow( "dst", CV_WINDOW_AUTOSIZE );	 ///define a window
        cvShowImage( "dst", dst );///  show  the  IplImage *dst

        cvWaitKey(0);/// push the key of esc to nextstep

}

///@brief   the filtercorner function
/*! the filtercorner function  is  to  filter coernerpoint 
*�����кܶ���ǵ���һ�� ����Ҫ���й���
* ������ֵ-- ����һ��������ѡȡ����acosֵΪ�ǵ㲢����
*/
///@param   int T  a threshold value
void filtercorner(int T)
{
	vector<CvPoint>  pp;/// vector pp is to store  the cornerpoint of mismatch condition 
	pp.clear();/// vector clear
	
	bool* flag=new bool[cornerpoints.size()];/// define a  bool array to mark the cornerpoint which  match condition

	//!  double circulation is to select  which cornerpoint match condition  in  threshold value area
	/** ˫��forѭ�������нǵ�ɸѡ  ���������������T4��ֵ�� ����acosֵ��Ľǵ�
	*/
		for(int i=0;i<cornerpoints.size();i++)//������ڽǵ������ �����ǵ�֮��ľ���С��T4 ����acos����
	{ 
		for (int j = cornerpoints.size()-1; j >i; j--)
		{
			int  max;
			{
				if((abs(cornerpoints[j].x-cornerpoints[i].x)<T)&&((abs(cornerpoints[j].y-cornerpoints[i].y))<T))/// �ж��������ǵ��Ƿ�����ֵ������
				{

					if(corneracos[i]>=corneracos[j]){max=j;}///ѡȡ��acosֵ��Ľǵ�
					else{max=i;} 
					flag[max]=true;///��acosֵС�Ľǵ��ڶ����������
				}
			}
			
		}
	}
		//! for circulation 
		/** the for circulation is to put the  points that bigger value of acos  in vector points
		* ��acos ֵ��Ľǵ�洢��vector<CvPoint> points*/
		for(int i=0;i<cornerpoints.size();i++)
		{
			if(flag[i]==true)/// acos ֵС�ĵ�
			{
             pp.push_back(cornerpoints[i]);/// the vector pp is to store the the cornerpoint of mismatch condition
			}
			else{ points.push_back(cornerpoints[i]);accos.push_back(corneracos[i]);}///put the the cornerpoint of match condition in vector points
			
		}

}



	///@brief   the transform function
	/*! the transform function  is  to  transform CEdgeHitChain chain to CvSeq* 
	*����������CEdgeHitChain ������ת��ΪCvSeq* ������
	*/
	///@param   input CEdgeHitChain chain
    ///@return  the contour of CvSeq* 
	CvSeq* transform(CEdgeHitChain chain)//�Զ����transform����
	{
		
		CEdgeHit *tempHit;///define the  object tempHit  of CEdgeHit class
		CvPoint tempPoint;/// define the object of CvPoint 
	    CvMemStorage* tempStorage;//CvMemStorage ��̬�ڴ�洢
		tempStorage = cvCreateMemStorage(0);///��ʼ���ڴ�
		CvSeq* tempContour = cvCreateSeq(CV_SEQ_POLYGON, sizeof(CvSeq), sizeof(CvPoint), tempStorage);//����CvSeq*����
		
		tempHit = chain.first;///��CEdgeHitChain ���׵�ַ��CEdgeHit *tempHit
		do ///ѭ��
		{
			tempPoint.x = tempHit->x;///��CEdgeHitChain�ϵĵ�ȫ����ֵ��CvPoint tempPoint
			tempPoint.y = tempHit->y;
			//cout<<"tempHit->x"<<tempHit->x<<"  "<<tempHit->y<<endl;
			//cout<<"tempPoint"<<tempPoint.x<<"  "<<tempPoint.y<<endl;
			cvSeqPush(tempContour ,&tempPoint);///��tempPoint һ��һ��д��CvSeq* tempContour
			tempHit = tempHit->next;///������һ����
		}while (tempHit!=chain.last->next);//ֱ��CEdgeHitChainβ������ѭ��

		return tempContour;///����CvSeq*��������
	}



public:///  the object of public
    CSixSword ss;
	CFrame fr;
	vector<CvPoint> cornerpoints;
    vector<double> corneracos;
	vector<CvPoint>  points;
	vector<double> accos;
};