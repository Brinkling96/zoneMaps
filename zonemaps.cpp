#include <cmath>
#include <fstream>
#include "zonemaps.h"


//SEE Functions with logs for comments, not both Also see header file for functions overviews


template<typename T>
zonemap<T>::zonemap(bool log, std::vector<T> _elements, uint _num_elements_per_zone)
{
    // constructor
    
    //file for logging building of zones 
    std::ofstream zmFile;
    if(log){
        zmFile.open("ZoneMapBuildLog.txt");
        zmFile<<"ZoneMapBuildLog" << std::endl;
    }
    //set elements and elements per zone members
    this->elements = _elements;
    this->num_elements_per_zone = _num_elements_per_zone;
    if(log){
        zmFile<<"Number of Elements{" +std::to_string(elements.size()) +  
        "}, Number of elements per Zone{" +std::to_string(num_elements_per_zone) + "}, ";
    }

    //num of (leaf) zones, would update to calculate number of total zones
    this->num_zones = ceil(((float) elements.size())
                            /((float)num_elements_per_zone));
    
    if(log){
        zmFile<<"Number of Zones{"+ std::to_string(num_zones) +"}" << std::endl << std::endl 
        << "Begin" << std::endl;

        //This builds the vector containing leaf zones
        std::vector<zone<T> > leafVector = buildSortedLeafVector(&zmFile,&elements, num_elements_per_zone);

        //This takes the leaf vector and returns the resulting root zone of the zoneTree
        this->zoneTree= buildZoneTree(&zmFile, &leafVector, 2);
        zmFile.close();

    }
    else{  
        std::vector<zone<T> > deafVector =buildSortedLeafVector(&elements,num_elements_per_zone);
        this->zoneTree = buildZoneTree(&deafVector, 2);
    }
     
}


template<typename T>
std::vector<zone<T> > zonemap<T>::buildSortedLeafVector(std::ofstream* log, std::vector<T>* elements, uint num_elements_per_zone){
    int position = 0; // position in the element vector
    std::vector<zone<T> >  partLeafVector; //leaf vector we are building
    while(position < (*elements).size()){ //dont stop till all elements processed
        //setup temp vars to be later used to build zone
        int zoneMin = (*elements).at(position);
        int zoneMax = (*elements).at(position);
        std::vector<T> tempZoneElements; 
        int i = 0;
        *log << "Begin new Zone" <<std::endl;
        for(; i<num_elements_per_zone && (position) < (*elements).size(); i++){ //dont stop until we reach num of elements per zone or position reaches num of elements
            *log <<std::to_string(position);
            int element = (*elements).at(position);
            *log << "\tNew Element: " + std::to_string((int) element); 
            //Assuming int here for testing
            if (element > zoneMax){ //check if this element is the new Max for this zone
                zoneMax = element;
                *log << " New Max Element!";
            }
            if (element < zoneMin){ //new min?
                zoneMin = element;
                *log << " New Min Element!";
            }
            *log <<std::endl;
            tempZoneElements.push_back(element); //Done checking, push element onto element vector for zone
            position++; //keep moving
        }//end for

        //setup zone with the data we just collected
        zone<T> tempZone;
        tempZone.elements = tempZoneElements;
        tempZone.min = zoneMin;
        tempZone.max = zoneMax;
        tempZone.size = i;
        tempZone.leaf = true;
        tempZone.height = 0; // leafs start at height 0 in my implementation. This way root knows height of tree.
        partLeafVector.push_back(tempZone);

        *log <<"End of Zone: Min{" + std::to_string(zoneMin) + "}, Max{" + std::to_string(zoneMax)+ "}, Size{" 
        + std::to_string(i) + "}" <<std::endl<<std::endl;
   
    }

    //sort zones by min size using insertion sort
    //Insertion sort since zones should be nearly sorted, beating other
    //sorting algorithms run times.
    for( int k =1; k < partLeafVector.size(); k++){
        zone<T> currZone = partLeafVector.at(k);
        int j = k -1;
        while (j >= 0 && partLeafVector.at(j).min > currZone.min){
            partLeafVector.at(j+1) = partLeafVector.at(j);
            j--;
        }
        partLeafVector.at(j+1) = currZone;
    }

    //logging
    *log <<std::endl<<"ZONE ORDER"<<std::endl;
    for(zone<T> zone: partLeafVector){
        *log <<"Zone: Min{" + std::to_string(zone.min) + "}, Max{" + std::to_string(zone.max)+ "}, Size{" 
        + std::to_string(zone.size) + "}" <<std::endl;
    }

    return partLeafVector;
}

