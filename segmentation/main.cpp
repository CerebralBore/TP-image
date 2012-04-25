#include <highgui.h>
#include <iostream>
#include <time.h>
#include <vector>
#include "Region.h"
#include "Pixel.h"

using namespace std;

cv::Vec3b filtreMatrice(cv::Mat matImage, cv::Mat matFiltre)
{
	double coef = 0;
	double totalR = 0;
	double totalG = 0;
	double totalB = 0;
	cv::Vec3b pix;
	for(int i=0; i<matImage.rows; i++)
	{
		for(int j=0; j<matImage.cols; j++)
		{
			coef = coef + matFiltre.at<double>(i,j);
			totalR = totalR + matImage.at<cv::Vec3b>(i,j)[2] * matFiltre.at<double>(i,j);
			totalG = totalG + matImage.at<cv::Vec3b>(i,j)[1] * matFiltre.at<double>(i,j);
			totalB = totalB + matImage.at<cv::Vec3b>(i,j)[0] * matFiltre.at<double>(i,j);
		}
	}
	if(coef !=0)
	{
		pix[2] = totalR/coef;
		pix[1] = totalG/coef;
		pix[0] = totalB/coef;
	}
	else
	{
		pix[2] = totalR;
		pix[1] = totalG;
		pix[0] = totalB;
	}

	return(pix);
}


cv::Mat filtreImage(cv::Mat imageS)
{
	cv::Range row;
	cv::Range col;
	double m[3][3] = {{3, 7, 3}, {7, 16, 7}, {3, 7, 3}};
	cv::Mat filtre = cv::Mat(3, 3, CV_64F, m);
	cv::Mat imageF = imageS.clone();

	cv::Mat matImage;
	for(int i=1; i<(imageS.rows-1); i++)
	{
		for(int j=1; j<(imageS.cols-1); j++)
		{
			row = cv::Range((i-1),(i+2));
			col = cv::Range((j-1),(j+2));
			matImage = cv::Mat(imageS, row, col);
			imageF.at<cv::Vec3b>(i,j) = filtreMatrice(matImage, filtre);
		}
	}
	return imageF;
}


cv::Mat gris(cv::Mat imageS)
{
	cv::Mat imageF = imageS.clone();
	int red, blue, green;

	for(int i=0; i<(imageS.rows); i++)
	{
		for(int j=0; j<(imageS.cols); j++)
		{	
			//remplir d'une couleur
			//imageF.at<cv::Vec3b>(i,j) = cv::Vec3b(255,255,255);
			//channels
			red = imageF.at<cv::Vec3b>(i,j)[2];
			blue = imageF.at<cv::Vec3b>(i,j)[0];
			green = imageF.at<cv::Vec3b>(i,j)[1];
			for(int k=0; k<3; k++){
				//imageF.at<cv::Vec3b>(i,j)[k] = (red + blue + green)/3 ;
				//recommandation 709
				imageF.at<cv::Vec3b>(i,j)[k] = 0.2125*red + 0.0721*blue + 0.7154*green ;
			}
		}
	}

	return imageF;
}

cv::Mat normalize(cv::Mat imageS)
{
	cv::Mat imageF = imageS.clone();
	int min, max;

	for(int k=0; k<3; k++)
	{
		//initialisation
		min = 255;
		max = 0;

		//recherche du maximum et minimum de l'histogramme dans l'image
		for(int i=0; i<imageS.rows; i++)
		{
			for(int j=0; j<imageS.cols; j++)
			{	
				if(imageF.at<cv::Vec3b>(i,j)[k] < min){
					min = imageF.at<cv::Vec3b>(i,j)[k];
				}
				if(imageF.at<cv::Vec3b>(i,j)[k] > max){
					max = imageF.at<cv::Vec3b>(i,j)[k];
				}
			}
		}

		//std::cout<<"canal "<<k<<" min "<<min<<" max "<<max<<std::endl;

		//pour éviter la division par zéro en cas de couleur uniforme
		if(min != max){
			//etirement de l'histogramme
			for(int i=0; i<imageS.rows; i++)
			{
				for(int j=0; j<imageS.cols; j++)
				{	
					imageF.at<cv::Vec3b>(i,j)[k] = ((imageF.at<cv::Vec3b>(i,j)[k] - min) * 255 )/(max-min); 
				}
			}
		}
	//	std::cout<<"image étirée :canal "<<k<<" min "<<min<<" max "<<max<<std::endl;

	}
	return imageF;
}

