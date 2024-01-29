#pragma once

#include "Other/Types.cpp"

enum TileDataEntries {
    TILEDATA_GAS_MASS,
    TILEDATA_INTERNAL_MASS,
    TILEDATA_INTERNAL_ELEMENT
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
    bool lock = false;
    _Entry * data;  // Internal structure used for map style indexing 
    u8 size;        // Quantity of data entries
    
   
    TileData(){}
    // Primary constructor detirmines entry count permanantly
    TileData(u8 size) {
        this->size = size;
        this->data = new _Entry[size];
    }

    // Clone tile data
    TileData Clone() {
        TileData ret = TileData(size);
        for(int i = 0; i < size; ++i) {
            ret.overwrite(i, data[i].index, data[i].value);
        }
        return ret;
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