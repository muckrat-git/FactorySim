#pragma once

#include "Other/Types.cpp"

enum TileDataEntries {
    TILEDATA_GAS_MASS
};

// TileData contains all other tile information not exposed by TileSprite (not required by the GPU)
class TileData {
    private:
    // _Entry contains the tile data value and its corresponding index for access
    struct _Entry {
        u8 index;
        float value;
    };
    float invalid;  // lvalue returned when an invalid entry is requested

    public:
    _Entry * data;  // Internal structure used for map style indexing 

    u8 size;        // Quantity of data entries
    
   
    TileData(){}
    // Primary constructor detirmines entry count permanantly
    TileData(u8 size) {
        this->size = size;
        this->data = new _Entry[size];
    }

    void Unload() {
        delete this->data;
    }

    float& operator[](u8 entry) {
        invalid = 0;    // Reset invalid lvalue
        
        for(u8 i = 0; i < size; ++i) {
            if(data[i].index == entry) return data[i].value;
        }
        return invalid;
    }

    // Overwrite a data entry
    void overwrite(u8 index, u8 entry, float value) {
        data[index].index = entry;
        data[index].value = value;
    }

    // Check if data contains entry
    bool entryExists(u8 entry) {
        for(u8 i = 0; i < size; ++i) {
            if(data[i].index == entry) return true;
        }
        return false;
    }
};