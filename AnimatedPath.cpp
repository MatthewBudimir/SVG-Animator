
#include "AnimatedPath.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <utility>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#define CURVE_TYPE_M 0
#define CURVE_TYPE_S 1
#define CURVE_TYPE_C 2

Coords scalarMult(Coords coord,double amount){
    Coords out;
    out.x=coord.x*amount;
    out.y=coord.y*amount;
    return out;
}
Coords addCoords(Coords a,Coords b){
    Coords out;
    out.x = a.x + b.x;
    out.y = a.y + b.y;
    return out;
}
// Render paths using all the paths you've found.
AnimatedPath::AnimatedPath(std::string svg, std::string brushName, int animationLength, 
 int w,int h, int xPos, int yPos, SDL_Renderer * rend){

    // Store arguments/initialize variables in Animatedpath object.
    dstRect.x = xPos;
    dstRect.y = yPos;
    dstRect.h = h;
    dstRect.w = w;
    renderer = rend;
    frameIndex = 0;

    // Load brush file into texture
    // Load svg information into vector of coordinates
    loadPaths(svg); //loads protopaths from file.
    std::cout << "File read complete - " << protopaths.size() <<" paths read" << std::endl;
    std::cout << "POINT COUNT: " << protopaths[0].points.size() << std::endl;
    SDL_Texture * brush = IMG_LoadTexture(renderer,brushName.c_str());
    std::cout << "Brush loaded" << std::endl;
    // Use paths
    std::vector <std::vector<Coords> > positions;
    
    // position generation: generate the positions to stamp the brush at. Much finer than frame count.
    
    // B(t) = (1-t)3 P0 + 3(1-t)2t P1 + 3(1-t)t2 P2 + t3 P3
    double smoothness = 1000; //XTRA SMOOTH
    double step = 1/smoothness;
    // for each segment...
    double scale1;
    double scale2;
    double scale3;
    double scale4;
    Coords first;
    Coords second;
    Coords third;
    Coords fourth;
    Coords result;
    // before we can make textures we need to make surfaces. Before we make surfaces we need to make our animations
    for(int i = 0; i<animationLength;i++)
    {
        frames.push_back(NULL);
    }

    // CALCULATE ANIMATION SEQUENCES.
    // If we have 100 frames of animation, and 4 paths we need to draw, each path gets drawn for 25 frames.
    int pathAnimationLength = animationLength/protopaths.size(); // more paths -> more segments
    std::cout << "Path animation Length: " << pathAnimationLength << std::endl;
    int paintCount = 0; // however many frames we've painted so far.
    // Create the canvas
    SDL_Texture * canvas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    // Set the canvas as the render target
    SDL_SetRenderTarget(renderer, canvas);
    SDL_RenderClear(renderer);
    // for every protopath.
    std::vector<Coords> temp;
    for(int i = 0; i<protopaths.size();i++)
    {
        for(int segment=0; segment<protopaths[i].points.size()-1;segment++)
        {
            std::cout <<"SEGMENT " << segment << std::endl;
            std::cout << "START: " << protopaths[i].points[segment].x << "," << protopaths[i].points[segment].y << std::endl;
            std::cout << "OUTBOUND: " << protopaths[i].out[segment].x << "," << protopaths[i].out[segment].y << std::endl;
            std::cout << "INBOUND: " << protopaths[i].in[segment].x << "," << protopaths[i].in[segment].y << std::endl;
            std::cout << "END: " << protopaths[i].points[segment+1].x << "," << protopaths[i].points[segment+1].y << std::endl;
            for(double t=0; t<1; t+=step){
                scale1= pow(1-t,3);
                // scale1= 1;
                scale2= 3*pow(1-t,2)*t;
                scale3= 3*(1-t)*pow(t,2);
                scale4= pow(t,3);
                // // calculate scaled components
                first = scalarMult(protopaths[i].points[segment],scale1);
                second = scalarMult(protopaths[i].out[segment],scale2);
                third = scalarMult(protopaths[i].in[segment],scale3);
                fourth = scalarMult(protopaths[i].points[segment+1],scale4);
                // sum together components
                result = addCoords(first,second);
                result = addCoords(result,third);
                result = addCoords(result,fourth);
                // // store the result
                temp.push_back(result);
            }
        }
        positions.push_back(temp);
        temp.clear();
    }
    // Normalize the points you have just created
    normalizePaths(w-50,h-50,positions);
    std::cout << "paths normalized" <<std::endl;
    for(int pathNum = 0; pathNum<protopaths.size();pathNum++)
    {
        // Every protopath has a colour: use it to modify the brush colour
        // SDL_SetTextureColorMod(brush,255,255,255);
        SDL_SetTextureColorMod(brush,protopaths[pathNum].colour[0],protopaths[pathNum].colour[1],protopaths[pathNum].colour[2]);
        
        //Snapshot frequency is how many frames we skip before we save one to our animation
        int snapshotFrequency = positions[pathNum].size()/pathAnimationLength;
        
        SDL_Rect rect;
        rect.w=100;
        rect.h=100;
        SDL_Texture * snapshot;
        for(int i=0;i<positions[pathNum].size();i++)
        {
            // Account for size of brush with -50 (brush is 100px by 100px)
            rect.x = positions[pathNum][i].x;
            rect.y = positions[pathNum][i].y;
            // Put the brush onto the canvas
            // SDL_BlitSurface(brush, NULL, canvas, &rect);
            SDL_RenderCopy(renderer, brush, NULL, &rect);
            if(i%snapshotFrequency == 0)
            {
                // save this image.
                snapshot =SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1200, 600);
                // Set the snapshot as the render target.
                SDL_SetRenderTarget(renderer, snapshot);
                // Copy the canvas to the snapshot.
                SDL_RenderCopy(renderer, canvas, NULL,NULL);
                // Make the canvas the render taget again.
                SDL_SetRenderTarget(renderer, canvas);
                // Store the snapshot
                frames[paintCount] = snapshot;
                paintCount++;
            }
        }
        std::cout << "PaintCount: " << paintCount << std::endl;
        // Fill in remaining frames that come from division error.
        snapshot =SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
        // Set the snapshot as the render target.
        SDL_SetRenderTarget(renderer, snapshot);
        // Copy the canvas to the snapshot.
        SDL_RenderCopy(renderer, canvas, NULL,NULL);
        // Make the canvas the render taget again.
        SDL_SetRenderTarget(renderer, canvas);
        while(paintCount < pathAnimationLength*(pathNum+1))
        {
            frames[paintCount] = snapshot;
            paintCount++;
        }
        std::cout << "PaintCount adjusted: " << paintCount << std::endl;
        // animationStartingFrame += paintCount;

        // if this is the last path, account for 
        if(pathNum == protopaths.size()-1){
            while(paintCount < animationLength){
                frames[paintCount] = snapshot;
                paintCount++;
            }
        }
    }
    // We're done creating frames
    
    if(frames.size()==0){
        std::cout << "No frames" << std::endl;
        exit(0);
    }
    std::cout << "we have: " << frames.size() << " frames" << std::endl;
    std::cout << "Drawing " << protopaths.size() << " paths" << std::endl; 

    SDL_SetRenderTarget(renderer, NULL);
}

