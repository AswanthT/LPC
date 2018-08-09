#include<iostream>
#include<fstream>
#include<vector>
#include<cmath>
using namespace std;

struct header	{
    char chunk_id[4];
    int chunk_size;
    char format[4];
    char subchunk1_id[4];
    int subchunk1_size;
    short int audio_format;
    short int num_channels;
    int sample_rate;
    int byte_rate;
    short int block_align;
    short int bits_per_sample;
    char subchunk2_id[4];
    int subchunk2_size;
};

void getCor(short* data, float* result,int n)	{
	/*
	The function assumes the input process is stationary.
	data 	=> input array
	result	=> correlation matrix(output array)
	n 	=> length of input array
	*/
	for(int i = 0; i < n; i++)	{
		result[i] = 0;
		for(int j = 0; j <= i; j++)
			result[i] += data[j]*data[n-1-i+j];
		result[i] /= n;
		result[2*n-2-i] = result[i];
	}
}

int main()	{

	header meta;
	ifstream inFile;
	int n=0;
	vector<short> data;
	inFile.open("aud.wav");
	if (!inFile.is_open()) {
    		cout << "Unable to open the file"<<endl;
    		exit(1);
	}
	inFile.read((char*)&meta, sizeof(meta));
	if(meta.num_channels==2)	{
		short buffer[2];
		while(!inFile.eof())	{
			inFile.read((char*)buffer, sizeof(buffer));
			data.push_back((buffer[0]+buffer[1])/2);
			n++;
		}
	}
	else	{
		short buffer[2];
		while(!inFile.eof())	{
			inFile.read((char*)buffer, sizeof(buffer));
			data.push_back(buffer[0]);
			data.push_back(buffer[1]);
		}
	}
	inFile.close();
	ofstream outFile, coeff;
	outFile.open("decorrelated.wav");
	coeff.open("coeff.bin");
	coeff.write((char*)&meta, sizeof(meta));
	outFile.write((char*)&meta, sizeof(meta));
	int order,imax;
	n = 180;
	float gamma;
	float coefficient;
	short temp[180];
	short r_temp[180];
	short buffer[180];
	short sample;
	int g=0;
	int w1=0, w2;
	cout<<"enter order";
	cin>>order;
	float* r_gap= new float[359];
	float* gap = new float[359];
	float ap[40] = {0};
	float r_ap[40] = {0};
	imax=data.size()/180;
	coeff.write((char*)&imax,sizeof(int));
	coeff.write((char*)&order,sizeof(int));
	cout<<order<<endl;
	//Entering main loop
	for(int i=0;i<imax;i++)	{
		for(int l=0;l<n;l++)	{
			temp[l]=data[w1];
			r_temp[l] = data[w1];
			w1++;
		}
		getCor(temp,gap,180);
		for(int j = 0; j< order+2; j++)	{
			ap[j] = 0;
			r_ap[j] = 0;
		}
		r_ap[1] = 1;
		ap[0] = 1;
		for(int p=0;p<order;p++)	{
			gamma=-gap[180+p]/gap[179];
			w2 = p+1;
			//updating gap function
			for(int j = 2*n-2; j > p; j--)
				r_gap[w2++] = gap[j];
			for(int j = 0; j<p+1; j++)
				r_gap[j] = 0;
			for(int y=0;y<359;y++)
				gap[y] +=gamma*r_gap[y];
			//
			//lattice for filter coefficient
			for(int j = 0; j<p+2; j++)
				ap[j] += gamma*r_ap[j];
			for(int j = 0; j<p+3; j++)
				r_ap[j] = ap[p+2-j];
			//
			//lattice form realization of prediction error filter
			for(int j = 179; j > 0; j--)
				r_temp[j] = r_temp[j-1];	// multiplying e_(n) by Z^(-1) or delaying
			r_temp[0] = 0;					// adding a zero at the left end
			for(int j = 0; j < 180; j++)
				buffer[j] = temp[j];		// storing e(n) temporarily 
			for(int j = 0; j < 180; j++)
				temp[j] += gamma*r_temp[j];	//updating e(n) in lattice
			for(int j = 0; j < 180; j++)
				r_temp[j] += gamma*buffer[j];	//updatig e_(n) in lattice
		}
		ap[0] = sqrt(gap[179]);
		for(int j = 0; j < order+1; j++)	{
			coefficient = ap[j];
			coeff.write((char*)&coefficient, 4);
		}
		for(int j = 0; j < 180; j++)	{
			sample = temp[j];
			outFile.write((char*)&sample,2);
		}
	}
	cout<<"Till here"<<endl;
	outFile.close();
	coeff.close();
	for(int i = 0; i<order+1; i++)
		cout<<ap[i]<<endl;
	return 0;
}
		
