#include "Level.h"
#include <map>
#include <cmath>
#include "pugixml/pugixml.hpp"
#include "Utility.h"


Level::Level(std::string path_){
    path = path_;
    savedBackground = NULL;
    completed = false;
}

bool Level::isCompleted(){
    return completed;
}

void Level::setCompleted(){
    completed = true;
}

void Level::load(SDL_Renderer* renderer, int windowWidth, int windowHeight){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path.c_str());
    if (!result)
        SDL_Log("Can't load level:%s", path.c_str());

    requiredOutput = doc.child("leveldata").attribute("requiredOutput").as_string();
    description = doc.child("leveldata").attribute("description").as_string();

    const int mapWidth = doc.child("map").attribute("width").as_int();
    const int mapHeight = doc.child("map").attribute("height").as_int();

    std::string tileSetPath = doc.child("map").child("tileset").child("image").attribute("source").value();
    pugi::xml_node tilesetNode = doc.child("map").child("tileset");
    const int firstGid = tilesetNode.attribute("firstgid").as_int();
    const int tileWidth = tilesetNode.attribute("tilewidth").as_int();
    const int tileHeight = tilesetNode.attribute("tileheight").as_int();
    const int columns = tilesetNode.attribute("columns").as_int();
    int renderTileWidth = ceil(windowWidth / static_cast<float>(mapWidth));
    int renderTileHeight = ceil(windowHeight / static_cast<float>(mapHeight));
    Sprite sprite(tileSetPath.c_str(), renderer);
    levelTileWidth = renderTileWidth;
    levelTileHeight = renderTileHeight;

    for(pugi::xml_node node : doc.child("map").children("layer")){
        int layerWidth = node.attribute("width").as_int();
        std::string str = node.child("data").last_child().value();
        int y = 0;
        int x = 0;
        for(int tile : Utility::splitStringInt(str, ',')){
            if(x == layerWidth){x = 0;y++;}
            if(tile == 0){x++;continue;}
            SDL_Rect srcRect{
                x: (tile - firstGid) % columns * tileWidth, y: (tile - firstGid) / columns * tileHeight,
                w: tileWidth,   h: tileHeight
            };
            SDL_Rect dstRect{
                x: renderTileWidth * x, y: renderTileHeight * y,
                w: renderTileWidth, h: renderTileHeight
            };
            SDL_RenderCopy(renderer, sprite.getTexture(), &srcRect, &dstRect);
            x++;
        }
    }

    loadObjects(renderer, windowWidth, windowHeight);
    for(LevelObject object : objects){
        if(object.getType() != ObjectType::DescriptionNoSee){
            object.draw(renderer, levelTileWidth, levelTileHeight);
        }
    }

    if(savedBackground != NULL){
        SDL_DestroyTexture(savedBackground);
    }

    SDL_Surface* savedSurface = SDL_CreateRGBSurfaceWithFormat(0, windowWidth, windowHeight, 32, SDL_PIXELFORMAT_ARGB8888);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, savedSurface->pixels, savedSurface->pitch);
    savedBackground = SDL_CreateTextureFromSurface(renderer, savedSurface);
    SDL_FreeSurface(savedSurface);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void Level::loadObjects(SDL_Renderer* renderer, int windowWidth, int windowHeight){
    if(!objects.empty()){
        objects.erase(objects.begin(), objects.end());
    }
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path.c_str());
    if (!result)
        SDL_Log("Can't load level:%s", path.c_str());

    const int mapWidth = doc.child("map").attribute("width").as_int();
    const int mapHeight = doc.child("map").attribute("height").as_int();

    std::string tileSetPath = doc.child("map").child("tileset").child("image").attribute("source").value();
    pugi::xml_node tilesetNode = doc.child("map").child("tileset");
    const int firstGid = tilesetNode.attribute("firstgid").as_int();
    const int tileWidth = tilesetNode.attribute("tilewidth").as_int();
    const int tileHeight = tilesetNode.attribute("tileheight").as_int();
    const int columns = tilesetNode.attribute("columns").as_int();

    for(pugi::xml_node node : doc.child("map").children("objectgroup")){
        for(pugi::xml_node objtile : node.children("object")){
            int gid = objtile.attribute("gid").as_int();
            int posX = objtile.attribute("x").as_float() / (mapWidth * tileWidth) * windowWidth;
            int posY = objtile.attribute("y").as_float() / (mapHeight * tileHeight) * windowHeight - levelTileHeight;
            SDL_Rect srcRect{
                x: (gid - firstGid) % columns * tileWidth, y: (gid - firstGid) / columns * tileHeight,
                w: tileWidth,   h: tileHeight
            };
            std::map<std::string, std::string> properties;
            for(pugi::xml_node prop : objtile.child("properties").children("property")){
                properties[prop.attribute("name").as_string()] = prop.attribute("value").as_string();
            }
            if(properties["type"] == "description"){
                LevelObject object(tileSetPath, ObjectType::Description, posX, posY, srcRect, properties["value"]);
                objects.push_back(object);
            }else if(properties["type"] == "description!"){
                LevelObject object(tileSetPath, ObjectType::DescriptionNoSee, posX, posY, srcRect, properties["value"]);
                objects.push_back(object);
            }else if(properties["type"] == "terminal"){
                LevelObject object(tileSetPath, ObjectType::Terminal, posX, posY, srcRect, "");
                objects.push_back(object);
            }else{
                SDL_Log("Undefined object type: %s", properties["type"].c_str());
            }
        }
    }

}

void Level::draw(SDL_Renderer* renderer){
    SDL_RenderCopy(renderer, savedBackground, NULL, NULL);
}

const int& Level::getTileWidth(){
    return levelTileWidth;
}

const int& Level::getTileHeight(){
    return levelTileHeight;
}

std::vector<LevelObject>& Level::getObjects(){
    return objects;
}

const std::string& Level::getRequiredOutput(){
    return requiredOutput;
}

const std::string& Level::getDescription(){
    return description;
}


InterpreterInfo& Level::getInterpreterInfo(){
    return interpreterInfo;
}

Level::~Level(){
    SDL_DestroyTexture(savedBackground);
}
