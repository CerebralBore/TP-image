#include <highgui.h>
#include <iostream>
#include <time.h>
#include <vector>
#include "Region.h"
#include "Pixel.h"
#include "Coffee.h"

using namespace std;

cv::Vec3b filtreMatrice(cv::Mat& matImage, cv::Mat& matFiltre)
{
	double coef = 0;

	double totalR = 0;
	double totalV = 0;
	double totalB = 0;

	cv::Vec3b pix;

	//parcourt les matrices 3x3 des deux images
	for(int i=0; i<matImage.rows; i++)
	{
		for(int j=0; j<matImage.cols; j++)
		{
			coef = coef + matFiltre.at<double>(i,j);
			totalR = totalR + matImage.at<cv::Vec3b>(i,j)[2] * matFiltre.at<double>(i,j);
			totalV = totalV + matImage.at<cv::Vec3b>(i,j)[1] * matFiltre.at<double>(i,j);
			totalB = totalB + matImage.at<cv::Vec3b>(i,j)[0] * matFiltre.at<double>(i,j);
		}
	}

	if(coef !=0)
	{
		pix[2] = totalR/coef;
		pix[1] = totalV/coef;
		pix[0] = totalB/coef;
	}
	else
	{
		pix[2] = totalR;
		pix[1] = totalV;
		pix[0] = totalB;
	}
	//renvoie la couleur du pixel central
	return(pix);
}

cv::Mat filtreImage(cv::Mat& imgSource)
{
	cv::Range lignes;
	cv::Range colonnes;
	double m[3][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};

	cv::Mat filtre = cv::Mat(3, 3, CV_64F, m);
	cv::Mat imgFiltre = imgSource.clone();
	cv::Mat matImage;

	for(int i=1; i<(imgSource.rows-1); i++)
	{
		for(int j=1; j<(imgSource.cols-1); j++)
		{
			//récuprération de la matrice 3x3 autour du point de l'image courrante
			lignes = cv::Range((i-1),(i+2));
			colonnes = cv::Range((j-1),(j+2));
			matImage = cv::Mat(imgSource, lignes, colonnes);
			imgFiltre.at<cv::Vec3b>(i,j) = filtreMatrice(matImage, filtre);
		}
	}
	return imgFiltre;
}

cv::Mat gris(cv::Mat& imgSource)
{
	cv::Mat imgFiltre = imgSource.clone();
	int rouge, bleu, vert;

	for(int i=0; i<(imgSource.rows); i++)
	{
		for(int j=0; j<(imgSource.cols); j++)
		{	
			//récupération des couleurs
			rouge = imgSource.at<cv::Vec3b>(i,j)[2];
			bleu = imgSource.at<cv::Vec3b>(i,j)[0];
			vert = imgSource.at<cv::Vec3b>(i,j)[1];
			for(int k=0; k<3; k++){
				imgFiltre.at<cv::Vec3b>(i,j)[k] = 0.2125*rouge + 0.0721*bleu + 0.7154*vert ;
			}
		}
	}
	return imgFiltre;
}

cv::Mat normalise(cv::Mat& imgSource)
{
	cv::Mat imgFiltre = imgSource.clone();
	int min, max;

	for(int k=0; k<3; k++)
	{
		//initialisation
		min = 255;
		max = 0;

		//recherche du maximum et minimum de l'histogramme dans l'image
		for(int i=0; i<imgSource.rows; i++)
		{
			for(int j=0; j<imgSource.cols; j++)
			{	
				if(imgFiltre.at<cv::Vec3b>(i,j)[k] < min){
					min = imgFiltre.at<cv::Vec3b>(i,j)[k];
				}
				if(imgFiltre.at<cv::Vec3b>(i,j)[k] > max){
					max = imgFiltre.at<cv::Vec3b>(i,j)[k];
				}
			}
		}

		//pour éviter la division par zéro en cas de couleur uniforme
		if(min != max){
			//étirement de l'histogramme
			for(int i=0; i<imgSource.rows; i++)
			{
				for(int j=0; j<imgSource.cols; j++)
				{	
					imgFiltre.at<cv::Vec3b>(i,j)[k] = ((imgFiltre.at<cv::Vec3b>(i,j)[k] - min) * 255 )/(max-min); 
				}
			}
		}
	}
	return imgFiltre;
}

