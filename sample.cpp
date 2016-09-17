bool VirtualFaceProcessor::setupGLSLPrograms() {
    if (!mProgramForPoint2D->isValid()) {
        GLSLShaderPtr vertShader = std::make_shared<GLSLShader>();
        if (!vertShader->create(GL_VERTEX_SHADER)) return false;
        std::string source =
        "attribute vec4 position;"
        "void main() {"
        "    gl_Position = position;"
        "}";
        vertShader->loadFromString(source);
        if (!vertShader->compile()) return false;
        
        GLSLShaderPtr fragShader = std::make_shared<GLSLShader>();
        if (!fragShader->create(GL_FRAGMENT_SHADER)) return false;
        source =
        "void main() {"
        "    gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);"
        "}";
        fragShader->loadFromString(source);
        if (!fragShader->compile()) return false;
        
        if (!mProgramForPoint2D->create()) return false;
        mProgramForPoint2D->attach(vertShader);
        mProgramForPoint2D->attach(fragShader);
        mProgramForPoint2D->bindAttribute("position", ATTRIB_VERTEX);
        if (!mProgramForPoint2D->link()) {
            mProgramForPoint2D->destroy();
        }
        mProgramForPoint2D->detach(vertShader);
        mProgramForPoint2D->detach(fragShader);
    }
    
    return mProgramForPoint2D->isValid();
}
