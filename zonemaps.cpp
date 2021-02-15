#include <cmath>
#include <fstream>
#include "zonemaps.h"


template<typename T>
zonemap<T>::zonemap(std::vector<T> _elements, uint _num_elements_per_zone)
{
    // constructor
    std::ofstream zmFile;
    zmFile.open("ZoneMapBuildLog.txt");
    zmFile<<"ZoneMapBuildLog" << std::endl;
    this->elements = _elements;
    this->num_elements_per_zone = _num_elements_per_zone;
    zmFile<<"Number of Elements{" +std::to_string(elements.size()) +  
    "}, Number of elements per Zone{" +std::to_string(num_elements_per_zone) + "}, ";



    this->num_zones = ceil(((float) elements.size())
                            /((float)num_elements_per_zone));
    zmFile<<"Number of Zones{"+ std::to_string(num_zones) +"}" << std::endl << std::endl 
    << "Begin" << std::endl;

    std::vector<zone<T> > leafVector = buildSortedLeafVector(&zmFile,&elements, num_elements_per_zone);
    
    zone<T>  zoneTree = buildZoneTree(&zmFile, &leafVector, 5);

    this->zoneTree = zoneTree;
    zmFile.close();
}


template<typename T>
std::vector<zone<T> > zonemap<T>::buildSortedLeafVector(std::ofstream* log, std::vector<T>* elements, uint num_elements_per_zone){
    int position = 0;
    std::vector<zone<int> >  partLeafVector;
    while(position < (*elements).size()){
        int zoneMin = (*elements).at(position);
        int zoneMax = (*elements).at(position);
        std::vector<int> tempZoneElements; 
        int i = 0;
        *log << "Begin new Zone" <<std::endl;
        for(; i<num_elements_per_zone && (position) < (*elements).size(); i++){
            *log <<std::to_string(position);
            int element = (*elements).at(position);
            *log << "\tNew Element: " + std::to_string((int) element); 
            //Assuming int here for testing
            if (element > zoneMax){
                zoneMax = element;
                *log << " New Max Element!";
            }
            if (element < zoneMin){
                zoneMin = element;
                *log << " New Min Element!";
            }
            *log <<std::endl;
            tempZoneElements.push_back(element);
            (position)++;
        }
        zone<int> tempZone;
        tempZone.elements = tempZoneElements;
        tempZone.min = zoneMin;
        tempZone.max = zoneMax;
        tempZone.size = i;
        tempZone.leaf = true;
        partLeafVector.push_back(tempZone);
        *log <<"End of Zone: Min{" + std::to_string(zoneMin) + "}, Max{" + std::to_string(zoneMax)+ "}, Size{" 
        + std::to_string(i) + "}" <<std::endl<<std::endl;
    for( int k =1; k < partLeafVector.size(); k++){
            zone<int> currZone = partLeafVector.at(k);
            int j = k -1;
            while (j >= 0 && partLeafVector.at(j).min > currZone.min){
                partLeafVector.at(j+1) = partLeafVector.at(j);
                j--;
            }
            partLeafVector.at(j+1) = currZone;
        }
    }
    *log <<std::endl<<"ZONE ORDER"<<std::endl;
    for(zone<int> zone: partLeafVector){
        *log <<"Zone: Min{" + std::to_string(zone.min) + "}, Max{" + std::to_string(zone.max)+ "}, Size{" 
        + std::to_string(zone.size) + "}" <<std::endl;
    }
    return partLeafVector;
}

template<typename T>
zone<T> zonemap<T>::buildZoneTree(std::ofstream* log,std::vector<zone<T> >* leafVector, uint num_ele){
    *log<<std::endl<<"BUILDING ZONE TREE"<<std::endl;
    std::vector<zone<int> > update = (*leafVector);
    int x =0;
    while(update.size() > 1){
        std::vector<zone<T> > temp = update;
        update.clear();
        int length = 0;
        while(length < temp.size()){    
            zone<T> testZone = temp.at(length);
            testZone.leaf = false;
            testZone.elements.clear();
            testZone.subZones.clear();
            for(int size = length + num_ele; length < size && length < temp.size(); length++) {
                *log<< "Input Zone: Min{" + std::to_string(temp.at(length).min) + "}, Max{" + std::to_string(temp.at(length).max)+ "}" <<std::endl;
                if (testZone.min > temp.at(length).min){
                    testZone.min = temp.at(length).min;
                }
                if (testZone.max < temp.at(length).max ){
                    testZone.max = temp.at(length).max;
                }
                testZone.subZones.push_back(temp.at(length));          
            }
            *log<< std::endl <<"Test Zone: Min{" + std::to_string(testZone.min) + "}, Max{" + std::to_string(testZone.max)+ "}" <<std::endl;
            int k = 0;
            for (zone <T> zone : testZone.subZones){
                *log<<"Sub Zone: "<< "Num{" + std::to_string(k) + "}, Min{" +  std::to_string(zone.min) + "}, Max{" + std::to_string(zone.max)+ "}" <<std::endl;
                k++;
            }
            *log<<std::endl;
            update.push_back(testZone);
        }
    }
    return update.at(0);
}



