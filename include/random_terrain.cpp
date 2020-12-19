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

    terrainShader.makeShader(config_struct.shaderLocation);
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

    if(dimensions["primitive"])
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

        if(grid["chancePerGrid"]) config_struct.treeChance = grid["chancePerGrid"].asFloat();
        if(grid["treeModel"]) config_struct.treeModel = grid["treeModel"].asString();
        if(grid["treeShader"]) config_struct.treeShader = grid["treeShader"].asString();

    }

    auto colors = temp["colors"];


    config_struct.staticColor = colors["staticColor"].asBool();
    config_struct.texture = colors["texture"].asBool();
    if(config_struct.texture)
    {
        config_struct.textureLocation = colors["textureLocation"].asString();
        if(colors["textureSlot"])
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
        if(colors["color1"])
        {

            Json::Value tmp = colors["color1"];
            config_struct.color1 = {tmp[0].asFloat(),tmp[1].asFloat(),tmp[2].asFloat()};
        }
        if(colors["color2"])
        {

            Json::Value tmp = colors["color2"];
            config_struct.color2 = {tmp[0].asFloat(),tmp[1].asFloat(),tmp[2].asFloat()};
        }
    }
    
    auto shaderConfig = temp["shader"];
    if(shaderConfig["shaderLocation"])
    {
        config_struct.shaderLocation = shaderConfig["shaderLocation"].asString();
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



    //perlInNoise2D(256,256,seeds,7,2,map);
    perlInNoise2D(config_struct.x,config_struct.y,seeds,config_struct.octaves,config_struct.bias,map);

    int offset = 0.4;
    int stride = nVertexFloats;

    //float vbTerrain[256*256 * 6];
    
    int xPlace = 0;
    int yPlace = 0;

    float *vbTerrain = new float[config_struct.x*config_struct.y * nVertexFloats];

    //Colors to blend between. Should be set in config but this is just for testing
    
    
    for(int x=0; x < config_struct.x; x++)
    {
        for(int y =0; y < config_struct.y;y++)
        {

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

            xPlace += stride;


        }
    }

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

    GLCall(glEnableVertexAttribArray(1));
    GLCall(glVertexAttribPointer(1,nVertexFloats-3,GL_FLOAT,GL_FALSE,nVertexFloats*sizeof(float),(void*)(3*sizeof(float))));
    

    if(config_struct.primitive != GL_POINTS)
    {
        GLCall(glGenBuffers(1,&ib));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ib));
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,(config_struct.x-1)*(config_struct.y-1)*6*sizeof(unsigned int),&terrainIB[0],GL_STATIC_DRAW));
    }



    GLCall(glBindVertexArray(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0));


    delete map;
    delete seeds;


    delete vbTerrain;
    if(config_struct.primitive != GL_POINTS) delete terrainIB;

    if(config_struct.trees)
    {
        genTerrainTrees();
        tree.loadModel(config_struct.treeModel);

    }

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
        float xLocationInSquare = ((float)x/config_struct.xTextureRepeatOffset - (int)(x/config_struct.xTextureRepeatOffset));
        float yLocationInSquare = ((float)y/config_struct.yTextureRepeatOffset - (int)(y/config_struct.yTextureRepeatOffset));


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

    if(config_struct.texture)
    {
        terrainTexture.Bind(config_struct.textureSlot);
        terrainShader.setUniform1i("u_Texture",config_struct.textureSlot); //Will need to have the name of the uniform set in confguration but hard coded for now

    }

    if(primitive != GL_POINTS)
    {
        glDrawElements(primitive,config_struct.x * config_struct.y * 6,GL_UNSIGNED_INT,nullptr);
    }
    else
    {
        glDrawArrays(primitive,0,config_struct.x * config_struct.y);
    }
    
    if(config_struct.trees)
    {
        treeShader.Bind();
        for(int i = 0; i < treePositions.size(); i++)
        {

            float positionX = ((treePositions[i][0] * config_struct.gridX) + config_struct.gridX/2) * config_struct.offset;
            float positionZ = ((treePositions[i][1] * config_struct.gridY) + config_struct.gridY/2) * config_struct.offset * -1;
            float positionY = getTerrainHeight(positionX+config_struct.posX,positionZ + config_struct.posY);

            treeShader.setUniformMat4f("model",glm::translate(glm::mat4(1.0f),glm::vec3(positionX,positionY,positionZ)));
            tree.Draw(treeShader,false);
            
        }
        treeShader.UnBind();

    }


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

            buffer[place + 3] = x*config_struct.y + y;
            buffer[place + 4] = x*config_struct.y + y+1;
            buffer[place + 5] = (x+1)*config_struct.y + y+1;

            place += 6;

        }

    }

}

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


void Terrain::genTerrainTrees()
{
    int gridsX = config_struct.x / config_struct.gridX;
    int gridsY = config_struct.y / config_struct.gridY;


    for(int x = 0; x < gridsX; x++)
    {
        for(int y = 0; y < gridsY; y++)
        {
            int result = randint(0,config_struct.treeChance);
            if(result == 1)
            {
                treePositions.push_back({x,y});
            }

        }

    }
}

/* old code i am too scared to delete incase i may need it again*/


/* 
            float point_height = (vbTerrain[xPlace + 1]-config_struct.height/(15/7))/3.0f;

            vbTerrain[xPlace+3] = interpolateFloat(config_struct.color1[0],config_struct.color2[0],point_height);
            vbTerrain[xPlace+4] = interpolateFloat(config_struct.color1[1],config_struct.color2[1],point_height);
            vbTerrain[xPlace+5] = interpolateFloat(config_struct.color1[2],config_struct.color2[2],point_height);

            if(config_struct.staticColor)
            {
                if(xPlace % 18 != 0)
                {
                    vbTerrain[xPlace+3] = vbTerrain[xPlace - 3];
                    vbTerrain[xPlace+4] = vbTerrain[xPlace - 2];
                    vbTerrain[xPlace+5] = vbTerrain[xPlace - 1];

                }
            }
*/



/*
    std::cout << "\nlocalX: " << localX << " localX: " << localX << "\n";
    std::cout << "gridX: " << gridX << " gridY: " << gridY << "\n";
    std::cout << "xCoord: " << xCoord << " yCoord: " << yCoord << "\n";

...
    std::cout << "heightmap " << answer << "\n\n\n";


*/