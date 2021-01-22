#include "random_terrain.h"


Terrain::Terrain(std::string config_file)
{
    if(config_file == "")
    {

    }
    else
    {
        read_config_file(config_file);
    }
    
    height_map = new float[config_struct.x * config_struct.y];

    length = config_struct.y * config_struct.offset;
    width = config_struct.x * config_struct.offset;

    if(config_struct.texture)
    {
        terrainTexture.InitTexture(config_struct.textureLocation,config_struct.wrapMode);
    }

    if(config_struct.geometryShader)
    {
        terrainShader.makeShader(config_struct.shaderLocation,true);
    }
    else
    {    
        terrainShader.makeShader(config_struct.shaderLocation);
    }

 
    shaders.push_back(&terrainShader);
    if(config_struct.waterTrue)shaders.push_back(&waterObj.waterShader);
    if(config_struct.uniformBuffer){
        terrainShader.proj_and_view_ubo = true;
    } 

    if(config_struct.trees) {
        treeShader.makeShader(config_struct.treeShader);
        shaders.push_back(&treeShader);
        if(config_struct.treeUniformBuffer)
        {
            treeShader.proj_and_view_ubo = true;
        }
    }

    if(config_struct.primitive == GL_POINTS || (config_struct.genNormals && config_struct.perFaceNormals)) genIB = false; else genIB = true; 
    
    for(int i=0; i < shaders.size();i++)
    {
        if(shaders[i]->proj_and_view_ubo)uboShaders.push_back(shaders[i]->getShaderID()); else notUboShaders.push_back(shaders[i]);
    }


}


Terrain::~Terrain()
{
    configuration->empty();
    delete configuration;
    
    delete height_map;
}



