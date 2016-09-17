//
//  GLSLShader.cpp
//  VFMorph
//
//  Created by 薛健 on 16/6/10.
//  Copyright © 2016年 Jian Xue. All rights reserved.
//
#include "stdafx.h"
#include "GLSLShader.h"
#include "ScopeGuard.h"

#include <iostream>
#include <sstream>
#include <fstream>

inline std::string get_shader_type_string(GLenum type) {
    switch (type) {
        case GL_VERTEX_SHADER: return std::string("Vertex");
        case GL_FRAGMENT_SHADER: return std::string("Fragment");
        default: return std::string("Unknown for OpenGL ES");
    }
}

GLSLShader::GLSLShader() {
    mId = 0;
    mType = GL_VERTEX_SHADER;
}

GLSLShader::GLSLShader(GLenum type) {
    mId = 0;
    this->create(type);
}

GLSLShader::~GLSLShader() {
    this->destroy();
}

bool GLSLShader::create(GLenum type) {
    mType = type;
    mId = glCreateShader(type);
    if (mId == 0) {
        std::cout << "Fail to create GLSL shader with type " << type << " (" << get_shader_type_string(type) << ")." << std::endl;
        return false;
    }
    return true;
}

void GLSLShader::destroy() {
    if (mId != 0) {
        glDeleteShader(mId);
        mId = 0;
    }
}

bool GLSLShader::loadFromString(const std::string &source) {
    if (mId == 0) return false;
    
    GLchar const *sourceChars = source.c_str();
    glShaderSource(mId, 1, &sourceChars, nullptr);
    
    return true;
}

bool GLSLShader::loadFromFile(const std::string &filename) {
    if (mId == 0) return false;
    
    std::ifstream file;
    file.open(filename);
    if (!file.good()) {
        std::cout << "Fail to open shader source file " << filename << std::endl;
        return false;
    }
    
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    
    return this->loadFromString(stream.str());
}

bool GLSLShader::compile() {
    if (mId == 0) return false;

    glCompileShader(mId);
    
    // Check the compilation status and report any errors
    GLint shaderStatus;
    glGetShaderiv(mId, GL_COMPILE_STATUS, &shaderStatus);
    
    // If the shader failed to compile, display the info log and quit out
    if (shaderStatus == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(mId, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        ON_SCOPE_EXIT([&]{ delete[] strInfoLog; });
        glGetShaderInfoLog(mId, infoLogLength, nullptr, strInfoLog);
        strInfoLog[infoLogLength-1] = '\0';
        std::cout << get_shader_type_string(mType) << " shader compilation failed: " << strInfoLog << std::endl;
        
        return false;
    }
    
    return true;
}
