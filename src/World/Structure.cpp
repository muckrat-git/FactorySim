#include "Other/Types.cpp"
#include "Tiles/Tile.cpp"

class Structure {
    public:
    u16 width, height;
    SerializedTile * tiles;

    u8 * Serialize(int * s) {
        const int tileCount = width * height;

        // Calculate size of final byte array
        int size = 0;
        for(int i = 0; i < tileCount; ++i) {
            size += tiles[i].GetSize();
        }
        u8 * bytes = (u8*)malloc(size + 4);

        // Reinterpret and set first 4 bytes to width and height
        ((u16*)bytes)[0] = width;
        ((u16*)bytes)[1] = height;

        
        // Get serialized tile data
        int offset = 4;
        for(int tileIndex = 0; tileIndex < tileCount; ++tileIndex) {
            u8 * tile = tiles[tileIndex].ToBytes();
            int s = tiles[tileIndex].GetSize();
            for(int i = 0; i < s; ++i) {
                bytes[i+offset] = tile[i];
            }
            offset += s;
        }

        (*s) = size + 4;
        return bytes;
    }

    void Deserialize(u8 * bytes) {
        // Get width and height
        width = ((u16*)bytes)[0];
        height = ((u16*)bytes)[1];
        
        // Allocate tiles
        tiles = new SerializedTile[width * height];
        
        // Get tiles
        int offset = 4;
        for(int tileIndex = 0; tileIndex < width*height; ++tileIndex) {
            u8 * buffer = bytes + offset;
            tiles[tileIndex].FromBytes(buffer);
            offset += tiles[tileIndex].GetSize();
        }
    }

    void set(int x, int y, Tile t) {
        tiles[(y * width) + x] = SerializedTile(t);
    }
    SerializedTile get(int x, int y) {
        return tiles[(y * width) + x];
    }

    void Unload() {
        for(int i = 0; i < width * height; ++i) {
            tiles[i].Unload();
        }
        delete tiles;
    }
};