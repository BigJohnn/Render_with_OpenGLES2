//
// Created by 薛健 on 16/6/7.
// Copyright (c) 2016 Jian Xue. All rights reserved.
//

#ifndef SharedPtr_h
#define SharedPtr_h

#if defined(__APPLE__) || defined(__MACOSX__)
#  include <ciso646>
#  if defined(_LIBCPP_VERSION)
#    include <memory>
using std::shared_ptr;
using std::weak_ptr;
#  else
#    include <tr1/memory>
using std::tr1::shared_ptr;
using std::tr1::weak_ptr;
#  endif
#else
#  include <memory>
using std::tr1::shared_ptr;
using std::tr1::weak_ptr;
#endif

#include <vector>

class GLSLShader;
class GLSLProgram;
class FaceModel;
class VirtualFace;

typedef shared_ptr<GLSLShader> GLSLShaderPtr;
typedef shared_ptr<GLSLProgram> GLSLProgramPtr;

typedef shared_ptr<VirtualFace> VirtualFacePtr;
typedef std::vector<VirtualFacePtr> VirtualFaceArray;

typedef shared_ptr<FaceModel> FaceModelPtr;

#endif // SharedPtr_h
