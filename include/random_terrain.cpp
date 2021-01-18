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
    
    if(config_struct.trees) treeShader.makeShader(config_struct.treeShader);

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

    config_struct.genNormals = dimensions["genNormals"].asBool();

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

    /*Set the correct amount of vertex floats for allocating the array and for the vertex buffer depending on whether we are using texture coords or not*/

    int nVertexFloats;
    if(config_struct.texture)nVertexFloats = 5;
    else nVertexFloats = 6;

    //Attrin index is the attribute of the  texCoords/colors
    int attribIndex;
    if(config_struct.genNormals) attribIndex = 2;
    else attribIndex = 1;


    perlInNoise2D(config_struct.x,config_struct.y,seeds,config_struct.octaves,config_struct.bias,map);

    int offset = 0.4;
    
    int stride = getStride();

    float *vbTerrain = new float[detVbSize()];
    genVertexBuffer(vbTerrain,map);


    unsigned int* terrainIB;
    
    //There is no need for an index buffer if we are just drawing the points
    if(config_struct.primitive != GL_POINTS) terrainIB = new unsigned int[(config_struct.x-1)*(config_struct.y-1)*6]; //Something wrong here
    
    
    if(config_struct.primitive == GL_TRIANGLES) indexBufferTriangles(terrainIB);
    else if(config_struct.primitive == GL_LINES) indexBufferLines(terrainIB);
    
    

    GLCall(glGenVertexArrays(1,&vao));
    GLCall(glBindVertexArray(vao));


    GLCall(glGenBuffers(1,&vbo));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER,vbo));
    GLCall(glBufferData(GL_ARRAY_BUFFER,config_struct.x*config_struct.y*nVertexFloats*sizeof(float),&vbTerrain[0],GL_STATIC_DRAW));



    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,nVertexFloats*sizeof(float),(void*)0));

    GLCall(glEnableVertexAttribArray(attribIndex));
    GLCall(glVertexAttribPointer(attribIndex,nVertexFloats-3,GL_FLOAT,GL_FALSE,nVertexFloats*sizeof(float),(void*)(3*sizeof(float))));
    

    if(config_struct.primitive != GL_POINTS)
    {
        GLCall(glGenBuffers(1,&ib));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ib));
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,(config_struct.x-1)*(config_struct.y-1)*6*sizeof(unsigned int),&terrainIB[0],GL_STATIC_DRAW));
    }



    GLCall(glBindVertexArray(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0));

    
    
    if(config_struct.genNormals) generateNormals(vbTerrain,terrainIB);

    delete map;
    delete seeds;


    delete vbTerrain;
    if(config_struct.primitive != GL_POINTS) delete terrainIB;

    if(config_struct.trees)
    {
        tree.loadModel(config_struct.treeModel);

        //This needs to be after the model is generated for instancing to work
        genTerrainTrees();


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
    if(config_struct.genNormals)
    {
        stride += 3;
    }
    return stride;
}

int Terrain::detVbSize(){
    int stride = getStride();
    int size = 0;

    if(config_struct.genNormals)
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

       buffer[place+3] = interpolateFloat(config_struct.color1[0],config_struct.color2[0],point_height);
            buffer[place+4] = interpolateFloat(config_struct.color1[1],config_struct.color2[1],point_height);
            buffer[place+5] = interpolateFloat(config_struct.color1[2],config_struct.color2[2],point_height);

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

    if(config_struct.texture)
    {
        terrainTexture.Bind(config_struct.textureSlot);
        terrainShader.setUniform1i(config_struct.textureUniformName,config_struct.textureSlot); 

    }

    if(primitive != GL_POINTS)
    {
        glDrawElements(primitive,config_struct.x * config_struct.y * 6,GL_UNSIGNED_INT,nullptr);
    }
    else
    {
        glDrawArrays(primitive,0,config_struct.x * config_struct.y);
    }
    
    terrainShader.UnBind();
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


}

//For primitive GL_TRIANGLES
void Terrain::genVertexBuffer(float*& vbTerrain,float*& map)
{
    
    int xPlace = 0;
    int yPlace = 0;
    
    int stride = getStride();

    if(!config_struct.genNormals){
        for(int x=0; x < config_struct.x; x++)
        {
            for(int y =0; y < config_struct.y;y++)
            {
                fillVertex(vbTerrain,map,xPlace,stride,x,y);
                xPlace += stride;
            }
        }
    }
    else{
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

    for(int i = 0; i < config_struct.x * config_struct.y;i++)
    {
        glm::vec3 col = colors[(int)colors.size()*newColorMap[i]];


        glBufferSubData(GL_ARRAY_BUFFER,i*(6*sizeof(float))+(3*sizeof(float)),3*sizeof(float),&col[0]);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    delete newColorMap;
}

/* This still does not work properly and i need to fix it*/

void Terrain::generateNormals(float* vertexBuffer, unsigned int* indexes)
{
    glm::vec3* normalArray = new glm::vec3[(config_struct.x-1) * (config_struct.y-1)];

    int stride;
    if (config_struct.texture) stride = 5*sizeof(float);
    else stride = 6 * sizeof(float);


    int place = 0;
    for(int x =0; x < ((config_struct.x - 1) * (config_struct.y-1))/3; x++)
    {
        
        glm::vec3 posA = {vertexBuffer[indexes[place] * stride],vertexBuffer[indexes[place] * stride + 1],vertexBuffer[indexes[place] * stride + 2]};
        glm::vec3 posB = {vertexBuffer[indexes[place+1] * stride],vertexBuffer[indexes[place+1] * stride + 1],vertexBuffer[indexes[place+1] * stride + 2]};
        glm::vec3 posC = {vertexBuffer[indexes[place+2] * stride],vertexBuffer[indexes[place+2] * stride + 1],vertexBuffer[indexes[place+2] * stride + 2]};

        glm::vec3 vecA = posB - posA;
        glm::vec3 vecB = posC - posA;

        glm::vec3 normal = glm::normalize(glm::cross(vecA,vecB));

        if (normal.y < 0) normal.y *= -1;
        
        std::cout << indexes[place+2] << "\n";
        normalArray[indexes[place]] = normal;
        normalArray[indexes[place+1]] = normal;
        normalArray[indexes[place+2]] = normal;
        place += 3;

    }

    GLCall(glGenBuffers(1,&normals));
    
    GLCall(glBindVertexArray(vao));
    
    GLCall(glBindBuffer(GL_ARRAY_BUFFER,normals));
    GLCall(glBufferData(GL_ARRAY_BUFFER,(config_struct.x-1) * (config_struct.y-1)*3*sizeof(float),&normalArray[0],GL_STATIC_DRAW));

    GLCall(glEnableVertexAttribArray(1));
    GLCall(glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0));

    GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
    glBindVertexArray(0);

    delete normalArray;
}
