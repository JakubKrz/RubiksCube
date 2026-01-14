#pragma once
#include "GLLoader.h"
#include "Math3D.h"
#include <string>

class Shader
{
private:
    unsigned int ID;
    std::string vertexPath;
    std::string fragmentPath;

    void checkCompileErrors(unsigned int shader, std::string type);

public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    bool compile();
    bool reload();
    void use() const;
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    //void setVec3(const std::string& name, const Vec3& value) const;
    void setMat4(const std::string& name, const Mat4& value) const;
};
