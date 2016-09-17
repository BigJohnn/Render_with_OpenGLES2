//
//  GLSLShader.hpp
//  VFMorph
//
//  Created by 薛健 on 16/6/10.
//  Copyright © 2016年 Jian Xue. All rights reserved.
//

#ifndef GLSLShader_h
#define GLSLShader_h

#include "GLIncludes.h"
#include <string>


class GLSLShader
{
public:
    GLSLShader();
    GLSLShader(GLenum type);
    ~GLSLShader();
    
    GLuint id() const { return mId; }
    bool isValid() const { return (mId != 0); }
    
    bool create(GLenum type);
    void destroy();
    
    bool loadFromString(std::string const &source);
    bool loadFromFile(std::string const &filename);
    
    bool compile();
    
private:
    GLuint mId;
    GLenum mType;
    
};

#endif /* GLSLShader_h */