template<typename T>
bool zonemap<T>::query(std::ofstream* log, T key)
{
    if(key >= zoneTree.min && key <= zoneTree.max){
        for(zone<T> zone: zoneTree.subZones){
            //*log<<"Root Querying Zone: Min{" + std::to_string(zone.min) + "}, Max{" + std::to_string(zone.max)+ "}" <<std::endl;
            if(subquery(log, key, &zone) == true){
                return true;
            }
        }
    }
    //*log<<"Not Found"<<std::endl;
    return false;
}

template<typename T>
bool zonemap<T>::subquery(std::ofstream* log, T key, zone<T>* currZone){
    if((*currZone).leaf != true){
        for(zone<T> zone : (*currZone).subZones){
            if(key >= zone.min && key <= zone.max){
                //*log<<"Sub Querying Zone: Min{" + std::to_string(zone.min) + "}, Max{" + std::to_string(zone.max)+ "}" <<std::endl;
                if(subquery(log, key, &zone)){
                    return true;
                }
            }
        }
        return false;
    } 
    else if((*currZone).leaf == true) {
        if(key >= (*currZone).min && key <= (*currZone).max){
            //*log<<"Searching Zone: Min{" + std::to_string(currZone.min) + "}, Max{" + std::to_string(currZone.max)+ "}" <<std::endl;
            for(T element: (*currZone).elements){
                if (element == key){
                    return true;
                }
            }
            return false;
        }
    }
    //*log<<"I am root"<<std::endl;
    return false;
}


template<typename T>
std::vector<T> zonemap<T>::query(std::ofstream* log,T low, T high){
    std::vector<T> results;
    int i =0;
    //*log <<"Input:" <<"low{" << low << "}, high{" << high << "}" << std::endl;
    if(low <= zoneTree.max && high >= zoneTree.min){  
        //*log<<"Valid Query!"<<std::endl; 
        for(zone<T> zone: zoneTree.subZones){
            if(low <= zone.max && high >= zone.min){
                //*log<<"Querying Zone: Min{" + std::to_string(zone.min) + "}, Max{" + std::to_string(zone.max)+ "}" <<std::endl;
                std::vector<T> temp = subquery(log,low, high, &zone);
                results.insert(results.end(),temp.begin(),temp.end());
                }
            i++;
        }
    }
    else{
        //*log<< "Out of Range"<< std::endl;
    }
    return results;
}

template<typename T>
std::vector<T> zonemap<T>::subquery(std::ofstream* log,T low, T high, zone<T>* currZone){
    std::vector<T> results;
    if((*currZone).leaf != true){
      for(zone<T> zone : (*currZone).subZones){
        if(low <= zone.max && high >= zone.min){ 
           // *log<<"Querying Zone: Min{" + std::to_string(zone.min) + "}, Max{" + std::to_string(zone.max)+ "}" <<std::endl;
            std::vector<T> temp = subquery(log, low, high, &zone);
            results.insert(results.end(),temp.begin(),temp.end());
          }
      }
    } else if((*currZone).leaf == true) {
        //*log<<"Leaf Zone: Min{" + std::to_string(currZone.min) + "}, Max{" + std::to_string(currZone.max)+ "}" <<std::endl;
        if(low <= (*currZone).max && high >= (*currZone).min){ 
           // *log<<"Searching Zone: Min{" + std::to_string(currZone.min) + "}, Max{" + std::to_string(currZone.max)+ "}" <<std::endl;
            for(T element: (*currZone).elements){
                if(low <= element && high >= element){
                    results.push_back(element);
                }
            }
        }
    }
    return results;
}