void AnimatedPath::loadPaths(std::string fileName) {
    // Loads paths into protopath vector from file
    // Open file
    std::ifstream vectorFile;
    vectorFile.open(fileName);
    if(vectorFile.is_open()){
        std::cout <<"File open" << std::endl;
    }
    std::string path = "";
    std::string buffer;
    std::vector<std::string> paths;
    bool readingPath = false;
    // Split paths into string.
    while(getline(vectorFile,buffer)){
        if(readingPath)
        {
            path+= buffer;
            if(buffer.find(">") != -1){
                readingPath = false;
                paths.push_back(path);
                std::cout <<"PATH:" <<path << std::endl;
                path = "";
            }
        } else {
            if(buffer.find("path") != -1){
                // then this line is useful.
                readingPath = true;
                path = buffer;
            }
        }
    }
    std::size_t index;
    std::string colour;
    std::string data;
    ProtoPath protopath;
    for(int g = 0; g<paths.size();g++)
    {

        std::string rawInput = paths[g];
        // get the colour substring.
        index = rawInput.find('#');
        colour = rawInput.substr(index+1,6);
        // Get the coordinate list
        index = rawInput.find("d=");
        data = rawInput.substr(index+4); // skip `d= "M`
        // remove last 3 elements.
        data.pop_back();
        data.pop_back();
        data.pop_back();
        data.pop_back();
        // Read through all coordinates.
        // Read starting position x and y.
        
        // Remove white space
        for(int n=0; n<data.length(); n++)
        {
            if(data[n] == '\t' || data[n] == ' '){
                data.erase(n,1);
                n--;
            }
        }
        // First, replace - with ,-
        for(int n=0; n<data.length(); n++)
        {
            if(data[n] == '-' && n != 0)
            {
                // If the thing preceding the '-' was a letter, don't put a comma since this is the first coordinate.
                if(data[n-1] >= '0' && data[n-1] <= '9'){
                    data.insert(n,",");
                    n++;
                }
            }
        }
        // Replace letters, with ,{letter}
        for(int n=0; n<data.length(); n++)
        {
            // Replace 'M' with ',M'
            if((data[n] == 'm' || data[n] == 'M' || data[n] == 'c' || data[n] == 'C' || data[n] == 's' || data[n] == 'S') && n != 0)
            {
                data.insert(n,",");
                n++;
            }
        }
        // Collect x and y for first "move command"
        // Check for 'c' command
        // If using C x,y,c1x,c1y,c2x,c2y
        std::string token = "";
        
        // Read the colour string into an array.
        // protopath.colour = std::stoi(colour, 0, 16);
        for(int b =0;b<6;b+=2)
        {
            protopath.colour[b/2] = std::stoi(colour.substr(b,2),0,16);
        }
        Coords outbound;
        Coords inbound;
        Coords dest;
        int readingCoor = 4;
        int curveType = CURVE_TYPE_M;
        double coordinateSet[6];
        double converted;
        for(int k=0;k<6;k++){
            coordinateSet[k]=0;
        }
        char c;
        // if(data[data.length()-1 != ','])
        // {
        //     data = data + ',';
        // }
        char commandType = 'm';
        int xOrigin=0;
        int yOrigin=0;
        bool relative = true;
        std::cout << "DATA:" << data << std::endl;
        while(data.length()>0){
            c = data[0];
            data.erase(0,1);
            if(c=='s' || c== 'S' || c=='c' || c=='C' || c=='M' || c=='m')
            {
                switch (c)
                {
                    case 'S':
                        // do the case below
                        commandType = 'S';
                        readingCoor = 2; // We don't need to read the outbound vector for this set.
                        curveType = CURVE_TYPE_S;
                        break;
                    case 's':
                        commandType = 's';
                        // We need to do something for S curves
                        readingCoor = 2; // We don't need to read the outbound vector for this set.
                        curveType = CURVE_TYPE_S;
                        break;
                    case 'c':
                        // Do the case below:
                        commandType = 'c';
                        readingCoor = 0; //Use all 6 numbers of coordinate set.
                        curveType = CURVE_TYPE_C;
                        break;
                    case 'C':
                        // We need to do something for cubic curves. 
                        commandType = 'C';
                        readingCoor = 0; //Use all 6 numbers of coordinate set.
                        curveType = CURVE_TYPE_C;
                        break;
                    case 'm':
                        // do the case below
                    case 'M':
                        // Inbound and outbound vectors are 0,0
                        commandType = 'm';
                        for(int k =0; k<4;k++){
                            coordinateSet[k] = 0;
                        }
                        readingCoor = 4; //we only need to read 1 set of coordinates.
                        curveType = CURVE_TYPE_M;
                        break;
                    default:
                        break;
                }
            }
            else if(c == ',')
            {
                // We've finished processing a coordinate.
                // Convert token to double.
                // std::cout << "CONVERTED: " << token << "##" << std::endl;
                converted = std::stod(token);
                token = "";
                // Load double into the correct location.
                coordinateSet[readingCoor] = converted;
                std::cout << "ADDING NUMBER " <<readingCoor<<": " << converted << std::endl;

                // Figure out if we've
                // based on the coordinate we're up to, load it into the correct one.  
                readingCoor++;
                if (readingCoor > 5){
                    std::cout << "ka'ching " << commandType << std::endl; 
                    // if we've read the last number (destination y), load our numbers into a datastructure and 
                    // Load coordinates into Path.
                    if(commandType == 'm'){
                        coordinateSet[0] = coordinateSet[4];
                        coordinateSet[1] = coordinateSet[5];
                        coordinateSet[2] = coordinateSet[4];
                        coordinateSet[3] = coordinateSet[5];
                        dest.x = coordinateSet[4];
                        dest.y = coordinateSet[5];
                        // TEMPORARY RELATIVE ORIGIN VARIABLES ASSIGNED
                        // xOrigin = coordinateSet[4];
                        // yOrigin = coordinateSet[5];
                        
                    }
                    else{
                        if(commandType == 's')
                        {
                            relative = true;
                            coordinateSet[0] = (inbound.x-protopath.points[protopath.points.size()-1].x)*-1;
                            coordinateSet[1] = (inbound.y-protopath.points[protopath.points.size()-1].y)*-1;
                            commandType = 'c';
                        }
                        else if(commandType == 'S'){
                            commandType = 'S';
                            relative = false; //temporarily?
                            std::cout << "CAPITAL S!!!!!" << std::endl;
                            std::cout << "Former Inbound: " << inbound.x << "," << inbound.y <<std::endl;
                            std::cout << "Former dest: " << dest.x << "," << dest.y << std::endl;
                            coordinateSet[0] = 2*dest.x - inbound.x;
                            coordinateSet[1] = 2*dest.y - inbound.y;
                            std::cout << "Reversal: " << coordinateSet[0] << "," <<coordinateSet[1] << std::endl;
                            //TODO: Do something with coordinateSet 0 and 1
                        }
                        else if(commandType == 'C'){
                            relative = false;
                        }
                        else if(commandType == 'c'){
                            relative = true;
                        }
                        
                        if(relative){
                            outbound.x = coordinateSet[0] + dest.x;
                            outbound.y = coordinateSet[1] + dest.y;
                            protopath.out.push_back(outbound);
                            inbound.x = coordinateSet[2] + dest.x;
                            inbound.y = coordinateSet[3] + dest.y;
                            protopath.in.push_back(inbound);
                            dest.x = coordinateSet[4] + dest.x;
                            dest.y = coordinateSet[5] + dest.y;
                        }
                        else{
                            outbound.x = coordinateSet[0];
                            outbound.y = coordinateSet[1];
                            protopath.out.push_back(outbound);
                            inbound.x = coordinateSet[2];
                            inbound.y = coordinateSet[3];
                            protopath.in.push_back(inbound);
                            dest.x = coordinateSet[4];
                            dest.y = coordinateSet[5];
                        }
                    }
                    
                    protopath.points.push_back(dest);
                    
                    //Reset readingCoor to appropriate value:
                    switch (curveType)
                    {
                        case CURVE_TYPE_M:
                            readingCoor = 4; // 'skip' the first 4 numbmers (inbound and outbound vectors)
                            break;
                        case CURVE_TYPE_C:
                            readingCoor = 0;
                            break;
                        case CURVE_TYPE_S:
                            readingCoor = 2; // 'skip' the first 2 numbers.
                        default:
                            break;
                    }
                }
                
            }
            else //I guess it must be a number?
            {
                token += c;
            }
        }
        protopaths.push_back(protopath);
        for(int i = 0; i<protopath.points.size();i++){
            std::cout << "POINT: " << protopath.points[i].x <<","<<protopath.points[i].x<<std::endl;
        }
        protopath.in.clear();
        protopath.out.clear();
        protopath.points.clear();
    }
    // for(int i=0;i<protopaths.size();i++)
    // {
    //     std::cout << "1";
    // }
    // std::cout <<std::endl;
    vectorFile.close();
}

