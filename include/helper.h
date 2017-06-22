#ifndef HELPER_H
#define HELPER_H

#include <bits/stdc++.h>
using namespace std;

struct options
{
	char GENOTYPE_FILE_PATH[100];
	int max_iterations ; 
	int num_of_evec ;
	bool getaccuracy ;
	bool debugmode;
};

extern options command_line_opts;

void parse_args(int argc, char const *argv[])
{
	command_line_opts.max_iterations=20;
	command_line_opts.num_of_evec=2;
	command_line_opts.getaccuracy=false;
	command_line_opts.debugmode=false;
	bool got_genotype_file=false;
	for (int i = 1; i < argc; i++) { 
		if (i + 1 != argc){
			if(argv[i] == "-g"){
				strcpy(command_line_opts.GENOTYPE_FILE_PATH,argv[i+1]);
				got_genotype_file=true;
			}
			else if(argv[i] == "-k")
				command_line_opts.num_of_evec = atoi(argv[i+1]);
			else if(argv[i]=="-m")
				command_line_opts.max_iterations = atoi(argv[i+1]);
			else if(argv[i]=="-v")
				command_line_opts.debugmode=true;
			else if(argv[i]=="-a")
				command_line_opts.getaccuracy=true;
			else{
				cout<<"Not Enough or Invalid arguments"<<endl;
				cout<<"Correct Usage is "<<argv[0]<<" -g <genotype file> -k <num_of_evec> -m <max_iterations> -v (for debugmode) -a (for getting accuracy)"<<endl;
				exit(-1);
			}
		}
	}
	if(got_genotype_file==false){
		cout<<"Not Enough or Invalide arguments"<<endl;
		cout<<"Correct Usage is "<<argv[0]<<" -g <genotype file> -k <num_of_evec> -m <max_iterations> -v (for debugmode) -a (for getting accuracy)"<<endl;
		exit(-1);
	}

}

#endif