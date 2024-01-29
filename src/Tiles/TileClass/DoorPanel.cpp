class DoorPanel : public TileClass {
    public:
    cstr name = "Door Panel";
    cstr texture;
        
    bool transparent = false;
    bool solid = true;
    bool interactable = true;

    u8 rotation = 2;

    DoorPanel(u8 type) {
        if(type == 0) texture = "resources/tiles/door_panel1.png";
        else texture = "resources/tiles/door_panel2.png";
    }

    static void Interact(WorldContext ctx, TileSprite& sprite, TileData& data, Vec2<int> position){
        u8 end = TILE_DOORPANEL_B;
        u8 door = TILE_DOOR;

        // Direction to check tiles
        Vec2<int> check = Vec2<float>{cosf(sprite.rotation * PI / 2.0f), sinf(sprite.rotation * PI / 2.0f)}.iRound();
        if(sprite.index == TILE_DOORPANEL_B) {
            check = check * -1;
            end = TILE_DOORPANEL_A;
        }

        // Check if door is closed or open
        door = ctx.GetTileSprite(position.x + check.x, position.y + check.y).index;
        if(door != TILE_DOOR && door != TILE_DOOR_OPEN) return;

        // Check for door tiles
        u8 tile = door;
        int i = 1;
        int max;
        while(tile == door && tile != end) {
            tile = ctx.GetTileSprite(position.x + check.x*i, position.y + check.y*i).index;
            ++i;
        }
        max = i-1;

        if(tile != end) return;

        // Door is valid, Open/Close it
        i = 1;
        if(door == TILE_DOOR) door = TILE_DOOR_OPEN;
        else door = TILE_DOOR;

        while(i < max) {
            Tile t = DefaultTile((TileType)door);
            t.sprite.rotation = ctx.GetTileSprite(position.x + check.x*i, position.y + check.y*i).rotation;
            ctx.Set(position.x + check.x*i, position.y + check.y*i, t);
            ++i;
        }
    }
};