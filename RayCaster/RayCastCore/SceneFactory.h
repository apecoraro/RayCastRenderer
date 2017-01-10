#pragma once

//#include "Group.h"

#include <string>
#include <map>
#include <sstream>

namespace cs500
{
    class SceneFactory
    {
    public:
        SceneFactory();
        bool parseSceneFile(const std::string& pathToFile, void* pUserData);

        //typedef bool (*)(std::stringstream&, void*) ObjectParserFuncPtr;
        void addObjectParser(const std::string& objectIdentifier,
                             bool (*funcPtr)(std::stringstream&, void*));
        void setSurfacePropertiesParser(bool (*funcPtr)(std::stringstream&, void*)) 
        { _pSurfacePropertiesParser = funcPtr; }
    private:
        typedef std::map<std::string, bool (*)(std::stringstream&, void*)> ObjectParsers;
        ObjectParsers _objectParsers;
        bool (*_pSurfacePropertiesParser)(std::stringstream&, void*);
    };
};