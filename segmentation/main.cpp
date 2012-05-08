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

void slipMerge(vector<Region>& reg, cv::Mat& img, int H, int B, int G, int D)
{
	//condition d'arret
	if(((B - H) * (D - G)) <= 0.0)
	{
		return;
	}

	double seuil = 30.0;

	double ecartTypeR =0.0;
	double ecartTypeV =0.0;
	double ecartTypeB =0.0;

	double moyR =0.0;
	double moyV =0.0;
	double moyB =0.0;

	//parcours de l'image dans le cadre en cours
	//récupération des moyennes de couleurs
	for(int i=H; i<B; i++)
	{
		for(int j=G; j<D; j++)
		{	
			 moyR += img.at<cv::Vec3b>(i, j)[2];
			 moyV += img.at<cv::Vec3b>(i, j)[1];
			 moyB += img.at<cv::Vec3b>(i, j)[0];
		}
	}

	//moyennes
	moyR /= ( (B - H) * (D - G) );
	moyV /= ( (B - H) * (D - G) );
	moyB /= ( (B - H) * (D - G) );
	
	for(int i=H; i<B; i++)
	{
		for(int j=G; j<D; j++)
		{	
			 ecartTypeR += pow ( (img.at<cv::Vec3b>(i, j)[2] - moyR), 2); 
			 ecartTypeV += pow ( (img.at<cv::Vec3b>(i, j)[1] - moyV), 2);
			 ecartTypeB += pow ( (img.at<cv::Vec3b>(i, j)[0] - moyB), 2);
		}
	}

	//écart type
	//ecartTypeR /= ( (B - H) * (D - G) );
	//ecartTypeV /= ( (B - H) * (D - G) );
	//ecartTypeB /= ( (B - H) * (D - G) );

	ecartTypeR *= moyR;
	ecartTypeV *= moyV;
	ecartTypeB *= moyB;


	ecartTypeR = sqrt(ecartTypeR);
	ecartTypeV = sqrt(ecartTypeR);
	ecartTypeB = sqrt(ecartTypeR);
	
	if(ecartTypeR < seuil && ecartTypeV < seuil && ecartTypeB < seuil)
	{
		//homogène
		//place un germe au centre du cadre
		reg.push_back( Region(H+(B - H)/2, G+(D - G)/2, img.rows, img.cols) );
	}else
	{
		slipMerge(reg, img, H, H+(B - H)/2, G, G+(D - G)/2 );
		slipMerge(reg, img, H, H+(B - H)/2, G+(D - G)/2, D );
		slipMerge(reg, img, H+(B - H)/2, B, G, G+(D - G)/2 );
		slipMerge(reg, img, H+(B - H)/2, B, G+(D - G)/2, D );
	}
}