void Terrain::read_config_file(std::string& name)
{
    configuration = make_json(name);

    Json::Value temp = *configuration;

    auto perlinnoise = temp["perlinnoise"];
    
    config_struct.octaves = perlinnoise["octaves"].asInt();
    config_struct.bias = perlinnoise["bias"].asFloat();
    config_struct.seed = perlinnoise["seed"].asInt();

    auto dimensions = temp["dimensions"];
    config_struct.x = dimensions["x"].asInt();
    config_struct.y = dimensions["y"].asInt();
    config_struct.height = dimensions["max_height"].asFloat();

    config_struct.posX = dimensions["posX"].asFloat();
    config_struct.posY = dimensions["posY"].asFloat();

    config_struct.offset = dimensions["offset"].asFloat();

    if(!dimensions["collisionOffset"].isNull()) config_struct.collisionOffset = dimensions["collisionOffset"].asFloat();

    if(!dimensions["primitive"].isNull())
    {
        /*Only these three primitives for now, may add more in the future*/
        if (dimensions["primitive"].asString() == "GL_POINTS") config_struct.primitive = GL_POINTS;
        if (dimensions["primitive"].asString() == "GL_TRIANGLES") config_struct.primitive = GL_TRIANGLES;
        if (dimensions["primitive"].asString() == "GL_LINES") config_struct.primitive = GL_LINES;
    }


    config_struct.trees = dimensions["trees"].asBool();

    if(config_struct.trees)
    {
        auto grid = dimensions["grid"];
        
        config_struct.gridX = grid["gridX"].asInt();
        config_struct.gridY = grid["gridY"].asInt(); 

        if(!grid["chancePerGrid"].isNull()) config_struct.treeChance = grid["chancePerGrid"].asFloat();
        if(!grid["maxNumInGrid"].isNull()) config_struct.maxNumInGrid = grid["maxNumInGrid"].asInt();

        if(!grid["treeModel"].isNull()) config_struct.treeModel = grid["treeModel"].asString();
        if(!grid["treeShader"].isNull()) config_struct.treeShader = grid["treeShader"].asString();
        config_struct.treeUniformBuffer = grid["treeUniformBufferForProjAndView"].asBool();
        config_struct.instancing = grid["instancing"].asBool();

    }

    auto colors = temp["colors"];


    config_struct.staticColor = colors["staticColor"].asBool();
    config_struct.texture = colors["texture"].asBool();
    if(config_struct.texture)
    {
        config_struct.textureLocation = colors["textureLocation"].asString();
        if(!colors["textureSlot"].isNull())
        {
            config_struct.textureSlot = colors["textureSlot"].asInt();
        }
       
        config_struct.textureRepeat = colors["textureRepeat"].asBool();
        if(config_struct.textureRepeat)
        {
    
            auto textureRepeatConfig = colors["textureRepeatConfig"];
            config_struct.xTextureRepeatOffset = textureRepeatConfig["xTextureRepeatOffset"].asFloat();
            config_struct.yTextureRepeatOffset = textureRepeatConfig["yTextureRepeatOffset"].asFloat();
            if(textureRepeatConfig["wrapMode"] == "GL_WRAP_BORDER") config_struct.wrapMode = GL_WRAP_BORDER;
         

        }
    }
    else
    {   
        if(!colors["color1"].isNull())
        {

            Json::Value tmp = colors["color1"];
            config_struct.color1 = {tmp[0].asFloat(),tmp[1].asFloat(),tmp[2].asFloat()};
        }
        if(!colors["color2"].isNull())
        {

            Json::Value tmp = colors["color2"];
            config_struct.color2 = {tmp[0].asFloat(),tmp[1].asFloat(),tmp[2].asFloat()};
        }
    }
    
    auto shaderConfig = temp["shader"];
    if(!shaderConfig["shaderLocation"].isNull())
    {
        config_struct.shaderLocation = shaderConfig["shaderLocation"].asString();
        if(!shaderConfig["textureUniformName"].isNull()) config_struct.textureUniformName = shaderConfig["textureUniformName"].asString();
        config_struct.geometryShader = shaderConfig["geometryShader"].asBool();
        config_struct.uniformBuffer = shaderConfig["UniformBufferForProjAndView"].asBool();

    }

    if(!temp["matrices"].isNull())
    {
        auto matricesConfig = temp["matrices"];
        if(!matricesConfig["model"].isNull()) {
            float model[16];

            auto modelJson = matricesConfig["model"];
            int index =0;
            for (const auto& el : modelJson)
            {   
                model[index] = el.asFloat();
                index++;
            }

            config_struct.modelMatrix = transpose(glm::make_mat4(model));            
        }
    }

    config_struct.genNormals = temp["genNormals"].asBool();
    if(!temp["lighting"].isNull() && config_struct.genNormals)
    {
        auto lightingConfig = temp["lighting"];
        config_struct.perFaceNormals = lightingConfig["perFaceNormals"].asBool();
    }

    config_struct.waterTrue = temp["waterPresent"].asBool();
    if(config_struct.waterTrue)
    {
        if(!temp["water"].isNull())
        {
            auto waterConfig = temp["water"];
            if(!waterConfig["waterHeight"].isNull()) config_struct.waterHeight = waterConfig["waterHeight"].asFloat();
            if(!waterConfig["waterColor"].isNull())
            {
                Json::Value tmp = waterConfig["waterColor"];
                config_struct.waterColor = {tmp[0].asFloat(),tmp[1].asFloat(),tmp[2].asFloat()};
            }
        }
    }


}

