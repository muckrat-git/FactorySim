class GasOutlet : public TileClass {
    public:
    cstr name = "_Gas Outlet";
    cstr texture = "resources/tiles/gas_outlet.png";
        
    bool transparent = true;
    bool solid = true;
    bool small = true;

    static void Update(WorldContext ctx, TileSprite& sprite, TileData& data, Vec2<int> position) {
        sprite.gas = GAS_OXYGEN;
        data[TILEDATA_GAS_MASS] += (2000) / UPDATES_PER_SECOND;
    }
};