#include<iostream>
#include<fstream>
#include<vector>
#include<random>

using namespace std;

struct header	{
    char chunk_id[4];
    int chunk_size;
    char format[4];
    char subchunk1_id[4];
    int subchunk1_size;
    short int audio_format;
    short int num_channels;
    int sample_rate;			// sample_rate denotes the sampling rate.
    int byte_rate;
    short int block_align;
    short int bits_per_sample;
    char subchunk2_id[4];
    int subchunk2_size;
};

int main()  {
    vector<short> out;
    header meta;
    ifstream inFile;
    ofstream outFile;
    inFile.open("coeff.bin");
    outFile.open("reconstructed.wav");
    inFile.read((char*)&meta,sizeof(meta));
    outFile.write((char*)&meta,sizeof(meta));

    int imax, order;
    inFile.read((char*)&imax,sizeof(int));
    cout<<imax<<endl<<order<<endl;
    inFile.read((char*)&order,sizeof(int));
    cout<<imax<<endl<<order<<endl;

    float coeff[40] = {0};
    short noise[180];
    short reconstructed[180],sample;
    float sd;
    default_random_engine generator;
    normal_distribution<double> distribution(0,1);
    for(int i = 0; i<imax; i++) {
        inFile.read((char*)&sd, sizeof(float));
        distribution = normal_distribution<double>(0,sd);
        for(int j = 0; j<180; j++)
            reconstructed[j] = distribution(generator);
        for(int j = 0; j<order; j++)    {
            inFile.read((char*)&coeff[j],sizeof(float));
            cout<<coeff[j]<<" ";
        }
        cout<<endl;
        for(int j = 0; j < order+1; j++)
            for(int k = 0; k<j; k++)
                reconstructed[j] -= coeff[k]*reconstructed[j-k-1];
        for(int j = order+1; j<180; j++)
            for(int k = 0; k<order; k++)
                reconstructed[j] -= coeff[k]*reconstructed[j-k-1];
        for(int j = 0; j< 180; j++) {
            sample = reconstructed[j];
            out.push_back(sample);
            outFile.write((char*)&sample, sizeof(short));
        }

    }
    inFile.close();
    outFile.close();
    ofstream out1;
    out1.open("reconstructed_filtered.wav");
    out1.write((char*)&meta,sizeof(meta));
    cout<<"Enter the order of averaging filter : ";
    cin>>order;
    float sample1;
    for(int i = 0; i< order; i++)   {
        sample1 = 0;
        for(int j = 0; j<=i; j++)
            sample1 += out[j];
        sample = sample1/order;
        out1.write((char*)&sample,sizeof(short));
    }
    for(int i = order; i<out.size(); i++) {
        sample1 = 0;
        for(int j = 0; j<order; j++)
            sample1 += out[i-j];
        sample = sample1/order;
        out1.write((char*)&sample,sizeof(short));
    }
    out1.close();
    return 0;

}