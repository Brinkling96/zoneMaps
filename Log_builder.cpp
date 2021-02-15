#include <iostream>
#include <fstream>
#include <vector>

#include "Log_builder.hh"

using namespace std;

int addHeaderToLog(ofstream* file, string fileName, string input_file, string test_case){
  string dataline = buildDataline(input_file);

  *file << fileName << endl;
  *file<< "Test: " << test_case << ";"<<endl;
  *file<<dataline<<endl<<endl;
    
  *file<<"BEGIN" <<endl;

  return 0;
}

string buildDataline(string input_file){
   string delimiter = "_";


  string workingString = input_file;

    vector<string> workload; 
    int position = 0;
    while((position = workingString.find(delimiter)) != std::string::npos){
        workload.push_back(workingString.substr(0,position));
        workingString.erase(0, position+delimiter.length());
    }
    workload.push_back(workingString);

    if(workload.size() != 5 ){
        cout<< "Error, likely unexpected format:" <<endl;
        cout << input_file << endl;
        return "";
    }
    //here is the string we are building
    string dataline = "";

    for(int i = 0; i< workload.size(); i++){
        switch(i){
            case 0:
                workload.at(i) = workload.at(i).substr(2, workload.at(i).length());
                dataline += workload.at(i) + ":";
                break;
            case 1:
                workload.at(i) = "Number of elements{" + workload.at(i) + "}";
                dataline += workload.at(i) + ", ";
                break;
            case 2:
                workload.at(i) = "Noise percentage{" + workload.at(i) + "}";
                dataline += workload.at(i) + ", ";
                break;
            case 3:
                workload.at(i) = "Window Threshold{" + workload.at(i) + "}";
                dataline += workload.at(i) + ", ";
                break;
            case 4:
                workload.at(i) = "Seed{" + workload.at(i).substr(0, workload.at(i).length()-4) + "}";
                dataline += workload.at(i) + ";";
                break;
        }
    }
  return dataline;
}