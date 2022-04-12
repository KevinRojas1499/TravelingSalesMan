#include "GeneticAlgorithm.h"
#include <bits/stdc++.h>
#include "GeneticAlgorithmCases.cpp"

class TravelingSalesman: public GeneticAlgorithm{
	public:
		class Path{
			public:
				float length;
				int* order;
				int size;
				
				Path(){
					size = 0;
				}
				~Path(){
					delete[] order;
				}
				
				void setSize(int size_){
					if(size>0){
						//Class has already been initialized and we are reseting
						delete [] order;
					}
					
					order = new int[size_];
					size = size_;
				}
				float getLength(){
					return length;
				}
				int* getOrder(){
					return order;
				}
				void set(Path &path1){
					length = path1.length;
					for(int i = 0; i<size; i++){
						order[i] = path1[i];
					}
				}
				
				int operator [](int i){
					return order[i];
				}
				void print(){
					for(int i = 0; i<size; i++){
						std::cout<<order[i]<<" ";
					}					
					std::cout<<"Tiene longitd "<<length<<"\n";
				}				
				
				void setOrder(int i, int val){
					order[i] = val;
				}
				void setLength(float lengthOfPath){
					length = lengthOfPath;
				}
		};
		float** points;
		int numberOfPoints,root;
		Path* population, bestPath;
		~TravelingSalesman(){			
			delete[] population;
		}
		float calculateLength(int m){
			float lengthOfPath = dist(root,population[m][0]);
			for(int i = 0; i<numberOfPoints-1; i++){
				lengthOfPath+=dist(population[m][i],population[m][i+1]);
			}
			lengthOfPath+=dist(population[m][numberOfPoints-1],root);
			return lengthOfPath;
		}
		void sortPopulation(){
			for(int i = 0; i<populationSize; i++){
				for(int j = 0; j<populationSize-i-1; j++){
					if(population[j].getLength()>population[j+1].getLength()){
						Path temp;
						temp.setSize(numberOfPoints);
						temp.set(population[j]);
						population[j].set(population[j+1]);
						population[j+1].set(temp);
					}
				}
			}
			
		}
		void printPopulation(){
			for(int i = 0; i<populationSize; i++){
				std::cout<<i<<":";
				population[i].print();
			}
		}		
		TravelingSalesman(int populationSize_,float elitismRatio_,float mutationRatio_,float sporadicRatio_,float** points_, int &numOfPoints):GeneticAlgorithm(populationSize_,elitismRatio_,mutationRatio_,sporadicRatio){
			points = points_;
			numberOfPoints = numOfPoints-1;
			root = numberOfPoints;
			this->populationSize = populationSize_;
			population = new Path[populationSize];
			for(int i = 0; i<populationSize; i++) 
				population[i].setSize(numberOfPoints);
			bestPath.setSize(numberOfPoints);
			bestPath.setLength(FLT_MAX);
		}			
		void reset(int populationSize_,float elitismRatio_,float mutationRatio_,float sporadicRatio_,float** points_, int &numOfPoints){			
			delete[] population;
			
			points = points_;
			numberOfPoints = numOfPoints-1;
			root = numberOfPoints;
			this->populationSize = populationSize_;
			population = new Path[populationSize];
			for(int i = 0; i<populationSize; i++) 
				population[i].setSize(numberOfPoints);
			bestPath.setSize(numberOfPoints);
			bestPath.setLength(FLT_MAX);
									
			this->elitismRatio = elitismRatio_;
			this->mutationRatio = mutationRatio_;
			this->sporadicRatio = sporadicRatio_;
			this->currentEpoch = 0;
		}
		float dist(int i, int j){
			return sqrt((points[i][0]-points[j][0])*(points[i][0]-points[j][0])+(points[i][1]-points[j][1])*(points[i][1]-points[j][1])); 
		}
		int selection(){
			int k = 0;
			for(int i = 0; i<populationSize; i++){
				float prob = ((float) rand()/(RAND_MAX));
				float distribution = exp(-10.0*((float)i/populationSize)*((float)i/populationSize));
				if(prob<=distribution){
					population[k].set(population[i]);
					k++;
				}
				if(k>=populationSize*elitismRatio){
					return k;
				}
			}
			return k;
		}		
		void mutation(int m){
			float mutate = ((float) rand()/(RAND_MAX));
			bool willMutate = mutate<=mutationRatio?1:0;
			if(willMutate){
				int l = rand()%numberOfPoints;
				int r = rand()%numberOfPoints;
				int temp = population[m][l];
				population[m].setOrder(l,population[m][r]);
				population[m].setOrder(r,temp);
			}
			population[m].setLength(calculateLength(m));			
			if(population[m].getLength()<bestPath.getLength()){
				bestPath.set(population[m]);
			}
		}		
		void createChild(int m,int p1,int p2){
			bool chosen[numberOfPoints];
			for(int i = 0; i<numberOfPoints; i++) chosen[i] = 0;
			int l = rand()%numberOfPoints;
			int r = rand()%numberOfPoints;
			if(l>r) std::swap(l,r);
			int beg = rand()%numberOfPoints;
			beg = std::min(beg,l);
			for(int i = l; i<=r; i++){
				population[m].setOrder(beg+i-l,population[p1][i]);
				chosen[population[p1][i]] = 1;
			}

			int k = 0;
			for(int i = 0; i<numberOfPoints; i++){
				if(beg<=k && k<=beg+r-l){
					k++;
					i--;
					continue;
				}
				if(!chosen[population[p2][i]]){
					chosen[population[p2][i]] = 1;
					population[m].setOrder(k,population[p2][i]); 
					k++;
				}

			}
			mutation(m);
		}
		void crossOver(int k){
			int m = populationSize-k;
			while(m<populationSize){
				int p1 = rand()%k, p2 = rand()%k;
				createChild(m,p1,p2);
				m++;
			}
		}	
		void fill(){
			for(int i = 0; i<populationSize; i++){
				for(int j = 0; j<numberOfPoints; j++){
					population[i].setOrder(j,j);
				}
				for(int j = 0; j<numberOfPoints; j++){
					int switchPos = rand()%numberOfPoints;
					int temp = population[i][j];
					population[i].setOrder(j,population[i][switchPos]);
					population[i].setOrder(switchPos, temp);
				}
				population[i].setLength(calculateLength(i));
				if(population[i].getLength()<bestPath.length){
					bestPath.set(population[i]);
				}
			}
		}
		int sporadicGeneration(int k){
			int m = k;
			while(m<populationSize && m<k+sporadicRatio*populationSize){
				for(int j = 0; j<numberOfPoints; j++){
					population[m].setOrder(j,j);
				}
				for(int j = 0; j<numberOfPoints; j++){
					int switchPos = rand()%numberOfPoints;
					int temp = population[m][j];
					population[m].setOrder(j,population[m][switchPos]);
					population[m].setOrder(switchPos, temp);
				}
				population[m].setLength(calculateLength(m));
				if(population[m].getLength()<bestPath.length){
					bestPath.set(population[m]);
				}
				m++;
			}
			return m;
		}				
		void epoch(){
			if(currentEpoch == 0){
				fill();
				currentEpoch++;
				return;
			}
			sortPopulation();
			int k = selection();//Chooses who stays
			int m = sporadicGeneration(k); 
			crossOver(m);//Chooses mating partners and generates new population		
			currentEpoch++;
		}
		float run(int rounds){
			int k = 0; 
			while(k<rounds){
				epoch();
				k++;
			}
			bestPath.print();
			return bestPath.length;
		}
		float getBestIndividual(){
			return bestPath.length;
		}		
		void drawBestIndividual(float** points_,unsigned int &numPoints){
			
		}
};