//In desperate need of some abstraction idk
void Terrain::init()
{
    setLehmer((uint32_t) config_struct.seed);
    
   
    float* map = new float[config_struct.x * config_struct.y];
    float* seeds = new float[config_struct.x * config_struct.y];
    
    for(int i = 0; i < config_struct.x*config_struct.y;i++)
    {
        //seeds[i] = (float)rand() / (float)RAND_MAX; b
        seeds[i] = randdouble(0.0,1.0);
    }

    int attribIndex =1;

    perlInNoise2D(config_struct.x,config_struct.y,seeds,config_struct.octaves,config_struct.bias,map);

    int stride = getStride();

    float *vbTerrain = new float[detVbSize()];

    genVertexBuffer(vbTerrain,map);


    unsigned int* terrainIB;
    
    //There is no need for an index buffer if we are just drawing the points
    if(genIB) 
    {

        terrainIB = new unsigned int[(config_struct.x-1)*(config_struct.y-1)*6]; 
        if(config_struct.primitive == GL_TRIANGLES) indexBufferTriangles(terrainIB);
        else if(config_struct.primitive == GL_LINES) indexBufferLines(terrainIB);
    }
    

    GLCall(glGenVertexArrays(1,&vao));
    GLCall(glBindVertexArray(vao));


    GLCall(glGenBuffers(1,&vbo));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER,vbo));
    GLCall(glBufferData(GL_ARRAY_BUFFER,detVbSize()*sizeof(float),&vbTerrain[0],GL_STATIC_DRAW));


    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,getStride()*sizeof(float),(void*)0));

    GLCall(glEnableVertexAttribArray(attribIndex));
    GLCall(glVertexAttribPointer(attribIndex,stride-3,GL_FLOAT,GL_FALSE,getStride()*sizeof(float),(void*)(3*sizeof(float))));
    

    if(genIB)
    {
        GLCall(glGenBuffers(1,&ib));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ib));
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,(config_struct.x-1)*(config_struct.y-1)*6*sizeof(unsigned int),&terrainIB[0],GL_STATIC_DRAW));
    }



    GLCall(glBindVertexArray(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0));

    if(config_struct.genNormals)
    {
        if(config_struct.perFaceNormals)
        {
            generateNormalsPerFace(vbTerrain);
        }
        else{
            generateNormalsAveraged(vbTerrain);
        }
    }


    delete map;
    delete seeds;


    delete vbTerrain;
    if(genIB) delete terrainIB;

    if(config_struct.trees)
    {
        tree.loadModel(config_struct.treeModel);

        //This needs to be after the model is generated for instancing to work
        genTerrainTrees();

    }

    if(config_struct.waterTrue)
    {
        waterObj = Water(10,config_struct.x*config_struct.offset);
        waterObj.setHeight(config_struct.waterHeight);
        waterObj.waterColor = config_struct.waterColor;
        waterObj.genBuffer();
        waterObj.setShader("res/shaders/2d.shader");
    }

}

int Terrain::getStride()
{
    int stride = 3;

    if(config_struct.texture)
    {
        stride += 2;
    }
    else{
        stride += 3;
    }
  
    return stride;
}

int Terrain::detVbSize(){
    int stride = getStride();
    int size = 0;

    if(config_struct.genNormals && config_struct.perFaceNormals)
    {
        size = (config_struct.x-1)*(config_struct.y-1)*6 * stride;
        return size;
    }
    return config_struct.x * config_struct.y * stride;
}

float Terrain::interpolateFloat(float color1, float color2, float fraction)
{
    float min = std::min(color1,color2);
    float max = std::max(color1,color2);

    float val = (color1 - color2) * fraction + color2;

    return val;
}


float Terrain::getTerrainHeight(float x, float y)
{
    /* Multiplying the -1 by the y and by the posY is to flip the z because yk opengl be facing the negative z. If you decide to not do the flipping remember to get rid of them*/
    y *= -1;

    float localX =  x - config_struct.posX;
    float localY =  y - config_struct.posY*-1;

    float squareSize = width * length / config_struct.y - 1;

    int gridX = std::floor(localX / config_struct.offset/*squareSize*/);
    int gridY = std::floor(localY / config_struct.offset/*squareSize*/);


    if(gridX < 0 || gridX > config_struct.x || gridY < 0 || gridY > config_struct.y)
    {
        return 0;
    }
    float xCoord = (std::fmod(localX , config_struct.offset)/*squareSize*/) / config_struct.offset /*squareSize*/;
    float yCoord = (std::fmod(localY , config_struct.offset)/*squareSize*/) / config_struct.offset /*squareSize*/;
    

    float answer;


    if(xCoord <= 1-yCoord) // Top left triangle
    {
        answer = barryCentric({0,height_map[gridX * config_struct.y + gridY],0},{1,height_map[(gridX+1)*config_struct.y+gridY],0},{0,height_map[gridX * config_struct.y + gridY + 1],1},{xCoord,yCoord});
    }
    else
    {    
        answer = barryCentric({1,height_map[(gridX+1) * config_struct.y + gridY],0},{1,height_map[(gridX+1)*config_struct.y+gridY+1],1},{0,height_map[gridX * config_struct.y + gridY + 1],1},{xCoord,yCoord});   
    }
    

    return answer;


}

