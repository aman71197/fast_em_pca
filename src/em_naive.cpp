#include <bits/stdc++.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/LU>
#include <Eigen/SVD>
#include "genotype.h"
#include "helper.h"

using namespace Eigen;
using namespace std;
clock_t total_begin = clock();

int MAX_ITER;
genotype g;
int k,p,n;

MatrixXf c; //(p,k)
MatrixXf x; //(k,n)
MatrixXf v; //(p,k)

options command_line_opts;

bool debug = false;
bool check_accuracy = false;

MatrixXf get_evec(MatrixXf &c)
{
	JacobiSVD<MatrixXf> svd(c, ComputeThinU | ComputeThinV);
	MatrixXf c_orth(k,p);
	MatrixXf data(k,n);
	c_orth = (svd.matrixU()).transpose();
	for(int n_iter=0;n_iter<n;n_iter++)
	{
		for(int k_iter=0;k_iter<k;k_iter++)
		{
			float res=0;
			for(int p_iter=0;p_iter<p;p_iter++)
				res+= c_orth(k_iter,p_iter)*(g.get_geno(p_iter,n_iter));
			data(k_iter,n_iter)=res;
		}
	}
	MatrixXf means(k,1);
	for(int i=0;i<k;i++)
	{
		float sum=0.0;
		for(int j=0;j<n;j++)
			sum+=data(i,j);
		means(i,0)=sum/(n*1.0);
	}
	data = data - (means*(MatrixXf::Constant(1,n,1)));
	MatrixXf cov(k,k);
	cov = data*(data.transpose())*(1.0/(n));
	JacobiSVD<MatrixXf> svd_cov(cov, ComputeThinU | ComputeThinV);
	MatrixXf to_return(p,k);
	to_return =(c_orth.transpose())*svd_cov.matrixU() ;
	return to_return;
}


float get_accuracy(MatrixXf &u)
{
	
	MatrixXf temp(k,k);
	temp = (u.transpose()) * v ;
	float accuracy = 0.0;
	for(int j=0;j<k;j++)
	{
		float sum=0.0;
		for(int i=0;i<k;i++)
			sum += temp(i,j)*temp(i,j);
		accuracy += sqrt(sum);
	}
	return (accuracy/k);
}

MatrixXf get_reference_evec()
{

	MatrixXf y_m(p,n);
	for(int i=0;i<p;i++)
	{
		for(int j=0;j<n;j++)
			y_m(i,j) = g.get_geno(i,j);
	}
	MatrixXf cov(p,p);
	if(debug)
		printf("Calculating covariance\n");
	cov = y_m*(y_m.transpose())*(1.0/n);
	if(debug)
		printf("Calculating SVD\n");
	JacobiSVD<MatrixXf> svd_cov(cov, ComputeThinU | ComputeThinV);
	MatrixXf to_return(p,k);
	MatrixXf U(p,k);
	U = svd_cov.matrixU();
	for(int i=0;i<p;i++)
	{
		for(int j=0;j<k;j++)
			to_return(i,j) = U(i,j);
	}

	return to_return;
}


int main(int argc, char const *argv[])
{

	clock_t io_begin = clock();
	
	parse_args(argc,argv);
	g.read_genotype_naive((char*)command_line_opts.GENOTYPE_FILE_PATH);	
	MAX_ITER =  command_line_opts.max_iterations ; 
	k = command_line_opts.num_of_evec ;
	debug = command_line_opts.debugmode ;
	check_accuracy = command_line_opts.getaccuracy;
	p = g.Nsnp;
	n = g.Nindv;
	c.resize(p,k);
	x.resize(k,n);
	v.resize(p,k);
	srand((unsigned int) time(0));
	
	clock_t io_end = clock();

	c = MatrixXf::Random(p,k);
	
	ofstream c_file;
	if(debug){
		c_file.open("cvals_orig_naive.txt");
		c_file<<c<<endl;
		c_file.close();
		printf("Read Matrix\n");
	}
	if(check_accuracy){
		v = get_reference_evec(); 
	 	printf("Computed Reference Eigen Vectors\n");	
	}
	cout<<"Running on Dataset of "<<g.Nsnp<<" SNPs and "<<g.Nindv<<" Individuals"<<endl;
	if(check_accuracy)
		cout<<endl<<"Iterations vs accuracy"<<endl;
	clock_t it_begin = clock();
	for(int i=0;i<MAX_ITER;i++)
	{
		if(debug)
			printf("Iteration %d -- E Step\n",i);
		
		MatrixXf c_temp(k,p);
		c_temp = ((c.transpose()*c).inverse())*(c.transpose());
		for(int n_iter=0;n_iter<n;n_iter++)
		{
			for(int k_iter=0;k_iter<k;k_iter++)
			{
				float res=0;
				for(int p_iter=0;p_iter<p;p_iter++)
					res+= c_temp(k_iter,p_iter)*(g.get_geno(p_iter,n_iter));
				x(k_iter,n_iter)=res;
			}
		}

		if(debug)
			printf("Iteration %d -- M Step\n",i);
		
		MatrixXf x_temp(n,k);
		x_temp = (x.transpose()) * ((x*(x.transpose())).inverse());
		for(int p_iter=0;p_iter<p;p_iter++)
		{
			for(int k_iter=0;k_iter<k;k_iter++)
			{
				float res=0;
				for(int n_iter=0;n_iter<n;n_iter++)
					res+= g.get_geno(p_iter,n_iter)*x_temp(n_iter,k_iter);
				c(p_iter,k_iter)=res;
			}
		}
		if(check_accuracy){
			MatrixXf eigenvectors(p,k);
			eigenvectors = get_evec(c);
			cout<<"Iteration "<<i<<" "<<get_accuracy(eigenvectors)<<endl;	
		}
		
	}
	clock_t it_end = clock();
	c_file.open("cvals_end_naive.txt");
	c_file<<c<<endl;
	c_file.close();
	ofstream x_file;
	x_file.open("xvals_naive.txt");
	x_file<<x<<endl;
	x_file.close();
	clock_t total_end = clock();
	double io_time = double(io_end - io_begin) / CLOCKS_PER_SEC;
	double avg_it_time = double(it_end - it_begin) / (MAX_ITER * 1.0 * CLOCKS_PER_SEC);
	double total_time = double(total_end - total_begin) / CLOCKS_PER_SEC;
	cout<<"IO Time:  "<< io_time << "\nAVG Iteration Time:  "<<avg_it_time<<"\nTotal runtime:   "<<total_time<<endl;
	return 0;
}