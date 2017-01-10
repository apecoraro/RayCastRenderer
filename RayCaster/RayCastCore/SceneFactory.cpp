#include "SceneFactory.h"
#include "Sphere.h"
#include "Box.h"
#include "Camera.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <sstream>

using namespace cs500;

SceneFactory::SceneFactory() :
    _pSurfacePropertiesParser(NULL)
{
}

void SceneFactory::addObjectParser(const std::string& objectIdentifier,
                                   bool (*funcPtr)(std::stringstream&, void*))
{
    _objectParsers.insert(std::make_pair(objectIdentifier, funcPtr));
}

bool SceneFactory::parseSceneFile(const std::string& pathToFile,
                                  void* pUserData)
{
    std::ifstream fstream(pathToFile.c_str());

    if(!fstream.is_open())
        return false;

    while(!fstream.eof())
    {
        std::string line;
        std::getline(fstream, line);

        std::stringstream sstream;
        sstream << line;
        
        std::string token;
        sstream >> token;

        ObjectParsers::iterator itr = _objectParsers.find(token);
        if(itr != _objectParsers.end())
        {
            if(!itr->second(sstream, pUserData))
                return false;
        }
        else//reset get pointer
            sstream.seekg(0);

        if(_pSurfacePropertiesParser != NULL &&
           !sstream.eof())
        {
            //attempt to parse the surface properties
            _pSurfacePropertiesParser(sstream, pUserData);
        }
    }

    return true;
}