float Terrain::barryCentric(std::vector<float> p1, std::vector<float> p2, std::vector<float> p3, std::vector<float> pos) const
    {
		float det = (p2[2] - p3[2]) * (p1[0] - p3[0]) + (p3[0] - p2[0]) * (p1[2] - p3[2]);
		float l1 = ((p2[2] - p3[2]) * (pos[0] - p3[0]) + (p3[0] - p2[0]) * (pos[1] - p3[2])) / det;
		float l2 = ((p3[2] - p1[2]) * (pos[0] - p3[0]) + (p1[0] - p3[0]) * (pos[1] - p3[2])) / det;
		float l3 = 1.0f - l1 - l2;
		return l1 * p1[1] + l2 * p2[1] + l3 * p3[1];
	}


void Terrain::determineColAttrib(float*& buffer,int place)
{
        float point_height = (buffer[place + 1]-config_struct.height/(15/7))/4.0f;
        glm::vec3 col {interpolateFloat(config_struct.color1[0],config_struct.color2[0],point_height),interpolateFloat(config_struct.color1[1],config_struct.color2[1],point_height),interpolateFloat(config_struct.color1[2],config_struct.color2[2],point_height)};
        clampColor(col);


            buffer[place+3] = col.r;
            buffer[place+4] = col.g;
            buffer[place+5] = col.b;

            if(config_struct.staticColor)
            {
                if(place % 18 != 0)
                {
                    buffer[place+3] = buffer[place - 3];
                    buffer[place+4] = buffer[place - 2];
                    buffer[place+5] = buffer[place - 1];

                }
            }

}

void Terrain::clampColor(glm::vec3& col)
{
    for(int i=0;i<3;i++)
    {
        if(col[i] < 0.0f) col[i] = 0.0f;
        if(col[i] > 1.0f) col[i] = 1.0f;
    }
}

void Terrain::determineTexAttrib(float*& buffer,int x, int y, int place)
{
   if(!config_struct.textureRepeat)
    {
        float xBlend = (float)x/(config_struct.x-1);
        float yBlend = (float)y/(config_struct.y-1);

        buffer[place+3] = yBlend;
        buffer[place+4] = xBlend;
    }
    else
    {
        float lengX = (float)config_struct.x/config_struct.xTextureRepeatOffset;
        float lengY = (float)config_struct.y/config_struct.yTextureRepeatOffset;


        float xLocationInSquare = ((float)x/(float)config_struct.x*lengX);
        float yLocationInSquare = ((float)y/(float)config_struct.y*lengY);


        buffer[place+3] = xLocationInSquare;
        buffer[place+4] = yLocationInSquare;
    }
}


void Terrain::Draw(GLenum primitive)
{
    if(primitive == -1) primitive = config_struct.primitive;


    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ib);

    terrainShader.Bind();
    terrainShader.setUniformMat4f("model",config_struct.modelMatrix);
    if(config_struct.texture){
        terrainTexture.Bind(config_struct.textureSlot);
        terrainShader.setUniform1i(config_struct.textureUniformName,config_struct.textureSlot); 

    }
    if(genIB){
        glDrawElements(primitive,config_struct.x * config_struct.y * 6,GL_UNSIGNED_INT,nullptr);
    }
    else{
        glDrawArrays(primitive,0,detVbSize()/getStride());
    }
    
    terrainShader.UnBind();

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    
    if(config_struct.trees)
    {
        treeShader.Bind();
        if(!config_struct.instancing)
        {
            for(int i = 0; i < treeModelMatrices.size(); i++)
            {
                treeShader.setUniformMat4f("model",treeModelMatrices[i]);
                tree.Draw(treeShader,false);
            }
        }
        else
        {
            tree.DrawInstanced(treeShader,treeModelMatrices.size(),false);
        }

        treeShader.UnBind();

    }
    if(config_struct.waterTrue)
    {
        waterObj.Draw();
    }

   
    

}