void AnimatedPath::normalizePaths(int w,int h, std::vector<std::vector <Coords> > &points){
    double minX = points[0][0].x;
    double minY = points[0][0].y;
    double maxX = points[0][0].x;
    double maxY = points[0][0].y;
    int pCount = 0;
    // Find max and min.
    std::cout << "UNOFFICIAL POINT COUNT: " << points[0].size() << std::endl;
    for(int i =0; i<points.size();i++)
    {
        for(int j = 0; j<points[i].size();j++)
        {
            pCount++;
            if(points[i][j].x < minX)
            {
                minX = points[i][j].x;
            }
            if(points[i][j].y < minY)
            {
                minY = points[i][j].y;
            }

            if(points[i][j].x > maxX)
            {
                maxX = points[i][j].x;
            }
            if(points[i][j].y > maxY)
            {
                maxY = points[i][j].y;
            }
        }
    }
    std::cout << "SCALING -> POINT COUNT:" << pCount << std::endl;
    std::cout << "INITIAL minX:" << minX << std::endl;
    std::cout << "INITIAL minY:" << minY << std::endl;
    std::cout << "INITIAL maxX:" << maxX << std::endl;
    std::cout << "INITIAL maxY:" << maxY << std::endl;
    // Shift all the points:
    for(int i =0; i<points.size();i++)
    {
        for(int j = 0; j<points[i].size();j++)
        {
            // Shift on the x axis
            points[i][j].x -= minX;
            // Shift on the y axis
            points[i][j].y -= minY;
        }

    }
    // rescale points
    for(int i =0; i<points.size();i++)
    {
        if(maxX == 0)
        {
            maxX = 1;
        }
        if(maxY == 0)
        {
            maxY = 1;
        }
        for(int j = 0; j<points[i].size();j++)
        {
            // Scale on x axis
            points[i][j].x = points[i][j].x*(double(w)/(maxX+1));

            // Scale on y axis
            points[i][j].y = points[i][j].y*(double(h)/(maxY+1));
        }
    }
    minX = points[0][0].x;
    minY = points[0][0].y;
    maxX = points[0][0].x;
    maxY = points[0][0].y;
    // Find max and min.
    for(int i =0; i<points.size();i++)
    {
        for(int j = 0; j<points[i].size();j++)
        {
            if(points[i][j].x < minX)
            {
                minX = points[i][j].x;
            }
            else if(points[i][j].y < minY)
            {
                minY = points[i][j].y;
            }

            if(points[i][j].x > maxX)
            {
                maxX = points[i][j].x;
            }
            else if(points[i][j].y > maxY)
            {
                maxY = points[i][j].y;
            }
        }
    }
    std::cout << "minX" << minX << std::endl;
    std::cout << "minY" << minY << std::endl;
    std::cout << "maxX" << maxX << std::endl;
    std::cout << "maxY" << maxY << std::endl;
}

bool AnimatedPath::render(){
    // Render the relevant animation of the frame and increment the 
    bool res = SDL_RenderCopy( renderer, frames[frameIndex], NULL, &dstRect ) > 0;
    if(frameIndex < frames.size()-1){
        //hang on the last frame.
        frameIndex++;
    }
    return res;
}