int main(){
	
	srand(0);
	int populationSize_ = 100;
	float elitismRatio_ = .3;
	float mutationRatio_ = .05;
	float sporadicRatio_ = .05;
	
	
	float** points1 = new float*[15];	
	int numOfPoints1 = 15;
	for(int i = 0; i<15; i++){
		points1[i] = new float[2];
		for(int j = 0; j<2; j++)
			points1[i][j] = caseLAU15[i][j];
	}
	
	float** points2 = new float*[59];
	int numOfPoints2 = 59;	
	for(int i = 0; i<59; i++){
		points2[i] = new float[2];
		for(int j = 0; j<2; j++)
			points2[i][j] = caseWG59[i][j];
	}
	
	float** points3 = new float*[128];
	int numOfPoints3 = 128;
	for(int i= 0; i<128; i++){
		points3[i] = new float[2];
		for(int j = 0; j<2; j++)
			points3[i][j] = caseSGB128[i][j];
	}
	
	float** points4 = new float*[312];
	int numOfPoints4 = 312;
	for(int i= 0; i<312; i++){
		points4[i] = new float[2];
		for(int j = 0; j<2; j++)
			points4[i][j] = caseUSCA312[i][j];
	}
	
	
	//Primer caso de prueba
	TravelingSalesman *problema1 = new TravelingSalesman(populationSize_,elitismRatio_,mutationRatio_,sporadicRatio_,points1,numOfPoints1);
	std::cout<<problema1->run(1000)<<"\n";

	populationSize_ = 120;
	elitismRatio_ = .6;
	mutationRatio_ = .05;
	sporadicRatio_ = .02;
	//Segundo caso de prueba
	problema1->reset(populationSize_,elitismRatio_,mutationRatio_,sporadicRatio_,points2,numOfPoints2);
	std::cout<<problema1->run(10000)<<"\n";
	
	populationSize_ = 130;
	elitismRatio_ = .6;
	mutationRatio_ = .05;
	sporadicRatio_ = .02;
	
	//Tercer caso de prueba
	problema1->reset(populationSize_,elitismRatio_,mutationRatio_,sporadicRatio_,points3,numOfPoints3);
	std::cout<<problema1->run(10000)<<"\n";
	
	populationSize_ = 100;
	elitismRatio_ = .45;
	mutationRatio_ = .05;
	sporadicRatio_ = .02;
	problema1->reset(populationSize_,elitismRatio_,mutationRatio_,sporadicRatio_,points4,numOfPoints4);
	std::cout<<problema1->run(10000)<<"\n";
	
	for(int i = 0; i<15; i++)
		delete [] points1[i];
	delete[] points1;
	
	
	
	for(int i = 0; i<numOfPoints2; i++){
		delete[] points2[i];
	}
	delete[] points2;
	
	for(int i = 0; i<numOfPoints3; i++){
		delete[] points3[i];
	}
	
	delete[] points3;
	
	for(int i = 0; i<numOfPoints4; i++){
		delete[] points4[i];
	}
	delete[] points4;

	delete problema1;
}
