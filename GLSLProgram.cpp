//
//  GLSLProgram.cpp
//  VFMorph
//
//  Created by 薛健 on 16/6/10.
//  Copyright © 2016年 Jian Xue. All rights reserved.
//
#include "stdafx.h"
#include "GLSLProgram.h"
#include "GLSLShader.h"
#include "ScopeGuard.h"

#include <iostream>

GLSLProgram::GLSLProgram() {
    mId = 0;
}

GLSLProgram::~GLSLProgram() {
    this->destroy();
}

bool GLSLProgram::create() {
    mId = glCreateProgram();
    if (mId == 0) {
        std::cout << "Fail to create GLSL program." << std::endl;
        return false;
    }
    return true;
}

void GLSLProgram::destroy() {
    if (mId != 0) {
        glDeleteProgram(mId);
        mId = 0;
    }
}

void GLSLProgram::attach(GLSLShaderPtr s) {
    if (!s) return;
    glAttachShader(mId, s->id());
}

void GLSLProgram::detach(GLSLShaderPtr s) {
    if (!s) return;
    glDetachShader(mId, s->id());
}

bool GLSLProgram::link() {
    if (mId == 0) return false;
    
    // bind attributes
    for (StringToIntMap::const_iterator it=mAttributes.begin(); it!=mAttributes.end(); ++it) {
        glBindAttribLocation(mId, it->second, it->first.c_str());
    }//Why here
    
    glLinkProgram(mId);
    
    GLint linkStatus;
    glGetProgramiv(mId, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(mId, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        ON_SCOPE_EXIT([&]{ delete[] strInfoLog; });
        glGetProgramInfoLog(mId, infoLogLength, nullptr, strInfoLog);
        strInfoLog[infoLogLength-1] = '\0';
        std::cout << "Shader program linking failed: " << strInfoLog << std::endl;
        return false;
    }
    return true;
}

GLint GLSLProgram::attribute(const std::string &attributeName) {
    StringToIntMap::const_iterator it = mAttributes.find(attributeName);
    if (it != mAttributes.end()) return it->second;
    return -1;
}

GLint GLSLProgram::uniform(const std::string &uniformName) {
    StringToIntMap::const_iterator it = mUniforms.find(uniformName);
    if (it != mUniforms.end()) return it->second;
    return this->addUniform(uniformName);
}

void GLSLProgram::bindAttribute(const std::string &attributeName, GLuint attributeIndex) {
    mAttributes[attributeName] = attributeIndex;
}

GLint GLSLProgram::addUniform(const std::string &uniformName) {
    GLint loc = glGetUniformLocation(mId, uniformName.c_str());
    if (loc == -1) {
        std::cout << "Could not add uniform: " << uniformName << " - location returned -1!" << std::endl;
    } else {
        mUniforms[uniformName] = loc;
    }
    
    return loc;
}
