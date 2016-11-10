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
void  setup(IplImage* frame)///输入原图
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
			  *   T1 是 设置double L 的最小范围阈值  即  L>T1
			  *input  int T2
			  *   T2  是设置double L的最大范围阈值   即L<=T2
			  *input   int T3
			  *   T3 是设置个单位像素之间进行角点的筛选  即多少像素之间的角点进行比较，acos值小的删除.
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
		*  T4是阈值  即在最终得到每个轮廓的角点坐标后需要对其所有轮廓的角点坐标进行
		*再次筛选 ，即在T4单位像素之内  筛选出acos值大的作为角点保留
		*/
		filtercorner(T4);

		///  探测角点  放在主函数----修改
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
*可能有很多个角点在一起 则需要进行过滤
* 设置阈值-- 即在一定区域内选取最大的acos值为角点并保存
*/
///@param   int T  a threshold value
void filtercorner(int T)
{
	vector<CvPoint>  pp;/// vector pp is to store  the cornerpoint of mismatch condition 
	pp.clear();/// vector clear
	
	bool* flag=new bool[cornerpoints.size()];/// define a  bool array to mark the cornerpoint which  match condition

	//!  double circulation is to select  which cornerpoint match condition  in  threshold value area
	/** 双重for循环来进行角点筛选  如果相邻两像素在T4阈值内 则保留acos值大的角点
	*/
		for(int i=0;i<cornerpoints.size();i++)//解决相邻角点的问题 若两角点之间的距离小于T4 则保留acos最大的
	{ 
		for (int j = cornerpoints.size()-1; j >i; j--)
		{
			int  max;
			{
				if((abs(cornerpoints[j].x-cornerpoints[i].x)<T)&&((abs(cornerpoints[j].y-cornerpoints[i].y))<T))/// 判断相邻两角点是否在阈值区域内
				{

					if(corneracos[i]>=corneracos[j]){max=j;}///选取出acos值大的角点
					else{max=i;} 
					flag[max]=true;///把acos值小的角点在对数组做标记
				}
			}
			
		}
	}
		//! for circulation 
		/** the for circulation is to put the  points that bigger value of acos  in vector points
		* 把acos 值大的角点存储进vector<CvPoint> points*/
		for(int i=0;i<cornerpoints.size();i++)
		{
			if(flag[i]==true)/// acos 值小的点
			{
             pp.push_back(cornerpoints[i]);/// the vector pp is to store the the cornerpoint of mismatch condition
			}
			else{ points.push_back(cornerpoints[i]);accos.push_back(corneracos[i]);}///put the the cornerpoint of match condition in vector points
			
		}

}



	///@brief   the transform function
	/*! the transform function  is  to  transform CEdgeHitChain chain to CvSeq* 
	*把数据类型CEdgeHitChain 的轮廓转换为CvSeq* 的轮廓
	*/
	///@param   input CEdgeHitChain chain
    ///@return  the contour of CvSeq* 
	CvSeq* transform(CEdgeHitChain chain)//自定义的transform函数
	{
		
		CEdgeHit *tempHit;///define the  object tempHit  of CEdgeHit class
		CvPoint tempPoint;/// define the object of CvPoint 
	    CvMemStorage* tempStorage;//CvMemStorage 动态内存存储
		tempStorage = cvCreateMemStorage(0);///初始化内存
		CvSeq* tempContour = cvCreateSeq(CV_SEQ_POLYGON, sizeof(CvSeq), sizeof(CvPoint), tempStorage);//创建CvSeq*序列
		
		tempHit = chain.first;///把CEdgeHitChain 的首地址给CEdgeHit *tempHit
		do ///循环
		{
			tempPoint.x = tempHit->x;///把CEdgeHitChain上的点全部赋值给CvPoint tempPoint
			tempPoint.y = tempHit->y;
			//cout<<"tempHit->x"<<tempHit->x<<"  "<<tempHit->y<<endl;
			//cout<<"tempPoint"<<tempPoint.x<<"  "<<tempPoint.y<<endl;
			cvSeqPush(tempContour ,&tempPoint);///把tempPoint 一个一个写入CvSeq* tempContour
			tempHit = tempHit->next;///继续下一个点
		}while (tempHit!=chain.last->next);//直到CEdgeHitChain尾端跳出循环

		return tempContour;///返回CvSeq*序列轮廓
	}



public:///  the object of public
    CSixSword ss;
	CFrame fr;
	vector<CvPoint> cornerpoints;
    vector<double> corneracos;
	vector<CvPoint>  points;
	vector<double> accos;
};