//For primitive GL_TRIANGLES
void Terrain::genVertexBuffer(float*& vbTerrain,float*& map)
{
    
    int xPlace = 0;
    int yPlace = 0;
    
    int stride = getStride();

    if(!config_struct.genNormals || (!config_struct.perFaceNormals)){
        for(int x=0; x < config_struct.x; x++)
        {
            for(int y =0; y < config_struct.y;y++)
            {
                fillVertex(vbTerrain,map,xPlace,stride,x,y);
                xPlace += stride;
            }
        }
    }
    else {
     for(int x=0; x < config_struct.x-1; x++)
        {
            for(int y =0; y < config_struct.y-1;y++)
            {
                fillVertex(vbTerrain,map,xPlace,stride,x,y);
                xPlace += stride;
                fillVertex(vbTerrain,map,xPlace,stride,x+1,y);
                xPlace += stride;
                fillVertex(vbTerrain,map,xPlace,stride,x,y+1);
                xPlace += stride;
                fillVertex(vbTerrain,map,xPlace,stride,x,y+1);
                xPlace += stride;
                fillVertex(vbTerrain,map,xPlace,stride,x+1,y+1);
                xPlace += stride;
                fillVertex(vbTerrain,map,xPlace,stride,x+1,y);
                xPlace += stride;
            }
        }
    
    
    }
   

}

void Terrain::fillVertex(float*& vbTerrain, float*& map,int& xPlace,const int& stride, int x, int y){
                vbTerrain[xPlace] = (float)x*config_struct.offset + config_struct.posX;
                vbTerrain[xPlace +1] = map[x * config_struct.y + y] * config_struct.height;  
                vbTerrain[xPlace + 2] = (float)(y*config_struct.offset*-1 + config_struct.posY);


                if(!config_struct.texture)
                {
                    determineColAttrib(vbTerrain,xPlace);

                }
                else
                {
                    determineTexAttrib(vbTerrain,x,y,xPlace);
                }
                
                height_map[x*config_struct.y + y] = vbTerrain[xPlace +1];
}


void Terrain::indexBufferTriangles(unsigned int*& buffer)
{
    int place = 0;
    for(int x = 0; x < config_struct.x-1; x++)
    {

        for(int y=0; y < config_struct.y-1;y++)
        {   
            
            buffer[place] = x*config_struct.y + y;
            buffer[place + 1] = (x+1)*config_struct.y + y;
            buffer[place + 2] = (x+1)*config_struct.y + y+1;

            buffer[place + 5] = (x+1)*config_struct.y + y+1;
            buffer[place + 4] = x*config_struct.y + y+1;
            buffer[place + 3] = x*config_struct.y + y;

            place += 6;

        }

    }

}

//For primitive GL_LINES
void Terrain::indexBufferLines(unsigned int*& buffer)
{
    int place = 0;
    for(int x = 0; x < config_struct.x-1; x++)
    {

        for(int y=0; y < config_struct.y-1;y++)
        {   
            
            buffer[place] = x*config_struct.y + y;
            buffer[place + 1] = (x+1)*config_struct.y + y;
            
            buffer[place + 2] = x*config_struct.y + y;
            buffer[place + 3] = x*config_struct.y + y+1;

            buffer[place + 4] = x*config_struct.y + y;
            buffer[place + 5] = (x+1)*config_struct.y + y+1;

            place += 6;

        }
    }
}