//skipping unlogged function, see comments above
template<typename T>
std::vector<zone<T> > zonemap<T>::buildSortedLeafVector(std::vector<T>* elements, uint num_elements_per_zone){
    int position = 0;
    std::vector<zone<T> >  partLeafVector;
    while(position < (*elements).size()){
        int zoneMin = (*elements).at(position);
        int zoneMax = (*elements).at(position);
        std::vector<T> tempZoneElements; 
        int i = 0;
        for(; i<num_elements_per_zone && (position) < (*elements).size(); i++){
            T element = (*elements).at(position);
            //Assuming int here for testing
            if (element > zoneMax){
                zoneMax = element;
            }
            if (element < zoneMin){
                zoneMin = element;
            }
            tempZoneElements.push_back(element);
            (position)++;
        }
        zone<T> tempZone;
        tempZone.elements = tempZoneElements;
        tempZone.min = zoneMin;
        tempZone.max = zoneMax;
        tempZone.size = i;
        tempZone.leaf = true;
        tempZone.height = 0;
        partLeafVector.push_back(tempZone);
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
    return partLeafVector;
}


template<typename T>
zone<T> zonemap<T>::buildZoneTree(std::ofstream* log,std::vector<zone<T> >* leafVector, uint num_ele){
    *log<<std::endl<<"BUILDING ZONE TREE"<<std::endl;
    std::vector<zone<int> > update = (*leafVector); //copy leafVector from previous funciton
    int height = 1; // set height to one, we are building the first internal layer next
    while(update.size() > 1){// keep going untill we reach 1 node length, meaning the root of the tree
        std::vector<zone<T> > temp = update; //copy current tree layer to temp and clear update
        //update will house the next layer now 
        update.clear();
        int length = 0; //set length to zero
        while(length < temp.size()){    
            //create copy of the zone at length 
            zone<T> testZone = temp.at(length);
            //clear the old members of the copy that arent needed
            //also added new values to memebers where we can.
            ///we want to keep old min and max
            testZone.leaf = false;
            testZone.elements.clear();
            testZone.subZones.clear();
            testZone.height = height;
            for(int size = length + num_ele; length < size && length < temp.size(); length++) { //loop until we reach the end of current temp, or we reach the maximun number of childern, num ele.
                *log<< "Input Zone: Min{" + std::to_string(temp.at(length).min) + "}, Max{" + std::to_string(temp.at(length).max)+ "}" <<std::endl;
                if (testZone.min > temp.at(length).min){ //update max and min when needed in our new super zone
                    testZone.min = temp.at(length).min;
                }
                if (testZone.max < temp.at(length).max ){
                    testZone.max = temp.at(length).max;
                }
                testZone.subZones.push_back(temp.at(length));//push that new aggregate zone to the test zone subZone vector;
            }
            *log<< std::endl <<"Test Zone: Min{" + std::to_string(testZone.min) + "}, Max{" + std::to_string(testZone.max)+ "}" <<std::endl;
            int k = 0;
            for (zone <T> zone : testZone.subZones){
                *log<<"Sub Zone: "<< "Num{" + std::to_string(k) + "}, Min{" +  std::to_string(zone.min) + "}, Max{" + std::to_string(zone.max)+ "}" <<std::endl;
                k++;
            }
            *log<<std::endl;
            update.push_back(testZone);
            //push each new testZone to the update vector
        }
        height++; //increment height between update vectors
    }
    *log << "Total Height: "<<  update.at(0).height << std::endl;
    return update.at(0); //return Root zone
}

//see logged version for comments
template<typename T>
zone<T> zonemap<T>::buildZoneTree(std::vector<zone<T> >* leafVector, uint num_ele){
    std::vector<zone<int> > update = (*leafVector);
    int height = 1;
    while(update.size() > 1){
        std::vector<zone<T> > temp = update;
        update.clear();
        int length = 0;
        while(length < temp.size()){    
            zone<T> testZone = temp.at(length);
            testZone.leaf = false;
            testZone.elements.clear();
            testZone.subZones.clear();
            testZone.height = height;
            for(int size = length + num_ele; length < size && length < temp.size(); length++) {
                if (testZone.min > temp.at(length).min){
                    testZone.min = temp.at(length).min;
                }
                if (testZone.max < temp.at(length).max ){
                    testZone.max = temp.at(length).max;
                }
                testZone.subZones.push_back(temp.at(length));          
            }
            int k = 0;
            for (zone <T> zone : testZone.subZones){
                k++;
            }
            update.push_back(testZone);
        }
        height++;
    }
    return update.at(0);
}



template<typename T>
bool zonemap<T>::query(std::ofstream* log, T key){
    *log<<"Element:" + std::to_string((int) key)<<std::endl; //TODO: assuming int for log, T does not have a to_string
    *log << "Zone Tree: Min{" <<zoneTree.min<< "}, Max{" <<zoneTree.max <<"}"<<std::endl;
    if(key >= zoneTree.min && key <= zoneTree.max){  //check if element is possibly in Data
        for(zone<T> zone: zoneTree.subZones){ //For each subZone on root...
            *log<<"Root Querying Zone: Min{" + std::to_string(zone.min) + "}, Max{" + std::to_string(zone.max)+ "}" <<std::endl;
            if(subquery(log, key, &zone) == true){ //check if element exsists
                *log<<"FOUND"<<std::endl<<std::endl;
                return true;    //if found return immediately
            }
        }
    }
    *log<<"Not Found"<<std::endl;
    return false;
}

//see logged version for comments
template<typename T>
bool zonemap<T>::query(T key){
    if(key >= zoneTree.min && key <= zoneTree.max){
        for(zone<T> zone: zoneTree.subZones){
            if(subquery(key, &zone) == true){
                return true;
            }
        }
    }
    return false;
}


template<typename T>
bool zonemap<T>::subquery(std::ofstream* log, T key, zone<T>* currZone){
    if((*currZone).leaf != true){ //If this isnt a leaf zone, we have more recusive calls to go
        for(zone<T> zone : (*currZone).subZones){
            if(key >= zone.min && key <= zone.max){// dont bother subquering if element cannot be in zone
                *log<<"Sub Querying Zone: Min{" + std::to_string(zone.min) + "}, Max{" + std::to_string(zone.max)+ "}" <<std::endl;
                if(subquery(log, key, &zone)){  //keep going
                    return true; //an time we recieve a true, we are done and can exit
                }
            }
        }
        return false; //we jsut check every zone under current zones parent, did not find element
    } 
    else if((*currZone).leaf == true) { //Finally at a leaf zone, recursion can end here
        if(key >= (*currZone).min && key <= (*currZone).max){//check if it could be in this zone again TODO probably not needed, already checked above
            *log<<"Searching Zone: Min{" + std::to_string((*currZone).min) + "}, Max{" + std::to_string((*currZone).max)+ "}" <<std::endl;
            for(T element: (*currZone).elements){
                if (element == key){
                    return true; //if found, return true right away
                }
            }
            return false; //we just check every element in zone and couldnt find our key, return false
        }
    }
    *log<<"I am root"<<std::endl;
    return false;
}
 //see logged version for comments
template<typename T>
bool zonemap<T>::subquery(T key, zone<T>* currZone){
    if((*currZone).leaf != true){
        for(zone<T> zone : (*currZone).subZones){
            if(key >= zone.min && key <= zone.max){
                if(subquery(key, &zone)){
                    return true;
                }
            }
        }
        return false;
    } 
    else if((*currZone).leaf == true) {
        if(key >= (*currZone).min && key <= (*currZone).max){
            for(T element: (*currZone).elements){
                if (element == key){
                    return true;
                }
            }
            return false;
        }
    }
    return false;
}



template<typename T>
std::vector<T> zonemap<T>::query(std::ofstream* log,T low, T high){

    //Initialize vars to store results and track 
    std::vector<T> results;
    *log <<"Input:" <<"low{" << low << "}, high{" << high << "}" << std::endl;
    *log << "Zone Tree: Min{" <<zoneTree.min<< "}, Max{" <<zoneTree.max <<"}"<<std::endl;
    if(low <= zoneTree.max && high >= zoneTree.min){   //make sure range is a valid range in data
        *log<<"Valid Query!"<<std::endl; 
        for(zone<T> zone: zoneTree.subZones){ 
            if(low <= zone.max && high >= zone.min){ //if element in range of the subZone , query it
                *log<<"Querying Zone: Min{" + std::to_string(zone.min) + "}, Max{" + std::to_string(zone.max)+ "}" <<std::endl;
                std::vector<T> temp = subquery(log,low, high, &zone);
                results.insert(results.end(),temp.begin(),temp.end()); // append results
            }
        }
    }
    else{
        *log<< "Out of Range"<< std::endl;
    }
    return results;
}
//see logged fucntion for comments
template<typename T>
std::vector<T> zonemap<T>::query(T low, T high){
    std::vector<T> results;
    int i =0;
    if(low <= zoneTree.max && high >= zoneTree.min){  
        for(zone<T> zone: zoneTree.subZones){
            if(low <= zone.max && high >= zone.min){
                std::vector<T> temp = subquery(low, high, &zone);
                results.insert(results.end(),temp.begin(),temp.end());
                }
            i++;
        }
    }
    return results;
}

template<typename T>
std::vector<T> zonemap<T>::subquery(std::ofstream* log,T low, T high, zone<T>* currZone){
    std::vector<T> results;
    //recusive section; again testing if the subZones possible contain elements in the range
    if((*currZone).leaf != true){
      for(zone<T> zone : (*currZone).subZones){
        if(low <= zone.max && high >= zone.min){ 
            *log<<"Querying Zone: Min{" + std::to_string(zone.min) + "}, Max{" + std::to_string(zone.max)+ "}" <<std::endl;
            std::vector<T> temp = subquery(log, low, high, &zone);
            results.insert(results.end(),temp.begin(),temp.end());
          }
      }
      //end of recusion section, check all element in zone if the zone possibly contains elements in range.
    } else if((*currZone).leaf == true) {
        *log<<"Leaf Zone: Min{" + std::to_string((*currZone).min) + "}, Max{" + std::to_string((*currZone).max)+ "}" <<std::endl;
        if(low <= (*currZone).max && high >= (*currZone).min){ 
            *log<<"Searching Zone: Min{" + std::to_string((*currZone).min) + "}, Max{" + std::to_string((*currZone).max)+ "}" <<std::endl;
            for(T element: (*currZone).elements){
                if(low <= element && high >= element){
                    results.push_back(element);
                }
            }
        }
    }
    return results;
}

//see logged function for comments
template<typename T>
std::vector<T> zonemap<T>::subquery(T low, T high, zone<T>* currZone){
    std::vector<T> results;
    if((*currZone).leaf != true){
      for(zone<T> zone : (*currZone).subZones){
        if(low <= zone.max && high >= zone.min){ 
            std::vector<T> temp = subquery(low, high, &zone);
            results.insert(results.end(),temp.begin(),temp.end());
          }
      }
    } else if((*currZone).leaf == true) {
        if(low <= (*currZone).max && high >= (*currZone).min){ 
            for(T element: (*currZone).elements){
                if(low <= element && high >= element){
                    results.push_back(element);
                }
            }
        }
    }
    return results;
}