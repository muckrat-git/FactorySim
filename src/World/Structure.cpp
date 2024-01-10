#include "Other/Types.cpp"
#include "Tiles/Tile.cpp"

class Structure {
    public:
    int tileCount;
    SerializedTile * tiles;

    void Serialize(u8 * bytes) {
        // Calculate size of final byte array
        int size = 0;
        for(int i = 0; i < tileCount; ++i) {
            size += tiles[i].GetSize();
        }
        bytes = new u8[size];
        
        // Get serialized tile data
        int offset = 0;
        for(int tileIndex = 0; tileIndex < tileCount; ++tileIndex) {
            u8 * tile = tiles[tileIndex].ToBytes();
            int s = tiles[tileIndex].GetSize();
            for(int i = 0; i < s; ++i) {
                bytes[i+offset] = tile[i];
            }
            offset += s;
        }
    }

    
};