//Determine where in terrain the trees will be
void Terrain::genTerrainTrees()
{
    int gridsX = config_struct.x / config_struct.gridX;
    int gridsY = config_struct.y / config_struct.gridY;

    std::vector<std::vector<int>> treePositions;
    for(int x = 0; x < gridsX; x++)
    {
        for(int y = 0; y < gridsY; y++)
        {
            int result = randint(0,config_struct.treeChance);
            if(result == 1)
            {
                int amount;
                if (config_struct.maxNumInGrid == 1) amount = 1; //To avoid floating point errors
                else amount = randint(1,config_struct.maxNumInGrid);

                for(int i = 0; i < amount; i++)
                {
                    treePositions.push_back({x,y});
                    
                    float xOffset = randdouble(0,config_struct.gridX);
                    float zOffset = randdouble(0,config_struct.gridY);


                    float positionX = ((treePositions[treePositions.size()-1][0] * config_struct.gridX) + xOffset) * config_struct.offset;
                    float positionZ = ((treePositions[treePositions.size()-1][1] * config_struct.gridY) + zOffset) * config_struct.offset * -1;
                    float positionY = getTerrainHeight(positionX+config_struct.posX,positionZ + config_struct.posY);

                    treeModelMatrices.push_back(config_struct.modelMatrix * glm::translate(glm::mat4(1.0f),glm::vec3(positionX,positionY,positionZ)));
                }


            }

        }

    }
    if(config_struct.instancing)
    {
        glGenBuffers(1,&modelMatrixBuffer);
        glBindBuffer(GL_ARRAY_BUFFER,modelMatrixBuffer);
        glBufferData(GL_ARRAY_BUFFER,treeModelMatrices.size() * sizeof(glm::mat4),&treeModelMatrices[0],GL_STATIC_DRAW);


        for(int i = 0; i < tree.meshes.size(); i++)
        {
            unsigned int vao = tree.meshes[i].VAO;
            glBindVertexArray(vao);

            std::size_t sizeVec4 = sizeof(glm::vec4);
            glEnableVertexAttribArray(3); //Assuming that the third and onwards attribute is already empty and that the shader is using the 3rd attrib for it. May need to make this a setting in the config
            glVertexAttribPointer(3,4,GL_FLOAT,GL_FALSE,4 * sizeVec4,(void*)0);

            glEnableVertexAttribArray(4); 
            glVertexAttribPointer(4,4,GL_FLOAT,GL_FALSE,4 * sizeVec4,(void*) (1 * sizeVec4));

            glEnableVertexAttribArray(5); 
            glVertexAttribPointer(5,4,GL_FLOAT,GL_FALSE,4 * sizeVec4,(void*) (2 * sizeVec4));

            glEnableVertexAttribArray(6); 
            glVertexAttribPointer(6,4,GL_FLOAT,GL_FALSE,4 * sizeVec4,(void*) (3 * sizeVec4));

            glVertexAttribDivisor(3,1); //Every instance rather than every vertex shader iteration
            glVertexAttribDivisor(4,1);
            glVertexAttribDivisor(5,1);
            glVertexAttribDivisor(6,1);

            glBindVertexArray(0);
        }
        glBindBuffer(GL_ARRAY_BUFFER,0);

    }
}


void Terrain::newColors(std::vector<glm::vec3>& colors)
{
    if(config_struct.texture)
    {
        return;
    }
    float* newColorMap = new float[config_struct.x * config_struct.y];
    float* newSeedMap = new float[config_struct.x * config_struct.y]; 
    for(int i = 0; i < config_struct.x * config_struct.y;i++)
    {
        newSeedMap[i] = randdouble(0,1);
    }
    perlInNoise2D(config_struct.x,config_struct.y,newSeedMap,6,1.6,newColorMap);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    if(!(config_struct.genNormals && config_struct.perFaceNormals))
    {
        for(int i = 0; i < config_struct.x * config_struct.y;i++)
        {
            glm::vec3 col = colors[(int)colors.size()*newColorMap[i]];
            glBufferSubData(GL_ARRAY_BUFFER,i*getStride()*sizeof(float)+(3*sizeof(float)),3*sizeof(float),&col[0]);
        }
    }
    else{
        int place = 0;
        for(int x = 0; x < config_struct.x-1;x++)
        {
            for(int y = 0; y < config_struct.y-1;y++)
            {
                glm::vec3 col = colors[(int)colors.size()*newColorMap[x*config_struct.y+y]];
                glBufferSubData(GL_ARRAY_BUFFER,place*(getStride()*sizeof(float))+(3*sizeof(float)),3*sizeof(float),&col[0]);
                
                col = colors[(int)colors.size()*newColorMap[(x+1)*config_struct.y+y]];
                glBufferSubData(GL_ARRAY_BUFFER,(place+1)*(getStride()*sizeof(float))+(3*sizeof(float)),3*sizeof(float),&col[0]);

                col = colors[(int)colors.size()*newColorMap[(x+1)*config_struct.y+(y+1)]];
                glBufferSubData(GL_ARRAY_BUFFER,(place+2)*(getStride()*sizeof(float))+(3*sizeof(float)),3*sizeof(float),&col[0]);

                col = colors[(int)colors.size()*newColorMap[x*config_struct.y+(y+1)]];
                glBufferSubData(GL_ARRAY_BUFFER,(place+3)*(getStride()*sizeof(float))+(3*sizeof(float)),3*sizeof(float),&col[0]);
                
                col = colors[(int)colors.size()*newColorMap[(x+1)*config_struct.y+(y+1)]];
                glBufferSubData(GL_ARRAY_BUFFER,(place+4)*(getStride()*sizeof(float))+(3*sizeof(float)),3*sizeof(float),&col[0]);

                col = colors[(int)colors.size()*newColorMap[(x+1)*config_struct.y+y]];
                glBufferSubData(GL_ARRAY_BUFFER,(place+5)*(getStride()*sizeof(float))+(3*sizeof(float)),3*sizeof(float),&col[0]);
                
                place += 6;
            }
        }
    }
 

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    delete newColorMap;
}

