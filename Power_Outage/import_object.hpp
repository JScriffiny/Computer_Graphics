#ifndef IMPORTOBJ_HPP
#define IMPORTOBJ_HPP

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "shape.hpp"




class ImportOBJ{
    public:
        ImportOBJ();

        /** Returns a new Shape object after loading the .OBJ/.MTL files
          * Only provide the base name (without .OBJ/.MTL extension) */
        Shape_Struct loadFiles(std::string name_without_file_extension);
        bool debugOutput = true;

        int getNumCombined();
        int getTexture();

    private:
        struct CompleteVertex {
            glm::vec3 Position;
            glm::vec3 Normal;
            glm::vec2 TexCoords;
            glm::vec3 Color;
            glm::vec3 sColor;
        };

        void readMTLFile(std::string fName);
        void readOBJFile(std::string fName);
        Shape_Struct genShape_Struct();
        void reset();

        int curMat = -1;
        int texture = -1;


        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> textCoords;
        std::vector<CompleteVertex> combinedData;
        std::map<std::string, int> matAbbrev;
        std::vector<glm::vec3> matDiffuse;
        std::vector<glm::vec3> matSpecular;

        void readLineFace(std::string line);
        void readFace(std::string lineSegment);
        glm::vec3 getVec3(std::string line);
        glm::vec2 getVec2(std::string line);
};



#endif // IMPORTOBJ_HPP