cv::Mat regionGrowing(cv::Mat imageS, double seuil, int germe)
{
	cv::Mat imageF = imageS.clone();

	srand(time(NULL));

	int x, y, autreRegion, changement, regionFus;
	double coulR, coulV, coulB;

	//racourcis pour imagesS.cols
	int lig = imageS.cols;

	//Matrice d'indication de region
	vector <int> imageR(imageS.rows*imageS.cols, -1);

	//tableau temporaire des prochains pixels a visiter 
	vector<Pixel> pixels;

	//initialisation du tableau des regions
	vector<Region> regions;
	regions.resize(0);

	//placement des germes avec l'algorithme Slip&Merge
	slipMerge(regions, imageF, 0, imageF.rows, 0, imageF.cols);

	/*for(int z = 0; z <5; z++)
	{*/
		//positionnement des germes
		/*for(int i=0; i<imageS.rows; i+= germe)
		{
			for(int j=0; j<imageS.cols; j+= germe)
			{
				if(imageR[i*lig+j] == -1)
				{
					regions.push_back(Region(i, j, imageS.rows, imageS.cols));
				}
			}
		} */

		//faire ... tant qu'il n'y a pas de changement
		do
		{
			changement = 0;

			//pour chaque region ...
			for(int k = 0; k < regions.size(); k++)
			{
				if(regions[k].isAlive())
				{
					regionFus = k;
					pixels.clear();
					pixels.resize(0);
					//pour chaque pixel a visiter ...
					for(int l = 0; l < regions[k].sizeFr(); l++)
					{
						x = regions[k].getFrPixelX(l);
						y = regions[k].getFrPixelY(l);

						//si le pixel ne fait pas partie de la même region alors ...
						if(imageR[x*lig+y] != k)
						{
							//si le pixel fait partie d'une autre region et que cette region a la même couleur que la region courante alors ...
							if(imageR[x*lig+y] >= 0 && regions[k].compare( regions[imageR[x*lig+y]].moyR(), regions[imageR[x*lig+y]].moyV(), regions[imageR[x*lig+y]].moyB(), seuil ) )
							{
								//récupération de l'id de la region
								autreRegion = imageR[x*lig+y];

								//parcours de l'image pour changer les pixels appartenant à l'autre region
								for(int i = regions[autreRegion].getCadreH(); i <= regions[autreRegion].getCadreB(); i++)
								{
									for(int j = regions[autreRegion].getCadreG(); j <= regions[autreRegion].getCadreD(); j++)
									{
										if(imageR[i*lig+j] == autreRegion)
										{
											regions[k].addPix(imageS.at<cv::Vec3b>(i, j)[2],imageS.at<cv::Vec3b>(i, j)[1],imageS.at<cv::Vec3b>(i, j)[0]);
											imageR[i*lig+j] = k;
											changement++;
										}
									}
								}

								regions[k].regionCadre(regions[autreRegion].getCadreH(), regions[autreRegion].getCadreG(), regions[autreRegion].getCadreB(), regions[autreRegion].getCadreD());

								//récupération des pixels a visiter de l'autre region
								if(regionFus < autreRegion)
								{
									for(int i = 0; i < regions[autreRegion].sizeFr(); i++)
									{
										if(imageR[regions[autreRegion].getFrPixelX(i)*lig+regions[autreRegion].getFrPixelY(i)] != k)
										{
											regions[k].insertFrPixel(regions[autreRegion].getFrPixel(i));
										}
									}
								}
								else
								{
									for(int i = 0; i < regions[autreRegion].sizeFr(); i++)
									{
										if(imageR[regions[autreRegion].getFrPixelX(i)*lig+regions[autreRegion].getFrPixelY(i)] != k)
										{
											pixels.push_back(regions[autreRegion].getFrPixel(i));
										}
									}
								}

								regionFus = autreRegion;

								//suppression de l'autre region
								regions[autreRegion].mmmmmmmonsterKill();
								//cout<<"fusion "<<autreRegion<<" avec "<<k<<endl;
							}

							//sinon si c'est une germe ou si le pixel a la même couleur que la region courante alors ...
							else if(regions[k].nbPix() == 0 || regions[k].compare( imageS.at<cv::Vec3b>(x, y)[2], imageS.at<cv::Vec3b>(x, y)[1], imageS.at<cv::Vec3b>(x, y)[0], seuil) && imageR[x*lig+y] < 0 )
							{
								regions[k].addPix(imageS.at<cv::Vec3b>(x, y)[2],imageS.at<cv::Vec3b>(x, y)[1],imageS.at<cv::Vec3b>(x, y)[0]);
								regions[k].pixelCadre(x, y);
								imageR[x*lig+y] = k;
								changement++;
								if(x-1 >= 0)
								{
									if(imageR[(x-1)*lig+y] != k)
									{
										pixels.push_back(Pixel(x-1,y));
									}
									if(y-1 >= 0)
									{
										if(imageR[(x-1)*lig+(y-1)] != k)
										{
											pixels.push_back(Pixel((x-1),y-1));
										}
									}
									if(y+1 < imageS.cols)
									{
										if(imageR[(x-1)*lig+(y+1)] != k)
										{
											pixels.push_back(Pixel((x-1),y+1));
										}
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
								if(x+1 < imageS.rows)
								{
									if(imageR[(x+1)*lig+y] != k)
									{
										pixels.push_back(Pixel(x+1,y));
									}
									if(y-1 >= 0)
									{
										if(imageR[(x+1)*lig+(y-1)] != k)
										{
											pixels.push_back(Pixel((x+1),y-1));
										}
									}
									if(y+1 < imageS.cols)
									{
										if(imageR[(x+1)*lig+(y+1)] != k)
										{
											pixels.push_back(Pixel((x+1),y+1));
										}
									}
								}
							}
							pixels.push_back(Pixel(x,y));
						}
					}
					//actualise le tableau frontiere
					regions[k].setFrontiere(pixels);
				}
			}
			cout<<"changement "<<changement<<endl;
			for(int i = 0; i < imageS.rows; i++)
			{
				for(int j = 0; j < imageS.cols; j++)
				{
					if(imageR[i*lig+j] == -1)
					{
						imageF.at<cv::Vec3b>(i,j)[2] = 255.;
						imageF.at<cv::Vec3b>(i,j)[1] = 255.;
						imageF.at<cv::Vec3b>(i,j)[0] = 255.;
					}
					else
					{
						imageF.at<cv::Vec3b>(i,j)[2] = regions[imageR[i*lig+j]].moyR();
						imageF.at<cv::Vec3b>(i,j)[1] = regions[imageR[i*lig+j]].moyV();
						imageF.at<cv::Vec3b>(i,j)[0] = regions[imageR[i*lig+j]].moyB();
					}
				}
			}
			cv::imshow("lenaRegion", imageF);
			cv::waitKey(10);
			//cv::waitKey ();
		}while(changement !=0);
		/*germe = germe/2;
	}*/
	//rendu de l'imageF
	for(int i = 0; i < imageS.rows; i++)
	{
		for(int j = 0; j < imageS.cols; j++)
		{
			if(imageR[i*lig+j] == -1)
			{
				imageF.at<cv::Vec3b>(i,j)[2] = 255.;
				imageF.at<cv::Vec3b>(i,j)[1] = 255.;
				imageF.at<cv::Vec3b>(i,j)[0] = 255.;
			}
			else
			{
				imageF.at<cv::Vec3b>(i,j)[2] = regions[imageR[i*lig+j]].moyR();
				imageF.at<cv::Vec3b>(i,j)[1] = regions[imageR[i*lig+j]].moyV();
				imageF.at<cv::Vec3b>(i,j)[0] = regions[imageR[i*lig+j]].moyB();
			}
		}
	}

	cv::imshow("lenaRegion", imageF);
	cv::waitKey ();

	for(int i = 0; i < imageS.rows; i++)
	{
		for(int j = 0; j < imageS.cols; j++)
		{
			if(j != 0)
			{
				if(imageR[i*lig+j] != imageR[i*lig+(j-1)])
				{
					imageF.at<cv::Vec3b>(i,j)[2] = 0;
					imageF.at<cv::Vec3b>(i,j)[1] = 0;
					imageF.at<cv::Vec3b>(i,j)[0] = 0;
					imageF.at<cv::Vec3b>(i,j-1)[2] = 0;
					imageF.at<cv::Vec3b>(i,j-1)[1] = 0;
					imageF.at<cv::Vec3b>(i,j-1)[0] = 0;
				}
			}
			if(i != 0)
			{
				if(imageR[i*lig+j] != imageR[(i-1)*lig+j])
				{
					imageF.at<cv::Vec3b>(i,j)[2] = 0;
					imageF.at<cv::Vec3b>(i,j)[1] = 0;
					imageF.at<cv::Vec3b>(i,j)[0] = 0;
					imageF.at<cv::Vec3b>(i-1,j)[2] = 0;
					imageF.at<cv::Vec3b>(i-1,j)[1] = 0;
					imageF.at<cv::Vec3b>(i-1,j)[0] = 0;
				}
			}
		}
	}
	for(int i = 0; i < imageS.rows; i++)
	{
		for(int j = 0; j < imageS.cols; j++)
		{
			if(imageR[i*lig+j] == -1)
			{
				imageF.at<cv::Vec3b>(i,j)[2] = 255.;
				imageF.at<cv::Vec3b>(i,j)[1] = 255.;
				imageF.at<cv::Vec3b>(i,j)[0] = 255.;
			}
		}
	}
	return(imageF);
}

int main (int argc, char* argv[])
{
	// Initialisation
	std::string path1 = "../image/bally_0.jpg";
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
	//cv::Mat imgS = regionGrowing(imgO);
	cv::Mat imgSF = regionGrowing(imgNF,20.,20);


	//affichage
	cvResizeWindow("win1",300,300);

	cv::imshow("Image Original", imgO);
	cv::waitKey ();
	cv::imshow("Image Filtre", imgF);
	cv::waitKey ();
	cv::imshow("Image Gris", imgGris);
	cv::waitKey ();
	cv::imshow("Image Gris Normalize", imgGrisN);
	cv::waitKey ();
	cv::imshow("Image Normalize", imgN);
	cv::waitKey ();
	cv::imshow("Image Normalize Filtre", imgNF);
	cv::waitKey ();
	//cv::imshow("Image Region Contour", imgS);
	//cv::waitKey ();
	cv::imshow("Image Region Contour Filtre", imgSF);
	cv::waitKey ();

	//ecriture
	cv::imwrite(path2,imgSF);

	std::cout << "Appuyez sur une touche pour continuer" << std::endl;
	cv::waitKey ();
	return EXIT_SUCCESS;
}

