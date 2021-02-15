#ifndef ZONEMAPS_H
#define ZONEMAPS_H
#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>

template <typename T>
struct zone
{
    bool leaf;               //Is this zone a leaf node in the ZoneTree
    std::vector<T> elements; // vector/list that holds elements in this zone
    std::vector<zone<T> > subZones; //vector that holds subZones in this zone; size 0 if leaf Zone
    T min;                   // minimum element in the zone
    T max;                   // maximum element in the zone
    uint size;               // no. of elements in current zone
};

template <typename T>
class zonemap
{
    std::vector<T> elements; // vector/list that will hold all elements. Can convert to array for faster processing
    zone<T> zoneTree; //Once std::vector<zone<T> > zones; now just a single Zone
    uint num_zones;             // keeps track of the number of zones/partitions to the array/vector of elements
    uint num_elements_per_zone; // keeps track of number of elements per zone (items in every zone has to be less than or equal to this counter)

public:
    /**
     * Purpose: basic constructor that initializes all data members. 
     * Param: number of elements per zone 
     * returns: N/A 
     */
    
    zonemap(std::vector<T> _elements, uint _num_elements_per_zone);

    /**
     * Purpose: builds the zone using the elements array/list. Maintains invariant that no. of elements in every zone
     *          is less than or equal to num_elements_per_zone 
     * Param: ptr to elements vector,  number of elements per zone
     * returns: vector of leaf zones; sorted by zone.min 
     */
    std::vector<zone<T> > buildSortedLeafVector(std::ofstream* log, std::vector<T>* elements, uint num_elements_per_zone);

    /**
     * Purpose: builds the zoneTree using the LeafVector of leaf Zones. Maintains invariant that no. of ones in each Zone
     *          is less than or equal to num_elements_per_zone 
     * Param: ptr sorted leafVector, number of elements per zone
     * Returns:RootZone
     */
    zone<T>  buildZoneTree(std::ofstream* log, std::vector<zone<T> >* leafVector, uint num_elements_per_zone);

    /**
     * Purpose: sorts the elements in the vector/list/array. 
     * Param: N/A 
     * returns: N/A 
     */
    void sort_elements();

    /**
     * Purpose: Query a key using the zonemap 
     * Param: Key to be queried  
     * returns: True if element is found, else returns false 
     */
    bool query(std::ofstream* log, T key);

    /**
     * Purpose: Helper to query; handling recursion
     * Param: log file to write to, key to be found, current zone;
     * return True if element is found
     */
    bool subquery(std::ofstream* log, T key, zone<T>* currZone);
    
    /**
     * Purpose: Query a range of keys using the zonemap that are between a lower and higher limit
     * Param: Lower and upper limit of range to be queried  
     * returns: list of elements found using the query 
     */
    std::vector<T> query(std::ofstream* log, T low, T high);

    /**
     * Purpose: Helper to query; handling recursion
     * Param: log file to write to, range, current zone;
     * return vector of elements in range
     */
    std::vector<T> subquery(std::ofstream* log, T low, T high, zone<T>* currZone);

};
#endif