void split(double seuil, vector<Region>& region, cv::Mat& img, int H, int B, int G, int D)
{
	//condition d'arret
	if(((B - H) * (D - G)) <= 0.0)
	{
		return;
	}

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
	ecartTypeR /= ( (B - H) * (D - G) );
	ecartTypeV /= ( (B - H) * (D - G) );
	ecartTypeB /= ( (B - H) * (D - G) );

	ecartTypeR = sqrt(ecartTypeR);
	ecartTypeV = sqrt(ecartTypeV);
	ecartTypeB = sqrt(ecartTypeB);
	
	if(ecartTypeR < seuil && ecartTypeV < seuil && ecartTypeB < seuil)
	{
		//homogène
		//place un germe au centre du cadre
		region.push_back( Region(H+(B - H)/2, G+(D - G)/2, img.rows, img.cols) );
	}
	else
	{
		//coupe en 4 zones
		split(seuil, region, img, H, H+(B - H)/2, G, G+(D - G)/2 );
		split(seuil, region, img, H, H+(B - H)/2, G+(D - G)/2, D );
		split(seuil, region, img, H+(B - H)/2, B, G, G+(D - G)/2 );
		split(seuil, region, img, H+(B - H)/2, B, G+(D - G)/2, D );
	}
}

cv::Mat regionGrowing(cv::Mat imgSource, double seuil, int germe, bool affTR, bool aleatoire)
{
	cv::Mat imgFiltre = imgSource.clone();

	srand(time(NULL));

	bool poseGerme = true;

	int x, y, autreRegion, changement, regionFus;
	double coulR, coulV, coulB;

	//racourcis pour imgSource.cols
	int lig = imgSource.cols;

	//Matrice d'indication de region
	vector <int> imageR(imgSource.rows*imgSource.cols, -1);

	//tableau temporaire des prochains pixels a visiter 
	vector<Pixel> pixels;

	//initialisation du tableau des regions
	vector<Region> regions;
	regions.resize(0);

	vector<cv::Vec3b> regionCoul;
	cv::Vec3b pix;
	
	//si germe < 1 alors on fait le placement des germes avec l'algorithme Slip&Merge
	if(germe < 1)
	{
		//placement des germes avec l'algorithme Slip&Merge
		split((seuil/2), regions, imgFiltre, 0, imgFiltre.rows, 0, imgFiltre.cols);
		germe = 1;
		poseGerme = false;
	}

	while(germe > 0)
	{
		if(poseGerme)
		{
			//positionnement des germes
			for(int i=0; i<imgSource.rows; i+= germe)
			{
				for(int j=0; j<imgSource.cols; j+= germe)
				{
					if(imageR[i*lig+j] == -1)
					{
						regions.push_back(Region(i, j, imgSource.rows, imgSource.cols));
					}
				}
			}
		}

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
							if(imageR[x*lig+y] >= 0 && regions[k].compare( regions[imageR[x*lig+y]].moyR(), regions[imageR[x*lig+y]].moyV(), regions[imageR[x*lig+y]].moyB(), seuil*0.6) )
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
											regions[k].addPix(imgSource.at<cv::Vec3b>(i, j)[2],imgSource.at<cv::Vec3b>(i, j)[1],imgSource.at<cv::Vec3b>(i, j)[0]);
											imageR[i*lig+j] = k;
											changement++;
										}
									}
								}

								regions[k].regionCadre(regions[autreRegion].getCadreH(), regions[autreRegion].getCadreG(), regions[autreRegion].getCadreB(), regions[autreRegion].getCadreD());

								//récupération des pixels a visiter de l'autre région
								//si l'autre région n'a pas été traité alors on la traite à la suite de la région courrante
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
							}

							//sinon si c'est une germe ou si le pixel a la même couleur que la region courante et qu'il n'appartient pas à une autre région alors ...
							else if(regions[k].nbPix() == 0 || regions[k].compare( imgSource.at<cv::Vec3b>(x, y)[2], imgSource.at<cv::Vec3b>(x, y)[1], imgSource.at<cv::Vec3b>(x, y)[0], seuil) && imageR[x*lig+y] < 0 )
							{
								regions[k].addPix(imgSource.at<cv::Vec3b>(x, y)[2],imgSource.at<cv::Vec3b>(x, y)[1],imgSource.at<cv::Vec3b>(x, y)[0]);
								regions[k].pixelCadre(x, y);
								imageR[x*lig+y] = k;
								changement++;
								//ajoute des pixels voisin dans la liste des pixels à visiter
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
									if(y+1 < imgSource.cols)
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
								if(y+1 < imgSource.cols)
								{
									if(imageR[x*lig+(y+1)] != k)
									{
										pixels.push_back(Pixel(x,y+1));
									}
								}
								if(x+1 < imgSource.rows)
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
									if(y+1 < imgSource.cols)
									{
										if(imageR[(x+1)*lig+(y+1)] != k)
										{
											pixels.push_back(Pixel((x+1),y+1));
										}
									}
								}
							}
						}
					}
					//actualise le tableau frontiere
					regions[k].setFrontiere(pixels);
				}
			}
			cout<<"changement "<<changement<<endl;
			if(affTR)
			{
				for(int i = 0; i < imgSource.rows; i++)
				{
					for(int j = 0; j < imgSource.cols; j++)
					{
						if(imageR[i*lig+j] == -1)
						{
							imgFiltre.at<cv::Vec3b>(i,j)[2] = 255.;
							imgFiltre.at<cv::Vec3b>(i,j)[1] = 255.;
							imgFiltre.at<cv::Vec3b>(i,j)[0] = 255.;
						}
						else
						{
							imgFiltre.at<cv::Vec3b>(i,j)[2] = regions[imageR[i*lig+j]].moyR();
							imgFiltre.at<cv::Vec3b>(i,j)[1] = regions[imageR[i*lig+j]].moyV();
							imgFiltre.at<cv::Vec3b>(i,j)[0] = regions[imageR[i*lig+j]].moyB();
						}
					}
				}
				cv::imshow("lenaRegion", imgFiltre);
				cv::waitKey(5);
			}
		}while(changement !=0);
		cv::waitKey();
		if(germe > 1)
		{
			germe = germe/2;
		}
		else
		{
			germe = 0;
		}
	}

	//rendu de l'imgFiltre
	if(aleatoire)
	{
		for(int i = 0; i < regions.size(); i++)
		{
			pix[2] = rand()%256;
			pix[1] = rand()%256;
			pix[0] = rand()%256;
			regionCoul.push_back(pix);
		}
	}

	for(int i = 0; i < imgSource.rows; i++)
	{
		for(int j = 0; j < imgSource.cols; j++)
		{
			if(imageR[i*lig+j] == -1)
			{
				imgFiltre.at<cv::Vec3b>(i,j)[2] = 255.;
				imgFiltre.at<cv::Vec3b>(i,j)[1] = 255.;
				imgFiltre.at<cv::Vec3b>(i,j)[0] = 255.;
			}
			else
			{
				if(aleatoire)
				{
					imgFiltre.at<cv::Vec3b>(i,j) = regionCoul[imageR[i*lig+j]];
				}
				else
				{
					imgFiltre.at<cv::Vec3b>(i,j)[2] = regions[imageR[i*lig+j]].moyR();
					imgFiltre.at<cv::Vec3b>(i,j)[1] = regions[imageR[i*lig+j]].moyV();
					imgFiltre.at<cv::Vec3b>(i,j)[0] = regions[imageR[i*lig+j]].moyB();
				}
			}
		}
	}

	cv::imshow("lenaRegion", imgFiltre);
	cv::waitKey ();

	for(int i = 0; i < imgSource.rows; i++)
	{
		for(int j = 0; j < imgSource.cols; j++)
		{
			if(j != 0)
			{
				if(imageR[i*lig+j] != imageR[i*lig+(j-1)])
				{
					if(imageR[i*lig+j] != -1)
					{
						imgFiltre.at<cv::Vec3b>(i,j)[2] = 0;
						imgFiltre.at<cv::Vec3b>(i,j)[1] = 0;
						imgFiltre.at<cv::Vec3b>(i,j)[0] = 0;
					}
					if(imageR[i*lig+(j-1)] != -1)
					{
						imgFiltre.at<cv::Vec3b>(i,j-1)[2] = 0;
						imgFiltre.at<cv::Vec3b>(i,j-1)[1] = 0;
						imgFiltre.at<cv::Vec3b>(i,j-1)[0] = 0;
					}
				}
			}
			if(i != 0)
			{
				if(imageR[i*lig+j] != imageR[(i-1)*lig+j])
				{
					if(imageR[i*lig+j] != -1)
					{
						imgFiltre.at<cv::Vec3b>(i,j)[2] = 0;
						imgFiltre.at<cv::Vec3b>(i,j)[1] = 0;
						imgFiltre.at<cv::Vec3b>(i,j)[0] = 0;
					}
					if(imageR[(i-1)*lig+j] != -1)
					{
						imgFiltre.at<cv::Vec3b>(i-1,j)[2] = 0;
						imgFiltre.at<cv::Vec3b>(i-1,j)[1] = 0;
						imgFiltre.at<cv::Vec3b>(i-1,j)[0] = 0;
					}
				}
			}
		}
	}
	return(imgFiltre);
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
	cv::Mat imgGrisN = normalise(imgGris);
	cv::Mat imgN = normalise(imgO);
	cv::Mat imgNF = normalise(imgF);
	cv::Mat imgS = regionGrowing(imgO,30.,0,true,true);
	//cv::Mat imgSF = regionGrowing(imgO,40.,0,false,false);


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
	cv::imshow("Image Region Contour", imgS);
	cv::waitKey ();
	//cv::imshow("Image Region Contour Filtre", imgSF);
	//cv::waitKey ();

	DoACupOfCoffee();

	//ecriture
	cv::imwrite(path2,imgN);

	std::cout << "Appuyez sur une touche pour continuer" << std::endl;
	cv::waitKey ();
	return EXIT_SUCCESS;
}