cv::Mat regionGrowing(cv::Mat imageS)
{
	cv::Mat imageF = imageS.clone();

	srand(time(NULL));

	int x, y, autreRegion, changement;
	double coulR, coulV, coulB;

	//int imageR[lig][col];
	vector <int> imageR(imageS.rows*imageS.cols, -1);

	double seuil = 40.;

	vector<Region> regions;
	regions.resize(0);

	for(int i=0; i<imageS.rows; i+= 5)
	{
		for(int j=0; j<imageS.cols; j+= 5)
		{	
			regions.push_back(Region(i, j));; 
		}
	}
	//regions.push_back(Region(imageS.rows/2, imageS.cols/2));; 

	int lig = imageS.cols;

	vector<Pixel> pixels;

	do
	{
		changement = 0;
		for(int k = 0; k < regions.size(); k++)
		{
			pixels.resize(0);
			for(int l = 0; l < regions[k].size(); l++)
			{
				x = regions[k].getPixelX(l);
				y = regions[k].getPixelY(l);
				//si le pixel a été ajouté à la région
				if(imageR[x*lig+y] != k)
				{
					if(imageR[x*lig+y] >= 0 && regions[k].compare( regions[imageR[x*lig+y]].moyR(), regions[imageR[x*lig+y]].moyV(), regions[imageR[x*lig+y]].moyB(), seuil ) )
					{
						autreRegion = imageR[x*lig+y];
						for(int i = 0; i < imageS.rows; i++)
						{
							for(int j = 0; j < imageS.cols; j++)
							{
								if(imageR[i*lig+j] == autreRegion)
								{
									regions[k].addPix(imageS.at<cv::Vec3b>(i, j)[2],imageS.at<cv::Vec3b>(i, j)[1],imageS.at<cv::Vec3b>(i, j)[0]);
									imageR[i*lig+j] = k;
									changement++;
								}
							}
						}
						for(int i = 0; i < regions[autreRegion].size(); i++)
						{
							pixels.push_back(regions[autreRegion].getPixel(i));
						}
						regions[autreRegion].mmmmmmmonsterKill();
					}
					else if(regions[k].nbPix() == 0 || regions[k].compare( imageS.at<cv::Vec3b>(x, y)[2], imageS.at<cv::Vec3b>(x, y)[1], imageS.at<cv::Vec3b>(x, y)[0], seuil ) )
					{
						regions[k].addPix(imageS.at<cv::Vec3b>(x, y)[2],imageS.at<cv::Vec3b>(x, y)[1],imageS.at<cv::Vec3b>(x, y)[0]);
						imageR[x*lig+y] = k;
						changement++;
						if(x-1 >= 0)
						{
							if(imageR[(x-1)*lig+y] != k)
							{
								pixels.push_back(Pixel(x-1,y));
							}
						}
						if(x+1 < imageS.rows)
						{
							if(imageR[(x+1)*lig+y] != k)
							{
								pixels.push_back(Pixel(x+1,y));
							}
						}
						if(y-1 >= 0)
						{
							if(imageR[x*lig+(y-1)] != k)
							{
								pixels.push_back(Pixel(x,y-1));
							}
						}
						if(y+1 < imageS.cols)
						{
							if(imageR[x*lig+(y+1)] != k)
							{
								pixels.push_back(Pixel(x,y+1));
							}
						}
					}
				}
			}
			//actualise le tableau frontiere
			regions[k].setFrontiere(pixels);
		}
	}while(changement !=0);
	//rendu de l'imageF
	for(int k = 0; k < regions.size(); k++)
	{
		cout<<"region "<<k<<" "<<regions[k].isAlive()<<endl;
		if(regions[k].isAlive())
		{
			coulR = rand()%255;
			coulV = rand()%255;
			coulB = rand()%255;
			for(int i = 0; i < imageS.rows; i++)
			{
				for(int j = 0; j < imageS.cols; j++)
				{
					if(imageR[i*lig+j] == k)
					{
						imageF.at<cv::Vec3b>(i,j)[2] = coulR;
						imageF.at<cv::Vec3b>(i,j)[1] = coulV;
						imageF.at<cv::Vec3b>(i,j)[0] = coulB;
					}
				}
			}
		}
	}
	return(imageF);
}

int main (int argc, char* argv[])
{
	// Initialisation
	std::string path1 = "../image/lena.jpg";
	std::string path2 = "../image/lenaModif.jpg";

	// Ouverture de l'image
	cv::Mat imgO = cv::imread (path1);
	if (imgO.empty ())
	{
		std::cerr << "Couldn't open image: " << path1 << std::endl;
		cv::waitKey ();
		return EXIT_FAILURE;
	}

	//application du filtre
	cv::Mat imgF = filtreImage(imgO);
	cv::Mat imgGris = gris(imgO);
	cv::Mat imgGrisN = normalize(imgGris);
	cv::Mat imgN = normalize(imgO);
	cv::Mat imgNF = normalize(imgF);
	cv::Mat imgS = regionGrowing(imgO);
	//cv::Mat imgSF = regionGrowing(imgNF);


	//affichage
	cvResizeWindow("win1",300,300);

	/*cv::imshow("lena", imgO);
	cv::waitKey ();
	cv::imshow("lenaFiltre", imgF);
	cv::waitKey ();
	cv::imshow("lenaGris", imgGris);
	cv::waitKey ();
	cv::imshow("lenaGrisNormalize", imgGrisN);
	cv::waitKey ();
	cv::imshow("lenaNormalize", imgN);
	cv::waitKey ();*/
	cv::imshow("lenaRegion1", imgS);
	cv::waitKey ();
	//cv::imshow("lenaRegion2", imgSF);
	//cv::waitKey ();

	//ecriture
	cv::imwrite(path2,imgGrisN);

	std::cout << "Appuyez sur une touche pour continuer" << std::endl;
	cv::waitKey ();
	return EXIT_SUCCESS;
}

