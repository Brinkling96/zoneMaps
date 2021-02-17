#include <fstream>
#include <vector>
#include <iostream>
#include <random>
#include <set>
#include <algorithm>
#include <functional>
#include <chrono>
#include "zonemaps.h"
#include "zonemaps.cpp"
#include "Log_builder.hh"

using namespace std;

std::vector<int> generatePointQueries(std::vector<int> data, int n)
{
  std::vector<int> queries(data.begin(), data.end());

  // add a few elements out of range
  int non_existing_counter = (data.size() * 0.1);
  std::uniform_int_distribution<int> dist{n, (int)(1.8 * n)};
  // Initialize the random_device
  std::random_device rd;
  // Seed the engine
  std::mt19937_64 generator(rd());
  std::set<int> non_existing;
  while (non_existing.size() != non_existing_counter)
  {
    non_existing.insert(dist(generator));
  }

  queries.insert(queries.end(), non_existing.begin(), non_existing.end());

  // shuffle indexes
  std::random_shuffle(queries.begin(), queries.end());

  return queries;
}


int main(int argc, char **argv)
{
  if (argc < 4)
  {
    std::cout << "Usage: ./main <input_file> <test_case> <Log Bool, 1 or 0>" << std::endl;
    return 0;
  }


  std::string input_file = argv[1];
  std::string test_case = argv[2];
  std::string log_bool_string = argv[3];

//Indicates whether or not to extensive logging
  bool log = std::stoi(log_bool_string) == 1;

  if (test_case != "test_pq" && test_case != "test_rq_1" && test_case != "test_rq_2" && test_case != "test_rq_3" && test_case != "test_rq_4")
  {
    std::cout << "<test_case> argument has to be one of the following: test_pq, test_rq_1, test_rq_2, test_rq_3 or test_rq_4" << std::endl;
    return 0;
  }


  // read data
  std::ifstream ifs;
  std::vector<int> data;

  ifs.open(input_file, std::ios::binary);
  ifs.seekg(0, std::ios::end);
  size_t filesize = ifs.tellg();
  ifs.seekg(0, std::ios::beg);

  data.resize(filesize / sizeof(int));
  ifs.read((char *)data.data(), filesize);


  //Setting custom number of elements per zone
  int num_ele_per_zone = 100;
  if (data.size()< 100 ){
    num_ele_per_zone = 2;
  }

  //1. ----------------------------- initialize zonemap and build -----------------------------
  //build zonemap
  if(log){
    //ELSE WILL NOT HAVE COMMENTS
    zonemap<int> zoneMap(true,data, num_ele_per_zone); //I make zonemap here

    ofstream file;
    string fileName = test_case +"&&&"+input_file.substr(2,input_file.length()-6)+".txt";
    file.open(fileName);
    addHeaderToLog(&file, fileName,input_file,test_case);


    if (test_case == "test_pq")
    {

      //2. ----------------------------- point queries -----------------------------
      std::vector<int> queries = generatePointQueries(data, data.size());

      auto start = std::chrono::high_resolution_clock::now();
      unsigned long long i =1;
      unsigned long long misses = 0;
      unsigned long long hits = 0;
      for(int query: queries){
        bool result = zoneMap.query(&file,query);
        if (result){
          file << "FOUND" <<std::endl;
          hits++;
        }else{
          file << "NOT FOUND" <<std::endl;
          misses++;
        }
        //This helps me track how fast my code in running while running
        std::cout<<i++<<std::endl;
      }
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      file<<"END" <<endl<<endl;
      unsigned long long point_query_time = duration.count();
      file << "Time taken to perform point queries from zonemap = " << point_query_time << " microseconds" << endl
      << "Results: Hits{" << hits <<"}, " <<"Misses{" <<misses << "}" << endl;
    }
    else if (test_case == "test_rq_1")
    {
      //3. ----------------------------- range queries -----------------------------

      auto start = std::chrono::high_resolution_clock::now();
      // RQ 1: 10 percent to 20 percent
      int n = data.size();
      int low = ((int)n* 0.1);
      int high = ((int)n* 0.2);
      vector<int> results = zoneMap.query(&file,low,high);

      auto stop = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      file<<"END" <<endl<<endl;
      unsigned long long range_query_time = duration.count();
      file << "Time taken to perform range query 1 from zonemap = " << range_query_time << " microseconds" << endl<<
      "Results: Size{" << results.size() <<"}, ExpectedSize{"<< ((int)n* 0.1)<< "}" << endl;
    }
    else if (test_case == "test_rq_2")
    {
      //3. ----------------------------- range queries -----------------------------
      auto start = std::chrono::high_resolution_clock::now();
      // RQ 2: 30 percent to 40 percent
      int n = data.size();
      int low = ((int)n* 0.3);
      int high = ((int)n* 0.4);
      vector<int> results = zoneMap.query(&file,low,high);

      auto stop = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      file<<"END" <<endl<<endl;
      unsigned long long range_query_time = duration.count();
      file << "Time taken to perform range query 2 from zonemap = " << range_query_time << " microseconds" << endl<<
      "Results: Size{" << results.size() <<"}, ExpectedSize{"<< ((int)n* 0.1)<< "}" << endl;
    }
    else if (test_case == "test_rq_3")
    {
      //3. ----------------------------- range queries -----------------------------
      auto start = std::chrono::high_resolution_clock::now();
      // RQ 3: 60 percent to 70 percent
      int n = data.size();
      int low = ((int)n* 0.6);
      int high = ((int)n* 0.7);
      vector<int> results = zoneMap.query(&file,low,high);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      file<<"END" <<endl<<endl;
      unsigned long long range_query_time = duration.count();
      file << "Time taken to perform range query 3 from zonemap = " << range_query_time << " microseconds" << endl<<
      "Results: Size{" << results.size() <<"}, ExpectedSize{"<< ((int)n* 0.1)<< "}" << endl;
    }
    else if (test_case == "test_rq_4")
    {
      //3. ----------------------------- range queries -----------------------------
      auto start = std::chrono::high_resolution_clock::now();
      // RQ 2: 80 percent to 90 percent
      int n = data.size();
      int low = ((int)n* 0.8);
      int high = ((int)n* 0.9);
      vector<int> results = zoneMap.query(&file,low,high);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      file<<"END" <<endl<<endl;
      unsigned long long range_query_time = duration.count();
      file << "Time taken to perform range query 4 from zonemap = " << range_query_time << " microseconds" << endl<<
      "Results: Size{" << results.size() <<"}, ExpectedSize{"<< ((int)n* 0.1)<< "}" << endl;
    }
    file.close();
  } else{
    zonemap<int> zoneMap(false,data, num_ele_per_zone); //I make zonemap here

    ofstream file;
    string fileName = test_case +"&&&"+input_file.substr(2,input_file.length()-6)+".txt";
    file.open(fileName);
    addHeaderToLog(&file, fileName,input_file,test_case);


    if (test_case == "test_pq")
    {

      //2. ----------------------------- point queries -----------------------------
      std::vector<int> queries = generatePointQueries(data, data.size());

      auto start = std::chrono::high_resolution_clock::now();
      unsigned long long i =1;
      unsigned long long misses = 0;
      unsigned long long hits = 0;
      for(int query: queries){
        bool result = zoneMap.query(query);
        if (result){
          hits++;
        }else{
          misses++;
        }
        std::cout<<i++<<std::endl;
      }
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      file<<"END" <<endl<<endl;
      unsigned long long point_query_time = duration.count();
      file << "Time taken to perform point queries from zonemap = " << point_query_time << " microseconds" << endl
      << "Results: Hits{" << hits <<"}, " <<"Misses{" <<misses << "}" << endl;
    }
    else if (test_case == "test_rq_1")
    {
      //3. ----------------------------- range queries -----------------------------

      auto start = std::chrono::high_resolution_clock::now();
      // range query from zonemaps here
      int n = data.size();
      int low = ((int)n* 0.1);
      int high = ((int)n* 0.2);
      vector<int> results = zoneMap.query(low,high);

      auto stop = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      file<<"END" <<endl<<endl;
      unsigned long long range_query_time = duration.count();
      file << "Time taken to perform range query 1 from zonemap = " << range_query_time << " microseconds" << endl<<
      "Results: Size{" << results.size() <<"}, ExpectedSize{"<< ((int)n* 0.1)<< "}" << endl;
    }
    else if (test_case == "test_rq_2")
    {
      //3. ----------------------------- range queries -----------------------------
      auto start = std::chrono::high_resolution_clock::now();
      // range query from zonemaps here
      int n = data.size();
      int low = ((int)n* 0.3);
      int high = ((int)n* 0.4);
      vector<int> results = zoneMap.query(low,high);

      auto stop = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      file<<"END" <<endl<<endl;
      unsigned long long range_query_time = duration.count();
      file << "Time taken to perform range query 2 from zonemap = " << range_query_time << " microseconds" << endl<<
      "Results: Size{" << results.size() <<"}, ExpectedSize{"<< ((int)n* 0.1)<< "}" << endl;
    }
    else if (test_case == "test_rq_3")
    {
      //3. ----------------------------- range queries -----------------------------
      auto start = std::chrono::high_resolution_clock::now();
      // range query from zonemaps here
      int n = data.size();
      int low = ((int)n* 0.6);
      int high = ((int)n* 0.7);
      vector<int> results = zoneMap.query(low,high);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      file<<"END" <<endl<<endl;
      unsigned long long range_query_time = duration.count();
      file << "Time taken to perform range query 3 from zonemap = " << range_query_time << " microseconds" << endl<<
      "Results: Size{" << results.size() <<"}, ExpectedSize{"<< ((int)n* 0.1)<< "}" << endl;
    }
    else if (test_case == "test_rq_4")
    {
      //3. ----------------------------- range queries -----------------------------
      auto start = std::chrono::high_resolution_clock::now();
      // range query from zonemaps here
      int n = data.size();
      int low = ((int)n* 0.8);
      int high = ((int)n* 0.9);
      vector<int> results = zoneMap.query(low,high);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      file<<"END" <<endl<<endl;
      unsigned long long range_query_time = duration.count();
      file << "Time taken to perform range query 4 from zonemap = " << range_query_time << " microseconds" << endl<<
      "Results: Size{" << results.size() <<"}, ExpectedSize{"<< ((int)n* 0.1)<< "}" << endl;
    }
    file.close();
  }
  
  return 0;
}