/* This still does not work properly and i need to fix it*/

void Terrain::generateNormalsPerFace(float*& vertexBuffer)
{
    glm::vec3* normalArray = new glm::vec3[(detVbSize()/getStride())];

    int stride = getStride();


    int place = 0;
    int normalPlace = 0;
    
        for(int x =0; x < (detVbSize()/getStride())/3; x++)
        {

            glm::vec3 posA = {vertexBuffer[place],vertexBuffer[place+1],vertexBuffer[place+2]};
            place += stride;
            glm::vec3 posB = {vertexBuffer[place],vertexBuffer[place+1],vertexBuffer[place+2]};
            place += stride;
            glm::vec3 posC = {vertexBuffer[place],vertexBuffer[place+1],vertexBuffer[place+2]};

            glm::vec3 normal = getNormalFromPositions(posA,posB,posC);

            normalArray[normalPlace] = normal;
            normalArray[normalPlace+1] = normal;
            normalArray[normalPlace+2] = normal;

            place += stride;
            normalPlace += 3;
        }
    


    setNormalBuffer(normalArray);


    delete normalArray;
}

void Terrain::generateNormalsAveraged(float*& vertexBuffer)
{
    glm::vec3* normalArray = new glm::vec3[(detVbSize()/getStride())];

    int stride = getStride();
    int place = 0;

    for(int x = 0; x < config_struct.x; x++)
    {
        for(int y = 0; y < config_struct.y; y++)
        {
            std::vector<glm::vec3> norms;
            
            glm::vec3 self;
            glm::vec3 up;
            glm::vec3 left;

            glm::vec3 topleft;
            glm::vec3 down;
            glm::vec3 right;
            glm::vec3 bottomright;

            self = {vertexBuffer[stride*(x*config_struct.x + y)],vertexBuffer[stride*(x*config_struct.x + y)+1],vertexBuffer[stride*(x*config_struct.x + y)+2]}; 
            
            if(!(x < 1))
            {
                left = {vertexBuffer[stride*((x-1)*config_struct.x + y)],vertexBuffer[stride*((x-1)*config_struct.x + y)+1],vertexBuffer[stride*((x-1)*config_struct.x + y)+2]}; 
                if(!(y>config_struct.y-1))
                {
                    topleft = {vertexBuffer[stride*((x+1)*config_struct.x + y-1)],vertexBuffer[stride*((x+1)*config_struct.x + y-1)+1],vertexBuffer[stride*((x+1)*config_struct.x + y-1)+2]}; 
                }
            }
            if(!(x > config_struct.x-1))
            {
                right = {vertexBuffer[stride*((x+1)*config_struct.x + y)],vertexBuffer[stride*((x+1)*config_struct.x + y)+1],vertexBuffer[stride*((x+1)*config_struct.x + y)+2]}; 
                if(!(y<1))
                {
                    bottomright = {vertexBuffer[stride*((x+1)*config_struct.x + y-1)],vertexBuffer[stride*((x+1)*config_struct.x + y-1)+1],vertexBuffer[stride*((x+1)*config_struct.x + y-1)+2]}; 
                }
            }
            if(!(y < 1))
            {
                down = {vertexBuffer[stride*(x*config_struct.x + y-1)],vertexBuffer[stride*(x*config_struct.x + y-1)+1],vertexBuffer[stride*(x*config_struct.x + y-1)+2]}; 
            }
            if(!(y>config_struct.y-1))
            {
                up = {vertexBuffer[stride*(x*config_struct.x + y+1)],vertexBuffer[stride*(x*config_struct.x + y+1)+1],vertexBuffer[stride*(x*config_struct.x + y+1)+2]}; 
            }

            if(!(x<1))
            {
                if(!(y<1))
                {
                  norms.push_back(getNormalFromPositions(left,self,down));
                }
                if(!(y>config_struct.y-1))
                {
                  norms.push_back(getNormalFromPositions(up,self,topleft));
                  norms.push_back(getNormalFromPositions(left,self,topleft));
                }
            }      
            if(!(x>config_struct.x-1))
            {
                if(!(y<1))
                {
                    norms.push_back(getNormalFromPositions(bottomright,self,down));
                    norms.push_back(getNormalFromPositions(bottomright,self,right));
                }
                if(!(y>config_struct.y-1))
                {
                    norms.push_back(getNormalFromPositions(up,self,right));
                }
            }        
            glm::vec3 total = {0,0,0};
            int divisor = 1;
            for(int i =0;i<6;i++)
            {
                if(!(norms[i] == glm::vec3(-1.0f,-1.0f,-1.0f)))
                {
                    total.x += norms[i].x;
                    total.y += norms[i].y;
                    total.z += norms[i].z;
                    divisor++;
                }
            }
            glm::vec3 normal = {total.x/divisor,total.y/divisor,total.z/divisor};

            normalArray[place] = normal;
            place++;
        }
    }
        /*
        glm::vec3 posA = {vertexBuffer[indices[place]*stride],vertexBuffer[(indices[place]*stride)+1],vertexBuffer[(indices[place]*stride)+2]};
        place++;
        glm::vec3 posB = {vertexBuffer[indices[place]*stride],vertexBuffer[(indices[place]*stride)+1],vertexBuffer[(indices[place]*stride)+2]};
        place++;
        glm::vec3 posC = {vertexBuffer[indices[place]*stride],vertexBuffer[(indices[place]*stride)+1],vertexBuffer[(indices[place]*stride)+2]};
        place++;

        glm::vec3 vecA = posB - posA;
        glm::vec3 vecB = posC - posA;
        glm::vec3 normal = glm::normalize(glm::cross(vecA,vecB));
        if(normal.y < 0)
        {
            normal = normal * glm::vec3(-1.0f,-1.0f,-1.0f);
        }
        unAveragedNormals[i] = normal;*/
    
    setNormalBuffer(normalArray);
    delete normalArray;
}

glm::vec3 Terrain::getNormalFromPositions(glm::vec3 posA,glm::vec3 posB,glm::vec3 posC)
{
  
    glm::vec3 vecA = posB - posA;
    glm::vec3 vecB = posC - posA;
    glm::vec3 normal = glm::normalize(glm::cross(vecA,vecB));
    if(normal.y < 0)
    {
        normal = normal * glm::vec3(-1.0f,-1.0f,-1.0f);
    }
    return normal;
}

void Terrain::setNormalBuffer(glm::vec3*& normalArray)
{
    GLCall(glGenBuffers(1,&normals));
    
    GLCall(glBindVertexArray(vao));

    
    GLCall(glBindBuffer(GL_ARRAY_BUFFER,normals));
    GLCall(glBufferData(GL_ARRAY_BUFFER,(detVbSize()/getStride())*3*sizeof(float),&normalArray[0][0],GL_STATIC_DRAW));

    GLCall(glEnableVertexAttribArray(2));
    GLCall(glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0));

    GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
    glBindVertexArray(0);
}


