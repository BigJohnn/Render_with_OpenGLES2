//
//  GLSLProgram.hpp
//  VFMorph
//
//  Created by 薛健 on 16/6/10.
//  Copyright © 2016年 Jian Xue. All rights reserved.
//

#ifndef GLSLProgram_h
#define GLSLProgram_h

#include "GLIncludes.h"
#include "SharedPointerDefines.h"

#include <map>

class GLSLProgram
{
public:
    typedef std::map<std::string, GLint> StringToIntMap;
    
    GLSLProgram();
    ~GLSLProgram();
    
    GLuint id() const { return mId; }
    
    bool isValid() const { return (mId != 0); }
    
    bool create();
    void destroy();
    
    void attach(GLSLShaderPtr s);
    void detach(GLSLShaderPtr s);
    bool link();
    
    GLint attribute(std::string const &attributeName);
    GLint uniform(std::string const &uniformName);
    
    void bindAttribute(std::string const &attributeName, GLuint attributeIndex);
    
    void use() { glUseProgram(this->id()); }
    void release() { glUseProgram(0); }
    
private:
    GLint addUniform(std::string const &uniformName);

    GLuint mId;
    StringToIntMap mAttributes;
    StringToIntMap mUniforms;
    
};

#endif /* GLSLProgram_h */
